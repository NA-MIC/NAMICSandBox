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

#ifndef __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_txx
#define __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_txx

#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.h"

namespace itk
{

template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >
::QuadEdgeMeshSphericalDiffeomorphicDemonsFilter()
{
  this->SetNumberOfRequiredInputs( 2 );
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfOutputs( 1 );
  this->SetNthOutput( 0, OutputMeshType::New() );

  this->m_BasisSystemAtNode = BasisSystemContainerType::New();
}

template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >
::~QuadEdgeMeshSphericalDiffeomorphicDemonsFilter()
{
}

template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void 
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >
::SetFixedMesh( const FixedMeshType * fixedMesh )
{
  itkDebugMacro("setting Fixed Mesh to " << fixedMesh ); 

  if (this->m_FixedMesh.GetPointer() != fixedMesh ) 
    { 
    this->m_FixedMesh = fixedMesh;

    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(0, 
                                   const_cast< FixedMeshType *>( fixedMesh ) );
    
    this->Modified(); 
    } 
}

template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void 
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >
::SetMovingMesh( const MovingMeshType * movingMesh )
{
  itkDebugMacro("setting Moving Mesh to " << movingMesh ); 

  if (this->m_MovingMesh.GetPointer() != movingMesh ) 
    { 
    this->m_MovingMesh = movingMesh;

    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(1, 
                                   const_cast< MovingMeshType *>( movingMesh ) );
    
    this->Modified(); 
    } 
}

template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
GenerateData()
{
  this->AllocateOutputMesh();

  this->ComputeBasisSystemAtEveryNode();

  OutputMeshPointer output = this->GetOutput( 0 );
}


template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
AllocateOutputMesh()
{
  FixedMeshConstPointer in = this->GetInput();
  OutputMeshPointer out = this->GetOutput();
 
  typedef typename FixedMeshType::PointsContainerConstIterator
    InputPointsContainerConstIterator;

  typedef typename OutputMeshType::PointType        OutputPointType;

  typedef typename FixedMeshType::CellsContainerConstIterator
    InputCellsContainerConstIterator;

  typedef typename FixedMeshType::EdgeCellType      InputEdgeCellType;
  typedef typename FixedMeshType::PolygonCellType   InputPolygonCellType;
  typedef typename FixedMeshType::PointIdList       InputPointIdList;

  typedef typename FixedMeshType::CellTraits        InputCellTraits;
  typedef typename InputCellTraits::PointIdInternalIterator
    InputPointsIdInternalIterator;

  // Copy points
  InputPointsContainerConstIterator inIt = in->GetPoints()->Begin();
  while( inIt != in->GetPoints()->End() )
    {
    OutputPointType pOut;
    pOut.CastFrom( inIt.Value() );
    out->SetPoint( inIt.Index(), pOut );
    inIt++;
    } 

  // Copy Edge Cells
  InputCellsContainerConstIterator ecIt = in->GetEdgeCells()->Begin();
  while( ecIt != in->GetEdgeCells()->End() )
    {
    InputEdgeCellType* pe = dynamic_cast< InputEdgeCellType* >( ecIt.Value());
    if( pe )
      {
      out->AddEdgeWithSecurePointList( pe->GetQEGeom()->GetOrigin(),
                                       pe->GetQEGeom()->GetDestination() );
      }
    ecIt++;
    }


  // Copy cells
  InputCellsContainerConstIterator cIt = in->GetCells()->Begin();
  while( cIt != in->GetCells()->End() )
    {
    InputPolygonCellType * pe = dynamic_cast< InputPolygonCellType* >( cIt.Value());
    if( pe )
      {
      InputPointIdList points;
      InputPointsIdInternalIterator pit = pe->InternalPointIdsBegin();
      while( pit != pe->InternalPointIdsEnd( ) )
        {
        points.push_back( ( *pit ) );
        ++pit;
        }
      out->AddFaceWithSecurePointList( points, false );
      }
    cIt++;
    }
}

template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TFixedMesh, TMovingMesh, TOutputMesh >::
ComputeBasisSystemAtEveryNode()
{
  const PointIdentifier numberOfNodes = this->m_FixedMesh->GetNumberOfPoints();

  // create a new container, in case the filter has been run previously
  // with a mesh having a larger number of nodes than the current mesh.
  this->m_BasisSystemAtNode = BasisSystemContainerType::New();

  this->m_BasisSystemAtNode->Reserve( numberOfNodes );

  typedef typename TFixedMesh::PointsContainer    PointsContainer;
  const PointsContainer * points = this->m_FixedMesh->GetPoints();

  typedef typename TFixedMesh::QEPrimal           EdgeType;
  typedef typename TFixedMesh::PointType          PointType;
  typedef typename PointType::VectorType          VectorType;

  for( PointIdentifier pointId1 = 0; pointId1 < numberOfNodes; pointId1++ )
    {
    const EdgeType * edge = this->m_FixedMesh->FindEdge( pointId1 );
    PointIdentifier pointId2 = edge->GetDestination();

    const PointType point1 = points->GetElement( pointId1 );
    const PointType point2 = points->GetElement( pointId2 );

    const VectorType v12    = point1 - point2;

    // v12 is not necessarily tangent to the sphere, therefore we must use
    // cross products in order to find an orthogonal system.

    const VectorType radial = point1.GetVectorFromOrigin();

    VectorType u12 = CrossProduct( v12, radial );
    VectorType w12 = CrossProduct( radial, u12 );

    w12.Normalize();
    u12.Normalize();

    BasisSystemType basis;
    basis.SetVector( 0, w12 );
    basis.SetVector( 1, u12 );

    this->m_BasisSystemAtNode->SetElement( pointId1, basis );
    }
}

}

#endif
