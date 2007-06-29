/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkYAFFImageIO.cxx,v $
  Language:  C++
  Date:      $Date: 2007/03/29 18:39:28 $
  Version:   $Revision: 1.69 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkExceptionObject.h"
#include "itkIOCommon.h"
#include "itkYAFFImageIO.h"
#include "itksys/SystemTools.hxx"

namespace itk
{

YAFFImageIO::YAFFImageIO()
{
  this->SetNumberOfDimensions(3); // YAFF is 3D.
  this->SetNumberOfComponents(1); // YAFF only has one component.
} 

YAFFImageIO::~YAFFImageIO()
{
}

void YAFFImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


bool YAFFImageIO::CanReadFile( const char* filename ) 
{ 
  std::cout << "YAFFImageIO::CanReadFile() " << std::endl;
  //
  // If the file exists, and have extension .yaff, then we are good to read it.
  //
  if( !itksys::SystemTools::FileExists( filename ) )
    {
    std::cout << "File doesn't exist" << std::endl;
    return false;
    }
std::cout << itksys::SystemTools::GetFilenameLastExtension( filename ) << std::endl;
  if( itksys::SystemTools::GetFilenameLastExtension( filename ) != ".yaff" )
    {
    std::cout << "Wrong extension" << std::endl;
    return false;
    }
 
  return true;
}


void YAFFImageIO::ReadImageInformation()
{ 
  // YAFF only reads 8-bits unsigned char images.
  this->SetPixelType( SCALAR );
  this->SetComponentType( UCHAR );

  this->m_InputStream.open( this->m_FileName.c_str() );
 
  if( this->m_InputStream.fail() )
    {
    itkExceptionMacro("Failed to open file " << this->m_InputStream );
    }

  unsigned int nx;
  unsigned int ny;
  unsigned int nz;

  double dx;
  double dy;
  double dz;

  this->m_InputStream >> nx >> ny >> nz;
  this->m_InputStream >> dx >> dy >> dz;

  std::string rawFileName;
  this->m_InputStream >> rawFileName;
 
  this->m_InputStream.close();

  std::cout << "::Read() filename= " << this->m_FileName << std::endl;
  this->m_RawDataFilename = itksys::SystemTools::GetFilenamePath( this->m_FileName );
  this->m_RawDataFilename += '/';
  this->m_RawDataFilename += rawFileName;

  if( !itksys::SystemTools::FileExists( this->m_RawDataFilename.c_str() ) )
    {
    itkExceptionMacro("Raw data file does not exist " << this->m_RawDataFilename );
    }
 
  this->SetDimensions( 0, nx );
  this->SetDimensions( 1, ny );
  this->SetDimensions( 2, nz );

  this->SetSpacing( 0, dx );
  this->SetSpacing( 1, dy );
  this->SetSpacing( 2, dz );
}


void YAFFImageIO::Read(void* buffer)
{ 
  this->m_InputStream.open( this->m_RawDataFilename.c_str() );
  
  ImageIORegion regionToRead = this->GetIORegion();

  ImageIORegion::SizeType  size  = regionToRead.GetSize();
  ImageIORegion::IndexType start = regionToRead.GetIndex();

  unsigned int mx = start[0];
  unsigned int my = start[1];
  unsigned int mz = start[2];

  unsigned int sx = size[0];
  unsigned int sy = size[1];
  unsigned int sz = size[2];

  unsigned int pos = mz * ( sx * sy ) + my * sx + mx;

  this->m_InputStream.seekg( pos, std::ios_base::beg );

  this->m_InputStream.close();
} 


bool YAFFImageIO::CanWriteFile( const char * name )
{
  //
  // YAFF is not affraid of writing either !!
  // 
  return true;
}

  
void 
YAFFImageIO
::WriteImageInformation(void)
{
  // add writing here
}


/**
 *
 */
void 
YAFFImageIO
::Write( const void* buffer) 
{
}

/** Given a requested region, determine what could be the region that we can
 * read from the file. This is called the streamable region, which will be
 * smaller than the LargestPossibleRegion and greater or equal to the 
RequestedRegion */
ImageIORegion 
YAFFImageIO
::DetermineStreamableRegionFromRequestedRegion( const ImageIORegion & requested ) const
{
  std::cout << "YAFFImageIO::DetermineStreamableRegionFromRequestedRegion()" << std::endl;
  std::cout << "Requested region = " << requested << std::endl;
  //
  // YAFF is the ultimate streamer.
  //
  ImageIORegion streamableRegion = requested;

  std::cout << "StreamableRegion = " << streamableRegion << std::endl;

  return streamableRegion;
}
 

} // end namespace itk
