/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlSensorDataMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlSensorDataMessage_h
#define __igtlSensorDataMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#define IGTL_STRING_MESSAGE_DEFAULT_ENCODING 3 /* Default encoding -- ANSI-X3.5-1968 */

namespace igtl
{

class IGTLCommon_EXPORT SensorDataMessage: public MessageBase
{
public:
  typedef SensorDataMessage                  Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::SensorDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::SensorDataMessage);

public:

  int        SetSensorData(const char* string);
  int        SetSensorData(std::string & string);
  int        SetEncoding(igtlUint16 enc);

  const char* GetSensorData();
  igtlUint16  GetEncoding();

protected:
  SensorDataMessage();
  ~SensorDataMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  igtlUint16   m_Encoding;
  
  //BTX
  std::string  m_SensorData;
  //ETX

};


} // namespace igtl

#endif // _igtlSensorDataMessage_h


