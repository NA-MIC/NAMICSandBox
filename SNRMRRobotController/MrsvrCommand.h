//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
// Copyright (C) 2005-2006 by Shiga University of Medical Science,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrCommand.h,v $
// $Revision: 1.6 $ 
// $Author: junichi $
// $Date: 2006/01/19 07:35:14 $
//====================================================================


//====================================================================
// Description: 
//    Class to pass the control command from the GUI / communication 
//    interface to the main controller.
//
//====================================================================


#ifndef _INC_MRSVR_COMMAND
#define _INC_MRSVR_COMMAND

#include "MrsvrSharedData.h"
#include "MrsvrStatus.h"

#define AUTOCALIB_PROC_HOME         0
#define AUTOCALIB_PROC_ANGLE        1
#define AUTOCALIB_PROC_REGISTRATION 2
#define NUM_AUTOCALIB_PROC          3

#define NEEDLE_STOP       0
#define NEEDLE_APPROACH   1
#define NEEDLE_LEAVE      -1

#ifdef DEBUG
#define DEBUG_MRSVR_COMMAND
#endif // DEBUG

typedef struct {
  // shutdown flag
  bool         shutdown;               // if 1,  mainController starts to shutdown.

  int          mode; 
  int          commandBy;
  
  // actuator power switch
  bool         actuatorActive[NUM_ACTUATORS];

  // actuators
  float        voltage[NUM_ACTUATORS];     // [v]
  // encoders
  float        velocity[NUM_ENCODERS];    // [mm/s], [angle/s], etc ... 
  float        position[NUM_ENCODERS];    // [mm], [angle], etc ...
  float        setPoint[NUM_ENCODERS];
  int          setZeroFlag[NUM_ENCODERS]; // flag to set a counter to zero

  float        tipOffset[3];              // offset of the tip from the rotation
                                          // center of the end effector
  float        tipApprOrient[3];          // tip approaching orientation

  float        tipApprOffset;              // tip approaching Offset [mm]
  /*
  int          tipApprSw;                 // tip approaching switch
                                          //  (NEEDLE_STOP, NEEDLE_APPROACH or NEEDLE_LEAVE)
  */

  float        lmtVoltage[NUM_ENCODERS];  // voltage limitter

  // for automatic calibration
  bool         autoCalibProcSwitch[NUM_AUTOCALIB_PROC];
  float        autoCalibPoints[NUM_CALIB_POINTS][3];
  int          setAngleDoneIndex;
  int          calibDoneIndex;
  float        setAngles[NUM_ENCODERS-NUM_ACTUATORS];

} MrsvrCommandInfo; 

#define SHARED_MEMORY_SIZE_COMMAND sizeof(MrsvrCommandInfo)


class MrsvrCommand : public MrsvrSharedData {
  
public:
  enum {     // for commandBy
    VOLTAGE,
    VELOCITY,
    POSITION,
  };

protected:  // shared data
  MrsvrCommandInfo*    cmdInfo;
  
public:
  MrsvrCommand(key_t); 
  ~MrsvrCommand();
  
};


class MrsvrCommandReader : public MrsvrCommand {

public:
  inline bool  getShutdown()         { return cmdInfo->shutdown;   };
  inline int   getMode()             { return cmdInfo->mode;       };
  inline int   getCommandBy()        { return cmdInfo->commandBy;  };
  inline float getVoltage(int i)     { return cmdInfo->voltage[i];  };
  inline float getVelocity(int i)    { return cmdInfo->velocity[i]; };
  inline float getPosition(int i)    { return cmdInfo->position[i]; };
  inline float getSetPoint(int i)    { return cmdInfo->setPoint[i]; };
  inline int   getZeroFlag(int i)    { return cmdInfo->setZeroFlag[i]; };
  inline float getTipOffset(int i)   { return cmdInfo->tipOffset[i]; };
  inline float getTipApprOrient(int i) { return cmdInfo->tipApprOrient[i]; };
  inline float getTipApprOffset()     { return cmdInfo->tipApprOffset; };

  inline float getLmtVoltage(int i)  { return cmdInfo->lmtVoltage[i]; };
  inline int   getSetAngleDoneIndex(){ return cmdInfo->setAngleDoneIndex; };
  inline int   getCalibDoneIndex()   { return cmdInfo->calibDoneIndex; };
  inline void  getSetAngles(float* v) {
    for (int i = 0; i < NUM_ENCODERS-NUM_ACTUATORS; i ++) {
      v[i] = cmdInfo->setAngles[i];
    }
  };

  inline void  getAutoCalibPoint(int i, MrsvrVector vec) {
    vec[0] = cmdInfo->autoCalibPoints[i][0];
    vec[1] = cmdInfo->autoCalibPoints[i][1];
    vec[2] = cmdInfo->autoCalibPoints[i][2];
  };
  inline float getAutoCalibPoint(int i, int d) {
    return cmdInfo->autoCalibPoints[i][d];
  }

  inline bool isAutoCalibProcEnable(int i) {
    return cmdInfo->autoCalibProcSwitch[i];
  }

  inline bool isActuatorActive(int i) {
    return cmdInfo->actuatorActive[i];
  }

public:
  MrsvrCommandReader(key_t);
  ~MrsvrCommandReader();

};


class MrsvrCommandWriter : public MrsvrCommandReader {

public:
  inline void  setShutdown(bool s)          { cmdInfo->shutdown = s;   };
  inline void  setMode(int v)               { cmdInfo->mode =  v;      };
  inline void  setCommandBy(int v)          { cmdInfo->commandBy = v;  };
  inline void  setVoltage(int i, float v)   { cmdInfo->voltage[i] = v;  };
  inline void  setVelocity(int i, float v)  { cmdInfo->velocity[i] = v; };
  inline void  setPosition(int i, float v)  { cmdInfo->position[i] = v; };
  inline void  setSetPoint(int i, float v)  { cmdInfo->setPoint[i] = v; };
  inline void  setZeroFlagOn(int i)         { cmdInfo->setZeroFlag[i] = 1; };
  inline void  setZeroFlagOff(int i)        { cmdInfo->setZeroFlag[i] = 0; };
  inline void  setTipOffset(int i, float v) { cmdInfo->tipOffset[i] = v; };
  inline void  setTipApprOrient(int i, float v) { cmdInfo->tipApprOrient[i] = v; };
  inline void  setTipApprOffset(float v)     { cmdInfo->tipApprOffset = v; };

  inline void  setLmtVoltage(int i, float v){ cmdInfo->lmtVoltage[i] = v;};
  inline void  setSetAngleDoneIndex(int n)  { cmdInfo->setAngleDoneIndex = n; };
  inline void  setCalibDoneIndex(int n)     { cmdInfo->calibDoneIndex = n; };
  inline void  setSetAngles(float* v) {
    for (int i = 0; i < NUM_ENCODERS-NUM_ACTUATORS; i ++) {
      cmdInfo->setAngles[i] = v[i];
    }
  };

  inline void  setAutoCalibPoints(int i, MrsvrVector vec) {
    cmdInfo->autoCalibPoints[i][0] = vec[0]; 
    cmdInfo->autoCalibPoints[i][1] = vec[1]; 
    cmdInfo->autoCalibPoints[i][2] = vec[2]; 
  };
  inline void  setAutoCalibPoitns(int i, int d, float v) {
    cmdInfo->autoCalibPoints[i][d] = v;
  }
  inline void enableAutoCalibProc(int i) {
    cmdInfo->autoCalibProcSwitch[i] = true;
  }

  inline void disableAutoCalibProc(int i) {
    cmdInfo->autoCalibProcSwitch[i] = false;
  }

  inline void activateActuator(int i) {
    cmdInfo->actuatorActive[i] = true;
  }

  inline void deactivateActuator(int i) {
    cmdInfo->actuatorActive[i] = false;
  }

public:
  MrsvrCommandWriter(key_t);
  ~MrsvrCommandWriter();

};

#endif //_INC_MRSVR_COMMAND


