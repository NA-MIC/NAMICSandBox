/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ITKHeader.h,v $
  Language:  C++
  Date:      $Date: 2006-04-25 23:20:16 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_h
#define __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"
#include "itkNodeScalarGradientCalculator.h"
#include "itkTriangleListBasisSystemCalculator.h"
#include "itkTriangleBasisSystem.h"
#include "itkVectorContainer.h"
#include "itkVector.h"


namespace itk
{
template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
class QuadEdgeMeshSphericalDiffeomorphicDemonsFilter : 
    public QuadEdgeMeshToQuadEdgeMeshFilter< TFixedMesh, TOutputMesh >
{
public:
  typedef QuadEdgeMeshSphericalDiffeomorphicDemonsFilter                Self;
  typedef SmartPointer< Self >                                          Pointer;
  typedef SmartPointer< const Self >                                    ConstPointer;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TFixedMesh, TOutputMesh >   Superclass;
  
  /** Method that instantiates a new object */
  itkNewMacro( Self );

  /** Method that provides the name of the class as a string as well as the
   * name of the parent class. */
  itkTypeMacro( QuadEdgeMeshSphericalDiffeomorphicDemonsFilter, QuadEdgeMeshToQuadEdgeMeshFilter );
  
  /** Input types. */
  typedef TFixedMesh                                FixedMeshType;
  typedef TMovingMesh                               MovingMeshType;
  typedef typename  FixedMeshType::ConstPointer     FixedMeshConstPointer;
  typedef typename  FixedMeshType::PointType        PointType;
  typedef typename  MovingMeshType::ConstPointer    MovingMeshConstPointer;

  /** Output types. */
  typedef TOutputMesh                               OutputMeshType;
  typedef typename  OutputMeshType::Pointer         OutputMeshPointer;
 
  /** Declaration of internal types, some of which are exposed for monitoring purposes */
  typedef typename PointType::VectorType                        VectorType;
  typedef TriangleBasisSystem< VectorType, 2 >                  BasisSystemType;
  typedef typename FixedMeshType::PointIdentifier               PointIdentifier;
  typedef VectorContainer< PointIdentifier, BasisSystemType >   BasisSystemContainerType;
  typedef typename BasisSystemContainerType::Pointer            BasisSystemContainerPointer;
  typedef VectorContainer< PointIdentifier, PointType >         DestinationPointContainerType;
  typedef typename DestinationPointContainerType::Pointer       DestinationPointContainerPointer;

  /** Set/Get the Fixed mesh. */
  void SetFixedMesh( const FixedMeshType * fixedMesh );
  itkGetConstObjectMacro( FixedMesh, FixedMeshType ); 

  /** Set/Get the Moving mesh. */
  void SetMovingMesh( const MovingMeshType * movingMesh );
  itkGetConstObjectMacro( MovingMesh, MovingMeshType );

  /** Returns the array of local coordinates systems at every node of the fixed
   * mesh. This array is only valid after a call to Update() has completed
   * successfully. */
  itkGetConstObjectMacro( BasisSystemAtNode, BasisSystemContainerType );

  /** Returns the array of destination points resulting from applying the
   * deformation field to all nodes of the Fixed Mesh. The content of this array
   * is only valid after the first iteration of the filter execution has been
   * completed. It can be used for tracking the progress of the filter. */
  itkGetConstObjectMacro( DestinationPoints, DestinationPointContainerType );

  /** Set/Get the maximum number of iterations that the filter will be
   * allowed to run.  The default is set to 50. */
  itkSetMacro( MaximumNumberOfIterations, unsigned int );
  itkGetMacro( MaximumNumberOfIterations, unsigned int );

protected:
  QuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  ~QuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  
  virtual void GenerateData( );
  
private:
  QuadEdgeMeshSphericalDiffeomorphicDemonsFilter( const Self& );
  void operator = ( const Self& );

  void AllocateInternalArrays();
  void ComputeBasisSystemAtEveryNode();
  void ComputeInitialArrayOfDestinationPoints();
  void InitializeInterpolators();

  void RunIterations();
  void ComputeMappedMovingValueAtEveryNode();
  void ComputeGradientsOfMappedMovingValueAtEveryNode();
  void ComputeDeformationFieldUpdate();
  void SmoothDeformationField();

  MovingMeshConstPointer                m_MovingMesh;
  FixedMeshConstPointer                 m_FixedMesh;

  /** This is the Array of "Qn" matrices 
   *  presented in equation 3.14 in the paper. */
  BasisSystemContainerPointer           m_BasisSystemAtNode; 

  /** Array containing the destination coordinates of every node in the Fixed
   * Mesh.  This array represents both the deformation field c(xn) and its
   * smoothed version, the field s(xn) as defined in.  */
  DestinationPointContainerPointer      m_DestinationPoints;

  /** Maximum number of iterations that the filter will be allowed to run. */
  unsigned int                          m_MaximumNumberOfIterations;

  typedef TriangleListBasisSystemCalculator< 
    FixedMeshType, BasisSystemType > TriangleListBasisSystemCalculatorType;

  /** Helper class that will compute basis systems at every triangle of the Fixed Mesh. */
  typename TriangleListBasisSystemCalculatorType::Pointer m_TriangleListBasisSystemCalculator;

  /** Types definitions for the container of values resampled from the Moving
   * mesh into the coordinates of the Fixed mesh nodes. */ 
  typedef typename MovingMeshType::PixelType                    MovingPixelType;
  typedef typename NumericTraits< MovingPixelType >::RealType   MovingPixelRealType;
  typedef VectorContainer< 
    PointIdentifier, MovingPixelRealType >                      ResampledMovingValuesContainerType;

  /** Container that stores values resampled from the Moving mesh field at the
   * coordinates resulting from mapping the fixed mesh nodes through the current
   * deformation field. */
  typename ResampledMovingValuesContainerType::Pointer          m_ResampledMovingValuesContainer;


  /** Interpolator type for bringing scalar values from the Moving Mesh into the Fixed Mesh. */
  typedef LinearInterpolateMeshFunction< MovingMeshType >       InterpolatorType;

  /** Interpolator object that will bring scalar valurs from the Moving Mesh into the Fixed Mesh. */
  typename InterpolatorType::Pointer                            m_ScalarInterpolator; 

  typedef NodeScalarGradientCalculator< 
    FixedMeshType, ResampledMovingValuesContainerType >         NodeScalarGradientCalculatorType;

  /** Helper class that will compute the gradient of resampled Moving mesh
   * values at every node of the Fixed mesh with respect to the coordinate system
   * of that node in the fixed mesh. */
  typename NodeScalarGradientCalculatorType::Pointer            m_NodeScalarGradientCalculator; 
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.txx"
#endif

#endif
