
#ifndef __vtkPerkStationModuleGUI_h
#define __vtkPerkStationModuleGUI_h

#include "vtkSmartPointer.h"

#include "vtkSlicerBaseGUIWin32Header.h"
#include "vtkSlicerModuleGUI.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkPerkStationModuleLogic.h"
class vtkPerkStationSecondaryMonitor;

#include "vtkKWWizardWidget.h"

#include "vtkTimerLog.h"

class vtkKWFrame;
class vtkKWPushButton;
class vtkKWComboBoxWithLabel;
class vtkKWPushButtonSet;
class vtkKWMenuButtonWithLabel;
class vtkKWLoadSaveButton;
class vtkKWScale;
class vtkKWScaleWithEntry;
class vtkSlicerNodeSelectorWidget;

class vtkPerkStationCalibrateStep;
class vtkPerkStationPlanStep;
class vtkPerkStationInsertStep;
class vtkPerkStationValidateStep;



/**
 * GUI class. One of the three main classes of a standard Slicer3
 * interactive module.
 */
class
VTK_PERKSTATIONMODULE_EXPORT
vtkPerkStationModuleGUI 
: public vtkSlicerModuleGUI
{
public:
  static vtkPerkStationModuleGUI *New();
  vtkTypeMacro( vtkPerkStationModuleGUI, vtkSlicerModuleGUI );
  void PrintSelf( ostream& os, vtkIndent indent );
  
  
  vtkGetObjectMacro( Logic, vtkPerkStationModuleLogic );
  vtkSetObjectMacro( Logic, vtkPerkStationModuleLogic );
  
  // Description:
  // Set the logic pointer from parent class pointer.
  // Overloads implementation in vtkSlicerModulesGUI
  // to allow loadable modules.
  virtual void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic( reinterpret_cast< vtkPerkStationModuleLogic* >( logic ) ); 
  }
  
  
  // Description: Get/Set MRML node
  vtkGetObjectMacro( MRMLNode, vtkMRMLPerkStationModuleNode );
  vtkSetObjectMacro( MRMLNode, vtkMRMLPerkStationModuleNode );
  
  
  vtkPerkStationSecondaryMonitor* GetSecondaryMonitor();
  
  vtkKWWizardWidget* GetWizardWidget();
  
  
  virtual void BuildGUI();
  virtual void BuildGUIForHelpFrame();
  virtual void BuildGUIForExperimentFrame();
  virtual void BuildGUIForWorkphases();
  
  
  virtual void TearDownGUI();
  
  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );
  
  void AddMRMLObservers();
  void RemoveMRMLObservers();
  
  
  
  // Pprocess events generated by Logic
  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event,
                                    void *callData ){};
  
  // Pprocess events generated by GUI widgets
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event,
                                  void *callData );
  
  // Create render window in the secondary monitor
  virtual void RenderSecondaryMonitor();
  
  // Reset and Start afresh
  virtual void ResetAndStartNewExperiment();
  
  
  //BTX
  // Mode identifier
  enum ModeId
    {
      Clinical = 1,
      Training = 2,
    };
  
  // Description:
  // State identifier
  enum StateId
    {
      Calibrate = 0,
      Plan,
      Insert,
      Validate
    };
  //ETX
  
  void TimerHandler();
  

protected:
  vtkPerkStationModuleGUI();
  virtual ~vtkPerkStationModuleGUI();
  vtkPerkStationModuleGUI( const vtkPerkStationModuleGUI& );
  void operator=( const vtkPerkStationModuleGUI& );
  
  
  // Pprocess events generated by MRML
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, 
                                   void *callData );
  
  // Describe behavior at module startup and exit.
  virtual void Enter( vtkMRMLNode *node );
  virtual void Enter();
  virtual void Exit();
  
    // Handle mrml node events.
  virtual void PlanningVolumeChanged();
  virtual void ValidationVolumeChanged();
  
  
  void UpdateGUI();
  void UpdateMRML();
  
  
  // Description
  // For creating a unique filename, to save the experiment
  char  *CreateFileName();

  // Description
  // To set up environment depending upon the mode
  void SetUpPerkStationMode();
  
  void SetUpPerkStationWizardWorkflow();
  
  //BTX
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > WizardFrame;
  
  vtkSmartPointer< vtkSlicerNodeSelectorWidget > VolumeSelector;
  vtkSmartPointer< vtkSlicerNodeSelectorWidget > ValidationVolumeSelector;
  vtkSmartPointer< vtkSlicerNodeSelectorWidget > PSNodeSelector;
  
  vtkPerkStationModuleLogic* Logic;
  vtkMRMLPerkStationModuleNode* MRMLNode;
  
  vtkSmartPointer< vtkPerkStationSecondaryMonitor > SecondaryMonitor;
  
  vtkSmartPointer< vtkKWLoadSaveButton > LoadExperimentFileButton;
  vtkSmartPointer< vtkKWLoadSaveButton > SaveExperimentFileButton;
  
    // two scales for window and level
  vtkSmartPointer< vtkKWScaleWithEntry > DisplayVolumeWindowValue;
  vtkSmartPointer< vtkKWScaleWithEntry > DisplayVolumeLevelValue;
  
  vtkSmartPointer< vtkKWLabel > CalibrateTimeLabel;
  vtkSmartPointer< vtkKWLabel > PlanTimeLabel;
  vtkSmartPointer< vtkKWLabel > InsertTimeLabel;
  vtkSmartPointer< vtkKWLabel > ValidateTimeLabel;
  vtkSmartPointer< vtkKWPushButton > TimerButton;
  vtkSmartPointer< vtkKWPushButton > ResetTimerButton;
  //ETX
  
  
    // Description:
    // Describes whether the GUI has been built or not
  bool Built;
  
  
private:
  
    // Description:
    // The wizard widget and steps
  //BTX
  vtkSmartPointer< vtkKWWizardWidget > WizardWidget;
  vtkSmartPointer< vtkPerkStationCalibrateStep > CalibrateStep;
  vtkSmartPointer< vtkPerkStationPlanStep > PlanStep;
  vtkSmartPointer< vtkPerkStationInsertStep > InsertStep;
  vtkSmartPointer< vtkPerkStationValidateStep > ValidateStep;
  //ETX
  
  
    // gui state variables
  
  bool Entered;
  int Mode; // clinical mode or training mode
  double SliceOffset;
  int ObserverCount;
  int State; // whether in calibration mode, insert, validation , or
  
  
    // Workflow phase tracking.

  //BTX
  vtkSmartPointer< vtkKWFrame > WorkphaseButtonFrame;
  vtkSmartPointer< vtkKWPushButtonSet > WorkphaseButtonSet;
  
  vtkSmartPointer< vtkTimerLog > TimerLog;
  
    // Seconds spent by calibration, planning, insertion and validation.
  double WorkingTimes[ 4 ];
  double LastTime;
  bool TimerOn;
  
  void UpdateTimerDisplay();
  
  //ETX
  
  int ChangeWorkphase( int phase );
  void UpdateWorkphaseButtons();
  
};

#endif

