/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAnalyzeObjectImageIO.h,v $
  Language:  C++
  Date:      $Date: 2007/03/22 14:28:47 $
  Version:   $1.0$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAnalyzeObjectImageIO_h
#define __itkAnalyzeObjectImageIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <fstream>
#include "itkImageIOBase.h"

namespace itk
{

/** \class AnalyzeObjectImageIO 
 *
 *  \brief Read Analyze Object file format. 
 *
 *  \author
 *  
 *  Credit
 *
 *  \ingroup IOFilters
 *
 */
class ITK_EXPORT AnalyzeObjectImageIO : public ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef AnalyzeObjectImageIO    Self;
  typedef ImageIOBase             Superclass;
  typedef SmartPointer<Self>      Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(AnalyzeObjectImageIO, ImageIOBase);

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanReadFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation();
  
  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void* buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine the file type. Returns true if this ImageIO can write the
   * file specified. */
  virtual bool CanWriteFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteImageInformation();
  
  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void Write(const void* buffer);


  AnalyzeObjectImageIO();
  ~AnalyzeObjectImageIO();
  void PrintSelf(std::ostream& os, Indent indent) const;
  
private:
  AnalyzeObjectImageIO(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#endif // __itkAnalyzeObjectImageIO_h
