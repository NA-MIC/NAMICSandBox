/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkControl4DGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkMRMLColorNode.h"
#include "vtkMRMLBSplineTransformNode.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"

#include "vtkKWScaleWithEntry.h"
#include "vtkKWScale.h"
#include "vtkKWPushButton.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWMenuButton.h"
#include "vtkKWSpinBox.h"
#include "vtkKWCanvas.h"
#include "vtkKWRange.h"

#include "vtkKWProgressDialog.h"

#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWPiecewiseFunctionEditor.h"
#include "vtkPiecewiseFunction.h"
#include "vtkDoubleArray.h"

#include "vtkKWProgressGauge.h"

#include "vtkCornerAnnotation.h"
#include "vtkCommandLineModuleGUI.h"

#include "itksys/DynamicLoader.hxx"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkControl4DGUI );
vtkCxxRevisionMacro ( vtkControl4DGUI, "$Revision$");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkControl4DGUI::vtkControl4DGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkControl4DGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->ProgressDialog = NULL;
  this->SelectImageButton             = NULL;
  this->LoadImageButton               = NULL;
  this->ForegroundVolumeSelectorScale = NULL;
  this->BackgroundVolumeSelectorScale = NULL;
  this->MaskSelectMenu    = NULL;
  this->MaskSelectSpinBox = NULL;
  this->MaskColorCanvas   = NULL;

  this->RunPlotButton  = NULL;
  this->PlotTypeButtonSet = NULL;
  this->FunctionEditor = NULL;
  this->SavePlotButton = NULL;

  this->RegistrationStartIndexSpinBox = NULL;
  this->RegistrationEndIndexSpinBox   = NULL;
  this->StartRegistrationButton       = NULL;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkControl4DGUI::~vtkControl4DGUI ( )
{

  //----------------------------------------------------------------
  // Remove Callbacks

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  //----------------------------------------------------------------
  // Remove Observers

  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Remove GUI widgets

  if (this->ProgressDialog)
    {
    this->ProgressDialog->SetParent(NULL);
    this->ProgressDialog->Delete();
    }
  if (this->SelectImageButton)
    {
    this->SelectImageButton->SetParent(NULL);
    this->SelectImageButton->Delete();
    }
  if (this->LoadImageButton)
    {
    this->LoadImageButton->SetParent(NULL);
    this->LoadImageButton->Delete();
    }
  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale->SetParent(NULL);
    this->ForegroundVolumeSelectorScale->Delete();
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale->SetParent(NULL);
    this->BackgroundVolumeSelectorScale->Delete();
    }
  if (this->WindowLevelRange)
    {
    this->WindowLevelRange->SetParent(NULL);
    this->WindowLevelRange->Delete();
    }
  if (this->ThresholdRange)
    {
    this->ThresholdRange->SetParent(NULL);
    this->ThresholdRange->Delete();
    }

  if (this->MaskSelectMenu)
    {
    this->MaskSelectMenu->SetParent(NULL);
    this->MaskSelectMenu->Delete();
    }
  if (this->MaskSelectSpinBox)
    {
    this->MaskSelectSpinBox->SetParent(NULL);
    this->MaskSelectSpinBox->Delete();
    }
  if (this->MaskColorCanvas)
    {
    this->MaskColorCanvas->SetParent(NULL);
    this->MaskColorCanvas->Delete();
    }
  if (this->RunPlotButton)
    {
    this->RunPlotButton->SetParent(NULL);
    this->RunPlotButton->Delete();
    }
  if (this->FunctionEditor)
    {
    this->FunctionEditor->SetParent(NULL);
    this->FunctionEditor->Delete();
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton->SetParent(NULL);
    this->SavePlotButton->Delete();
    }
  if (this->RegistrationStartIndexSpinBox)
    {
    this->RegistrationStartIndexSpinBox->SetParent(NULL);
    this->RegistrationStartIndexSpinBox->Delete();
    }
  if (this->RegistrationEndIndexSpinBox)
    {
    this->RegistrationEndIndexSpinBox->SetParent(NULL);
    this->RegistrationEndIndexSpinBox->Delete();
    }

  if (this->StartRegistrationButton)
    {
    this->StartRegistrationButton->SetParent(NULL);
    this->StartRegistrationButton->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "Control4DGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->LoadImageButton)
    {
    this->LoadImageButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->WindowLevelRange)
    {
    this->WindowLevelRange
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ThresholdRange)
    {
    this->ThresholdRange
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->MaskSelectMenu)
    {
    this->MaskSelectMenu->GetMenu()
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if (this->MaskSelectSpinBox)
    {
    this->MaskSelectSpinBox
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if (this->RunPlotButton)
    {
    this->RunPlotButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->PlotTypeButtonSet)
    {
    this->PlotTypeButtonSet->GetWidget(0)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->PlotTypeButtonSet->GetWidget(1)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton->GetWidget()->GetLoadSaveDialog()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->RegistrationStartIndexSpinBox)
    {
    this->RegistrationStartIndexSpinBox
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->RegistrationEndIndexSpinBox)
    {
    this->RegistrationEndIndexSpinBox
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->StartRegistrationButton)
    {
    this->StartRegistrationButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  if (this->LoadImageButton)
    this->LoadImageButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->WindowLevelRange)
    {
    this->WindowLevelRange
      ->AddObserver(vtkKWRange::RangeValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ThresholdRange)
    {
    this->ThresholdRange
      ->AddObserver(vtkKWRange::RangeValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if (this->MaskSelectMenu)
    {
    this->MaskSelectMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }
  if (this->MaskSelectSpinBox)
    {
    this->MaskSelectSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RunPlotButton)
    {
    this->RunPlotButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->PlotTypeButtonSet)
    {
    this->PlotTypeButtonSet->GetWidget(0)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->PlotTypeButtonSet->GetWidget(1)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton->GetWidget()->GetLoadSaveDialog()
      ->AddObserver(vtkKWLoadSaveDialog::FileNameChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RegistrationStartIndexSpinBox)
    {
    this->RegistrationStartIndexSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RegistrationEndIndexSpinBox)
    {
    this->RegistrationEndIndexSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if (this->StartRegistrationButton)
    {
    this->StartRegistrationButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkControl4DLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  if (this->LoadImageButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    const char* path = this->SelectImageButton->GetWidget()->GetFileName();
    this->GetLogic()->AddObserver(vtkControl4DLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    int n = this->GetLogic()->LoadImagesFromDir(path, this->RangeLower, this->RangeUpper);
    this->RegistrationEndIndexSpinBox->SetRange(0, n);
    this->WindowLevelUpdateStatus.resize(n);
    this->Window = 1.0;
    this->Level  = 0.5;
    this->ThresholdUpper = 0.0;
    this->ThresholdLower = 1.0;
    this->GetLogic()->RemoveObservers(vtkControl4DLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    // Adjust range of the scale
    this->ForegroundVolumeSelectorScale->SetRange(0.0, (double) n);
    this->BackgroundVolumeSelectorScale->SetRange(0.0, (double) n);
    SetForeground(0);
    SetBackground(0);
    }
  else if (this->ForegroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/)
    {
    int value = (int)this->ForegroundVolumeSelectorScale->GetValue();
    SetForeground(value);
    }
  else if (this->BackgroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/ )
    {
    int value = (int)this->BackgroundVolumeSelectorScale->GetValue();
    SetBackground(value);
    }
  else if (this->WindowLevelRange == vtkKWRange::SafeDownCast(caller)
      && event == vtkKWRange::RangeValueChangingEvent)
    {
    double wllow, wlhigh;
    this->WindowLevelRange->GetRange(wllow, wlhigh);
    this->Window = wlhigh - wllow;
    this->Level  = (wlhigh + wllow) / 2.0;
    SetWindowLevelForCurrentFrame();
    }
  else if (this->ThresholdRange == vtkKWRange::SafeDownCast(caller)
      && event == vtkKWRange::RangeValueChangingEvent)
    {
    double thlow, thhigh;
    this->ThresholdRange->GetRange(thlow, thhigh);
    this->ThresholdUpper  = thhigh; 
    this->ThresholdLower  = thlow; 
    SetWindowLevelForCurrentFrame();
    }
  else if (this->MaskSelectMenu->GetMenu() == vtkKWMenu::SafeDownCast(caller)
      && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();
    SelectMask(nodeID, label);
    }
  else if (this->MaskSelectSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();
    SelectMask(nodeID, label);
    }
  else if (this->RunPlotButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();

    vtkDoubleArray* p;
    if (this->PlotTypeButtonSet->GetWidget(0)->GetSelectedState())
      {
      p = this->GetLogic()->GetIntensityCurve(nodeID, label, vtkControl4DLogic::TYPE_MEAN);
      }
    else
      {
      p = this->GetLogic()->GetIntensityCurve(nodeID, label, vtkControl4DLogic::TYPE_SD);
      }
    UpdateFunctionEditor(p);
    }
  else if (this->PlotTypeButtonSet->GetWidget(0) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->PlotTypeButtonSet->GetWidget(0)->GetSelectedState() == 1)
    {
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();

    if (nodeID)
      {
      vtkDoubleArray* p;
      p = this->GetLogic()->GetIntensityCurve(nodeID, label, vtkControl4DLogic::TYPE_MEAN);
      UpdateFunctionEditor(p);
      }
    }
  else if (this->PlotTypeButtonSet->GetWidget(1) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->PlotTypeButtonSet->GetWidget(1)->GetSelectedState() == 1)
    {
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();

    if (nodeID)
      {
      vtkDoubleArray* p;
      p = this->GetLogic()->GetIntensityCurve(nodeID, label, vtkControl4DLogic::TYPE_SD);
      UpdateFunctionEditor(p);
      }
    }
  else if (this->SavePlotButton->GetWidget()->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller)
           && event == vtkKWLoadSaveDialog::FileNameChangedEvent)
    {
    const char* filename = (const char*)callData;
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();
    this->GetLogic()->SaveIntensityCurve(nodeID, label, filename);
    //this->SavePlotButton->GetWidget()->GetLoadSaveDialog()->
    }
  else if (this->RegistrationStartIndexSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    int sid = (int)this->RegistrationStartIndexSpinBox->GetValue();
    int eid = (int)this->RegistrationEndIndexSpinBox->GetValue();
    if (sid < 0)
      {
      this->RegistrationStartIndexSpinBox->SetValue(0);
      this->RegistrationStartIndexSpinBox->Modified();
      }
    else if (sid > eid)
      {
      this->RegistrationStartIndexSpinBox->SetValue(eid);
      this->RegistrationStartIndexSpinBox->Modified();
      }
    }
  else if (this->RegistrationEndIndexSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    int sid = (int)this->RegistrationStartIndexSpinBox->GetValue();
    int eid = (int)this->RegistrationEndIndexSpinBox->GetValue();
    std::cerr << "number of frames = " << this->GetLogic()->GetNumberOfFrames() << std::endl;
    if (eid >= this->GetLogic()->GetNumberOfFrames())
      {
      this->RegistrationEndIndexSpinBox->SetValue(this->GetLogic()->GetNumberOfFrames()-1);
      this->RegistrationEndIndexSpinBox->Modified();
      }
    if (sid > eid)
      {
      this->RegistrationEndIndexSpinBox->SetValue(sid);
      this->RegistrationEndIndexSpinBox->Modified();
      }
    }
  else if (this->StartRegistrationButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    int sid = (int)this->RegistrationStartIndexSpinBox->GetValue();
    int eid = (int)this->RegistrationEndIndexSpinBox->GetValue();
    RunSeriesRegistration(sid, eid, sid);
    }
} 


//---------------------------------------------------------------------------
void vtkControl4DGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkControl4DGUI *self = reinterpret_cast<vtkControl4DGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkControl4DGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{
  if (event == vtkControl4DLogic::StatusUpdateEvent)
    {
    //this->UpdateDeviceStatus();
    }
  else if (event ==  vtkCommand::ProgressEvent) 
    {
    double progress = *((double *)callData);
    this->GetApplicationGUI()->GetMainSlicerWindow()->GetProgressGauge()->SetValue(100*progress);
    }
  else if (event == vtkControl4DLogic::ProgressDialogEvent)
    {
    vtkControl4DLogic::StatusMessageType* smsg
      = (vtkControl4DLogic::StatusMessageType*)callData;
    if (smsg->show)
      {
      if (!this->ProgressDialog)
        {
        this->ProgressDialog = vtkKWProgressDialog::New();
        this->ProgressDialog->SetParent(this->GetApplicationGUI()->GetMainSlicerWindow());
        this->ProgressDialog->SetMasterWindow(this->GetApplicationGUI()->GetMainSlicerWindow());
        this->ProgressDialog->Create();
        }
      this->ProgressDialog->SetMessageText(smsg->message.c_str());
      this->ProgressDialog->UpdateProgress(smsg->progress);
      this->ProgressDialog->Display();
      }
    else
      {
      if (this->ProgressDialog)
        {
        this->ProgressDialog->SetParent(NULL);
        this->ProgressDialog->Delete();
        this->ProgressDialog = NULL;
        }
      }
    }

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  if (event == vtkMRMLScene::NodeAddedEvent)
    {
    UpdateMaskSelectMenu();
    }
  else if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
  else if (event == vtkMRMLVolumeNode::ImageDataModifiedEvent)
    {
    vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);

    if (strcmp(node->GetNodeTagName(), "Volume") == 0)
      {
      vtkMRMLScalarVolumeNode* svnode = vtkMRMLScalarVolumeNode::SafeDownCast(caller);
      if (svnode->GetLabelMap()) // if the updated node is label map
        {
        // delete cache in the logic class
        this->GetLogic()->ClearIntensityCurveCache(node->GetID());
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "Control4D", "Control4D", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForLoadFrame();
  BuildGUIForFrameControlFrame();
  BuildGUIForFunctionViewer();
  BuildGUIForRegistrationFrame();

}


void vtkControl4DGUI::BuildGUIForHelpFrame ()
{
  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------

  // Define your help text here.
  const char *help = 
    "**The OpenIGTLink Interface Module** helps you to manage OpenIGTLink connections:"
    "OpenIGTLink is an open network protocol for communication among software / hardware "
    "for image-guided therapy. See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:OpenIGTLinkIF</a> for details.";
  const char *about =
    "The module is designed and implemented by Junichi Tokuda for Brigham and Women's Hospital."
    "This work is supported by NCIGT, NA-MIC and BRP \"Enabling Technologies for MRI-Guided Prostate Intervention\" project.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Control4D" );
  this->BuildHelpAndAboutFrame (page, help, about);

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForLoadFrame ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Load");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Select File Frame
  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Input Directory");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );
  
  this->SelectImageButton = vtkKWLoadSaveButtonWithLabel::New();
  this->SelectImageButton->SetParent(frame->GetFrame());
  this->SelectImageButton->Create();
  this->SelectImageButton->SetWidth(50);
  this->SelectImageButton->GetWidget()->SetText ("Browse Image File");
  this->SelectImageButton->GetWidget()->GetLoadSaveDialog()->ChooseDirectoryOn();
  /*
    this->SelectImageButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes(
    "{ {ProstateNav} {*.dcm} }");
  */
  this->SelectImageButton->GetWidget()->GetLoadSaveDialog()
    ->RetrieveLastPathFromRegistry("OpenPath");

  this->LoadImageButton = vtkKWPushButton::New ( );
  this->LoadImageButton->SetParent ( frame->GetFrame() );
  this->LoadImageButton->Create ( );
  this->LoadImageButton->SetText ("Load Series");
  this->LoadImageButton->SetWidth (12);

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->SelectImageButton->GetWidgetName(),
               this->LoadImageButton->GetWidgetName());

  conBrowsFrame->Delete();
  frame->Delete();
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForFrameControlFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Frame Control");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Foreground child frame

  vtkKWFrameWithLabel *fframe = vtkKWFrameWithLabel::New();
  fframe->SetParent(conBrowsFrame->GetFrame());
  fframe->Create();
  fframe->SetLabelText ("Frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 fframe->GetWidgetName() );

  this->ForegroundVolumeSelectorScale = vtkKWScaleWithEntry::New();
  this->ForegroundVolumeSelectorScale->SetParent( fframe->GetFrame() );
  this->ForegroundVolumeSelectorScale->Create();
  this->ForegroundVolumeSelectorScale->SetEntryPosition(vtkKWScaleWithEntry::EntryPositionRight);
  this->ForegroundVolumeSelectorScale->SetOrientationToHorizontal();
  this->ForegroundVolumeSelectorScale->SetLabelText("FG Frame #");
  this->ForegroundVolumeSelectorScale->SetRange(0.0, 100.0);
  this->ForegroundVolumeSelectorScale->SetResolution(1.0);

  this->BackgroundVolumeSelectorScale = vtkKWScaleWithEntry::New();
  this->BackgroundVolumeSelectorScale->SetParent( fframe->GetFrame() );
  this->BackgroundVolumeSelectorScale->Create();
  this->BackgroundVolumeSelectorScale->SetEntryPosition(vtkKWScaleWithEntry::EntryPositionRight);
  this->BackgroundVolumeSelectorScale->SetOrientationToHorizontal();
  this->BackgroundVolumeSelectorScale->SetLabelText("BG Frame #");
  this->BackgroundVolumeSelectorScale->SetRange(0.0, 100.0);
  this->BackgroundVolumeSelectorScale->SetResolution(1.0);

  this->Script("pack %s %s -side top -fill x -padx 2 -pady 2", 
               this->ForegroundVolumeSelectorScale->GetWidgetName(),
               this->BackgroundVolumeSelectorScale->GetWidgetName());



  // -----------------------------------------
  // Contrast control

  vtkKWFrameWithLabel *cframe = vtkKWFrameWithLabel::New();
  cframe->SetParent(conBrowsFrame->GetFrame());
  cframe->Create();
  cframe->SetLabelText ("Contrast");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 cframe->GetWidgetName() );

  vtkKWFrame *lwframe = vtkKWFrame::New();
  lwframe->SetParent(cframe->GetFrame());
  lwframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 lwframe->GetWidgetName() );

  vtkKWLabel *lwLabel = vtkKWLabel::New();
  lwLabel->SetParent(lwframe);
  lwLabel->Create();
  lwLabel->SetText("Window/Level: ");

  this->WindowLevelRange = vtkKWRange::New();
  this->WindowLevelRange->SetParent(lwframe);
  this->WindowLevelRange->Create();
  this->WindowLevelRange->SymmetricalInteractionOn();
  this->WindowLevelRange->EntriesVisibilityOff ();  
  this->WindowLevelRange->SetWholeRange(0.0, 1.0);
  /*
  this->WindowLevelRange->SetCommand(this, "ProcessWindowLevelCommand");
  this->WindowLevelRange->SetStartCommand(this, "ProcessWindowLevelStartCommand");
  */
  this->Script("pack %s %s -side left -anchor nw -expand yes -fill x -padx 2 -pady 2",
               lwLabel->GetWidgetName(),
               this->WindowLevelRange->GetWidgetName());

  vtkKWFrame *thframe = vtkKWFrame::New();
  thframe->SetParent(cframe->GetFrame());
  thframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 thframe->GetWidgetName() );

  vtkKWLabel *thLabel = vtkKWLabel::New();
  thLabel->SetParent(thframe);
  thLabel->Create();
  thLabel->SetText("Threashold:   ");

  this->ThresholdRange = vtkKWRange::New();
  this->ThresholdRange->SetParent(thframe);
  this->ThresholdRange->Create();
  this->ThresholdRange->SymmetricalInteractionOff();
  this->ThresholdRange->EntriesVisibilityOff ();
  this->ThresholdRange->SetWholeRange(0.0, 1.0);
  /*
  this->ThresholdRange->SetCommand(this, "ProcessThresholdCommand");
  this->ThresholdRange->SetStartCommand(this, "ProcessThresholdStartCommand");
  */
  this->Script("pack %s %s -side left -anchor w -expand y -fill x -padx 2 -pady 2", 
               thLabel->GetWidgetName(),
               this->ThresholdRange->GetWidgetName());


  // -----------------------------------------
  // Delete pointers

  conBrowsFrame->Delete();
  fframe->Delete();

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForFunctionViewer()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Function Viewer");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());


  // -----------------------------------------
  // Mask selector frame
  
  vtkKWFrameWithLabel *msframe = vtkKWFrameWithLabel::New();
  msframe->SetParent(conBrowsFrame->GetFrame());
  msframe->Create();
  msframe->SetLabelText ("Mask Selector");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 msframe->GetWidgetName() );

  vtkKWLabel *menuLabel = vtkKWLabel::New();
  menuLabel->SetParent(msframe->GetFrame());
  menuLabel->Create();
  menuLabel->SetText("Mask: ");

  this->MaskSelectMenu = vtkKWMenuButton::New();
  this->MaskSelectMenu->SetParent(msframe->GetFrame());
  this->MaskSelectMenu->Create();
  this->MaskSelectMenu->SetWidth(20);

  this->MaskSelectSpinBox = vtkKWSpinBox::New();
  this->MaskSelectSpinBox->SetParent(msframe->GetFrame());
  this->MaskSelectSpinBox->Create();
  this->MaskSelectSpinBox->SetWidth(3);

  this->MaskColorCanvas = vtkKWCanvas::New();
  this->MaskColorCanvas->SetParent(msframe->GetFrame());
  this->MaskColorCanvas->Create();
  this->MaskColorCanvas->SetWidth(15);
  this->MaskColorCanvas->SetHeight(15);
  this->MaskColorCanvas->SetBackgroundColor(0.0, 0.0, 0.0);
  this->MaskColorCanvas->SetBorderWidth(2);
  this->MaskColorCanvas->SetReliefToSolid();

  this->RunPlotButton = vtkKWPushButton::New();
  this->RunPlotButton->SetParent(msframe->GetFrame());
  this->RunPlotButton->Create();
  this->RunPlotButton->SetText ("Plot");
  this->RunPlotButton->SetWidth (4);

  this->Script("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               menuLabel->GetWidgetName(),
               this->MaskSelectMenu->GetWidgetName(),
               this->MaskSelectSpinBox->GetWidgetName(),
               this->MaskColorCanvas->GetWidgetName(),
               this->RunPlotButton->GetWidgetName());
  
  // -----------------------------------------
  // Plot frame
  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Intencity Plot");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );
  
  this->PlotTypeButtonSet = vtkKWRadioButtonSet::New();
  this->PlotTypeButtonSet->SetParent(frame->GetFrame());
  this->PlotTypeButtonSet->Create();
  this->PlotTypeButtonSet->PackHorizontallyOn();
  this->PlotTypeButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->PlotTypeButtonSet->UniformColumnsOn();
  this->PlotTypeButtonSet->UniformRowsOn();

  this->PlotTypeButtonSet->AddWidget(0);
  this->PlotTypeButtonSet->GetWidget(0)->SetText("Mean");
  this->PlotTypeButtonSet->AddWidget(1);
  this->PlotTypeButtonSet->GetWidget(1)->SetText("SD");

  this->PlotTypeButtonSet->GetWidget(0)->SelectedStateOn();

  vtkPiecewiseFunction* tfun = vtkPiecewiseFunction::New();
  /*
  tfun->AddPoint(70.0, 0.0);
  tfun->AddPoint(599.0, 0);
  tfun->AddPoint(600.0, 0);
  tfun->AddPoint(1195.0, 0);
  tfun->AddPoint(1200, .2);
  tfun->AddPoint(1300, .3);
  tfun->AddPoint(2000, .3);
  tfun->AddPoint(4095.0, 1.0);
  */

  this->FunctionEditor = vtkKWPiecewiseFunctionEditor::New();
  this->FunctionEditor->SetParent(frame->GetFrame());
  this->FunctionEditor->Create();
  this->FunctionEditor->SetReliefToGroove();
  this->FunctionEditor->SetPadX(2);
  this->FunctionEditor->SetPadY(2);
  this->FunctionEditor->ParameterRangeVisibilityOff();
  this->FunctionEditor->SetCanvasHeight(200);
  //this->FunctionEditor->SetPiecewiseFunction(tfun);
  this->FunctionEditor->ReadOnlyOn();
  this->FunctionEditor->SetPointRadius(2);
  this->FunctionEditor->ParameterTicksVisibilityOn();
  this->FunctionEditor->SetParameterTicksFormat("%-#4.2f");
  this->FunctionEditor->ValueTicksVisibilityOn();
  this->FunctionEditor->SetValueTicksFormat("%-#4.2f");

  this->Script("pack %s %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->PlotTypeButtonSet->GetWidgetName(),
               this->FunctionEditor->GetWidgetName());

  // -----------------------------------------
  // Output frame
  
  vtkKWFrameWithLabel *oframe = vtkKWFrameWithLabel::New();
  oframe->SetParent(conBrowsFrame->GetFrame());
  oframe->Create();
  oframe->SetLabelText ("Output");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 oframe->GetWidgetName() );

  this->SavePlotButton = vtkKWLoadSaveButtonWithLabel::New();
  this->SavePlotButton->SetParent(oframe->GetFrame());
  this->SavePlotButton->Create();
  this->SavePlotButton->SetWidth(50);
  this->SavePlotButton->GetWidget()->SetText ("Save");
  this->SavePlotButton->GetWidget()->GetLoadSaveDialog()->SaveDialogOn();
  this->SavePlotButton->GetWidget()->GetLoadSaveDialog()->SetDefaultExtension(".csv");
  

  this->Script("pack %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->SavePlotButton->GetWidgetName());

  conBrowsFrame->Delete();
  frame->Delete();
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForRegistrationFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Registration");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Parameter
  
  vtkKWFrameWithLabel *pframe = vtkKWFrameWithLabel::New();
  pframe->SetParent(conBrowsFrame->GetFrame());
  pframe->Create();
  pframe->SetLabelText ("Frames");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 pframe->GetWidgetName() );

  vtkKWLabel *fromLabel = vtkKWLabel::New();
  fromLabel->SetParent(pframe->GetFrame());
  fromLabel->Create();
  fromLabel->SetText("From: ");

  this->RegistrationStartIndexSpinBox = vtkKWSpinBox::New();
  this->RegistrationStartIndexSpinBox->SetParent(pframe->GetFrame());
  this->RegistrationStartIndexSpinBox->Create();
  this->RegistrationStartIndexSpinBox->SetWidth(3);

  vtkKWLabel *toLabel = vtkKWLabel::New();
  toLabel->SetParent(pframe->GetFrame());
  toLabel->Create();
  toLabel->SetText(" to: ");

  this->RegistrationEndIndexSpinBox = vtkKWSpinBox::New();
  this->RegistrationEndIndexSpinBox->SetParent(pframe->GetFrame());
  this->RegistrationEndIndexSpinBox->Create();
  this->RegistrationEndIndexSpinBox->SetWidth(3);

  this->StartRegistrationButton = vtkKWPushButton::New();
  this->StartRegistrationButton->SetParent(pframe->GetFrame());
  this->StartRegistrationButton->Create();
  this->StartRegistrationButton->SetText ("Run");
  this->StartRegistrationButton->SetWidth (4);

  this->Script("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               fromLabel->GetWidgetName(),
               this->RegistrationStartIndexSpinBox->GetWidgetName(),
               toLabel->GetWidgetName(),
               this->RegistrationEndIndexSpinBox->GetWidgetName(),
               this->StartRegistrationButton->GetWidgetName());
  
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::UpdateAll()
{
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SetForeground(int index)
{
  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  const char* nodeID = this->GetLogic()->GetFrameNodeID(index);
  vtkMRMLVolumeNode* volNode =
    vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));

  if (volNode)
    {
    //std::cerr << "node id = " << nodeID << std::endl;
    nnodes = this->GetMRMLScene()->GetNumberOfNodesByClass ( "vtkMRMLSliceCompositeNode");
    for ( i=0; i<nnodes; i++)
      {
      cnode = vtkMRMLSliceCompositeNode::SafeDownCast (
        this->GetMRMLScene()->GetNthNodeByClass (i, "vtkMRMLSliceCompositeNode"));
      if ( cnode != NULL)
        {
        cnode->SetForegroundVolumeID( nodeID );
        }
      }
    SetWindowLevelForCurrentFrame();
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SetBackground(int index)
{
  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  const char* nodeID = this->GetLogic()->GetFrameNodeID(index);
  vtkMRMLVolumeNode* volNode =
    vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));

  if (volNode)
    {
    //std::cerr << "node id = " << nodeID << std::endl;
    nnodes = this->GetMRMLScene()->GetNumberOfNodesByClass ( "vtkMRMLSliceCompositeNode");          
    for ( i=0; i<nnodes; i++)
      {
      cnode = vtkMRMLSliceCompositeNode::SafeDownCast (
        this->GetMRMLScene()->GetNthNodeByClass (i, "vtkMRMLSliceCompositeNode"));
      if ( cnode != NULL)
        {
        cnode->SetBackgroundVolumeID( nodeID );
        }
      }
    SetWindowLevelForCurrentFrame();
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SetWindowLevelForCurrentFrame()
{

  vtkMRMLSliceCompositeNode *cnode = 
    vtkMRMLSliceCompositeNode::SafeDownCast (this->GetMRMLScene()->GetNthNodeByClass (0, "vtkMRMLSliceCompositeNode"));
  if (cnode != NULL)
    {
    const char* fgNodeID = cnode->GetForegroundVolumeID();
    const char* bgNodeID = cnode->GetBackgroundVolumeID();
    if (fgNodeID)
      {
      vtkMRMLVolumeNode* fgNode =
        vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(fgNodeID));
      vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(fgNode->GetDisplayNode());
      if (displayNode)
        {
        //double r[2];
        //fgNode->GetImageData()->GetScalarRange(r);
        double lower = this->RangeLower;
        double upper = this->RangeUpper;
        double range = upper - lower;
        double thLower = lower + range * this->ThresholdLower;
        double thUpper = lower + range * this->ThresholdUpper;
        double window  = range * this->Window;
        double level   = lower + range * this->Level;
        displayNode->SetAutoWindowLevel(0);
        displayNode->SetAutoThreshold(0);
        double cThLower = displayNode->GetLowerThreshold();
        double cThUpper = displayNode->GetUpperThreshold();
        double cWindow  = displayNode->GetWindow();
        double cLevel   = displayNode->GetLevel();

        int m = 0;
        if (cThLower!=thLower) { displayNode->SetLowerThreshold(thLower); m = 1;}
        if (cThUpper!=thUpper) { displayNode->SetUpperThreshold(thUpper); m = 1;}
        if (cWindow!=window)   { displayNode->SetWindow(window); m = 1;}
        if (cLevel!=level)     { displayNode->SetLevel(level); m = 1;}
        if (m) { displayNode->UpdateImageDataPipeline(); }
        }
      }
    if (bgNodeID && strcmp(fgNodeID, bgNodeID) != 0)
      {
      vtkMRMLVolumeNode* bgNode =
        vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bgNodeID));
      vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(bgNode->GetDisplayNode());
      if (displayNode)
        {
        //double r[2];
        //bgNode->GetImageData()->GetScalarRange(r);
        double lower = this->RangeLower;
        double upper = this->RangeUpper;
        double range = upper - lower;
        double thLower = lower + range * this->ThresholdLower;
        double thUpper = lower + range * this->ThresholdUpper;
        double window  = range * this->Window;
        double level   = lower + range * this->Level;
        displayNode->SetAutoWindowLevel(0);
        displayNode->SetAutoThreshold(0);
        double cThLower = displayNode->GetLowerThreshold();
        double cThUpper = displayNode->GetUpperThreshold();
        double cWindow  = displayNode->GetWindow();
        double cLevel   = displayNode->GetLevel();

        int m = 0;
        if (cThLower!=thLower) { displayNode->SetLowerThreshold(thLower); m = 1;}
        if (cThUpper!=thUpper) { displayNode->SetUpperThreshold(thUpper); m = 1;}
        if (cWindow!=window)   { displayNode->SetWindow(window); m = 1;}
        if (cLevel!=level)     { displayNode->SetLevel(level); m = 1;}
        if (m) { displayNode->UpdateImageDataPipeline(); }
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::UpdateMaskSelectMenu()
{

  if (!this->MaskSelectMenu)
    {
    return;
    }

  const char* className = this->GetMRMLScene()->GetClassNameByTag("Volume");
  std::vector<vtkMRMLNode*> nodes;
  this->GetMRMLScene()->GetNodesByClass(className, nodes);

  std::vector<vtkMRMLNode*>::iterator iter;
  if (nodes.size() > 0)
    {
    this->MaskSelectMenu->GetMenu()->DeleteAllItems();
    this->MaskNodeIDList.clear();
    for (iter = nodes.begin(); iter != nodes.end(); iter ++)
      {
      //(*iter)->GetName();
      //nodeInfo.nodeID = (*iter)->GetID();
      char str[256];

      vtkMRMLScalarVolumeNode* node =
        vtkMRMLScalarVolumeNode::SafeDownCast(*iter);
        //vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID((*iter)->GetID()));
      if (node != NULL && node->GetLabelMap())
        {
        //sprintf(str, "%s (%s)", (*iter)->GetName(), (*iter)->GetID());
        sprintf(str, "%s", (*iter)->GetName());
        this->MaskSelectMenu->GetMenu()->AddRadioButton(str);
        std::string id((*iter)->GetID());
        this->MaskNodeIDList.push_back(id);
        
        vtkIntArray* events;
        events = vtkIntArray::New();
        events->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent); 
        vtkMRMLNode *nd = NULL; // TODO: is this OK?
        vtkSetAndObserveMRMLNodeEventsMacro(nd,node,events);
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SelectMask(const char* nodeID, int label)
{

  vtkMRMLScalarVolumeNode* node =
    vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));

  if (node == NULL || !node->GetLabelMap())
    {
    return;
    }

  vtkMRMLVolumeDisplayNode* dnode = node->GetVolumeDisplayNode();
  if (dnode == NULL)
    {
    return;
    }

  vtkMRMLColorNode* cnode = dnode->GetColorNode();
  if (cnode == NULL)
    {
    return;
    }

  vtkLookupTable* lt = cnode->GetLookupTable();
  double l, h;

  l = lt->GetRange()[0];
  h = lt->GetRange()[1];
  if (label < l)
    {
    this->MaskSelectSpinBox->SetValue(l);
    label = (int)l;
    }
  else if (label > h)
    {
    this->MaskSelectSpinBox->SetValue(h);
    label = (int)h;
    }
  double color[3];
  lt->GetColor(label, color);
  if (this->MaskColorCanvas)
    {
    this->MaskColorCanvas->SetBackgroundColor(color[0], color[1], color[2]);
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::UpdateFunctionEditor(vtkDoubleArray* data)
{
  if (data == NULL)
    {
    std::cerr << "no data..." << std::endl;
    return;
    }

  int n = data->GetNumberOfTuples();
  std::cerr << "number of components = " << n << std::endl;
  if (n > 0)
    {
    vtkPiecewiseFunction* tfun = vtkPiecewiseFunction::New();
    for (int i = 0; i < n; i ++)
      {
      tfun->AddPoint((double)i, data->GetValue(i));
      }
    double range[2];
    data->GetRange(range);
    this->FunctionEditor->SetWholeParameterRange(0.0, (double)(n-1));
    this->FunctionEditor->SetVisibleParameterRange(0.0, (double)(n-1));
    this->FunctionEditor->SetWholeValueRange(0.0, range[1]);
    this->FunctionEditor->SetVisibleValueRange(0.0, range[1]);
    this->FunctionEditor->SetPiecewiseFunction(tfun);
    //this->FunctionEditor->SetVisibleParameterRangeToWholeParameterRange ();
    //this->FunctionEditor->SetWholeParameterRangeToFunctionRange();
    this->FunctionEditor->Update();
    }
}


//---------------------------------------------------------------------------
int vtkControl4DGUI::RunSeriesRegistration(int sIndex, int eIndex, int kIndex)
{
  if (sIndex < 0 ||
      eIndex >= this->GetLogic()->GetNumberOfFrames() ||
      kIndex < sIndex ||
      kIndex > eIndex)
    {
    std::cerr << "int vtkControl4DGUI::RunSeriesRegistration(): irregular index" << std::endl;
    return 0;
    }
  
  this->GetLogic()->CreateRegisteredVolumeNodes();
  
  const char* fixedFrameNodeID = this->GetLogic()->GetFrameNodeID(kIndex);
  vtkMRMLScalarVolumeNode* fixedNode;  
  if (fixedFrameNodeID)
    {
    fixedNode = 
      vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(fixedFrameNodeID));
    }
  else
    {
    std::cerr << "int vtkControl4DGUI::RunSeriesRegistration(): no fixed frame node ID found." << std::endl;
    }

  for (int i = sIndex; i <= eIndex; i++)
    {
    const char* movingFrameNodeID = this->GetLogic()->GetFrameNodeID(i);
    const char* outputFrameNodeID = this->GetLogic()->GetRegisteredFrameNodeID(i);
    if (movingFrameNodeID && outputFrameNodeID)
      {
      vtkMRMLScalarVolumeNode* movingNode;
      movingNode = 
        vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(movingFrameNodeID));

      vtkMRMLScalarVolumeNode* outputNode;
      outputNode = 
        vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(outputFrameNodeID));

      RunRegistration(fixedNode, movingNode, outputNode);
      }
    }

  return 1;

}
  

//---------------------------------------------------------------------------
int vtkControl4DGUI::RunRegistration(vtkMRMLScalarVolumeNode* fixedNode, vtkMRMLScalarVolumeNode* movingNode, vtkMRMLScalarVolumeNode* outputNode)
{
  vtkCommandLineModuleGUI* cligui;

  vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast (this->GetApplication());
  cligui = vtkCommandLineModuleGUI::SafeDownCast(app->GetModuleGUIByName ("Deformable BSpline registration"));
  //cligui = vtkCommandLineModuleGUI::SafeDownCast(app->GetModuleGUIByName ("Linear registration"));

  if (cligui)
    {
    std::cerr << "Found CommandLineModule !!!" << std::endl;
    
    cligui->Enter();
    vtkMRMLCommandLineModuleNode* node = 
      static_cast<vtkMRMLCommandLineModuleNode*>(this->GetMRMLScene()->CreateNodeByClass("vtkMRMLCommandLineModuleNode"));
    if(!node)
      {
      std::cerr << "Cannot create Rigid registration node." << std::endl;
      }
    this->GetMRMLScene()->AddNode(node);
    //tensorCLM->SetModuleDescription("Diffusion Tensor Estimation");
    //tensorCLM->SetName("GradientEditor: Tensor Estimation");
    node->SetModuleDescription(cligui->GetModuleDescription());  // this is very important !!!
    node->SetName("Deformable BSpline registration");

    //if(outputNode)
    //  {
    //  this->GetMRMLScene()->RemoveNode(outputNode);
    //  }
    
    // Create output transform node
    vtkMRMLBSplineTransformNode *transformNode =
      vtkMRMLBSplineTransformNode::New();
    //assert(transformNode);
    char name[128];
    sprintf(name, "BSpline-%s", movingNode->GetName());
    std::cerr << "Transform = " << name << std::endl;
    transformNode->SetName(name);
    this->GetMRMLScene()->AddNode(transformNode);
    
    //node->SetModuleDescription( this->ModuleDescriptionObject );

    node->SetParameterAsInt("Iterations", 20);
    node->SetParameterAsInt("gridSize", 5);  /* 3 - 20, step 1*/
    node->SetParameterAsInt("HistogramBins", 100); /* 1 - 500, step 5*/
    node->SetParameterAsInt("SpatialSamples", 5000); /* 1000 - 500000, step 1000*/
    node->SetParameterAsBool("ConstrainDeformation", 0); 
    node->SetParameterAsDouble("MaximumDeformation", 1.0);
    node->SetParameterAsInt("DefaultPixelValue", 0); /* 1000 - 500000, step 1000*/
    //node->SetParameterAsString("InitialTransform", NULL);
    node->SetParameterAsString("FixedImageFileName", fixedNode->GetID());
    node->SetParameterAsString("MovingImageFileName", movingNode->GetID());
    //node->SetParameterAsString("OutputTransform", transformNode->GetID());
    node->SetParameterAsString("ResampledImageFileName", outputNode->GetID());
#if 0
    node->SetParameterAsString("Iterations", "20");
    node->SetParameterAsString("gridSize", "5");  /* 3 - 20, step 1*/
    node->SetParameterAsString("HistogramBins", "100"); /* 1 - 500, step 5*/
    node->SetParameterAsString("SpatialSamples", "5000"); /* 1000 - 500000, step 1000*/
    node->SetParameterAsString("ConstrainDeformation", "0"); 
    node->SetParameterAsString("MaximumDeformation", "1.0");
    node->SetParameterAsString("DefaultPixelValue", "0"); /* 1000 - 500000, step 1000*/
    //node->SetParameterAsString("InitialTransform", NULL);
    node->SetParameterAsString("FixedImageFileName", fixedNode->GetID());
    node->SetParameterAsString("MovingImageFileName", movingNode->GetID());
    node->SetParameterAsString("OutputTransform", transformNode->GetID());
    node->SetParameterAsString("ResampledImageFileName", outputNode->GetID());
#endif
    transformNode->Delete();

    cligui->GetLogic()->SetTemporaryDirectory(app->GetTemporaryDirectory());

    ModuleDescription moduleDesc = node->GetModuleDescription();
    if(moduleDesc.GetTarget() == "Unknown")
      {
      // Entry point is unknown
      // "Linear registration" is shared object module, at least at this moment
      assert(moduleDesc.GetType() == "SharedObjectModule");
      typedef int (*ModuleEntryPoint)(int argc, char* argv[]);
      itksys::DynamicLoader::LibraryHandle lib =
        itksys::DynamicLoader::OpenLibrary(moduleDesc.GetLocation().c_str());
      if(lib)
        {
        ModuleEntryPoint entryPoint = 
          (ModuleEntryPoint) itksys::DynamicLoader::GetSymbolAddress(
                                                                     lib, "ModuleEntryPoint");
        if(entryPoint)
          {
          char entryPointAsText[256];
          std::string entryPointAsString;
          
          sprintf(entryPointAsText, "%p", entryPoint);
          entryPointAsString = std::string("slicer:")+entryPointAsText;
          moduleDesc.SetTarget(entryPointAsString);
          node->SetModuleDescription(moduleDesc);      
          } 
        else
          {
          std::cerr << "Failed to find entry point for Rigid registration. Abort." << std::endl;
          }
        } else {
      std::cerr << "Failed to locate module library. Abort." << std::endl;
      }
    }

    cligui->SetCommandLineModuleNode(node);
    cligui->GetLogic()->SetCommandLineModuleNode(node);

    std::cerr << "Starting Registration.... " << std::endl;    
    cligui->GetLogic()->ApplyAndWait(node);
    //cligui->GetLogic()->Apply();
    std::cerr << "Starting Registration.... done." << std::endl;    
    //this->SaveVolume(app, outputNode);
    node->Delete(); // AF: is it right to delete this here?
    //std::cerr << "Temp dir = " << ((vtkSlicerApplication*)this->GetApplication())->GetTemporaryDirectory() << std::endl;

    return 1;
    }
  else
    {
    std::cerr << "Couldn't find CommandLineModule !!!" << std::endl;
    return 0;
    }

}
