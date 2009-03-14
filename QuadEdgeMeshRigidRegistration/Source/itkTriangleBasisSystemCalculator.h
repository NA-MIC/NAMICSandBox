/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTriangleBasisSystemCalculator.h,v $
  Language:  C++
  Date:      $Date: 2008-10-17 13:35:26 $
  Version:   $Revision: 1.47 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTriangleBasisSystemCalculator_h
#define __itkTriangleBasisSystemCalculator_h

#include "itkMesh.h"
#include "itkPoint.h"

namespace itk
{


/** \class TriangleBasisSystemCalculator
 * \brief  Computes basis coefficients at a triangular cell.
 *
 * TriangleBasisSystemCalculator computes basis coefficients
 * within a triangle. Basis coefficients can be used thereafter
 * for interpolation and gradient computation within that triangle. 
 *
 * This class is templated over the input vector type and dimension of basis.
 *
 * \sa TriangleBasisSystem
 *
 */
template < class TMesh, class TBasisSystem >
class ITK_EXPORT TriangleBasisSystemCalculator : public Object
{
public:
  /** Standard class typedefs. */
  typedef TriangleBasisSystemCalculator             Self;
  typedef Object                                    Superclass;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Standard part of every itk Object. */
  itkTypeMacro(TriangleBasisSystemCalculator, Object);

  typedef TBasisSystem                            BasisSystemType;
  typedef TMesh                                   MeshType;
  typedef typename MeshType::ConstPointer         MeshConstPointer;
    
  itkSetConstObjectMacro( InputMesh, MeshType );
  itkGetConstObjectMacro( InputMesh, MeshType );

  /** Compute the basis system at the triangular cell of the Mesh that is
   * identified by cellIndex. */
  void CalculateTriangle( unsigned int cellIndex, TBasisSystem & bs ) const; 

protected:
  TriangleBasisSystemCalculator();
  virtual ~TriangleBasisSystemCalculator();

private:
  MeshConstPointer     m_InputMesh;   

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTriangleBasisSystemCalculator.txx"
#endif

#endif
