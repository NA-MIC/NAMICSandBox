/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Language:  C++
  Date:      $Date: 2009-06-14 11:52:00 $
  Version:   $Revision: 1.69 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "lsqr.h"
#include <math.h>

#define Abs(x) ((x) >= 0 ? (x) : -(x))

#define CopyVector(n,x,y) \
  { \
  const double * xp = x; \
  const double * xend = xp + n; \
  double * yp = y; \
  while( xp != xend ) \
    { \
    *yp++ = *xp++; \
    } \
  }

#define AccumulateVector(n,x,y) \
  { \
  const double * xp = x; \
  const double * xend = xp + n; \
  double * yp = y; \
  while( xp != xend ) \
    { \
    *yp++ += *xp++; \
    } \
  }

#define AssignValueToVectorElements(n1,n2,v,x) \
  { \
  double * xp   = x + n1; \
  double * xend = x + n2; \
  while( xp != xend ) \
    { \
    *xp++ = v; \
    } \
  }

#define AssignScalarValueToVectorElements(n1,n2,v,x) \
  { \
  double * xp   = x + n1; \
  double * xend = x + n2; \
  while( xp != xend ) \
    { \
    *xp++ = v; \
    } \
  }

#define ElementWiseProductVector(n1,n2,x,y,z) \
  { \
  const double * xp   = x + n1; \
  const double * xend = x + n2; \
  double * yp = y; \
  double * zp = z; \
  while( xp != xend ) \
    { \
    *zp++ = *xp++ * *yp++; \
    } \
  }



lsqr::lsqr()
{
  this->eps = 1e-16;
  this->atol = 1e-6;
  this->btol = 1e-6;
  this->conlim = 1.0 / ( 10 * sqrt( eps ) );
  this->itnlim = 10;
  this->nout = NULL;
  this->istop = 0;
  this->itn = 0;
  this->Anorm = 0.0;
  this->Acond = 0.0;
  this->rnorm = 0.0;
  this->Arnorm = 0.0;
  this->xnorm = 0.0;
  this->wantse = false;
  this->se = NULL;
  this->damp = 0.0;
}


lsqr::~lsqr()
{
}


unsigned int
lsqr::GetStoppingReason() const
{
  return this->istop;
}


unsigned int
lsqr::GetNumberOfIterationsPerformed() const
{
  return this->itn;
}


double
lsqr::GetFrobeniusNormEstimateOfAbar() const
{
  return this->Anorm;
}


double
lsqr::GetConditionNumberEstimateOfAbar() const
{
  return this->Acond;
}


double
lsqr::GetFinalEstimateOfNormRbar() const
{
  return this->rnorm;
}


double 
lsqr::GetFinalEstimateOfNormOfResiduals() const
{
  return this->Arnorm;
}


double 
lsqr::GetFinalEstimateOfNormOfX() const
{
  return this->xnorm;
}


void
lsqr::SetStandardErrorEstimatesFlag(bool flag)
{
  this->wantse = flag;
}


void
lsqr::SetEpsilon( double value )
{
  this->eps = value;
}


void
lsqr::SetDamp( double value )
{
  this->damp = value;
}


void
lsqr::SetToleranceA( double value )
{
  this->atol = value;
}


void
lsqr::SetToleranceB( double value )
{
  this->btol = value;
}


void
lsqr::SetMaximumNumberOfIterations( unsigned int value )
{
  this->itnlim = value;
}


void
lsqr::SetUpperLimitOnConditional( double value )
{
  this->conlim = value;
}


void
lsqr::SetStandardErrorEstimates( double * array )
{
  this->se = array;
}


void
lsqr::SetMatrix( double ** inputA )
{
  this->A = inputA;
}


void
lsqr::SetOutputStream( std::ostream & os )
{
  this->nout = &os;
}


/**
 *  returns sqrt( a**2 + b**2 )
 *  with precautions to avoid overflow.
 */
double
lsqr::D2Norm( double a, double b ) const
{
  const double scale = Abs(a) + Abs(b);
  const double zero = 0.0;

  if( scale == zero )
    {
    return zero;
    }
  
  const double sa = a / scale;
  const double sb = b / scale;

  return scale * sqrt( sa * sa + sb * sb );
}


/* 

  returns x = (I - 2*z*z')*x.

  Implemented as x = x - z * ( 2*( z'*x ) )

*/
void lsqr::
HouseholderTransformation(unsigned int n, const double * z, double * x ) const
{
  // First, compute z'*x as a scalar product.
  double scalarProduct = 0.0;
  const double * zp = z;
  const double * zend = zp + n;
  double * xp = x;
  while( zp != zend )
    {
    scalarProduct += (*zp++) * (*xp++);
    }

  // Second, double the value of the scalar product.
  scalarProduct += scalarProduct;

  // Last, compute x = x - z * (2*z'*x) This subtract from x, double
  // the componenent of x that is parallel to z, effectively reflecting
  // x across the hyperplane whose normal is defined by z.
  zp = z;
  xp = x;
  zend = zp + n;
  while( zp != zend )
    {
    *xp++ -= scalarProduct * (*zp++);
    }
}


/**
 * computes y = y + A*x without altering x.
 */
void lsqr::
Aprod1(unsigned int m, unsigned int n, const double * x, double * y ) const
{
  for ( unsigned int row = 0; row < m; row++ )
    {
    const double * rowA = this->A[row];
    double sum = 0.0;

    for ( unsigned int col = 0; col < n; col++ )
      {
      sum += rowA[col] * x[col];
      }

    y[row] +=  sum;
    }
}


/**
 * computes x = x + A'*y without altering y.
 */
void lsqr::
Aprod2(unsigned int m, unsigned int n, double * x, const double * y ) const
{
  for ( unsigned int col = 0; col < n; col++ )
    {
    double sum = 0.0;

    for ( unsigned int row = 0; row < m; row++ )
      {
      sum += this->A[row][col] * y[row];
      }

    x[col] +=  sum;
    }
}



/** Simplified for this use from the BLAS version. */
void
lsqr::Scale( unsigned int n, double factor, double *x ) const
{
  double * xend = x + n;
  while( x != xend )
    {
    *x++ *= factor;
    }
}


/** Simplified for this use from the BLAS version. */
double
lsqr::Dnrm2( unsigned int n, const double *x ) const
{
  double magnitudeOfLargestElement = 0.0;

  double sumOfSquaresScaled = 1.0;

  for ( unsigned int i = 0; i < n; i++ )
    {
    if ( x[i] != 0.0 ) 
      {
      double dx = x[i];
      const double absxi = Abs(dx);

      if ( magnitudeOfLargestElement < absxi ) 
        {
        // rescale the sum to the range of the new element
        dx = magnitudeOfLargestElement / absxi;
        sumOfSquaresScaled = sumOfSquaresScaled * (dx * dx) + 1.0;
        magnitudeOfLargestElement = absxi;
        }
      else
        {
        // rescale the new element to the range of the sum
        dx = absxi / magnitudeOfLargestElement;
        sumOfSquaresScaled += dx * dx;
        }
      }
    }

  const double norm = magnitudeOfLargestElement * sqrt( sumOfSquaresScaled );

  return norm;
}


/** 
 *
 *  The array b must have size m
 *
 */
void lsqr::
Solve( unsigned int m, unsigned int n, double * b, double * x )
{
  const double zero = 0.0;
  const double one = 1.0;

  if( this->nout )
    {
    (*this->nout) << "Enter LSQR " << std::endl;
    (*this->nout) << m << ", " << n << std::endl;
    (*this->nout) << this->damp << ", " << this->wantse << std::endl; 
    (*this->nout) << this->atol << ", " << this->conlim << std::endl; 
    (*this->nout) << this->btol << ", " << this->itnlim << std::endl; 
    }

  const bool damped = ( this->damp > zero );

  this->itn = 0;
  this->istop = 0;
  
  unsigned int nstop = 0;
  unsigned int maxdx = 0;

  double ctol = zero;
  if( this->conlim > zero )
    {
    ctol = one / this->conlim;
    }

  this->Anorm = zero;
  this->Acond = zero;

  double dnorm = zero;
  double dxmax = zero;
  double res2 = zero;
  double psi = zero;

  this->xnorm = zero;

  double xnorm1 = zero;
  double cs2 = -one;
  double sn2 = zero;
  double z = zero;

  double * u = new double[m];
  double * v = new double[n];
  double * w = new double[n];

  //-------------------------------------------------------------------
  //  Set up the first vectors u and v for the bidiagonalization.
  //  These satisfy  beta*u = b,  alpha*v = A(transpose)*u.
  //-------------------------------------------------------------------
  CopyVector( m, b, u );
  AssignScalarValueToVectorElements( 0, n, zero, v );
  AssignScalarValueToVectorElements( 0, n, zero, x );

  if( this->wantse )
    {
    AssignScalarValueToVectorElements( 0, n, zero, se );
    }

  double alpha = zero;

  double beta =  this->Dnrm2( m, u );

  if( beta > zero )
    {
    this->Scale( m, ( one / beta ), u );
    this->Aprod2( m, n, v, u );   //     v = A'*u
    alpha = this->Dnrm2( n, v );
    }

  if( alpha > zero )
    {
    this->Scale( n, ( one / alpha ), v );
    CopyVector( n, v, w );
    }

  this->Arnorm = alpha * beta;

  double rhobar = alpha;
  double phibar = beta;
  double bnorm = beta;

  this->rnorm = beta;

  double test1 = 0.0;
  double test2 = 0.0;


  if ( this->Arnorm != zero )
    {

    if ( this->nout )
      {
      if ( damped )
        {
        (*this->nout) << " Itn       x(0)           Function"\
        "     Compatible   LS     Norm Abar Cond Abar alfa_opt" << std::endl;
        }
      else
        {
        (*this->nout) << " Itn       x(0)           Function"\
        "     Compatible   LS        Norm A    Cond A" << std::endl;
        }

      test1 = one;
      test2 = alpha / beta;

      this->nout->width(6);
      (*this->nout) << this->itn;
      this->nout->precision(9);
      this->nout->precision(17);
      (*this->nout) << x[0] << " ";
      this->nout->precision(2);
      this->nout->precision(10);
      (*this->nout) << rnorm << " ";
      this->nout->precision(1);
      this->nout->precision(9);
      (*this->nout) << test1 << " ";
      (*this->nout) << test2 << " ";
      (*this->nout) << std::endl;
      }
    }

  double temp;
  double test3;
  double rtol;


  //  Main itertation loop
  //
  do {

    this->itn++; 

    //----------------------------------------------------------------
    //  Perform the next step of the bidiagonalization to obtain the
    //  next beta, u, alpha, v.  These satisfy
    //      beta*u = A*v  - alpha*u,
    //     alpha*v = A'*u -  beta*v.
    //----------------------------------------------------------------
    this->Scale( m, (-alpha), u );

    this->Aprod1( m, n, v, u );   //   u = A * v

    beta = this->Dnrm2( m, u );

    //
    //  Accumulate Anorm = ||Bk|| = norm([alpha beta damp]).
    //
    temp   = this->D2Norm( alpha, beta );
    temp   = this->D2Norm( temp , damp );
    this->Anorm  = this->D2Norm( this->Anorm, temp );

    if ( beta > zero )
      {
      this->Scale( m, (one/beta), u );
      this->Scale( n, (- beta), v );
      this->Aprod2( m, n, v, u );    // v = A'*u

      alpha  = this->Dnrm2( n, v );

      if ( alpha > zero )
        {
        this->Scale( n, (one/alpha), v );
        }
     }

    //----------------------------------------------------------------
    //  Use a plane rotation to eliminate the damping parameter.
    //  This alters the diagonal (rhobar) of the lower-bidiagonal matrix.
    //---------------------------------------------------------------
    double rhbar1 = rhobar;

    if ( damped )
      {
      rhbar1 = this->D2Norm( rhobar, damp );
      const double cs1    = rhobar / rhbar1;
      const double sn1    = this->damp / rhbar1;
      psi    = sn1 * phibar;
      phibar = cs1 * phibar;
      }

    //----------------------------------------------------------------
    // Use a plane rotation to eliminate the subdiagonal element (beta)
    // of the lower-bidiagonal matrix, giving an upper-bidiagonal matrix.
    //----------------------------------------------------------------
    double rho    =   this->D2Norm( rhbar1, beta );
    double cs     =   rhbar1/rho;
    double sn     =   beta  /rho;
    double theta  =   sn * alpha;
    rhobar = - cs * alpha;
    double phi    =   cs * phibar;
    phibar =   sn * phibar;
    double tau    =   sn * phi;


    //----------------------------------------------------------------
    //  Update  x, w  and (perhaps) the standard error estimates.
    //---------------------------------------------------------------
    double t1     =     phi / rho;
    double t2     = - theta / rho;
    double t3     =     one / rho;
    double dknorm =    zero;

    if ( wantse ) 
      {
      for ( unsigned int i = 0; i < n; i++ )
        {
        double t = w[i];
        x[i]   = t1 * t +  x[i];
        w[i]   = t2 * t +  v[i];
        t      = ( t3 * t ) * ( t3 * t );
        se[i]  = t + se[i];
        dknorm = t + dknorm;
        }
      }
    else
      {
      for ( unsigned int i = 0; i < n; i++ )
        {
        double t = w[i];
        x[i]   = t1 * t + x[i];
        w[i]   = t2 * t + v[i];
        dknorm = ( t3 * t )*( t3 * t ) + dknorm;
        }
      }


    //----------------------------------------------------------------
    //  Monitor the norm of d_k, the update to x.
    //  dknorm = norm( d_k )
    //  dnorm  = norm( D_k ),       where   D_k = (d_1, d_2, ..., d_k )
    //  dxk    = norm( phi_k d_k ), where new x = x_k + phi_k d_k.
    //----------------------------------------------------------------
    dknorm = sqrt( dknorm );
    dnorm  = this->D2Norm( dnorm, dknorm );
    double dxk  = fabs( phi* dknorm );
    if (dxmax < dxk)
      {
      dxmax  = dxk;
      maxdx  = this->itn;
      }


    //----------------------------------------------------------------
    //  Use a plane rotation on the right to eliminate the
    //  super-diagonal element (theta) of the upper-bidiagonal matrix.
    //  Then use the result to estimate  norm(x).
    //----------------------------------------------------------------
    const double delta  =   sn2 * rho;
    const double gambar = - cs2 * rho;
    const double rhs    =   phi - delta * z;
    const double zbar   =   rhs   /gambar;
    this->xnorm  =   this->D2Norm( xnorm1, zbar );
    const double gamma  =   this->D2Norm( gambar, theta );
    cs2    =   gambar / gamma;
    sn2    =   theta  / gamma;
    z      =   rhs    / gamma;
    xnorm1 =   this->D2Norm( xnorm1, z );

    //----------------------------------------------------------------
    //  Test for convergence.
    //  First, estimate the norm and condition of the matrix  Abar,
    //  and the norms of  rbar  and  Abar(transpose)*rbar.
    //----------------------------------------------------------------
    this->Acond  = this->Anorm * dnorm;
    res2   = this->D2Norm( res2, psi );
    this->rnorm  = this->D2Norm( res2, phibar );

    this->rnorm  += 1e-30;       //  Prevent rnorm == 0.0
    this->Arnorm = alpha * fabs( tau );


    // Now use these norms to estimate certain other quantities,
    // some of which will be small near a solution.

    const double alfopt = sqrt( this->rnorm / ( dnorm * this->xnorm ) );
    test1  = this->rnorm / bnorm;
    test2  = zero;
    test2  = Arnorm / ( this->Anorm * this->rnorm );
    test3  = one   / this->Acond;
    t1     = test1 / ( one + this->Anorm* xnorm / bnorm );
    rtol   = btol  +   atol* this->Anorm* xnorm / bnorm;


    // The following tests guard against extremely small values of
    // atol, btol  or  ctol.  (The user may have set any or all of
    // the parameters  atol, btol, conlim  to zero.)
    // The effect is equivalent to the normal tests using
    // atol = eps,  btol = eps,  conlim = 1/eps.

    t3 = one + test3;
    t2 = one + test2;
    t1 = one + t1;
    if ( this->itn >= this->itnlim ) istop = 5;
    if ( t3  <= one    ) istop = 4;
    if ( t2  <= one    ) istop = 2;
    if ( t1  <= one    ) istop = 1;


    //  Allow for tolerances set by the user.

    if ( test3 <= ctol ) istop = 4;
    if ( test2 <= atol ) istop = 2;
    if ( test1 <= rtol ) istop = 1;


    //----------------------------------------------------------------
    // See if it is time to print something.
    //----------------------------------------------------------------
    bool prnt = false;
    if (nout > 0)
      {
      if (n     <=        40) prnt = true;
      if (this->itn   <=        10) prnt = true;
      if (this->itn   >= this->itnlim-10) prnt = true;
      if ( (this->itn % 10)  ==  0) prnt = true;
      if (test3 <=  2.0*ctol) prnt = true;
      if (test2 <= 10.0*atol) prnt = true;
      if (test1 <= 10.0*rtol) prnt = true;
      if (istop !=         0) prnt = true;

      if ( prnt ) // Print a line for this iteration.
        {
        this->nout->width(6);
        (*this->nout) << this->itn << " ";
        this->nout->precision(9);
        this->nout->precision(17);
        (*this->nout) << x[0] << " ";
        this->nout->precision(2);
        this->nout->precision(10);
        (*this->nout) << rnorm << " ";
        this->nout->precision(1);
        this->nout->precision(9);
        (*this->nout) << test1 << " ";
        (*this->nout) << test2 << " ";
        (*this->nout) << this->Anorm << " ";
        (*this->nout) << this->Acond << " ";
        (*this->nout) << alfopt << " ";
        (*this->nout) << std::endl;
        }
      }


    //----------------------------------------------------------------
    // Stop if appropriate.
    // The convergence criteria are required to be met on  nconv
    // consecutive iterations, where  nconv  is set below.
    // Suggested value:  nconv = 1, 2  or  3.
    //----------------------------------------------------------------
    if (istop == 0)
      {
      nstop = 0;
      }
    else
      {
      const unsigned int nconv = 1;
      nstop = nstop + 1;

      if ( ( nstop < nconv ) && ( this->itn < this->itnlim ) )
        {
        istop = 0;
        }
      }

    } while ( istop != 0); 

  //===================================================================
  // End of iteration loop.
  //===================================================================


  if ( this->wantse )         //  Finish off the
    {                         //  standard error estimates.
    double t = one;

    if ( m > n )
      {
      t = m - n;
      }

    if ( damped )
      {
      t = m;
      }

    t = this->rnorm / sqrt(t);

    for ( unsigned int i = 0; i < n; i++ )
      {
      se[i] = t * sqrt( se[i] );
      }
    }


  //
  //  Main itertation loop
  //

  // Release locally allocated arrays.
  delete [] u;
  delete [] v;
  delete [] w;
}

