
#ifndef __vtkPerkProcedureEvaluatorGUI_h
#define __vtkPerkProcedureEvaluatorGUI_h

#ifdef WIN32
#include "vtkPerkProcedureEvaluatorWin32Header.h"
#endif

#include "vtkCallbackCommand.h"
#include "vtkSlicerModuleGUI.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkPerkProcedureEvaluatorLogic.h"

#include "vtkMRMLPerkProcedureNode.h"


class vtkKWPushButton;
class vtkKWLoadSaveButton;
class vtkKWMultiColumnListWithScrollbars;
class vtkSlicerNodeSelectorWidget;
class vtkTimerLog;


class
VTK_PerkProcedureEvaluator_EXPORT
vtkPerkProcedureEvaluatorGUI
: public vtkSlicerModuleGUI
{

public:

  vtkTypeRevisionMacro ( vtkPerkProcedureEvaluatorGUI, vtkSlicerModuleGUI );
  
  
  //----------------------------------------------------------------
  // Set/Get Methods
  
  vtkGetObjectMacro ( Logic, vtkPerkProcedureEvaluatorLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

  
  vtkGetObjectMacro( ProcedureNode, vtkMRMLPerkProcedureNode );
  vtkSetObjectMacro( ProcedureNode, vtkMRMLPerkProcedureNode );
  
  
protected:
  
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  
  vtkPerkProcedureEvaluatorGUI ( );
  virtual ~vtkPerkProcedureEvaluatorGUI ( );


public:
  
  
  //----------------------------------------------------------------
  // New method, Initialization etc.
  
  static vtkPerkProcedureEvaluatorGUI* New ();
  void Init();
  virtual void Enter ( );
  virtual void Exit ( );
  void PrintSelf (ostream& os, vtkIndent indent );
  
  
  //----------------------------------------------------------------
  // Observer Management
  
  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();
  void AddLogicObservers();
  void RemoveLogicObservers();
  
  
  //----------------------------------------------------------------
  // Event Handlers
  
  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  // void ProcessTimerEvents();
  void HandleMouseEvent( vtkSlicerInteractorStyle *style );
  static void DataCallback( vtkObject *caller, unsigned long eid, void *clientData, void *callData );
  void OnNoteSelectionChanged();
  
  
  //----------------------------------------------------------------
  // Build Frames
  
  virtual void BuildGUI();
  void BuildGUIForHelpFrame();
  void BuildGUIForInputFrame();
  void BuildGUIForNotesList();
  void BuildGUIForPlaybackFrame();
  void BuildGUIForResultsFrame();
  
  virtual void TearDownGUI();
  
  
  //----------------------------------------------------------------
  // Update routines
  
  void UpdateAll();
  void UpdatePlayback();
  

protected:
  
  //----------------------------------------------------------------
  // Timer
  
  int TimerFlag;
  int TimerInterval;
  
  
  //----------------------------------------------------------------
  // GUI widgets
  
  vtkSlicerNodeSelectorWidget* PerkProcedureSelector;
  vtkSlicerNodeSelectorWidget* PlanFiducialsSelector;
  vtkSlicerNodeSelectorWidget* BoxFiducialsSelector;
  vtkSlicerNodeSelectorWidget* NeedleTransformSelector;
  vtkKWLoadSaveButton* LoadButton;
  
  vtkSlicerModuleCollapsibleFrame* NotesFrame;
  vtkKWMultiColumnListWithScrollbars* NotesList;
  
  
    // Playback frame.
  
  vtkKWPushButton* ButtonBegin;
  vtkKWPushButton* ButtonPrevious;
  vtkKWPushButton* ButtonNext;
  vtkKWPushButton* ButtonEnd;
  vtkKWPushButton* ButtonPlay;
  vtkKWPushButton* ButtonStop;
  vtkKWEntry*      EntrySec;
  vtkKWPushButton* ButtonGo;
  
  vtkKWLabel* PositionLabel;
  
    // Results frame.
  
  vtkKWLabel*      LabelBegin;
  vtkKWLabel*      LabelEnd;
  vtkKWPushButton* ButtonMeasureBegin;
  vtkKWPushButton* ButtonMeasureEnd;
  vtkKWPushButton* ButtonMeasure;
  vtkKWLabel*      LabelTotalTime;
  vtkKWLabel*      LabelPathInside;
  vtkKWLabel*      LabelTimeInside;
  vtkKWLabel*      LabelSurfaceInside;
  vtkKWLabel*      LabelAngleFromAxial; // Deviation from axial plane.
  vtkKWLabel*      LabelAngleInAxial;   // Deviation from plan in axial plane.
  
  
  //----------------------------------------------------------------
  // Logic Values
  
  vtkPerkProcedureEvaluatorLogic* Logic;
  vtkCallbackCommand*             DataCallbackCommand;
  int                             CloseScene;

public:
  void PerkTimerHandler();
  

private:

  vtkPerkProcedureEvaluatorGUI ( const vtkPerkProcedureEvaluatorGUI& ); // Not implemented.
  void operator = ( const vtkPerkProcedureEvaluatorGUI& ); //Not implemented.

  void ProcessLoadButton();
  void ProcessProcedureSelected();
  
  
  vtkMRMLPerkProcedureNode* ProcedureNode;
  char*                     ProcedureNodeID;
  
  bool AutoPlayOn;
  vtkTimerLog* TimerLog;
  bool TimerEventProcessing;
};



#endif
