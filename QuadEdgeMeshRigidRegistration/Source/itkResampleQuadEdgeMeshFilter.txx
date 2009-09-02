/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNodeScalarGradientCalculator.h,v $
  Language:  C++
  Date:      $Date: 2008-05-14 09:26:05 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkResampleQuadEdgeMeshFilter_txx
#define __itkResampleQuadEdgeMeshFilter_txx

#include "itkResampleQuadEdgeMeshFilter.h"
#include "itkProgressReporter.h"
#include "itkVersor.h"
#include "itkNumericTraitsVectorPixel.h"

namespace itk
{


template< class TInputMesh, class TOutputMesh >
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::ResampleQuadEdgeMeshFilter()
{
}


template< class TInputMesh, class TOutputMesh >
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::~ResampleQuadEdgeMeshFilter()
{
}


template< class TInputMesh, class TOutputMesh >
void
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::SetReferenceMesh( const TOutputMesh * mesh )
{
  itkDebugMacro("setting input ReferenceMesh to " << mesh);
  if( mesh != static_cast<const TOutputMesh *>(this->ProcessObject::GetInput( 1 )) )
    {
    this->ProcessObject::SetNthInput(1, const_cast< TOutputMesh *>( mesh ) );
    this->Modified();
    }
}


template< class TInputMesh, class TOutputMesh >
const typename ResampleQuadEdgeMeshFilter<TInputMesh,TOutputMesh>::OutputMeshType *
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::GetReferenceMesh() const
{
  Self * surrogate = const_cast< Self * >( this );
  const OutputMeshType * referenceMesh = 
    static_cast<const OutputMeshType *>(surrogate->ProcessObject::GetInput(1));
  return referenceMesh;
}


template< class TInputMesh, class TOutputMesh >
void
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::GenerateData()
{
  // Copy the input mesh into the output mesh.
  this->CopyReferenceMeshToOutputMesh();

  OutputMeshPointer outputMesh = this->GetOutput();

  //
  // Visit all nodes of the Mesh 
  //
  typedef typename OutputPointDataContainer::ConstIterator OutputPointDataIterator;

  OutputPointsContainerPointer points = outputMesh->GetPoints();

  if( points.IsNull() )
    {
    itkExceptionMacro("Mesh has NULL PointData");
    }

  OutputPointDataContainerPointer pointData = outputMesh->GetPointData();

  if( pointData.IsNull() )
    {
    itkExceptionMacro("Output Mesh has NULL PointData");
    }

  const unsigned int numberOfPoints = outputMesh->GetNumberOfPoints();

  ProgressReporter progress(this, 0, numberOfPoints);

  std::cout << "Output Mesh numberOfPoints " << numberOfPoints << std::endl;

  typedef typename OutputMeshType::PointsContainer::ConstIterator    PointIterator;
  typedef typename OutputMeshType::PointDataContainer::Iterator      PointDataIterator;

  PointIterator pointItr = outputMesh->GetPoints()->Begin();
  PointIterator pointEnd = outputMesh->GetPoints()->End();

  PointDataIterator pointDataItr = outputMesh->GetPointData()->Begin();
  PointDataIterator pointDataEnd = outputMesh->GetPointData()->End();

  typedef typename TransformType::InputPointType     InputPointType;
  typedef typename TransformType::OutputPointType    MappedPointType;

  OutputPointType  inputPoint;
  OutputPointType  pointToEvaluate;

  while( pointItr != pointEnd && pointDataItr != pointDataEnd )
    {
    inputPoint.CastFrom( pointItr.Value() );

    MappedPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    pointToEvaluate.CastFrom( transformedPoint );
    pointDataItr.Value() = this->m_Interpolator->Evaluate( pointToEvaluate );

    progress.CompletedPixel();

    ++pointItr;
    ++pointDataItr;
    }

}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMesh()
{
  this->CopyReferenceMeshToOutputMeshGeometry();
  this->CopyReferenceMeshToOutputMeshFieldData();
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshGeometry()
{
  this->CopyReferenceMeshToOutputMeshPoints();
  this->CopyReferenceMeshToOutputMeshEdgeCells();
  this->CopyReferenceMeshToOutputMeshCells();
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshFieldData()
{
  this->CopyReferenceMeshToOutputMeshPointData();
  this->CopyReferenceMeshToOutputMeshCellData();
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshPoints()
{

  OutputMeshConstPointer in = this->GetReferenceMesh();
  OutputMeshPointer out = this->GetOutput();

  // Copy points
  OutputPointsContainerConstPointer inPoints = in->GetPoints();

  if( inPoints )
    {
    OutputPointsContainerConstIterator inIt = inPoints->Begin();
    while( inIt != inPoints->End() )
      {
      OutputPointType pOut;
      pOut.CastFrom( inIt.Value() );
      out->SetPoint( inIt.Index(), pOut );
      inIt++;
      } 
    }
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshEdgeCells()
{
  OutputMeshConstPointer in = this->GetReferenceMesh();
  OutputMeshPointer out = this->GetOutput();

  // Copy Edge Cells
  OutputCellsContainerConstPointer inEdgeCells = in->GetEdgeCells();

  if( inEdgeCells )
    {
    OutputCellsContainerConstIterator ecIt = inEdgeCells->Begin();
    while( ecIt != inEdgeCells->End() )
      {
      OutputEdgeCellType* pe = dynamic_cast< OutputEdgeCellType* >( ecIt.Value());
      if( pe )
        {
        out->AddEdgeWithSecurePointList( pe->GetQEGeom()->GetOrigin(),
                                         pe->GetQEGeom()->GetDestination() );
        }
      ecIt++;
      }
    }
}


// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshCells()
{
  OutputMeshConstPointer in = this->GetReferenceMesh();
  OutputMeshPointer out = this->GetOutput();

  // Copy cells
  OutputCellsContainerConstPointer inCells = in->GetCells();

  if( inCells )
    {
    OutputCellsContainerConstIterator cIt = inCells->Begin();
    while( cIt != inCells->End() )
      {
      OutputPolygonCellType * pe = dynamic_cast< OutputPolygonCellType* >( cIt.Value());
      if( pe )
        {
        OutputPointIdList points;
        OutputPointsIdInternalIterator pit = pe->InternalPointIdsBegin();
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
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshPointData()
{

  OutputMeshConstPointer in = this->GetReferenceMesh();
  OutputMeshPointer out = this->GetOutput();

  typedef typename OutputPointDataContainer::Pointer      OutputPointDataContainerPointer;

  OutputPointDataContainerConstPointer inputPointData = in->GetPointData();

  if( inputPointData.IsNull() )
    {
    // There is nothing to copy
    itkWarningMacro("Reference mesh point data is NULL");
    return;
    }

  OutputPointDataContainerPointer outputPointData = OutputPointDataContainer::New();
  outputPointData->Reserve( inputPointData->Size() );

  // Copy point data
  typedef typename OutputPointDataContainer::ConstIterator  OutputPointDataContainerConstIterator;
  OutputPointDataContainerConstIterator inIt = inputPointData->Begin();
  while( inIt != inputPointData->End() )
    {
    outputPointData->SetElement( inIt.Index(), inIt.Value() );
    inIt++;
    } 

  out->SetPointData( outputPointData );
}


// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
ResampleQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshCellData()
{

  OutputMeshConstPointer in = this->GetReferenceMesh();
  OutputMeshPointer out = this->GetOutput();

  typedef typename OutputCellDataContainer::ConstPointer  OutputCellDataContainerConstPointer;
  typedef typename OutputCellDataContainer::Pointer      OutputCellDataContainerPointer;

  OutputCellDataContainerConstPointer inputCellData = in->GetCellData();

  if( inputCellData.IsNull() )
    {
    // There is nothing to copy
    return;
    }

  OutputCellDataContainerPointer outputCellData = OutputCellDataContainer::New();
  outputCellData->Reserve( inputCellData->Size() );

  // Copy point data
  typedef typename OutputCellDataContainer::ConstIterator  OutputCellDataContainerConstIterator;
  OutputCellDataContainerConstIterator inIt = inputCellData->Begin();
  while( inIt != inputCellData->End() )
    {
    outputCellData->SetElement( inIt.Index(), inIt.Value() );
    inIt++;
    } 

  out->SetCellData( outputCellData );
}
} // end namespace itk

#endif
