/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKJetRecursiveGaussianImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/05/06 15:20:16 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkKJetRecursiveGaussianImageFilter_h
#define __itkKJetRecursiveGaussianImageFilter_h

#include "itkRecursiveGaussianImageFilter.h"
#include "itkNthElementImageAdaptor.h"
#include "itkImage.h"
#include "itkVector.h"
#include "itkPixelTraits.h"
#include "itkCommand.h"
#include "itkProgressAccumulator.h"


namespace itk
{

/** \class KJetRecursiveGaussianImageFilter
 * \brief Computes the KJet of an image by convolution
 *        with the First, Second and Cross derivatives of a Gaussian.
 * Sofar it is only implemented to work for a k=2 Jet. Additionally it 
 * might not work for 2D.
 * The convention used for the 2Jet is the following order:
 * Ix, Iy, Iz, Ixx, Ixy, Ixz, Iyy, Iyz, Izz, i.e there are 9 elements, 
 * and the original image values I are not included.
 * 
 * Must also fix the Progress report, does not include the first derivative copying.
 *
 * This filter is implemented using the recursive gaussian
 * filters
 *
 * 
 * \ingroup GradientFilters   
 * \ingroup Singlethreaded
 */
// NOTE that the ITK_TYPENAME macro has to be used here in lieu 
// of "typename" because VC++ doesn't like the typename keyword 
// on the defaults of template parameters
template <typename TInputImage, 
          typename TOutputImage= Image< Vector< 
  ITK_TYPENAME NumericTraits< ITK_TYPENAME TInputImage::PixelType>::RealType,
  ((::itk::GetImageDimension<TInputImage>::ImageDimension+1)*::itk::GetImageDimension<TInputImage>::ImageDimension)/2 + ::itk::GetImageDimension<TInputImage>::ImageDimension >,
                                        ::itk::GetImageDimension<TInputImage>::ImageDimension > >
class ITK_EXPORT KJetRecursiveGaussianImageFilter:
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef KJetRecursiveGaussianImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>        ConstPointer;
  
  
  /** Pixel Type of the input image */
  typedef TInputImage                                    InputImageType;
  typedef typename TInputImage::PixelType                PixelType;
  typedef typename NumericTraits<PixelType>::RealType    RealType;


  /** Image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      ::itk::GetImageDimension<TInputImage>::ImageDimension);

  /** Number of smoothing filters. */
  itkStaticConstMacro(NumberOfSmoothingFilters, unsigned int,
                      ::itk::GetImageDimension<TInputImage>::ImageDimension-2);

  /** Define the image type for internal computations 
      RealType is usually 'double' in NumericTraits. 
      Here we prefer float in order to save memory.  */

  typedef float                                            InternalRealType;
  typedef Image<InternalRealType, 
                ::itk::GetImageDimension<TInputImage>::ImageDimension >   RealImageType;




  /**  Output Image Nth Element Adaptor
   *  This adaptor allows to use conventional scalar 
   *  smoothing filters to compute each one of the 
   *  components of the gradient image pixels. */
  typedef NthElementImageAdaptor< TOutputImage,
                                  InternalRealType >  OutputImageAdaptorType;
  typedef typename OutputImageAdaptorType::Pointer OutputImageAdaptorPointer;

  /**  Smoothing filter type */
  typedef RecursiveGaussianImageFilter<
                                RealImageType,
                                RealImageType
                                        >    GaussianFilterType;

  /**  Derivative filter type, it will be the first in the pipeline  */
  typedef RecursiveGaussianImageFilter<
                                InputImageType,
                                RealImageType
                                        >    DerivativeFilterAType;

  typedef RecursiveGaussianImageFilter<
                                RealImageType,
                                RealImageType
                                        >    DerivativeFilterBType;

  /**  Pointer to a gaussian filter.  */
  typedef typename GaussianFilterType::Pointer    GaussianFilterPointer;
  typedef std::vector< GaussianFilterPointer >    GaussianFiltersArray;

  /**  Pointer to a derivative filter.  */
  typedef typename DerivativeFilterAType::Pointer  DerivativeFilterAPointer;
  typedef typename DerivativeFilterBType::Pointer  DerivativeFilterBPointer;

  /**  Pointer to the Output Image */
  typedef typename TOutputImage::Pointer          OutputImagePointer;                                  


  /** Type of the output Image */
  typedef TOutputImage      OutputImageType;
  typedef typename          OutputImageType::PixelType      OutputPixelType;
  typedef typename PixelTraits<OutputPixelType>::ValueType  OutputComponentType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Set Sigma value. Sigma is measured in the units of image spacing.  */
  void SetSigma( RealType sigma );

  /** Define which normalization factor will be used for the Gaussian */
  void SetNormalizeAcrossScale( bool normalizeInScaleSpace );
  itkGetMacro( NormalizeAcrossScale, bool );

  /** KJetRecursiveGaussianImageFilter needs all of the input to produce an
   * output. Therefore, KJetRecursiveGaussianImageFilter needs to provide
   * an implementation for GenerateInputRequestedRegion in order to inform
   * the pipeline execution model.
   * \sa ImageToImageFilter::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion() throw(InvalidRequestedRegionError);

protected:
  
  KJetRecursiveGaussianImageFilter();
  virtual ~KJetRecursiveGaussianImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Generate Data */
  void GenerateData( void );

  // Override since the filter produces the entire dataset
  void EnlargeOutputRequestedRegion(DataObject *output);

  
private:

  KJetRecursiveGaussianImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  GaussianFiltersArray          m_SmoothingFilters;
  DerivativeFilterAPointer      m_DerivativeFilterA;
  DerivativeFilterBPointer      m_DerivativeFilterB;
  OutputImageAdaptorPointer     m_ImageAdaptor;

  /** Normalize the image across scale space */
  bool m_NormalizeAcrossScale; 

  ProgressAccumulator::Pointer  m_Progress;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkKJetRecursiveGaussianImageFilter.txx"
#endif

#endif




