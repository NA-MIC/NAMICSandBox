/*=========================================================================

  Program:   Open IGT Link -- Example for Image Meta Data Client
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlImgMetaMessage.h"
#include "igtlClientSocket.h"


int ReceiveImgMetea(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);


int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);

  //------------------------------------------------------------
  // Establish Connection
  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // loop
  for (int i = 0; i < 100; i ++)
    {
    //------------------------------------------------------------
    // Send request data
    igtl::GetImgMetaMessage::Pointer getImgMetaMsg;
    getImgMetaMsg = igtl::GetImgMetaMessage::New();
    getImgMetaMsg->SetDeviceName("Client");
    getImgMetaMsg->Pack();
    socket->Send(getImgMetaMsg->GetPackPointer(), getImgMetaMsg->GetPackSize());
    //igtl::Sleep(1000); // wait
    
    //------------------------------------------------------------
    // Wait for a reply
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();
    int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
    if (r == 0)
      {
      std::cerr << "Connection closed." << std::endl;
      socket->CloseSocket();
      exit(0);
      }
    if (r != headerMsg->GetPackSize())
      {
      std::cerr << "Message size information and actual data size don't match." << std::endl; 
      exit(0);
      }
    if (strcmp(headerMsg->GetDeviceType(), "IMGMETA") == 0)
      {
      ReceiveImgMetea(socket, headerMsg);
      }
    else
      {
      std::cerr << "Invalid response from the server." << std::endl; 
      exit(0);
      }
    }
    
  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();

}


int ReceiveImgMetea(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving STATUS data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::ImgMetaMessage::Pointer imgMeta;
  imgMeta = igtl::ImgMetaMessage::New();
  imgMeta->SetMessageHeader(header);
  imgMeta->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(imgMeta->GetPackBodyPointer(), imgMeta->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = imgMeta->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    


    std::cerr << "========== STATUS ==========" << std::endl;
    //std::cerr << " Code      : " << imgMeta->GetCode() << std::endl;
    //std::cerr << " SubCode   : " << imgMeta->GetSubCode() << std::endl;
    //std::cerr << " Error Name: " << imgMeta->GetErrorName() << std::endl;
    //std::cerr << " Status    : " << imgMeta->GetStatusString() << std::endl;
    std::cerr << "============================" << std::endl;
    }

  return 0;

}



