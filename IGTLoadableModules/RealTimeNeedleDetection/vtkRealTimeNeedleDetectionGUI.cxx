/*==========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
  Author: Roland Goerlitz, University of Karlsruhe

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: ??$
  Date:      $Date: 2009/03/09 09:10:09$
  Version:   $Revision: 1.00$

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkRealTimeNeedleDetectionGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLVolumeNode.h"
#include "vtkMRMLDisplayableNode.h"
#include "vtkMRMLTransformableNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLROINode.h"

#include "vtkTransformPolyDataFilter.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"
#include "vtkKWPushButton.h"
#include "vtkKWEntry.h"
#include "vtkKWCheckButton.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWScale.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkCornerAnnotation.h"

#define PI 3.1415926535897932384626433832795 
//TODO: how do I access the math pi?
#define PATIENTLEFT      1
#define PATIENTRIGHT     2
#define PATIENTPOSTERIOR 3
#define PATIENTANTERIOR  4
#define PATIENTINFERIOR  5
#define PATIENTSUPERIOR  6

#define AXIAL     1
#define CORONAL   2
#define SAGITTAL  3

#define DEFAULTTHRESHOLD 18000

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkRealTimeNeedleDetectionGUI);
vtkCxxRevisionMacro (vtkRealTimeNeedleDetectionGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constructor
vtkRealTimeNeedleDetectionGUI::vtkRealTimeNeedleDetectionGUI()
{
  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void *> (this));
  this->DataCallbackCommand->SetCallback(vtkRealTimeNeedleDetectionGUI::DataCallback);
  this->DataManager = vtkIGTDataManager::New();  //TODO: Do I need the DataManager?
  started       = 0;
  ROIpresent    = 0;
  showNeedle    = 0;
  scanPlane     = 0;
  needleOrigin  = PATIENTLEFT; //if this default value gets changed, the initial value for pEntryPointButtonSet needs to get changed, too
  
  //----------------------------------------------------------------
  // GUI widgets
  this->pVolumeSelector           = NULL; 
  this->pThresholdScale           = NULL;
  this->pEntryPointButtonSet      = NULL;
  this->pStartButton              = NULL;
  this->pStopButton               = NULL;
  this->pShowNeedleButton         = NULL;
  this->pXLowerEntry              = NULL;
  this->pXUpperEntry              = NULL;
  this->pYLowerEntry              = NULL;
  this->pYUpperEntry              = NULL;
  this->pZLowerEntry              = NULL;
  this->pZUpperEntry              = NULL;
  
  //--------------------------------------------------------------------
  // MRML nodes  
  this->pVolumeNode          = NULL;
  this->pSourceNode          = NULL;
  this->pNeedleModelNode     = NULL;
  this->pNeedleTransformNode = NULL;
  this->pScanPlaneNormalNode = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)   
  this->TimerFlag = 0;
   
  //----------------------------------------------------------------
  // Image Values - default  
  currentXLowerBound = initialXLowerBound = 0;
  currentXUpperBound = initialXUpperBound = 256;
  currentYLowerBound = initialYLowerBound = 0;
  currentYUpperBound = initialYUpperBound = 256;
  currentXImageRegionSize                 = 256;
  currentYImageRegionSize                 = 256;
  needleDetectionThreshold                = DEFAULTTHRESHOLD;
  imageDimensions[0]    = 256;
  imageDimensions[1]    = 256;
  imageDimensions[2]    = 1;
  imageSpacing[0]       = 1;  
  imageSpacing[1]       = 1;
  imageSpacing[2]       = 1;
  imageOrigin[0]        = 0;
  imageOrigin[1]        = 0;
  imageOrigin[2]        = 0;
  scalarSize            = 2;                      // scalarType 0,1 = 0 | 2,3 (char) = 1 | 4,5 (short) = 2 | 6,7 = 4
  lastModified          = 0;
  pImage                = NULL;
  pImageProcessor       = new ImageProcessor::ImageProcessor();  //TODO:move the new to starting the whole detection
  std::cout << "NeedleDetection constructed" << std::endl;
}

//---------------------------------------------------------------------------
// Destructor
vtkRealTimeNeedleDetectionGUI::~vtkRealTimeNeedleDetectionGUI()
{
  std::cout << "NeedleDetection destruction started" << std::endl;
  if (this->DataManager)
  {
    // If we don't set the scene to NULL for DataManager,
    // Slicer will report lots of leaks when it is closed.
    //TODO: Do I really need this datamanager?
    this->DataManager->SetMRMLScene(NULL);
    this->DataManager->Delete();
  }

  //----------------------------------------------------------------
  // Remove Callbacks
  if (this->DataCallbackCommand)
    this->DataCallbackCommand->Delete();

  //----------------------------------------------------------------
  // Remove Observers
  this->RemoveGUIObservers();
  //TODO: remove MRMLObservers

  //----------------------------------------------------------------
  // Remove GUI widgets
  if (this->pVolumeSelector)
  {
    this->pVolumeSelector->SetParent(NULL);
    this->pVolumeSelector->Delete();
  }
  if(this->pThresholdScale) 
  {
    this->pThresholdScale->SetParent(NULL);
    this->pThresholdScale->Delete();
  }
  if(this->pEntryPointButtonSet)
  {
    this->pEntryPointButtonSet->SetParent(NULL);
    this->pEntryPointButtonSet->Delete();
  }
  if(this->pStartButton)
  {
    this->pStartButton->SetParent(NULL);
    this->pStartButton->Delete();
  }
  if(this->pStopButton)
  {
    this->pStopButton->SetParent(NULL);
    this->pStopButton->Delete();
  }
  if(this->pShowNeedleButton)
  {
    this->pShowNeedleButton->SetParent(NULL );
    this->pShowNeedleButton->Delete ( );
  }
  
  if (this->pXLowerEntry)
  {
    this->pXLowerEntry->SetParent(NULL);
    this->pXLowerEntry->Delete();
  }
  if(this->pXUpperEntry)
  {
    this->pXUpperEntry->SetParent(NULL);
    this->pXUpperEntry->Delete();
  }
  if (this->pYLowerEntry)
  {
    this->pYLowerEntry->SetParent(NULL);
    this->pYLowerEntry->Delete();
  }
  if(this->pYUpperEntry)
  {
    this->pYUpperEntry->SetParent(NULL);
    this->pYUpperEntry->Delete();
  }
  if(this->pZLowerEntry)
  {
    this->pZLowerEntry->SetParent(NULL);
    this->pZLowerEntry->Delete();
  }
  if (this->pZUpperEntry)
  {
    this->pZUpperEntry->SetParent(NULL);
    this->pZUpperEntry->Delete();
  }
    
  //-----------------------------------------------------------------
  // Remove MRML nodes
  if(this->pVolumeNode)
    this->pVolumeNode->Delete(); 
  if(this->pSourceNode)           
    this->pSourceNode->Delete();
  if(this->pNeedleModelNode)
    this->pNeedleModelNode->Delete(); 
  if(this->pNeedleTransformNode)
    this->pNeedleTransformNode->Delete();
  if(this->pScanPlaneNormalNode)
    this->pScanPlaneNormalNode->Delete();

  //----------------------------------------------------------------
  // Unregister Logic class
  this->SetModuleLogic(NULL);
  
  //----------------------------------------------------------------
  // Delete pointers
  //delete pImageProcessor; //TODO: Does not work properly yet
  // TODO: delete pImage
  
  std::cout << "NeedleDetection destructed" << std::endl;
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::Init()
{
  this->DataManager->SetMRMLScene(this->GetMRMLScene());
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::Enter()
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::Exit()
{
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "RealTimeNeedleDetectionGUI: " << this->GetClassName() << "\n";
  os << indent << "Logic: " << this->GetLogic() << "\n";
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML Observers
  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
  {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
  }
  events->Delete();
  
  // register the volume node in the event observer  
  vtkMRMLNode *node = NULL; // TODO: is this OK? ->NodeSelector->GetselectedID
  vtkIntArray* nodeEvents = vtkIntArray::New();
  nodeEvents->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent); 
  vtkSetAndObserveMRMLNodeEventsMacro(node,pVolumeNode,nodeEvents);  //TODO:Steve What does this "node" do???
  nodeEvents->Delete();

  //----------------------------------------------------------------
  // GUI Observers
  this->pVolumeSelector->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pThresholdScale->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pStartButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pStopButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pShowNeedleButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTLEFT)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTRIGHT)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTANTERIOR)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTINFERIOR)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);
  this->pEntryPointButtonSet->GetWidget(PATIENTSUPERIOR)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*) this->GUICallbackCommand);

  this->AddLogicObservers();
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::RemoveGUIObservers ( )
{
  //----------------------------------------------------------------
  // MRML Observers
  this->MRMLObserverManager->RemoveObjectEvents(pVolumeNode);
  //this->MRMLObserverManager->RemoveObjectEvents(pSourceNode);
  //this->MRMLObserverManager->RemoveObjectEvents(pNeedleModelNode);
  //TODO: Remove Observers of MRMLSceneEvents
  
  //----------------------------------------------------------------
  // GUI Observers
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  this->pVolumeSelector->RemoveObservers(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*) this->GUICallbackCommand);
  if(this->pThresholdScale)
    this->pThresholdScale->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand*) this->GUICallbackCommand );
  if (this->pEntryPointButtonSet)
  {
    this->pEntryPointButtonSet->GetWidget(PATIENTLEFT)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    this->pEntryPointButtonSet->GetWidget(PATIENTRIGHT)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    this->pEntryPointButtonSet->GetWidget(PATIENTANTERIOR)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    this->pEntryPointButtonSet->GetWidget(PATIENTINFERIOR)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR)->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
  }
  if(this->pStartButton)
    this->pStartButton->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
  if(this->pStopButton)
    this->pStopButton->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
  if (this->pShowNeedleButton)
    this->pShowNeedleButton->RemoveObserver((vtkCommand*) this->GUICallbackCommand);
    
  this->RemoveLogicObservers();
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkRealTimeNeedleDetectionLogic::StatusUpdateEvent, (vtkCommand*) this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent, (vtkCommand*) this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::HandleMouseEvent(vtkSlicerInteractorStyle* style)
{
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::ProcessGUIEvents(vtkObject* caller, unsigned long event, void* callData)
{
  //TODO:check events first in these ifs, because it is probably faster than the downcast

  const char* eventName = vtkCommand::GetStringFromEventId(event);

  if(strcmp(eventName, "LeftButtonPressEvent") == 0)  // This is not used yet, since only mouse clicks on buttons are processed
  {
    vtkSlicerInteractorStyle* style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
  }
  
  else if (this->pVolumeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent && this->pVolumeSelector->GetSelected() != NULL) 
  { 
    pSourceNode = vtkMRMLVolumeNode::SafeDownCast(this->pVolumeSelector->GetSelected());
  }
  
  
  else if(this->pThresholdScale == vtkKWScaleWithEntry::SafeDownCast(caller) && event == vtkKWScale::ScaleValueChangedEvent) 
  {
    needleDetectionThreshold = this->pThresholdScale->GetValue();
  }
  
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTLEFT) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTLEFT)->GetSelectedState() == 1)
  { 
    needleOrigin = PATIENTLEFT;
  }
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTRIGHT) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTRIGHT)->GetSelectedState() == 1)
  {
    needleOrigin = PATIENTRIGHT; 
  }
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR)->GetSelectedState() == 1)
  {
    needleOrigin = PATIENTPOSTERIOR; 
  }
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTANTERIOR) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTANTERIOR)->GetSelectedState() == 1)
  {
    needleOrigin = PATIENTANTERIOR; 
  }
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTINFERIOR) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTINFERIOR)->GetSelectedState() == 1)
  {
    needleOrigin = PATIENTINFERIOR; 
  }
  else if (this->pEntryPointButtonSet->GetWidget(PATIENTSUPERIOR) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->pEntryPointButtonSet->GetWidget(PATIENTSUPERIOR)->GetSelectedState() == 1)
  {
    needleOrigin = PATIENTSUPERIOR; 
  }  
  
  else if(this->pStartButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
  {
    std::cout << "StartButton is pressed." << std::endl;
       
    //-----------------------------------------------------------------------------------------------------------------
    // Set the sourceNode and register it to the event observer | it gets unregistered when the StopButton is pressed
   // pSourceNode = vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->pScannerIDEntry->GetValue()));
    vtkMRMLNode* node = NULL; // TODO: is this OK?
    vtkIntArray* nodeEvents = vtkIntArray::New();
    nodeEvents->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent); 
    vtkSetAndObserveMRMLNodeEventsMacro(node,pSourceNode,nodeEvents);
    nodeEvents->Delete();     
      
    if(pSourceNode)
    { //-----------------------------------------------------------------------------------------------------------------
      // Create the volumeNode and the corresponding displayNode, which displays the detected needle   
      std::cout << "pSourceNode exists" << std::endl;   
      pVolumeNode = vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("VolumeNode"));
      if(pVolumeNode == NULL) // pVolumeNode does not exist yet
      {
        std::cout << "items == 0" << std::endl; 
        vtkMRMLScalarVolumeNode* pScalarNode = vtkMRMLScalarVolumeNode::New();
        pScalarNode->SetLabelMap(0);   // set the label map to grey scale
        pVolumeNode = pScalarNode;    
        if(pVolumeNode == NULL)
          std::cout << "ERROR! VolumeNode == NULL!" << std::endl;
        pVolumeNode->UpdateID("VolumeNode");
        pVolumeNode->SetName("VolumeNode");
        pVolumeNode->SetDescription("MRMLNode that displays the tracked needle and the original image");
        pVolumeNode->SetScene(this->GetMRMLScene()); 
        vtkMatrix4x4* matrix = vtkMatrix4x4::New();
        matrix->Element[0][0] = -1.0;
        matrix->Element[1][0] = 0.0;
        matrix->Element[2][0] = 0.0;
        matrix->Element[0][1] = 0.0;
        matrix->Element[1][1] = 1.0;
        matrix->Element[2][1] = 0.0;
        matrix->Element[0][2] = 0.0;
        matrix->Element[1][2] = 0.0;
        matrix->Element[2][2] = -1.0;
        matrix->Element[0][3] = 128.0;
        matrix->Element[1][3] = 128.0;
        matrix->Element[2][3] = 0.0;
        pVolumeNode->SetRASToIJKMatrix(matrix); // TODO: make this matrix generic! right now it is adapted to the scanner simulation
        matrix->Delete();        
        
        vtkMRMLScalarVolumeDisplayNode* pScalarDisplayNode = vtkMRMLScalarVolumeDisplayNode::New();
        pScalarDisplayNode->SetDefaultColorMap();   
        vtkMRMLScalarVolumeDisplayNode* pDisplayNode = pScalarDisplayNode;         
        this->GetMRMLScene()->AddNode(pDisplayNode);   
        pVolumeNode->SetAndObserveDisplayNodeID(pDisplayNode->GetID());        
        this->GetMRMLScene()->AddNode(pVolumeNode);
        pScalarNode->Delete();
        pScalarDisplayNode->Delete();
        pDisplayNode->Delete(); 
      }
      else //VolumeNode exists already
      {
        std::cerr << "VolumeNode exists already. Starting again" << std::endl;        
      }
    
      //--------------------------------------------------------------------------------------
      // Get the image variables
      vtkImageData* pImageData = ((vtkMRMLVolumeNode*) pSourceNode)->GetImageData();
      std::cerr << "got ImageData" << std::endl;
      pImageData->GetDimensions(imageDimensions);
      pImageData->GetSpacing(imageSpacing);
      pImageData->GetOrigin(imageOrigin);
      scalarSize = pImageData->GetScalarSize();
      //TODO: Do I need to get the scalarType, too?
      if(ROIpresent)
      {
        std::cout << "MRLMROINode exists" << std::endl;
        vtkCollection* collectionOfROINodes = this->GetMRMLScene()->GetNodesByName("MRMLROINode");
        int nItems = collectionOfROINodes->GetNumberOfItems();
        if(nItems > 0)
        {
          vtkMRMLROINode* pROINode = vtkMRMLROINode::SafeDownCast(collectionOfROINodes->GetItemAsObject(0));
          double center[3];
          double radius[3];
          // the origin of the transform is always in the center of the imge: (Dimension*spacing = fov) / 2
          double fovI = imageDimensions[0] * imageSpacing[0] / 2.0;
          double fovJ = imageDimensions[1] * imageSpacing[1] / 2.0;
          double fovK = imageDimensions[2] * imageSpacing[2] / 2.0;         
          pROINode->GetRadiusXYZ(radius);
          pROINode->GetXYZ(center);
          this->pXLowerEntry->SetValueAsInt((int) ((-center[0]) - radius[0] + fovI)); // negative center point for the x-axis, because the ROIMRMLNode coordinates are in RAS (LR direction of X-axis), 
          this->pXUpperEntry->SetValueAsInt((int) ((-center[0]) + radius[0] + fovI)); // but the slicer axial and coronal view, which are used as reference, switch the direction (RL direction of X-axis)
          this->pYLowerEntry->SetValueAsInt((int) (center[1] - radius[1] + fovJ));
          this->pYUpperEntry->SetValueAsInt((int) (center[1] + radius[1] + fovJ));  
          this->pZLowerEntry->SetValueAsInt((int) (center[2] - radius[2] + fovK));
          this->pZUpperEntry->SetValueAsInt((int) (center[2] + radius[2] + fovK));
        }         
      }
      currentXLowerBound = initialXLowerBound = this->pXLowerEntry->GetValueAsInt();
      currentXUpperBound = initialXUpperBound = this->pXUpperEntry->GetValueAsInt();
      currentYLowerBound = initialYLowerBound = this->pYLowerEntry->GetValueAsInt();
      currentYUpperBound = initialYUpperBound = this->pYUpperEntry->GetValueAsInt();  
      currentXImageRegionSize                 = currentXUpperBound - currentXLowerBound;
      currentYImageRegionSize                 = currentYUpperBound - currentYLowerBound;  
      started = 1; // start checking for changes in pSourceNode to update pVolumeNode     
      std::cerr << "Start checking for changes" << std::endl;
    }
    else // no Scanner node selected from MRMLScene
      std::cerr << "ERROR! No Scanner detected. RealTimeNeedleDetection did not start." << std::endl;
  }
  
  else if (this->pStopButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
  {    
    std::cerr << "StopButton is pressed." << std::endl;
    // unregister the observer of the ScannerMRMLNode
    if(this->pSourceNode)
    {
      this->MRMLObserverManager->RemoveObjectEvents(pSourceNode);
      this->pSourceNode = NULL;  
    }
    
    // Set the VolumeNode to NULL, because it should not get used while started==0
    // It will not get deleted, because it is still referenced in the MRMLScene
    if(this->pVolumeNode)
    {
      //this->GetMRMLScene()->RemoveNodeNoNotify((vtkMRMLNode*) pVolumeNode);
      this->pVolumeNode = NULL; 
    }
    //Do not set pNeedleModelNode to NULL, because it might still be accessed
    
    started = 0; // Stop checking MRLM events of pSourceNode 
  }
  
  else if (this->pShowNeedleButton == vtkKWCheckButton::SafeDownCast(caller) && event == vtkKWCheckButton::SelectedStateChangedEvent )
  {
    std::cout << "ShowNeedleButton is pressed." << std::endl;
    showNeedle = this->pShowNeedleButton->GetSelectedState(); 
    if(showNeedle)
    {
      //-------------------------------------------------------------------
      // Create a MRMLModelNode that displays the needle
      if(!pNeedleModelNode)       
      {
        pNeedleModelNode = vtkMRMLModelNode::New();
        pNeedleModelNode->SetName("NeedleModel");
        pNeedleModelNode->UpdateID("NeedleModel");
        pNeedleModelNode->SetScene(this->GetMRMLScene());
        pNeedleModelNode->SetHideFromEditors(0);
        
        this->GetMRMLScene()->AddNode(pNeedleModelNode);
        std::cout << "NeedleModelNode added" << std::endl;
        MakeNeedleModel();        
      }
      //------------------------------------------------------------------------------------------------
      // Create a TransformNode that changes according to the found needle and will be observed by the ModelNode to display the needle  
      if(!pNeedleTransformNode) // If the NeedleTransformNode doesn't exist yet -> make a new one 
      {
        pNeedleTransformNode = vtkMRMLLinearTransformNode::New();
        pNeedleTransformNode->SetName("NeedleTransform");
        pNeedleTransformNode->UpdateID("NeedleTransform");
        pNeedleTransformNode->SetScene(this->GetMRMLScene());
        pNeedleTransformNode->SetHideFromEditors(0);
        vtkMatrix4x4* transform = vtkMatrix4x4::New(); // vtkMatrix is initialized with the identity matrix
        //TODO:Steve Can I delete this transform after I set it to the Node?
        pNeedleTransformNode->ApplyTransform(transform);  // SetAndObserveMatrixTransformToParent called in this function
        
        this->GetMRMLScene()->AddNode(pNeedleTransformNode);         
        std::cout << "NeedleTransformNode added" << std::endl;
        
        pNeedleModelNode->SetAndObserveTransformNodeID(pNeedleTransformNode->GetID());
        pNeedleModelNode->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
      }
      //------------------------------------------------------------------------------------------------
      // Create a TransformNode that is the normal of the scan plane  
      if(!pScanPlaneNormalNode) // If the NeedleTransformNode doesn't exist yet -> make a new one 
      {
        pScanPlaneNormalNode = vtkMRMLLinearTransformNode::New();
        pScanPlaneNormalNode->SetName("ScanPlaneNormal");
        pScanPlaneNormalNode->UpdateID("ScanPlaneNormal");
        pScanPlaneNormalNode->SetScene(this->GetMRMLScene());
        pScanPlaneNormalNode->SetHideFromEditors(0);
        vtkMatrix4x4* transform = vtkMatrix4x4::New(); // vtkMatrix is initialized with the identity matrix
        pScanPlaneNormalNode->ApplyTransform(transform);  // SetAndObserveMatrixTransformToParent called in this function
        
        this->GetMRMLScene()->AddNode(pScanPlaneNormalNode);         
        std::cout << "ScanPlaneNormalNode added" << std::endl;
      }
      vtkMRMLDisplayNode* pNeedleDisplay = pNeedleModelNode->GetDisplayNode(); //TODO:Steve Can I delete this displayNode later on? 
      if(pNeedleDisplay)
      {
        pNeedleDisplay->SetVisibility(1);
        pNeedleModelNode->Modified();
        this->GetMRMLScene()->Modified();
      }
      else
        std::cerr << "Error! DisplayNode for needle does not exist!" << std::endl;
      
    }
    else // !showNeedle
    {
      if(pNeedleModelNode) // if a node exists, set visibility to 0
      {
        vtkMRMLDisplayNode* pNeedleDisplay = pNeedleModelNode->GetDisplayNode(); //TODO:Steve Can I delete this displayNode later on? 
        if(pNeedleDisplay)
        {
          pNeedleDisplay->SetVisibility(0);
          pNeedleModelNode->Modified();
          this->GetMRMLScene()->Modified();
        }
        else
          std::cerr << "Error! DisplayNode for needle does not exist!" << std::endl;
      }
      else 
      {
        std::cout << "Error! Node does not exist!" << std::endl;
         // If a node doesn't exist, do nothing! However, this should never get called
      }
    }
  }
      
  UpdateGUI(); // enable or disable options for the user
} 

void vtkRealTimeNeedleDetectionGUI::DataCallback(vtkObject* caller, unsigned long eid, void* clientData, void* callData)
{
  vtkRealTimeNeedleDetectionGUI* self = reinterpret_cast<vtkRealTimeNeedleDetectionGUI*> (clientData);
  vtkDebugWithObjectMacro(self, "In vtkRealTimeNeedleDetectionGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::ProcessLogicEvents(vtkObject* caller, unsigned long event, void* callData)
{
  if (this->GetLogic() == vtkRealTimeNeedleDetectionLogic::SafeDownCast(caller))
  {
    if (event == vtkRealTimeNeedleDetectionLogic::StatusUpdateEvent)
    {
      //this->UpdateDeviceStatus();
    }
  }
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData )
{
  //TODO: If new mrmlNode added -> pScannerIDEntry=new mrml node 
  //TODO: if MRMLNode deleted -> pScannerIDEntry=""
  
  if(event == vtkMRMLScene::SceneCloseEvent) //TODO: Do I actually catch this event?
  {
    if (this->pShowNeedleButton != NULL && this->pShowNeedleButton->GetSelectedState())
      this->pShowNeedleButton->SelectedStateOff();
  }
   
  else if(vtkMRMLScene::SafeDownCast(caller) == this->GetMRMLScene() && event == vtkMRMLScene::NodeAddedEvent)
  {
    vtkMRMLNode* node =  reinterpret_cast<vtkMRMLNode*> (callData);
    if(node->IsA("vtkMRMLROINode"))
    {
      ROIpresent++;
      UpdateGUI();
    }
  }
  
  else if(vtkMRMLScene::SafeDownCast(caller) == this->GetMRMLScene() && event == vtkMRMLScene::NodeRemovedEvent)
  {
    vtkMRMLNode* node =  reinterpret_cast<vtkMRMLNode*> (callData);
    if(node->IsA("vtkMRMLROINode"))
    {
      ROIpresent--;
      UpdateGUI();
    }
  }
  
  else if(started && (lastModified != this->pSourceNode->GetMTime()))
  {
    lastModified = this->pSourceNode->GetMTime(); // This prevents unnecessarily issued ImageDataModifiedEvents from beging processed 
    std::cout << started << ":MRMLEvent processing while started";
    if((this->pSourceNode == vtkMRMLVolumeNode::SafeDownCast(caller)) && (event == vtkMRMLVolumeNode::ImageDataModifiedEvent))
    {
      std::cout << "-SourceNode Event" << std::endl;
      clock_t begin = clock();
      double points[4]; // Array that contains 2 points of the needle transform (x1,y1,x2,y2)
                        // points[0],points[1] is the point of the needle entering the image
      points[0] = 0.0;
      points[1] = 0.0;
      points[2] = 0.0;
      points[3] = 0.0;
      //------------------------------------------------------------------------------------------------
      // Crop out the imageRegion specified by the X- and Y-boundaries
      vtkImageData* pImageData  = vtkImageData::New();        
      pImageData->DeepCopy(((vtkMRMLVolumeNode*) pSourceNode)->GetImageData());
      unsigned char* pImageRegionInput = new unsigned char[currentXImageRegionSize*currentYImageRegionSize*scalarSize];  
      unsigned char* pImageRegion1 = new unsigned char[currentXImageRegionSize*currentYImageRegionSize*scalarSize];
      unsigned char* pImageRegion2 = new unsigned char[currentXImageRegionSize*currentYImageRegionSize*scalarSize]; 
      GetImageRegion(pImageData, pImageRegionInput);
  
      //--------------------------------------------------------------------------------------------------
      // Use the ImageProcessor to alter the region of interest and calculate the needle position
      // In the ImageProcessor ITK image segmentation/processing classse are used 
      pImageProcessor->SetImage((void*) pImageRegionInput, currentXImageRegionSize, currentYImageRegionSize, scalarSize, imageSpacing, imageOrigin);
      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/1-Input.png",INPUT);
      //pImageProcessor->PassOn();
      
      pImageProcessor->DilateAndErode(false, true, 2, 3); //2 == erode value, 2 == dilate value
      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/2-DilateAndErode.png",TMP);
      
      pImageProcessor->Threshold(true, false, MAX, 0, (int) needleDetectionThreshold);
      pImageProcessor->GetImage((void*) pImageRegion1);
      SetImageRegion(pImageData, pImageRegion1, false);  // write the region of interest with the drawn needle in it below the MRI image received from the scanner
      //TODO:DELETE pImageRegion!!
      
      pImageProcessor->Threshold(true, true, MAX, 0, (int) needleDetectionThreshold);
      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/3-Threshold.png",TMP);
      pImageProcessor->BinaryThinning(true, true);
      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/4-Thinning.png",TMP);
//      pImageProcessor->SobelFilter(true, true, 1);
//      pImageProcessor->Write("/projects/mrrobot/goerlitz/test/sobel.png",TMP);
      
   //   pImageProcessor->LaplacianRecursiveGaussian(false,true);  //makes the line white -> no inversion in houghtransform needed
   //   pImageProcessor->Write("/projects/mrrobot/goerlitz/test/LaplacianRecursiveGaussian.png",TMP);
  //    pImageProcessor->SobelFilter(true,true,1);
  //    pImageProcessor->Threshold(true,true,MAX,0,10000);
  //    pImageProcessor->SobelEdgeDetection(false, true);
  //    pImageProcessor->SobelEdgeDetection(true, false);
       //pImageProcessor->GradientMagnitude(false,true);
       //pImageProcessor->Threshold(false,true,0,0,20000);
       //pImageProcessor->Threshold(true,true,MAX,18000,MAX);
       //pImageProcessor->Threshold(true,true,MAX,0,1);
        pImageProcessor->HoughTransformation(true, points);
        pImageProcessor->Write("/projects/mrrobot/goerlitz/test/output.png",OUTPUT);     
       //pImageProcessor->CannyEdgeDetection(true,false);           
      std::cout << "ImageRegion processed" << std::endl;    
      pImageProcessor->GetImage((void*) pImageRegion2);
      SetImageRegion(pImageData, pImageRegion2, true);  // write the region of interest with the drawn needle in it above the MRI image received from the scanner
      //pImageRegion2->Delete();  //TODO:DELETE pImageRegion!!
      pVolumeNode->SetAndObserveImageData(pImageData); //automatically removes old observer and sets modified flag, if new image is different  TODO: Does it also delete the old observer?
      pImageData->Delete();
      
      if(showNeedle)
      {   
        //------------------------------------------------------------------------------------------------
        // Retrieve the line from the houghtransformation
        if((points[0] == 0) && (points[1] == 0) && (points[2] == 0) && (points[3] == 0)) 
          std::cerr << "Error! Points of line are all 0.0! No needle detected!" << std::endl;
        else // if everything is ok
        {//TODO: make this generic!! Right now I assume the needle enters from the patientleft                  
          points[0] += currentXLowerBound;
          points[1] += currentYLowerBound;
          points[2] += currentXLowerBound;
          points[3] += currentYLowerBound;
          std::cout << "bounds: " << currentXLowerBound << "|" << currentYLowerBound << "|" << currentXUpperBound << "|" <<  currentYUpperBound << std::endl;
          std::cout << "points: " << points[0] << "|" << points[1] << "|" << points[2] << "|" <<  points[3] << std::endl;
          
          double vector[2]; //vector from Tip of the needle to the end
          vector[0] = points[0] - points[2];
          vector[1] = points[1] - points[3];
          double length = sqrt(vector[0] * vector[0] + vector[1] * vector[1]);
          std::cout << "length: " << length << std::endl;
          double angle = (atan2(points[1]-points[3],points[0]-points[2]))*180/PI; 
          std::cout << "angle: " << angle << std::endl;
          
          //-------------------------------------------------------------------------------------------
          // make the needle transform fit the line detected in the image
          // the origin of the transform is always in the center of the imge: (Dimension*spacing = fov) / 2
          double fovI = imageDimensions[0] * imageSpacing[0] / 2.0;
          double fovJ = imageDimensions[1] * imageSpacing[1] / 2.0;        
          // fovK is not needed, because the images are 2D only
          double translationLR = 0;   //(X-axis)
          double translationPA = 0;   //(Y-axis)
          double translationIS = 0;   //(Z-axis)
          
          vtkTransform* transform = vtkTransform::New(); // initialized with identity matrix
          transform->Identity();
          transform->PostMultiply(); // = global coordinate system
          switch (needleOrigin) {
            case PATIENTLEFT: //and axial! TODO: Take care of differences in scan plane
            {              
              transform->RotateZ(90); // rotate to have the cylinder pointing from right to left
              transform->RotateZ(-angle);
              translationLR = -(points[2]-fovI); // negative because positive X-axis direction in RAS-coordinates is to the patient right, but in the slicer axial and coronal view it is to the patient left 
              translationPA = points[3]-fovJ;                
              break;
            }
            case PATIENTRIGHT:
            {
              //transform->RotateZ(90); // rotate to have the cylinder pointing from right to left
              //transform->RotateZ(angle);
              //translationLR = -(points[0]-fovI); 
              //translationPA = points[1]-fovJ; 
              break;
            }
            case PATIENTPOSTERIOR:
            {
              //no RotateZ because the transform points in the right direction
              //transform->RotateZ(angle);
              //translationLR = -(points[0]-fovI); 
              //translationPA = points[1]-fovJ; 
              break;
            }
            case PATIENTANTERIOR:
            {
              //transform->RotateZ(-180); // rotate to have the cylinder pointing from anterior to posterior
              //transform->RotateZ(angle);
              //translationLR = -(points[0]-fovI); 
              //translationPA = points[1]-fovJ; 
              break;
            }
            case PATIENTINFERIOR:
            {
              //transform->RotateX(90); // rotate to have the cylinder pointing from inferior to superior
              //transform->Rotate(angle);
              //translationLR = -(points[0]-fovI); 
              //translationPA = points[1]-fovJ; 
              break;
            }
            case PATIENTSUPERIOR:
            {
              //transform->RotateX(-90); // rotate to have the cylinder pointing from superior to inferior
              //transform->Rotate(angle);
              //translationLR = -(points[0]-fovI); 
              //translationPA = points[1]-fovJ; 
              break;
            }
            default:
              std::cerr << "ERROR! needleOrigin has an unknown value!" << std::endl;
              break;
          } //end switch          
          transform->Translate(translationLR, translationPA, translationIS);    
                
          vtkMatrix4x4* transformToParent1 = pNeedleTransformNode->GetMatrixTransformToParent();
          transformToParent1->DeepCopy(transform->GetMatrix()); // This calls the modified event
          transform->Translate(-translationLR, -translationPA, -translationIS);
          transform->RotateZ(90); //TODO: this might include errors
          vtkMatrix4x4* transformToParent2 = pScanPlaneNormalNode->GetMatrixTransformToParent();
          transformToParent2->DeepCopy(transform->GetMatrix()); // This calls the modified event
          transform->Delete();
        }
  
            
//        currentXLowerBound = (int) points[2] - 20;
//        //currentYLowerBound = (int) points[3] - 10;
//        currentXUpperBound = (int) points[0];        
//        //currentYUpperBound = (int) points[1] + 10;
//        std::cout << "bounds: " << currentXLowerBound << "|" << currentYLowerBound << "|" << currentXUpperBound << "|" <<  currentYUpperBound << std::endl;
//        currentXImageRegionSize = currentXUpperBound - currentXLowerBound;
//        currentYImageRegionSize = currentYUpperBound - currentYLowerBound;       
       

      }  
    clock_t end = clock();
    cout << "               Time elapsed: " << double(diffclock(end,begin)) << " ms"<< endl;  
    }
    started++;  //I also use started as a counter of the frames -> TODO:Check if started gets bigger than int
  }
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
  {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), this->TimerInterval, this, "ProcessTimerEvents");        
  }
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::BuildGUI()
{
  this->UIPanel->AddPage("RealTimeNeedleDetection", "RealTimeNeedleDetection", NULL);

  BuildGUIForHelpFrame();
  BuildGUIForGeneralParameters();
}


void vtkRealTimeNeedleDetectionGUI::BuildGUIForHelpFrame ()
{
  const char* help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:RealTimeNeedleDetection</a> for details.";
  const char* about =
    "This module is designed and implemented by Roland Goerlitz for the Brigham and Women's Hospital.\nThis work is supported by Junichi Tokuda and Nobuhiko Hata.";
    
  vtkKWWidget* page = this->UIPanel->GetPageWidget("RealTimeNeedleDetection");
  this->BuildHelpAndAboutFrame(page, help, about);
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::BuildGUIForGeneralParameters()
{
  vtkSlicerApplication* app = (vtkSlicerApplication*) this->GetApplication();
  vtkKWWidget* page = this->UIPanel->GetPageWidget("RealTimeNeedleDetection");
  
  vtkSlicerModuleCollapsibleFrame* parentFrame = vtkSlicerModuleCollapsibleFrame::New();

  parentFrame->SetParent(page);
  parentFrame->Create();
  parentFrame->SetLabelText("needle detection parameters");
  app->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               parentFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // control frame
  vtkKWFrameWithLabel* controlFrame = vtkKWFrameWithLabel::New();
  controlFrame->SetParent(parentFrame->GetFrame());
  controlFrame->Create();
  controlFrame->SetLabelText("Needle detection controls");
  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", controlFrame->GetWidgetName());

  // ----------------------------------------------------               
  // scanner ID
  this->pVolumeSelector = vtkSlicerNodeSelectorWidget::New();
  this->pVolumeSelector->SetNodeClass("vtkMRMLVolumeNode", NULL, NULL, NULL);
  this->pVolumeSelector->SetParent(controlFrame->GetFrame() );
  this->pVolumeSelector->Create();
  this->pVolumeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->pVolumeSelector->UpdateMenu();

  this->pVolumeSelector->SetBorderWidth(2);
  this->pVolumeSelector->SetLabelText( "Scanner MRMLNode: ");
  this->pVolumeSelector->SetBalloonHelpString("select an input scanner from the current MRML scene.");
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
                this->pVolumeSelector->GetWidgetName());
   
  // ------------------------------------------------------
  // Needle entering direction button set
  vtkKWFrame *buttonSetFrame = vtkKWFrame::New();
  buttonSetFrame->SetParent(controlFrame->GetFrame());
  buttonSetFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                buttonSetFrame->GetWidgetName());

  vtkKWLabel *buttonSetLabel = vtkKWLabel::New();
  buttonSetLabel->SetParent(buttonSetFrame);
  buttonSetLabel->Create();
  buttonSetLabel->SetWidth(15);
  buttonSetLabel->SetText("Entering direction \n of the needle: ");

  this->pEntryPointButtonSet = vtkKWRadioButtonSet::New();
  this->pEntryPointButtonSet->SetParent(buttonSetFrame);
  this->pEntryPointButtonSet->Create();
  this->pEntryPointButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->pEntryPointButtonSet->UniformColumnsOn();
  this->pEntryPointButtonSet->UniformRowsOn();  

  this->pEntryPointButtonSet->AddWidget(PATIENTLEFT);
  this->pEntryPointButtonSet->GetWidget(PATIENTLEFT)->SetText("Left");
  this->pEntryPointButtonSet->AddWidget(PATIENTRIGHT);
  this->pEntryPointButtonSet->GetWidget(PATIENTRIGHT)->SetText("Right");
  this->pEntryPointButtonSet->AddWidget(PATIENTPOSTERIOR);
  this->pEntryPointButtonSet->GetWidget(PATIENTPOSTERIOR)->SetText("Posterior");
  this->pEntryPointButtonSet->AddWidget(PATIENTANTERIOR);
  this->pEntryPointButtonSet->GetWidget(PATIENTANTERIOR)->SetText("Anterior");
  this->pEntryPointButtonSet->AddWidget(PATIENTINFERIOR);
  this->pEntryPointButtonSet->GetWidget(PATIENTINFERIOR)->SetText("Inferior");
  this->pEntryPointButtonSet->AddWidget(PATIENTSUPERIOR);
  this->pEntryPointButtonSet->GetWidget(PATIENTSUPERIOR)->SetText("Superior");
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              buttonSetLabel->GetWidgetName() , this->pEntryPointButtonSet->GetWidgetName());
  
  this->pEntryPointButtonSet->GetWidget(PATIENTLEFT)->SelectedStateOn();  //default, always needs to correspond to the member variable needleOrigin
  this->pEntryPointButtonSet->EnabledOn();
  buttonSetFrame->Delete();
  
  // -----------------------------------------
  // push buttons
  vtkKWFrame* buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent(controlFrame->GetFrame());
  buttonFrame->Create();
  app->Script("pack %s -fill both -expand true", buttonFrame->GetWidgetName());
                
  this->pStartButton = vtkKWPushButton::New();
  this->pStartButton->SetParent(buttonFrame);
  this->pStartButton->Create();
  this->pStartButton->SetText("Start");
  this->pStartButton->SetWidth(15);

  this->pStopButton = vtkKWPushButton::New();
  this->pStopButton->SetParent(buttonFrame);
  this->pStopButton->Create();
  this->pStopButton->SetText("Stop");
  this->pStopButton->SetWidth(15);
  this->pStopButton->EnabledOff();
  
  this->pShowNeedleButton = vtkKWCheckButton::New();
  this->pShowNeedleButton->SetParent(buttonFrame);
  this->pShowNeedleButton->Create();
  this->pShowNeedleButton->SelectedStateOff();
  this->pShowNeedleButton->SetText("Show Needle");
  this->pShowNeedleButton->EnabledOff();

  this->Script("pack %s %s %s %s -side left -padx 2 -pady 2", 
               this->pStartButton->GetWidgetName(),
               this->pStopButton->GetWidgetName(),
               this->pShowNeedleButton->GetWidgetName(),
               this->pShowNeedleButton->GetWidgetName());
                          
  buttonFrame->Delete();
  
    // ------------------------------------------------------
  // threshold slider button  
  vtkKWFrame* sliderFrame = vtkKWFrame::New();
  sliderFrame->SetParent(controlFrame->GetFrame());
  sliderFrame->Create();
  app->Script("pack %s -fill both -expand true", sliderFrame->GetWidgetName());
  
  this->pThresholdScale = vtkKWScaleWithEntry::New();
  this->pThresholdScale->SetParent(controlFrame->GetFrame());
  this->pThresholdScale->SetLabelText("Threshold");
  this->pThresholdScale->Create();
  this->pThresholdScale->GetScale()->SetLength(180); 
  this->pThresholdScale->SetRange(0,MAX);
  this->pThresholdScale->SetResolution(10);
  //TODO:Steve can I constrict the values to integer?  -> floor?
  this->pThresholdScale->SetValue(DEFAULTTHRESHOLD);
  
  app->Script("pack %s -side left -padx 2 -pady 2", this->pThresholdScale->GetWidgetName());
  
  sliderFrame->Delete();  //TODO:Steve put slider above buttons
  
  //TODO: this->script? app->script?
  
  controlFrame->Delete();  
  
  // -----------------------------------------
  // Coordinates frame
  vtkKWFrameWithLabel* coordinatesFrame = vtkKWFrameWithLabel::New();
  coordinatesFrame->SetParent(parentFrame->GetFrame());
  coordinatesFrame->Create();
  coordinatesFrame->SetLabelText("Coordinates");
  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", coordinatesFrame->GetWidgetName() );

  // -----------------------------------------
  // Boundary edit fields TODO: Check for bounds! 0 up to imageDimensions is possible
  //TODO:Steve How do I check suff that gets typed in?
  vtkKWFrame* descriptionFrame = vtkKWFrame::New();
  descriptionFrame->SetParent(coordinatesFrame->GetFrame());
  descriptionFrame->Create();
  app->Script ( "pack %s -fill both -expand true", descriptionFrame->GetWidgetName());
  vtkKWTextWithHyperlinksWithScrollbars* descriptionText = vtkKWTextWithHyperlinksWithScrollbars::New();  //TODO:Steve what to use if I only need text?
  descriptionText->SetParent(descriptionFrame);
  descriptionText->Create(); 
  descriptionText->SetHorizontalScrollbarVisibility(0);
  descriptionText->SetVerticalScrollbarVisibility(0);
  descriptionText->GetWidget()->SetReliefToFlat();
  descriptionText->GetWidget()->SetWrapToWord();
  descriptionText->GetWidget()->QuickFormattingOn();
  descriptionText->SetText("Set the boundaries for the part of the image where the needle detection will start. They can be set manually or with an MRMLROINode. If a MRLMROINode exists, all the following values will be overriden." );
  //Important that Read only after SetText otherwise it doesn't work
  descriptionText->GetWidget()->ReadOnlyOn();
  app->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 4", descriptionText->GetWidgetName());
  descriptionText->Delete();
  descriptionFrame->Delete();    
    
  vtkKWFrame* xFrame = vtkKWFrame::New();
  xFrame->SetParent(coordinatesFrame->GetFrame());
  xFrame->Create();
  app->Script ( "pack %s -fill both -expand true", xFrame->GetWidgetName());
  vtkKWLabel* xLabel = vtkKWLabel::New();
  xLabel->SetParent(xFrame);
  xLabel->Create();
  xLabel->SetWidth(23);
  xLabel->SetText("Boundaries in LR-direction: ");

  this->pXLowerEntry = vtkKWEntry::New();
  this->pXLowerEntry->SetParent(xFrame);
  this->pXLowerEntry->Create();
  this->pXLowerEntry->SetWidth(7);
  this->pXLowerEntry->SetValueAsInt(100);
              
  this->pXUpperEntry = vtkKWEntry::New();
  this->pXUpperEntry->SetParent(xFrame);
  this->pXUpperEntry->Create();
  this->pXUpperEntry->SetWidth(7);
  this->pXUpperEntry->SetValueAsInt(180);

  app->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              xLabel->GetWidgetName(), this->pXLowerEntry->GetWidgetName(), this->pXUpperEntry->GetWidgetName());
  xLabel->Delete();
  xFrame->Delete();
  
  vtkKWFrame* yFrame = vtkKWFrame::New();
  yFrame->SetParent(coordinatesFrame->GetFrame());
  yFrame->Create();
  app->Script ("pack %s -fill both -expand true", yFrame->GetWidgetName());
  vtkKWLabel* yLabel = vtkKWLabel::New();
  yLabel->SetParent(yFrame);
  yLabel->Create();
  yLabel->SetWidth(23);
  yLabel->SetText("Boundaries in PA-direction: ");

  this->pYLowerEntry = vtkKWEntry::New();
  this->pYLowerEntry->SetParent(yFrame);
  this->pYLowerEntry->Create();
  this->pYLowerEntry->SetWidth(7);
  this->pYLowerEntry->SetValueAsInt(125);
              
  this->pYUpperEntry = vtkKWEntry::New();
  this->pYUpperEntry->SetParent(yFrame);
  this->pYUpperEntry->Create();
  this->pYUpperEntry->SetWidth(7);
  this->pYUpperEntry->SetValueAsInt(145);

  app->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              yLabel->GetWidgetName(), this->pYLowerEntry->GetWidgetName(), this->pYUpperEntry->GetWidgetName());
  yLabel->Delete();
  yFrame->Delete();
  
  vtkKWFrame* zFrame = vtkKWFrame::New();
  zFrame->SetParent(coordinatesFrame->GetFrame());
  zFrame->Create();
  app->Script ("pack %s -fill both -expand true", zFrame->GetWidgetName());
  vtkKWLabel* zLabel = vtkKWLabel::New();
  zLabel->SetParent(zFrame);
  zLabel->Create();
  zLabel->SetWidth(23);
  zLabel->SetText("Boundaries for Z (!not yet used!): ");

  this->pZLowerEntry = vtkKWEntry::New();
  this->pZLowerEntry->SetParent(zFrame);
  this->pZLowerEntry->Create();
  this->pZLowerEntry->SetWidth(7);
  this->pZLowerEntry->SetValueAsInt(1);
              
  this->pZUpperEntry = vtkKWEntry::New();
  this->pZUpperEntry->SetParent(zFrame);
  this->pZUpperEntry->Create();
  this->pZUpperEntry->SetWidth(7);
  this->pZUpperEntry->SetValueAsInt(1);

  app->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              zLabel->GetWidgetName(), this->pZLowerEntry->GetWidgetName(), this->pZUpperEntry->GetWidgetName());
  zLabel->Delete();
  zFrame->Delete();
  
  coordinatesFrame->Delete();
  
  parentFrame->Delete();
  UpdateGUI();
}


//----------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::UpdateAll()
{
  UpdateGUI();
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::UpdateGUI()
{
  if(started)
  {
    this->pVolumeSelector->EnabledOff();
    this->pEntryPointButtonSet->EnabledOff();
    this->pStartButton->EnabledOff();
    this->pStopButton->EnabledOn();
    this->pShowNeedleButton->EnabledOn();  //initially this Button is disabled until the needle detection is started for the first time
    this->pXLowerEntry->EnabledOff();
    this->pXUpperEntry->EnabledOff();
    this->pYLowerEntry->EnabledOff();
    this->pYUpperEntry->EnabledOff();
    this->pZLowerEntry->EnabledOff();
    this->pZUpperEntry->EnabledOff();
  }
  else if(started == 0)
  {
    this->pVolumeSelector->EnabledOn();
    this->pEntryPointButtonSet->EnabledOn();
    this->pStartButton->EnabledOn();
    this->pStopButton->EnabledOff();  
    if(ROIpresent)
    {
      this->pXLowerEntry->EnabledOff();
      this->pXUpperEntry->EnabledOff();
      this->pYLowerEntry->EnabledOff();
      this->pYUpperEntry->EnabledOff();
      this->pZLowerEntry->EnabledOff();
      this->pZUpperEntry->EnabledOff();
    }
    else 
    {
      this->pXLowerEntry->EnabledOn();
      this->pXUpperEntry->EnabledOn();
      this->pYLowerEntry->EnabledOn();
      this->pYUpperEntry->EnabledOn();
      this->pZLowerEntry->EnabledOn();
      this->pZUpperEntry->EnabledOn();
    }
  }
}

// Function to evoke changes in the image in every itaration | not used anymore
void vtkRealTimeNeedleDetectionGUI::ProcessImage(vtkImageData* pImageData, int i)
{
  unsigned char* pImage = (unsigned char*) pImageData->GetScalarPointer();
  for (int j = 50000; j< 50000+i*500; j++)
    if((int) pImage[j] != 0)
      pImage[j] = 0;    
  std::cout << "Image processed" << std::endl;      
}

//-----------------------------------------------------------------------------
// hard copy the region of interest of the original image byte by byte
void vtkRealTimeNeedleDetectionGUI::GetImageRegion(vtkImageData* pImageData, unsigned char* pImageRegion)
{
  unsigned char* pImage = (unsigned char*) pImageData->GetScalarPointer(); 
  int j = 0;
  for(long i = 0; i < (imageDimensions[0] * imageDimensions[1] * scalarSize) ; i++)
  {
    if((i >= currentYLowerBound*imageDimensions[1]*scalarSize) && (i < currentYUpperBound*imageDimensions[1]*scalarSize))  // Y-axis restrictions
    {
      if((currentXLowerBound*scalarSize <= (i%(imageDimensions[0]*scalarSize))) && (currentXUpperBound*scalarSize >  (i%(imageDimensions[0]*scalarSize))))
      {            
        pImageRegion[j] = pImage[i];
        j++;
      }
    }
  }
}

//--------------------------------------------------------------------------
// hard copy the region of interest back to the imageRegion byte by byte
void vtkRealTimeNeedleDetectionGUI::SetImageRegion(vtkImageData* pImageData, unsigned char* pImageRegion, bool above)
{
  unsigned char* pImage = (unsigned char*) pImageData->GetScalarPointer();
  int positionInMessageImage = 0;
  for(long i = 0; i <= currentXImageRegionSize * currentYImageRegionSize * scalarSize; i++)
  {
    positionInMessageImage = currentXLowerBound*scalarSize + (i%(currentXImageRegionSize*scalarSize)) + (i/(currentXImageRegionSize*scalarSize))*imageDimensions[0]*scalarSize;
    if(above) //the position of the region of interest is above the original image
      pImage[positionInMessageImage+10*imageDimensions[0]*scalarSize] = pImageRegion[i];      
    else //the position of the region of interest is below the original image
      pImage[positionInMessageImage+(imageDimensions[1]-currentYImageRegionSize-10)*imageDimensions[0]*scalarSize] = pImageRegion[i];
  }    
}

//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionGUI::MakeNeedleModel()
{  
  int heightOfNeedle = 80;
  vtkMRMLModelDisplayNode* pNeedleModelDisplay = vtkMRMLModelDisplayNode::New();
  GetMRMLScene()->AddNode(pNeedleModelDisplay);
  pNeedleModelDisplay->SetScene(this->GetMRMLScene());
  pNeedleModelNode->SetAndObserveDisplayNodeID(pNeedleModelDisplay->GetID());
  
  // Cylinder represents the needle stick
  vtkCylinderSource* pCylinder = vtkCylinderSource::New();
  pCylinder = vtkCylinderSource::New();
  pCylinder->SetRadius(1);
  pCylinder->SetHeight(heightOfNeedle);
  pCylinder->SetCenter(0, 0, 0);
  pCylinder->Update();

  // Rotate cylinder
  vtkTransformPolyDataFilter *tFilter = vtkTransformPolyDataFilter::New();
  vtkTransform* trans = vtkTransform::New();
  trans->Translate(0.0, heightOfNeedle/2, 0.0);
  trans->Update();
  tFilter->SetInput(pCylinder->GetOutput());
  tFilter->SetTransform(trans);
  tFilter->Update();
  
  // Sphere represents the needle tip 
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(3.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();
  
  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(sphere->GetOutput());
  //apd->AddInput(cylinder->GetOutput());
  apd->AddInput(tFilter->GetOutput());
  apd->Update();
  
  pNeedleModelNode->SetAndObservePolyData(apd->GetOutput());
  
  double color[3];
  color[0] = 0.0; // R
  color[1] = 1.0; // G
  color[2] = 0.0; // B
  pNeedleModelDisplay->SetPolyData(pNeedleModelNode->GetPolyData());
  pNeedleModelDisplay->SetColor(color);
  
  trans->Delete();
  tFilter->Delete();
  sphere->Delete();
  pCylinder->Delete();
  apd->Delete();
  
  pNeedleModelDisplay->Delete();
  std::cout << "made NeedleModel" << std::endl;
}

//TODO: take that out when done measuring
double vtkRealTimeNeedleDetectionGUI::diffclock(clock_t clock1,clock_t clock2)
{
  double diffticks=clock1-clock2;
  double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
  return diffms;
} 






