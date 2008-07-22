/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStatisticsPrintTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/24 15:16:21 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkFixedArray.h"
#include "itkImage.h"
#include "itkPointSet.h"

// #include "itkSample.h"   // abstract class
// #include "itkSampleToSubsampleFilter.h"   // abstract class
#include "itkHistogram.h"
#include "itkListSample.h"
#include "itkSubsample.h"
#include "itkSampleClassifierFilter.h"
#include "itkSampleToHistogramFilter.h"
#include "itkMembershipSample.h"
#include "itkNeighborhoodSampler.h"
#include "itkImageToListSampleFilter.h"
#include "itkScalarImageToCooccurrenceMatrixFilter.h"
#include "itkScalarImageToCooccurrenceListSampleFilter.h"
#include "itkHistogramToTextureFeaturesFilter.h"
#include "itkScalarImageToTextureFeaturesFilter.h"
#include "itkMeanFilter.h"
#include "itkWeightedMeanFilter.h"
#include "itkCovarianceFilter.h"
#include "itkWeightedCovarianceFilter.h"
#include "itkImageToListSampleAdaptor.h"
#include "itkPointSetToListSampleAdaptor.h"
#include "itkJointDomainImageToListSampleAdaptor.h"
#include "itkDenseFrequencyContainer2.h"
#include "itkSparseFrequencyContainer2.h"
#include "itkMembershipFunctionBase.h"
#include "itkDistanceMetric.h"
#include "itkEuclideanDistance.h"

int itkStatisticsPrintTest(int , char* [])
{
  typedef float TMeasurementType;
  typedef float FrequencyType;
  typedef itk::FixedArray< TMeasurementType, 2 >  TMeasurementVectorType;
  typedef itk::Image< TMeasurementVectorType, 3 > ImageType;
  typedef itk::Image< unsigned char, 3>           ScalarImageType;
  typedef itk::PointSet< TMeasurementType, 2 >    PointSetType;

  typedef itk::Statistics::ListSample< TMeasurementVectorType >
    SampleType;

  typedef itk::Statistics::Subsample< SampleType >
    SubSampleType;

  typedef itk::Statistics::Histogram< TMeasurementType, 2 > HistogramType ;

  typedef itk::Statistics::SampleToHistogramFilter< 
    SampleType, HistogramType > SampleToHistogramFilterType;

  typedef itk::Statistics::SampleClassifierFilter< 
    SampleType > SampleClassifierFilterType;

  typedef itk::Statistics::ImageToListSampleFilter< 
    ImageType, ImageType > ImageToListSampleFilterType;

  typedef itk::Statistics::ImageToListSampleAdaptor< 
    ImageType> ImageToListSampleAdaptorType;

  typedef itk::Statistics::JointDomainImageToListSampleAdaptor< 
    ImageType> JointDomainImageToListSampleAdaptorType;

  typedef itk::Statistics::ScalarImageToCooccurrenceMatrixFilter< 
    ScalarImageType > ScalarImageToCooccurrenceMatrixFilterType;

  typedef itk::Statistics::ScalarImageToCooccurrenceListSampleFilter< 
    ScalarImageType > ScalarImageToCooccurrenceListSampleFilterType;

  typedef itk::Statistics::ScalarImageToTextureFeaturesFilter< 
    ScalarImageType > ScalarImageToTextureFeaturesFilterType;

  typedef itk::Statistics::MembershipSample< SampleType >
    MembershipSampleType;

  typedef itk::Statistics::MembershipFunctionBase< TMeasurementVectorType >
    MembershipFunctionBaseType;
 
  typedef itk::Statistics::DistanceMetric< TMeasurementVectorType >
    DistanceType;

  typedef itk::Statistics::EuclideanDistance< TMeasurementVectorType >
    EuclideanDistanceType;


  typedef itk::Statistics::HistogramToTextureFeaturesFilter<
    HistogramType > HistogramToTextureFeaturesFilterType;

  typedef itk::Statistics::MeanFilter< SampleType > MeanFilterType;
  typedef itk::Statistics::WeightedMeanFilter< SampleType > WeightedMeanFilterType;

  typedef itk::Statistics::CovarianceFilter< SampleType > CovarianceFilterType;
  typedef itk::Statistics::WeightedCovarianceFilter< SampleType > WeightedCovarianceFilterType;

  typedef itk::Statistics::NeighborhoodSampler< SampleType > NeighborhoodSamplerType;

  typedef itk::Statistics::PointSetToListSampleAdaptor< PointSetType > PointSetToListSampleAdaptorType;

  typedef itk::Statistics::DenseFrequencyContainer2 DenseFrequencyContainer2Type;
  typedef itk::Statistics::SparseFrequencyContainer2 SparseFrequencyContainer2Type;
  
 
  SampleType::Pointer sampleObj = SampleType::New();
  std::cout << "----------ListSample " << sampleObj;

  SubSampleType::Pointer subsampleObj = SubSampleType::New();
  std::cout << "----------Subsample " << subsampleObj;

  HistogramType::Pointer HistogramObj=
    HistogramType::New();
  std::cout << "----------Histogram " << HistogramObj;

  SampleToHistogramFilterType::Pointer SampleToHistogramFilterObj =
    SampleToHistogramFilterType::New();
  std::cout << "----------SampleToHistogramFilter ";
  std::cout << SampleToHistogramFilterObj;

  SampleClassifierFilterType::Pointer SampleClassifierFilterObj =
    SampleClassifierFilterType::New();
  std::cout << "----------SampleClassifierFilter ";
  std::cout << SampleClassifierFilterObj;

  ImageToListSampleFilterType::Pointer ImageToListSampleFilterObj =
    ImageToListSampleFilterType::New();
  std::cout << "----------ImageToListSampleFilter ";
  std::cout << ImageToListSampleFilterObj;

  ImageToListSampleAdaptorType::Pointer ImageToListSampleAdaptorObj =
    ImageToListSampleAdaptorType::New();
  std::cout << "----------ImageToListSampleAdaptor ";
  std::cout << ImageToListSampleAdaptorObj;

  JointDomainImageToListSampleAdaptorType::Pointer JointDomainImageToListSampleAdaptorObj =
    JointDomainImageToListSampleAdaptorType::New();
  std::cout << "----------JointDomainImageToListSampleAdaptor ";
  std::cout << JointDomainImageToListSampleAdaptorObj;

  PointSetToListSampleAdaptorType::Pointer PointSetToListSampleAdaptorObj =
    PointSetToListSampleAdaptorType::New();
  std::cout << "----------PointSetToListSampleAdaptor ";
  std::cout << PointSetToListSampleAdaptorObj;

  ScalarImageToCooccurrenceMatrixFilterType::Pointer ScalarImageToCooccurrenceMatrixFilterObj =
    ScalarImageToCooccurrenceMatrixFilterType::New();
  std::cout << "----------ScalarImageToCooccurrenceMatrixFilter ";
  std::cout << ScalarImageToCooccurrenceMatrixFilterObj;

  ScalarImageToCooccurrenceListSampleFilterType::Pointer ScalarImageToCooccurrenceListSampleFilterObj =
    ScalarImageToCooccurrenceListSampleFilterType::New();
  std::cout << "----------ScalarImageToCooccurrenceListSampleFilter ";
  std::cout << ScalarImageToCooccurrenceListSampleFilterObj;

  ScalarImageToTextureFeaturesFilterType::Pointer ScalarImageToTextureFeaturesFilterObj =
    ScalarImageToTextureFeaturesFilterType::New();
  std::cout << "----------ScalarImageToCooccurrenceMatrixFilter ";
  std::cout << ScalarImageToCooccurrenceMatrixFilterObj;


  HistogramToTextureFeaturesFilterType::Pointer HistogramToTextureFeaturesFilterObj=
    HistogramToTextureFeaturesFilterType::New();
  std::cout << "----------HistogramToTextureFeaturesFilter " << HistogramToTextureFeaturesFilterObj;

  MembershipSampleType::Pointer MembershipSampleObj = 
    MembershipSampleType::New();
  std::cout << "----------MembershipSample " << MembershipSampleObj;

  MeanFilterType::Pointer meanFilterObj = 
    MeanFilterType::New();
  std::cout << "----------Mean filter " << meanFilterObj;

  WeightedMeanFilterType::Pointer weighedMeanFilterObj = 
    WeightedMeanFilterType::New();
  std::cout << "----------WeightedMean filter " << weighedMeanFilterObj;

  CovarianceFilterType::Pointer covarianceFilterObj = 
    CovarianceFilterType::New();
  std::cout << "----------Covariance filter " << covarianceFilterObj;

  WeightedCovarianceFilterType::Pointer weighedCovarianceFilterObj = 
    WeightedCovarianceFilterType::New();
  std::cout << "----------WeightedCovariance filter " << weighedCovarianceFilterObj;

  NeighborhoodSamplerType::Pointer neighborhoodSamplerObj = 
    NeighborhoodSamplerType::New();
  std::cout << "----------NeighborhoodSamplerType filter " << neighborhoodSamplerObj;

  DenseFrequencyContainer2Type::Pointer DenseFrequencyContainer2Obj=
    DenseFrequencyContainer2Type::New();
  std::cout << "----------DenseFrequencyContainer " << DenseFrequencyContainer2Obj;

  SparseFrequencyContainer2Type::Pointer SparseFrequencyContainer2Obj=
    SparseFrequencyContainer2Type::New();
  std::cout << "----------SparseFrequencyContainer2 " << SparseFrequencyContainer2Obj;

  EuclideanDistanceType::Pointer euclideanDistance=
    EuclideanDistanceType::New();
  std::cout << "----------EuclideanDistanceType " << euclideanDistance ;
 
  return EXIT_SUCCESS;
}
