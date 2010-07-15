/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkSecondaryWindowGUI_h
#define __vtkSecondaryWindowGUI_h

#ifdef WIN32
#include "vtkSecondaryWindowWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSecondaryWindowLogic.h"
#include "vtkSecondaryWindowViwerWindow.h"
#include "vtkMutexLock.h"
#include "vtkMultiThreader.h"
#include "vtkImageImport.h"

class vtkKWPushButton;

class VTK_SecondaryWindow_EXPORT vtkSecondaryWindowGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkSecondaryWindowGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkSecondaryWindowLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkSecondaryWindowGUI ( );
  virtual ~vtkSecondaryWindowGUI ( );

 private:
  vtkSecondaryWindowGUI ( const vtkSecondaryWindowGUI& ); // Not implemented.
  void operator = ( const vtkSecondaryWindowGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkSecondaryWindowGUI* New ();
  void Init();
  virtual void Enter ( );
  virtual void Exit ( );

  virtual void TearDownGUI();

  void PrintSelf (ostream& os, vtkIndent indent );

  //----------------------------------------------------------------
  // Observer Management
  //----------------------------------------------------------------

  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );
  void AddLogicObservers ( );
  void RemoveLogicObservers ( );

  //----------------------------------------------------------------
  // Event Handlers
  //----------------------------------------------------------------

  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  void ProcessTimerEvents();
  void HandleMouseEvent(vtkSlicerInteractorStyle *style);
  static void DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData);
  
  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------

  virtual void BuildGUI ( );
  void BuildGUIForHelpFrame();
  void BuildGUIForWindowConfigurationFrame();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();

 protected:

  int SwitchViewerBackground(vtkSlicerViewerWidget* vwidget, int sw);
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  // Thread (by A. Yamada, M. Komura)
  int ThreadLock;
  int ThreadID;
  int fRunThread;
  vtkMutexLock*     Mutex;
  vtkMultiThreader* Thread;
  int StartCamera();
  int StopCamera();
  static void *CameraThread(void*);
  
  vtkImageImport *importer;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWPushButton* ShowSecondaryWindowButton;
  vtkKWPushButton* HideSecondaryWindowButton;

  vtkSecondaryWindowViwerWindow* SecondaryViewerWindow;

  vtkSlicerNodeSelectorWidget* CameraNodeSelector;

  vtkKWPushButton* StartCaptureButton;
  vtkKWPushButton* StopCaptureButton;


  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkSecondaryWindowLogic *Logic;
  vtkCallbackCommand      *DataCallbackCommand;
  int                     CloseScene;


};



#endif
