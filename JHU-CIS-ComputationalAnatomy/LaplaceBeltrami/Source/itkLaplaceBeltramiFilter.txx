/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLaplaceBeltramiFilter.txx,v $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLaplaceBeltramiFilter_txx
#define __itkLaplaceBeltramiFilter_txx

#include "itkLaplaceBeltramiFilter.h"
#include "itkExceptionObject.h"

#include "vnl/vnl_math.h"
#include "vnl/vnl_cross.h"
#include "vnl/algo/vnl_sparse_symmetric_eigensystem.h"

#include <float.h>  // for DBL_MIN
#include "itkArray.h"


namespace itk
{
/**
 *
 */
template <class TInputMesh, class TOutputMesh>
LaplaceBeltramiFilter< TInputMesh, TOutputMesh >
::LaplaceBeltramiFilter()
{
  /// \todo Instead of calling SetEigenValueCount you can initialize
  /// m_EigenValueCount in the initialization part of the constructor:
  /// LaplaceBeltramiFilter() : m_EigenValueCount( 0 ) {}
  SetEigenValueCount(0);
}

/**
 * Get the Laplace Beltrami operator
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter<TInputMesh, TOutputMesh>
::GetLBOperator( LBMatrixType& lbOp ) const
{
  lbOp = this->m_LBOperator;
}

/**
 * Get the areas for each vertex
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter<TInputMesh, TOutputMesh>
::GetVertexAreas( LBMatrixType& lbVa ) const
{
  lbVa = this->m_VertexAreas;
}

/**
 * Get the surface harmonics at each vertex
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter<TInputMesh, TOutputMesh>
::GetHarmonics( HarmonicSetType& harms ) const
{
  harms = this->m_Harmonics;
}
/**
 *
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Harmonic " << this->m_Harmonics << std::endl;
  os << indent << "Eigen Value Count " << this->m_EigenValueCount << std::endl;
  os << indent << "Vertex Areas dimensions " << this->m_VertexAreas.rows() << " x "
     << this->m_VertexAreas.rows() << std::endl;
  os << indent << "Laplace Beltrami Operator dimensions "
     << this->m_LBOperator.rows() << " x "
     << this->m_LBOperator.rows() << std::endl;

}

/**
 *
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh >
::SetInput(TInputMesh *input)
{
  this->ProcessObject::SetNthInput(0, input);
}

/**
 * This method causes the filter to generate its output.
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh >
::GenerateData(void)
{
  typedef typename TInputMesh::PointsContainer  InputPointsContainer;
  typedef typename TOutputMesh::PointsContainer OutputPointsContainer;

  typedef typename TInputMesh::PointsContainerConstPointer
    InputPointsContainerConstPointer;
  typedef typename TOutputMesh::PointsContainerPointer
    OutputPointsContainerPointer;

  this->CopyInputMeshToOutputMesh();

  InputMeshConstPointer  inputMesh      =  this->GetInput();
  OutputMeshPointer      outputMesh     =  this->GetOutput();

  unsigned int dim = 3;
  unsigned int sides = 3;

  InputPointsContainerConstPointer  inPoints  = inputMesh->GetPoints();

  unsigned int cellCount = inputMesh->GetNumberOfCells();
  unsigned int vertexCount = inputMesh->GetNumberOfPoints();

  itk::Array<double> faceAreas(cellCount);
  itk::Array<double> vertexAreas(vertexCount);
  itk::Array<unsigned int> vertexCounts(vertexCount);

  faceAreas.Fill(0.0);
  vertexAreas.Fill(0.0);
  vertexCounts.Fill(0);

  InputCellAutoPointer cellPtr;

  /// \todo Iterate on all vertex of the mesh. Your code is going to be
  /// really simplified and shorter. Then later, you can consider other
  /// existing functionalities like area for triangle, edge weights...
  //InputPointsContainerConstIterator pit = inPoints->Begin();
  //
  //while( pit != inPoints->End() )
  //  {
  //  // First get one edge of the neighborhood
  //  InputQEType* qe = pit->Value()->GetEdge();
  //
  //  // Then iterate on the 0-ring 
  //  InputQEType* temp = qe;
  //  do
  //    {
  //    // here you can easily compute the area and all matrix
  //    // elements you need.
  //  
  //    // Go to the next quad edge in the 0-ring of the current vertex
  //    temp = temp->GetONext();
  //    } while( temp != qe );
  //  ++pit;
  //  }

  for (unsigned int cellId = 0; cellId < cellCount; cellId++)
    {
    inputMesh->GetCell( cellId, cellPtr );
    unsigned int aCellNumberOfPoints = cellPtr->GetNumberOfPoints();
    if( aCellNumberOfPoints != dim )
      {
      itkExceptionMacro("cell has " << aCellNumberOfPoints << " points\n"
      "This filter can only process triangle meshes.");
      return;
      }

    const unsigned long *tp;
    tp = cellPtr->GetPointIds();

    InputPointType v0;
    InputPointType v1;
    InputPointType v2;
    inputMesh->GetPoint((int)(tp[0]), &v0);
    inputMesh->GetPoint((int)(tp[1]), &v1);
    inputMesh->GetPoint((int)(tp[2]), &v2);

    faceAreas(cellId) = vcl_abs(vnl_cross_3d(
        (v1-v0).GetVnlVector(), (v2-v0).GetVnlVector()).two_norm()/2.0);

    for (unsigned int vertexIx = 0; vertexIx < 3; vertexIx++)
      {
      vertexAreas(tp[vertexIx]) += faceAreas(cellId);
      vertexCounts(tp[vertexIx])++;
      }
    }

  for (unsigned int vertexId = 0; vertexId < vertexCount; vertexId++)
    {
    vertexAreas(vertexId) /= vertexCounts(vertexId);
    }

  m_LBOperator.set_size(vertexCount, vertexCount);
  unsigned int ix[sides];
  for (unsigned int i = 0; i < sides; i++)
    {
    ix[i] = i;
    }

  for (unsigned int faceIx = 0; faceIx < cellCount; faceIx++)
    {
    for (unsigned int l = 0; l < sides; l++)
      {
      inputMesh->GetCell( faceIx, cellPtr );
      const unsigned long *tp;
      tp = cellPtr->GetPointIds();

      // Beginning of replacement for code commented out above...  Check with Michael...
      typedef typename InputPointType::VectorType   InputVectorType;

      InputPointType p0;
      InputPointType p1;
      InputPointType p2;

      inputMesh->GetPoint((int)(tp[ix[0]]), &p0);
      inputMesh->GetPoint((int)(tp[ix[1]]), &p1);
      inputMesh->GetPoint((int)(tp[ix[2]]), &p2);

      const InputVectorType e0 = p2 - p1;
      const InputVectorType e1 = p0 - p2;
      const InputVectorType e2 = p1 - p0;

      const double c0 = e0.GetSquaredNorm();
      const double c1 = e0 * e1;
      const double c2 = e0 * e2;

      // End of replacement for code commented out above...  Check with Michael...

      const double faceArea = faceAreas(faceIx);

      m_LBOperator(tp[ix[0]], tp[ix[1]]) -= c1 / faceArea;
      m_LBOperator(tp[ix[0]], tp[ix[2]]) -= c2 / faceArea;
      m_LBOperator(tp[ix[0]], tp[ix[0]]) -= c0 / faceArea;

      unsigned int last = ix[sides - 1];
      for(unsigned int i = sides - 1; i > 0; i--)
        {
        ix[i] = ix[i - 1];
        }
      ix[0] = last;

      }
    }

  //
  // saves area as a sparse diagonal matrix
  //
  m_VertexAreas.set_size(vertexCount, vertexCount);
  for (unsigned int k = 0; k < vertexCount; k++)
    {
    m_VertexAreas(k, k) = vertexAreas(k);
    }

  // compute harmonics?
  if (this->m_EigenValueCount)
    {

    // Need to solve the following eigensystem:
    // A * V = B * V * D, or
    // m_LBOperator * eVectors = m_VertexAreas * eVectors * eValues
    // the vnl_sparse_symmetric_eigensystem class works on the following system:
    // A * x = lambda * x, where x is an eigenvector and lambda an eigenvalue...
    // so, to use this class:
    //   (1) create a new sparse matrix C by dividing each nonzero coefficient
    //       of A, say A[k][l], by vcl_sqrt(B[k][k]*B[l][l]);
    //   (2) Solve the eigenvalue problem C*x = lambda*x for the first N
    //       eigenvectors; then A and C have the same eigenvalues.
    //   (3) let W[k][l] be the lth coordinate of the kth eigenvector of
    //       C (I am not sure of what the correct order of indices is, you may
    //       need to switch them). Define V[k][l] = W[k][l]/vcl_sqrt(B[l][l])
    //   (4) Divide all coefficients V[k][l] by vcl_sqrt(sum_l V[k][l]^2 B[l][l])
    //       for normalization.

    vnl_sparse_matrix< double > C(m_LBOperator.rows(), m_LBOperator.cols());
    
    for(unsigned int rowIx = 0; rowIx < C.rows(); rowIx++)
      {
      for(unsigned int colIx = 0; colIx < C.cols(); colIx++)
        {
        if (m_LBOperator(rowIx, colIx))
          {
          C(rowIx, colIx) = m_LBOperator(rowIx, colIx) /
            (vcl_sqrt(m_VertexAreas(rowIx, rowIx) * m_VertexAreas(colIx, colIx)));
          }
        }
      }

    // get the eigenvalues for the sparse symmetric matrix
    vnl_sparse_symmetric_eigensystem sse;
    sse.CalculateNPairs(C, this->m_EigenValueCount, false);

    // get the output
    this->m_Harmonics.set_size(this->m_EigenValueCount, vertexCount);

    for (unsigned int k = 0; k < this->m_EigenValueCount; k++)
      {
      vnl_vector< double > eigenvector(sse.get_eigenvector(k));

      double evFactorSum = 0.0;
      for (unsigned int eigIx = 0; eigIx < eigenvector.size(); eigIx++)
        {
        eigenvector(eigIx) = eigenvector(eigIx) / vcl_sqrt(m_VertexAreas(eigIx, eigIx));
        evFactorSum += eigenvector(eigIx) * eigenvector(eigIx) * m_VertexAreas(eigIx, eigIx);
        }

      eigenvector /= vcl_sqrt(evFactorSum);
      this->m_Harmonics.set_row(k, eigenvector);
      }
    this->SetSurfaceHarmonic(0);
    }
  else
    {
    this->m_Harmonics.clear();
    }
}

/**
 * Get a single surface harmonic
 */
template <class TInputMesh, class TOutputMesh>
bool
LaplaceBeltramiFilter<TInputMesh, TOutputMesh>
::SetSurfaceHarmonic( unsigned int harmonic )
{
  if ( harmonic >= this->m_Harmonics.rows() )
    {
    return false;  // FIXME: Why to fail silently ? shouldn't this throw an exception ?
    }

  OutputMeshPointer surface = this->GetOutput();

  for (unsigned int k = 0; k < this->m_Harmonics.cols(); k++)
    {
    surface->SetPointData(k, this->m_Harmonics(harmonic, k));
    }

  this->Modified();
      
  return true;
}

} // end namespace itk

#endif
