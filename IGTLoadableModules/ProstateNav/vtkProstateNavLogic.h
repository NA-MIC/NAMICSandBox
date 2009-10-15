/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

// .NAME vtkProstateNavLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 

#ifndef __vtkProstateNavLogic_h
#define __vtkProstateNavLogic_h

#include "vtkProstateNavWin32Header.h"

#include "vtkKWTkUtilities.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLSliceNode.h"

#include "vtkMRMLProstateNavManagerNode.h"

class vtkProstateNavGUI;

class VTK_PROSTATENAV_EXPORT vtkProstateNavLogic : public vtkSlicerModuleLogic 
{
  
 public:

   //BTX
   enum 
   {  // Events
     LocatorUpdateEvent      = 50000,
     StatusUpdateEvent       = 50001,
   };
   //ETX

  
  static vtkProstateNavLogic *New();
  
  vtkTypeRevisionMacro(vtkProstateNavLogic,vtkObject);
  
  void SetGUI(vtkProstateNavGUI* gui) { this->GUI = gui; };
  vtkProstateNavGUI* GetGUI()         { return this->GUI; };

  void PrintSelf(ostream&, vtkIndent);  
  
  int  Enter();
  void TimerHandler();
  
  int  RobotStop();
  int  RobotMoveTo(float px, float py, float pz,
                   float nx, float ny, float nz,
                   float tx, float ty, float tz);
  int  RobotMoveTo(float position[3], float orientation[4]);

  int  RobotMoveTo();
  
  int  ScanStart();
  int  ScanPause();
  int  ScanStop();
  
  //BTX
  //Image* ReadCalibrationImage(const char* filename, int* width, int* height,
  //                            std::vector<float>& position, std::vector<float>& orientation);

  bool AddTargetToNeedle(std::string needleType, float* rasLocation, unsigned int & targetDescIndex);

  // Description:
  // Add volume to MRML scene and return the MRML node.
  // If volumeType is specified, then the volume is also selected as the current Calibration
  // targeting or verification volume.
  vtkMRMLScalarVolumeNode *AddVolumeToScene(const char *fileName, VolumeType volumeType=VOL_GENERIC);

  int ShowCoverage(bool show);

  //ETX

  void UpdateTargetListFromMRML();
  
 protected:

  //BTX
  std::string GetFoRStrFromVolumeNodeID(const char* volNodeID);
  //ETX

  // Description:
  // Helper method for loading a volume via the Volume module.
  vtkMRMLScalarVolumeNode *AddArchetypeVolume(const char* fileName, const char *volumeName);  

  // Description:
  // Set window/level computation, forcing the volume type to scalar (not statistical)
  void SetAutoScaleScalarVolume(vtkMRMLScalarVolumeNode *volumeNode);

  void UpdateAll();

  vtkProstateNavLogic();
  ~vtkProstateNavLogic();
  vtkProstateNavLogic(const vtkProstateNavLogic&);
  void operator=(const vtkProstateNavLogic&);
  
  static void DataCallback(vtkObject*, unsigned long, void *, void *);

  /*
  void UpdateAll();
  void UpdateSliceDisplay();
  void UpdateLocator();
  */

  vtkCallbackCommand *DataCallbackCommand;
  
 private:
  
  // Description:
  // Set Slicers's 2D view orientations from the image orientation.
  void SetSliceViewFromVolume(vtkMRMLVolumeNode *volumeNode);

  int GetTargetIndexFromFiducialID(const char* fiducialID);

  int CreateCoverageVolume();
  void DeleteCoverageVolume();
  int UpdateCoverageVolumeImage();

  bool IsTargetReachable(int needleIndex, double rasLocation[3]);

  vtkProstateNavGUI* GUI;

  /*
  bool  Connected;
  bool  RealtimeImageUpdate;
  */

  int   TimerOn;

};

#endif


  
