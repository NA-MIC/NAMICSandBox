/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkDistractorModelingLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkDistractorModelingLogic_h
#define __vtkDistractorModelingLogic_h

#include "vtkDistractorModelingWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;

class VTK_DistractorModeling_EXPORT vtkDistractorModelingLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkDistractorModelingLogic *New();
  
  vtkTypeRevisionMacro(vtkDistractorModelingLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

  void MoveSlider(vtkMRMLModelNode* Slider, double value, vtkMRMLLinearTransformNode* SliderTransformationNode);

 protected:
  
  vtkDistractorModelingLogic();
  ~vtkDistractorModelingLogic();

  void operator=(const vtkDistractorModelingLogic&);
  vtkDistractorModelingLogic(const vtkDistractorModelingLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif


  
