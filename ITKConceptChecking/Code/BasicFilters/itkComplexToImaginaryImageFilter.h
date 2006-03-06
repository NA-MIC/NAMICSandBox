/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkComplexToImaginaryImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/01/23 17:55:47 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkComplexToImaginaryImageFilter_h
#define __itkComplexToImaginaryImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "vnl/vnl_math.h"

namespace itk
{
  
/** \class ComplexToImaginaryImageFilter
 * \brief Computes pixel-wise the imaginary part of a complex image.
 * 
 * \ingroup IntensityImageFilters  Multithreaded
 */
namespace Function {  
  
template< class TInput, class TOutput>
class ComplexToImaginary
{
public:
  ComplexToImaginary() {}
  ~ComplexToImaginary() {}
  bool operator!=( const ComplexToImaginary & ) const
  {
    return false;
  }
  bool operator==( const ComplexToImaginary & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A )
  { return (TOutput)( A.imag() ); }
}; 
}

template <class TInputImage, class TOutputImage>
class ITK_EXPORT ComplexToImaginaryImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                        Function::ComplexToImaginary< 
  typename TInputImage::PixelType, 
  typename TOutputImage::PixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef ComplexToImaginaryImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                                  Function::ComplexToImaginary< typename TInputImage::PixelType, 
                                                 typename TOutputImage::PixelType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(InputConvertibleToOutputCheck,
    (Concept::Convertible<typename TInputImage::PixelType::value_type,
                          typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  ComplexToImaginaryImageFilter() {}
  virtual ~ComplexToImaginaryImageFilter() {}

private:
  ComplexToImaginaryImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
