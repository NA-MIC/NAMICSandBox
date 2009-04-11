/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWeightedCovarianceFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:54 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkWeightedCovarianceFilter_h
#define __itkWeightedCovarianceFilter_h

#include "itkFunctionBase.h"
#include "itkCovarianceFilter.h"
#include "itkDataObjectDecorator.h"

namespace itk { 
namespace Statistics {
  
/** \class WeightedCovarianceFilter
 * \brief Calculates the covariance matrix of the target sample data.
 *  where each measurement vector has an associated weight value
 *  
 * Weight values can be specified in two ways: using a weighting function
 * or an array containing weight values. If none of these two is specified, 
 * the covariance matrix is generated with equal weights.  
 *
 * \sa CovarianceFilter
 *
 */

template< class TSample >
class WeightedCovarianceFilter :
    public CovarianceFilter< TSample >
{
public:
  /** Standard class typedefs. */
  typedef WeightedCovarianceFilter        Self;
  typedef CovarianceFilter< TSample >     Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Standard Macros */
  itkTypeMacro(WeightedCovarianceFilter, CovarianceFilter);
  itkNewMacro(Self);
  
  /** Traits derived from the base class */
  itkSuperclassTraitMacro( SampleType )
  itkSuperclassTraitMacro( MeasurementVectorType )
  itkSuperclassTraitMacro( MeasurementVectorSizeType )
  itkSuperclassTraitMacro( MeasurementVectorDecoratedType )
  itkSuperclassTraitMacro( OutputType )
  
  /** Typedef for WeightedCovariance output */
  typedef VariableSizeMatrix< double >               MatrixType;

  /** Weight calculation function typedef */
  typedef FunctionBase< MeasurementVectorType, double > WeightingFunctionType;

  /** VariableSizeMatrix is not a DataObject, we need to decorate it to push it down
   * a ProcessObject's pipeline */
  typedef  SimpleDataObjectDecorator< MatrixType >  MatrixDecoratedType;

  /** Array typedef for weights */
  typedef Array< double > WeightArrayType;

  /** Type of DataObjects to use for the weight array type */
  typedef SimpleDataObjectDecorator< WeightArrayType > InputWeightArrayObjectType;

  /** Method to set the input value of the weight array */
  itkSetDecoratedInputMacro( Weights, WeightArrayType, 1 );

  /** Type of DataObjects to use for Weight function */
  typedef DataObjectDecorator< WeightingFunctionType > InputWeightingFunctionObjectType;

  /** Method to set the weighting function */
  itkSetDecoratedObjectInputMacro( WeightingFunction, WeightingFunctionType, 2 );

protected:
  WeightedCovarianceFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  WeightedCovarianceFilter();
  virtual ~WeightedCovarianceFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateData();

  /** Compute covariance matrix with weights computed from a function */
  void ComputeCovarianceMatrixWithWeightingFunction();

  /** Compute covariance matrix with weights specified in an array */
  void ComputeCovarianceMatrixWithWeights();

private:
}; // end of class
    
} // end of namespace Statistics 
} // end of namespace itk 

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWeightedCovarianceFilter.txx"
#endif

#endif
