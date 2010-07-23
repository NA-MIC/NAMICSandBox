
#ifndef VTKMRMLPERKPROCEDURENODE_H
#define VTKMRMLPERKPROCEDURENODE_H


#ifdef WIN32
#include "vtkPerkProcedureEvaluatorWin32Header.h"
#endif

#include <iostream>
#include <string>
#include <vector>

#include "vtkSmartPointer.h"

#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"

#include "vtkTransformTimeSeries.h"


class vtkMRMLLinearTransformNode;



//BTX
/**
 * One record in the perk procedure log.
 */
class
VTK_PerkProcedureEvaluator_EXPORT
PerkNote
{
public:
  
  enum EventType
    {
    NoEvent = 0,
    PierceSkin,
    MoveForward,
    Retract,
    AdjustAngle,
    PullOut
    };
  
  double Time;
  EventType Event;
  std::string Message;
};
//ETX



// ==================================================================



class
VTK_PerkProcedureEvaluator_EXPORT
vtkMRMLPerkProcedureNode
: public vtkMRMLNode
{

public:

  static vtkMRMLPerkProcedureNode* New();
  vtkTypeMacro( vtkMRMLPerkProcedureNode, vtkMRMLNode );
  void PrintSelf( ostream& os, vtkIndent indent );
  virtual vtkMRMLNode* CreateNodeInstance();
  
  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  
  void ImportFromFile( const char* fileName );
  
  virtual void Copy( vtkMRMLNode* node );
  
  virtual const char* GetNodeTagName() {
    return "PerkProcedureNode";
  }
  
  
  int GetNumberOfNotes() {
    return this->NoteList.size();
  }
  
  PerkNote* GetNoteAtIndex( int index );
  double GetTimeAtTransformIndex( int index );
  
  
  vtkGetStringMacro( ObservedTransformNodeID );
  vtkMRMLLinearTransformNode* GetObservedTransformNode();
  void SetAndObserveObservedTransformNodeID( const char *TransformNodeRef );
  
  vtkGetStringMacro( NeedleTransformNodeID );
  vtkMRMLLinearTransformNode* GetNeedleTransformNode();
  void SetAndObserveNeedleTransformNodeID( const char *TransformNodeRef );
  
  
  void SetNoteIndex( int ind );
  vtkGetMacro( NoteIndex, int );
  
  vtkSetMacro( TransformIndex, int );
  vtkGetMacro( TransformIndex, int );
  
  void UpdateTransformIndex();
  

protected:
  
  vtkMRMLPerkProcedureNode();
  ~vtkMRMLPerkProcedureNode();
  
  void RemoveMRMLObservers();
  

private:

  vtkMRMLPerkProcedureNode( const vtkMRMLPerkProcedureNode& );
  void operator=( const vtkMRMLPerkProcedureNode& );
  
  void ClearData();
  
  
  //BTX
  std::vector< PerkNote* > NoteList;
  int NoteIndex;
  
  vtkTransformTimeSeries* TransformTimeSeries;
  int TransformIndex;
  //ETX
  
  
    // Transform: Needle tip to Calibration.
  
  char* ObservedTransformNodeID;
  vtkSetReferenceStringMacro( ObservedTransformNodeID );
  vtkMRMLLinearTransformNode* ObservedTransformNode;
  
    // Transform: Needle tip to World (RAS).
  char* NeedleTransformNodeID;
  vtkSetReferenceStringMacro( NeedleTransformNodeID );
  vtkMRMLLinearTransformNode* NeedleTransformNode;
};


#endif
