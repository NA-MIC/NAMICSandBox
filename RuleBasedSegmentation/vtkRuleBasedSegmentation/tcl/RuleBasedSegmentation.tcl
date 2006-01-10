#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationInit {} {
    global RuleBasedSegmentation Module Volume Model

    set m RuleBasedSegmentation

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This module is for Semi_Automatic Segmentation."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Ramsey Al-Hakim, GaTech ramsey@gatech.edu"

    #  Set the level of development that this module falls under, from the list defined in Main.tcl,
    #  Module(categories) or pick your own
    #  This is included in the Help->Module Categories menu item
    set Module($m,category) "Segmentation"

    # Define Tabs
    #------------------------------------
    # Description:
    #   Each module is given a button on the Slicer's main menu.
    #   When that button is pressed a row of tabs appear, and there is a panel
    #   on the user interface for each tab.  If all the tabs do not fit on one
    #   row, then the last tab is automatically created to say "More", and 
    #   clicking it reveals a second row of tabs.
    #
    #   Define your tabs here as shown below.  The options are:
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #

    set Module($m,row1List) "Help DLPFC Striatum"
    set Module($m,row1Name) "{Help} {DLPFC} {Striatum}"
    set Module($m,row1,tab) Striatum

    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name (which
    #   can be anything) is registered with a line like this:
    #
    #   set Module($m,procVTK) RuleBasedSegmentationBuildVTK
    #
    #   All the options are:

    #   procGUI   = Build the graphical user interface
    #   procVTK   = Construct VTK objects
    #   procMRML  = Update after the MRML tree changes due to the creation
    #               of deletion of nodes.
    #   procEnter = Called when the user enters this module by clicking
    #               its button on the main menu
    #   procExit  = Called when the user leaves this module by clicking
    #               another modules button
    #   procCameraMotion = Called right before the camera of the active 
    #                      renderer is about to move 
    #   procStorePresets  = Called when the user holds down one of the Presets
    #               buttons.
    #               
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."
    #   
    set Module($m,procGUI) RuleBasedSegmentationBuildGUI
    set Module($m,procVTK) RuleBasedSegmentationBuildVTK
    set Module($m,procEnter) RuleBasedSegmentationEnter
    set Module($m,procExit) RuleBasedSegmentationExit
    set Module($m,procMRML) RuleBasedSegmentationUpdateGUI

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) ""

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.6.2.3 $} {$Date: 2004/11/30 20:49:54 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set RuleBasedSegmentation(DLPFCVolumeIn) $Volume(idNone)
    set RuleBasedSegmentation(DLPFCLabelVolumeIn) $Volume(idNone)
    # added by Delphine to be able to input to the bayesian
    # it will be set in RuleBasedSegmentationApply
    set RuleBasedSegmentation(DLPFCSubVolumeID) $Volume(idNone)


    set RuleBasedSegmentation(OutputName) "roi"
    set RuleBasedSegmentation(OutputName2) "labelmap_roi"
    set RuleBasedSegmentation(StriatumLabelVolumeIn)  $Volume(idNone) 
    set RuleBasedSegmentation(StriatumVolumeIn) $Volume(idNone)
    set RuleBasedSegmentation(Striatum_OutputName) ""
    set RuleBasedSegmentation(Striatum_AlgoMarking_id) $Volume(idNone) 
    set RuleBasedSegmentation(Striatum_SegResult_id) $Volume(idNone) 
    set RuleBasedSegmentation(lastinput) 100000

    set RuleBasedSegmentation(eventManager) ""
    
    
    set RuleBasedSegmentation(InOrigin) "0 0 0"
    set RuleBasedSegmentation(InExtent) "0 0 0 0 0 0"
    set RuleBasedSegmentation(InSpacing) "0 0 0"

    #2D case variables
    set RuleBasedSegmentation(Ext2D,TypeList) "Ax Sag Cor"
    set RuleBasedSegmentation(Ext2D,Type) "All"
    set RuleBasedSegmentation(Ext2D,Ax,init) 0
    set RuleBasedSegmentation(Ext2D,Ax,end) 1
    set RuleBasedSegmentation(Ext2D,Sag,init) 1
    set RuleBasedSegmentation(Ext2D,Sag,end) 2
    set RuleBasedSegmentation(Ext2D,Cor,init) 2
    set RuleBasedSegmentation(Ext2D,Cor,end) 3

    #3D case variable
    foreach t "DLPFC Striatum" {

  foreach type "AxMin AxMax SagMin SagMax CorMin CorMax" {
      set RuleBasedSegmentation($t,Ext3D,$type) 0
      set RuleBasedSegmentation($t,Ext3D,$type,min) 0
      set RuleBasedSegmentation($t,Ext3D,$type,max) 0
  }

  set RuleBasedSegmentation($t,Ext3D,volId) $Volume(idNone)
  set RuleBasedSegmentation($t,Ext3D,volId2) $Volume(idNone)
  set RuleBasedSegmentation($t,Ext3D,AxMax,title) "Superior Tip:"
  set RuleBasedSegmentation($t,Ext3D,AxMin,title) "Inferior Tip:"
  set RuleBasedSegmentation($t,Ext3D,SagMax,title) "Sag Max (R):"
  set RuleBasedSegmentation($t,Ext3D,SagMin,title) "Sag Min (L):"
  set RuleBasedSegmentation($t,Ext3D,CorMax,title) "Frontal Pole:"
  set RuleBasedSegmentation($t,Ext3D,CorMin,title) "Temporal Tip:"

  set RuleBasedSegmentation($t,Ext3D,AxMin,Id) 0 
  set RuleBasedSegmentation($t,Ext3D,AxMax,Id) 0
  set RuleBasedSegmentation($t,Ext3D,SagMin,Id) 1
  set RuleBasedSegmentation($t,Ext3D,SagMax,Id) 1
  set RuleBasedSegmentation($t,Ext3D,CorMin,Id) 2
  set RuleBasedSegmentation($t,Ext3D,CorMax,Id) 2

  set RuleBasedSegmentation($t,Ext3D,CubeColor) "1 0 0"
  set RuleBasedSegmentation($t,Ext3D,CubeOpacity) 0.5
  set RuleBasedSegmentation($t,Ext3D,OutlineColor) "0 0 1"
  set RuleBasedSegmentation($t,Ext3D,OutlineOpacity) 1
  set RuleBasedSegmentation($t,Ext3D,RenderCube) 1
  }

    #overwrite for Striatum
    set RuleBasedSegmentation(Striatum,Ext3D,AxMax,title) "Ax Max:"
    set RuleBasedSegmentation(Striatum,Ext3D,AxMin,title) "Ax Min:"
    set RuleBasedSegmentation(Striatum,Ext3D,SagMax,title) "Sag Max:"
    set RuleBasedSegmentation(Striatum,Ext3D,SagMin,title) "Sag Min:"
    set RuleBasedSegmentation(Striatum,Ext3D,CorMax,title) "Cor Max:"
    set RuleBasedSegmentation(Striatum,Ext3D,CorMin,title) "Cor Min:"
    
    


    

    set RuleBasedSegmentation(diff,Right) 0
    set RuleBasedSegmentation(diff,Left) 0
}

# NAMING CONVENTION:
#-------------------------------------------------------------------------------
#
# Use the following starting letters for names:
# t  = toplevel
# f  = frame
# mb = menubutton
# m  = menu
# b  = button
# l  = label
# s  = slider
# i  = image
# c  = checkbox
# r  = radiobutton
# e  = entry
#
#-------------------------------------------------------------------------------


proc RuleBasedSegmentationBuildGUI {} {
    global Gui RuleBasedSegmentation Module Volume Model
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(RuleBasedSegmentation,fStuff)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Stuff
    #   Top
    #   Middle
    #   Bottom
    #     FileLabel
    #     CountDemo
    #     TextBox
    #-------------------------------------------
   #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The RuleBasedSegmentation module allows to extract a volume of interest for further processing
    Note:  The extraction of subvolume must be done in RAS space and the slice direction must match the following:
    Pink = Axial
    Yellow = Sagittal
    Green = Coronal
    *DO NOT USE AxiSlice,CorSlice, OR SagSlice*
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>DLPFC:</B> 1.  Select the Original Raw Data in the drop down menu of Box 1.
    <B>2.  Optional: Change the name of the Subvolume in the text box in Box 1.
     Box 4 gives the option of changing the opacity and viewing of the subvolume red box.
    <B>3.  Scan through the coronal slice window until the slice with frontal pole is viewed.
    Select the 'pick' button in Box 2 next to 'Frontal Pole.'  Do the same for the 
    temporal tip in the coronal slice window.  Use the axial slice window and repeat
    the process for the most inferior and superior slices containing the cortex.
    <B>4.  Click 'ApplyBoundaryConditions' in Box 3 to create the SubVolume.
    <B>5.  Go to the editor tab and create a labelmap that roughly encloses the DLPFC.
    <B>6.  Select the labelmap created in Editor from the drop down menu in Box 5.
    <B>7.  Optional: Change the name of the labelmap output in the text box in Box 6.
    <B>8.  Click the 'ApplyBayesianSegmentation' button in Box 7.
    <LI><B>Future Areas:</B> Extract other areas of the brain.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags RuleBasedSegmentation $help
    MainHelpBuildGUI RuleBasedSegmentation

    #-------------------------------------------
    # DLPFC frame
    #-------------------------------------------
    set fDLPFC $Module(RuleBasedSegmentation,fDLPFC)
    set f $fDLPFC
    
    foreach frame "IO 3D Apply Render DLPFCLabelmapInput DLPFCLabelmapOutput ApplyBayesian" {
    frame $f.f$frame -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
     #-------------------------------------------
    # DLPFC->I/O frame
    #-------------------------------------------   
    set f $fDLPFC.fIO
    
    foreach frame "Input Output" {
      frame $f.f$frame -bg $Gui(activeWorkspace)
      pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    set f $fDLPFC.fIO.fInput
    
    DevAddLabel $f.l0 "Step 1:"  
    
    DevAddSelectButton RuleBasedSegmentation $f DLPFCVolumeIn "Input:"   Grid "Input Volume"

    set f $fDLPFC.fIO.fOutput
    
    DevAddLabel $f.l "Output Name for SubVolume:"      
    eval {entry $f.e -justify right -width 10 \
          -textvariable  RuleBasedSegmentation(OutputName)  } $Gui(WEA)
    pack $f.l $f.e -side left -padx $Gui(pad) -pady 0


#WORKING HERE TO PUT INPUT OUTPUT VOLUMES:
   # eval {label $f.f2.lVolume -text "Volume:"} $Gui(WLA)
   # eval {menubutton $f.f2.mbVolume -text "Select" -menu $f.f2.mbVolume.m} $Gui(WMBA)
   # eval {menu $f.f2.mbVolume.m} $Gui(WMA)
   # eval {label $f.f2.lLabelMap -text "Label map:"} $Gui(WLA)
   # eval {menubutton $f.f2.mbLabel -text "Select" -menu $f.f2.mbLabel.m} $Gui(WMBA)
   # eval {menu $f.f2.mbLabel.m} $Gui(WMA)

  
    
    #-------------------------------------------
    # DLPFC->3D cube
    #-------------------------------------------
    set f $fDLPFC.f3D
    
    frame $f.fLabel  -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fControl -bg $Gui(activeWorkspace) -bd 3
    pack  \
          $f.fLabel \
          $f.fControl \
      -side top -padx 0 -pady 1 -fill x
    
    set f $fDLPFC.f3D.fLabel
    
    eval {label $f.l3D -text "Boundaries of the DLPFC"} $Gui(WLA)
    pack $f.l3D -side left -padx $Gui(pad) -pady 0
    
    set fControl $fDLPFC.f3D.fControl
    
   #--------------------------------------------------
   foreach type "AxMin AxMax CorMin CorMax" {
     frame $fControl.f$type -bg $Gui(activeWorkspace)
   }
   pack $fControl.fAxMin $fControl.fAxMax $fControl.fCorMin $fControl.fCorMax \
        -side top -padx 0 -pady 1 -fill x
   
   foreach type "AxMin AxMax CorMin CorMax" {
     set f $fControl.f$type 
     eval {label $f.l$type -text $RuleBasedSegmentation(DLPFC,Ext3D,$type,title) -width 10 } $Gui(WLA)
     eval {label $f.r$type -text $RuleBasedSegmentation(DLPFC,Ext3D,$type,title) -width 10 } $Gui(WLA)
   }  
   
   set f $fControl.fAxMin
   eval {button $f.bAxMin -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D DLPFC AxMin} } $Gui(WBA)
   TooltipAdd $f.bAxMin "Choose the current Axial slice"
   set f $fControl.fAxMax
   eval {button $f.bAxMax -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D DLPFC AxMax} } $Gui(WBA)
   TooltipAdd $f.bAxMax "Choose the current Axial slice"
   #set f $fControl.fSagMin
   #eval {button $f.bSagMin -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D DLPFC SagMin} } $Gui(WBA)
   #TooltipAdd $f.bSagMin "Choose the current Sagittal slice"
   #set f $fControl.fSagMax
   #eval {button $f.bSagMax -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D DLPFC SagMax} } $Gui(WBA)
   #TooltipAdd $f.bSagMax "Choose the current Sagittal slice"
   set f $fControl.fCorMin
   eval {button $f.bCorMin -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D DLPFC CorMin} } $Gui(WBA)
   TooltipAdd $f.bCorMin "Choose the current Coronal slice"
   set f $fControl.fCorMax
   eval {button $f.bCorMax -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D DLPFC CorMax} } $Gui(WBA)
   TooltipAdd $f.bCorMax "Choose the current Coronal slice"
        
   foreach type "AxMin AxMax CorMin CorMax" {  
     set f $fControl.f$type
     eval {entry $f.e$type -justify right -width 4 \
          -textvariable  RuleBasedSegmentation(DLPFC,Ext3D,$type)  } $Gui(WEA)

     eval {scale $f.s$type -from $RuleBasedSegmentation(DLPFC,Ext3D,$type,min) -to $RuleBasedSegmentation(DLPFC,Ext3D,$type,max)        \
         -variable  RuleBasedSegmentation(DLPFC,Ext3D,$type) -command {RuleBasedSegmentationUpdate3DScales DLPFC}\
          -orient vertical     \
          -resolution 1      \
          } $Gui(WSA)

        
     pack $f.l$type $f.b$type $f.e$type $f.s$type -side left -padx 0 -pady 0 -fill x
   } 
   
   #-------------------------------------------
   # DLPFC->Apply Boundary Conditions
   #-------------------------------------------
   set f $fDLPFC.fApply
    
   DevAddButton $f.bApply ApplyBoundaryConditions RuleBasedSegmentationApply
    
    # Tooltip example: Add a tooltip for the button
    TooltipAdd $f.bApply "Apply the RuleBasedSegmentation extraction"

    pack $f.bApply -side top -pady $Gui(pad) -padx $Gui(pad) \
        -fill x -expand true
    
   #-------------------------------------------
   # DLPFC->Render
   #-------------------------------------------
   set f $fDLPFC.fRender
   
    eval {checkbutton $f.c3D -text "Render 3D Bounding Box" -variable RuleBasedSegmentation(DLPFC,Ext3D,RenderCube) -indicatoron 1 -command {RuleBasedSegmentationRenderCube DLPFC} } $Gui(WCA)
   pack $f.c3D -side top -pady 2 -padx 1
   
   eval {label $f.lOpacity -text "Opacity:"\
          -width 11 -justify right } $Gui(WTA)

    eval {entry $f.eOpacity -justify right -width 4 \
          -textvariable RuleBasedSegmentation(DLPFC,Ext3D,CubeOpacity)  } $Gui(WEA)

    eval {scale $f.sOpacity -from 0 -to 1     \
          -variable  RuleBasedSegmentation(DLPFC,Ext3D,CubeOpacity)\
          -orient vertical     \
          -resolution 0.1      \
      -command { RuleBasedSegmentation3DOpacity DLPFC} \
          } $Gui(WSA)
     
     pack  $f.lOpacity $f.eOpacity $f.sOpacity -side left -padx 3 -pady 2 -expand 0
      
   #-------------------------------------------
   # DLPFC->DLPFCLabelmapInput
   #-------------------------------------------
   set f $fDLPFC.fDLPFCLabelmapInput
  
   DevAddSelectButton RuleBasedSegmentation $f DLPFCLabelVolumeIn "Input Label:"   Grid "Input Volume"
  
   #-------------------------------------------
   # DLPFC->DLPFCLabelmapOutput
   #-------------------------------------------
   set f $fDLPFC.fDLPFCLabelmapOutput

       DevAddLabel $f.r "Output Name for NEW Labelmap:"      
    eval {entry $f.e -justify right -width 10 \
          -textvariable  RuleBasedSegmentation(OutputName2)  } $Gui(WEA)
    pack $f.r $f.e -side left -padx $Gui(pad) -pady 0

   #-------------------------------------------
   # DLPFC->Apply ITK Bayesian Segmentation
   #-------------------------------------------
   set f $fDLPFC.fApplyBayesian
    
   DevAddButton $f.bApplyBayesian ApplyBayesianSegmentation RuleBasedSegmentationApplyBayesian
    
    # Tooltip example: Add a tooltip for the button
    TooltipAdd $f.bApplyBayesian "Apply Bayesian Segmentation"

    pack $f.bApplyBayesian -side top -pady $Gui(pad) -padx $Gui(pad) \
        -fill x -expand true





######### WORKING HERE#####################################



#-------------------------------------------
    # Striatum frame
    #-------------------------------------------
    set fStriatum $Module(RuleBasedSegmentation,fStriatum)
    set f $fStriatum
    
    foreach frame "Inputs 3D Delineate Outputs" {
    frame $f.f$frame -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
     #-------------------------------------------
    # Striatum->Inputs frame
    #-------------------------------------------   
    set f $fStriatum.fInputs
    
    foreach frame "Labelmap MarkedGray OutputName" {
      frame $f.f$frame -bg $Gui(activeWorkspace)
      pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    set f $fStriatum.fInputs.fLabelmap
       
    DevAddSelectButton RuleBasedSegmentation $f StriatumLabelVolumeIn "Segmented Striatum Labelmap:"   Grid "Input Labelmap"

    set f $fStriatum.fInputs.fMarkedGray
    
    DevAddSelectButton RuleBasedSegmentation $f StriatumVolumeIn "Markings:"   Grid "Input Marked Grayscale"

    set f $fStriatum.fInputs.fOutputName
    
    DevAddLabel $f.l "Output Name:"      
    eval {entry $f.e -justify right -width 10 \
          -textvariable  RuleBasedSegmentation(Striatum_OutputName)  } $Gui(WEA)
    pack $f.l $f.e -side left -padx $Gui(pad) -pady 0


    #-------------------------------------------
    # Striatum->3D
    #-------------------------------------------
    set f $fStriatum.f3D
    
    frame $f.fLabel  -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fControl -bg $Gui(activeWorkspace) -bd 3
    pack  \
          $f.fLabel \
          $f.fControl \
      -side top -padx 0 -pady 1 -fill x
    
    set f $fStriatum.f3D.fLabel
    
    eval {label $f.l3D -text "Boundaries of the Striatum"} $Gui(WLA)
    pack $f.l3D -side left -padx $Gui(pad) -pady 0
    
    set fControl $fStriatum.f3D.fControl
    
   #--------------------------------------------------
   foreach type "AxMin AxMax SagMin SagMax CorMin CorMax" {
     frame $fControl.f$type -bg $Gui(activeWorkspace)
   }
   pack $fControl.fAxMin $fControl.fAxMax $fControl.fSagMin $fControl.fSagMax $fControl.fCorMin $fControl.fCorMax \
        -side top -padx 0 -pady 1 -fill x
   
   foreach type "AxMin AxMax SagMin SagMax CorMin CorMax" {
     set f $fControl.f$type 
     eval {label $f.l$type -text $RuleBasedSegmentation(Striatum,Ext3D,$type,title) -width 10 } $Gui(WLA)
     eval {label $f.r$type -text $RuleBasedSegmentation(Striatum,Ext3D,$type,title) -width 10 } $Gui(WLA)
   }  
           
   

   set f $fControl.fAxMin
   eval {button $f.bAxMin -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D Striatum AxMin} } $Gui(WBA)
   TooltipAdd $f.bAxMin "Choose the current Axial slice"
   set f $fControl.fAxMax
   eval {button $f.bAxMax -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D Striatum AxMax} } $Gui(WBA)
   TooltipAdd $f.bAxMax "Choose the current Axial slice"
   set f $fControl.fSagMin
   eval {button $f.bSagMin -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D Striatum SagMin} } $Gui(WBA)
   TooltipAdd $f.bSagMin "Choose the current Sagittal slice"
   set f $fControl.fSagMax
   eval {button $f.bSagMax -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D Striatum SagMax} } $Gui(WBA)
   TooltipAdd $f.bSagMax "Choose the current Sagittal slice"
   set f $fControl.fCorMin
   eval {button $f.bCorMin -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D Striatum CorMin} } $Gui(WBA)
   TooltipAdd $f.bCorMin "Choose the current Coronal slice"
   set f $fControl.fCorMax
   eval {button $f.bCorMax -text "Pick" -width 4 -command {RuleBasedSegmentationPick3D Striatum CorMax} } $Gui(WBA)
   TooltipAdd $f.bCorMax "Choose the current Coronal slice"


   foreach type "AxMin AxMax SagMin SagMax CorMin CorMax" {  
     set f $fControl.f$type
     eval {entry $f.e$type -justify right -width 4 \
          -textvariable  RuleBasedSegmentation(Striatum,Ext3D,$type)  } $Gui(WEA)


     eval {scale $f.s$type -from $RuleBasedSegmentation(Striatum,Ext3D,$type,min) -to $RuleBasedSegmentation(Striatum,Ext3D,$type,max)        \
         -variable  RuleBasedSegmentation(Striatum,Ext3D,$type) -command {RuleBasedSegmentationUpdate3DScales Striatum}\
          -orient vertical     \
          -resolution 1      \
          } $Gui(WSA)

        
     pack $f.l$type $f.b$type $f.e$type $f.s$type -side left -padx 0 -pady 0 -fill x
   } 

    # end of addition

   
   #-------------------------------------------
   # Striatum->Delineate
   #-------------------------------------------
   set f $fStriatum.fDelineate
    
   DevAddButton $f.bDelineate DelineateStriatum RuleBasedSegmentation_Striatum_Delineate
    
    # Tooltip example: Add a tooltip for the button
    TooltipAdd $f.bDelineate "Delineate Striatum"

    pack $f.bDelineate -side top -pady $Gui(pad) -padx $Gui(pad) \
        -fill x -expand true

}








proc RuleBasedSegmentation3DOpacity {tab opacity} {
 global RuleBasedSegmentation
 
 if { [info command RuleBasedSegmentation($tab,Ext3D,CubeActor)] != "" } {
     eval [RuleBasedSegmentation($tab,Ext3D,CubeActor) GetProperty] SetOpacity $opacity
     Render3D
 }
}

#----------------------------------------------------------------------
#
#
#
#----------------------------------------------------------------------
proc RuleBasedSegmentationApply {} {
#    --------------

#REPEAT ALL OF THE FOLLOWING FOR THE ORIGINAL VOLUME
#START HERE..


global RuleBasedSegmentation Volume

set volID $RuleBasedSegmentation(DLPFCVolumeIn)


################### this part of the code makes a subvolume ##########

set x1 [expr round([lindex $RuleBasedSegmentation(DLPFC,Ext3D,Ijk) 0])]
set x2 [expr round([lindex $RuleBasedSegmentation(DLPFC,Ext3D,Ijk) 1])]

set u1 [expr round([lindex $RuleBasedSegmentation(DLPFC,Ext3D,Ijk) 2])]
set y2 [expr round([lindex $RuleBasedSegmentation(DLPFC,Ext3D,Ijk) 3])]

set w1 [expr round([lindex $RuleBasedSegmentation(DLPFC,Ext3D,Ijk) 4])]
set w2 [expr round([lindex $RuleBasedSegmentation(DLPFC,Ext3D,Ijk) 5])]

set z2 [expr round($w1 + 0.6*(abs($w2-$w1)))]
set z1 [expr round($w1 + 0.2*(abs($w2-$w1)))]

set y1 [expr round(($y2 + $u1)/2)]

  vtkExtractVOI op
  op SetInput [Volume($volID,vol) GetOutput]
  op SetVOI $x1 $x2 $y1 $y2 $z1 $z2
  op Update

set newvol [RuleBasedSegmentationAddMrmlImage $volID $RuleBasedSegmentation(OutputName)]


#added by Delphine (will be needed as input to ITK filter) 
set RuleBasedSegmentation(DLPFCSubVolumeID) $newvol


  set res [op GetOutput]


  $res SetExtent 0 [expr $x2-$x1] 0 [expr $y2-$y1] 0 [expr $z2-$z1]
  Volume($newvol,vol) SetImageData  $res
  # DISCONNECT the VTK PIPELINE !!!!....
  op SetOutput ""
  op Delete


    if {$::Module(verbose) == 1} {
        puts [[Volume($newvol,vol) GetOutput] GetExtent]
    }
  #Reseting the Extent so goes from 0.
  [Volume($newvol,vol) GetOutput] SetExtent 0 [expr $x2-$x1] 0 [expr $y2-$y1] 0 [expr $z2-$z1]
    if {$::Module(verbose) == 1} {
        puts [[Volume($newvol,vol) GetOutput] GetExtent]
    }

  # Set  new dimensions
  set dim [Volume($volID,node) GetDimensions]
  Volume($newvol,node) SetDimensions [expr $x2-$x1+1]  [expr $y2-$y1+1]

  # Set  new range
  set range   [Volume($volID,node) GetImageRange]
  Volume($newvol,node) SetImageRange $z1 $z2


  MainUpdateMRML
  MainVolumesUpdate $newvol

  # update matrices
  Volume($newvol,node) ComputeRasToIjkFromScanOrder [Volume($volID,node) GetScanOrder]

  # Set the RasToWld matrix
  # Ras2ToWld = Ras2ToIjk2 x Ijk2ToIjk1 x Ijk1ToRas1 x Ras1ToWld
    if {$::Module(verbose) == 1} {
        puts "Set the RasToWld matrix\n"
    }
  set ras1wld1 [Volume($volID,node)   GetRasToWld]

  # It's weird ... : I need to call SetRasToWld in order to update RasToIjk !!!
  Volume($newvol,node) SetRasToWld $ras1wld1

  MainVolumesUpdate $newvol

  MainMrmlUpdateMRML
  #
  # Add a Transform 
  #

  set tid [DataAddTransform 0 Volume($newvol,node) Volume($newvol,node)]

  #
  # Set the Transform
  #
  set n Matrix($tid,node)

  set Dx  [lindex  [Volume($volID,node) GetDimensions] 0]
  set Dy  [lindex  [Volume($volID,node) GetDimensions] 1]
  set Dz1 [lindex  [Volume($volID,node) GetImageRange] 0]
  set Dz2 [lindex  [Volume($volID,node) GetImageRange] 1]

  set dx  [lindex  [Volume($newvol,node) GetDimensions] 0]
  set dy  [lindex  [Volume($newvol,node) GetDimensions] 1]
  set dz1 [lindex  [Volume($newvol,node) GetImageRange] 0]
  set dz2 [lindex  [Volume($newvol,node) GetImageRange] 1]

  set ras2ijk2 [Volume($newvol,node) GetRasToIjk]

  vtkMatrix4x4 ijk2ijk1
  ijk2ijk1 Identity
  ijk2ijk1 SetElement 0 3 $x1
  ijk2ijk1 SetElement 1 3 $y1
  ijk2ijk1 SetElement 2 3 $z1

  vtkMatrix4x4 ijk1ras1 
  ijk1ras1 DeepCopy [Volume($volID,node) GetRasToIjk]
  ijk1ras1 Invert

  vtkMatrix4x4 ras2ras1
  ras2ras1 Identity
  ras2ras1 Multiply4x4 ijk2ijk1  $ras2ijk2  ras2ras1
  ras2ras1 Multiply4x4 ijk1ras1  ras2ras1   ras2ras1

  vtkTransform transf
  transf SetMatrix ras2ras1
  $n SetTransform transf

  MainMrmlUpdateMRML

  ijk2ijk1    Delete
  ijk1ras1    Delete
  ras2ras1    Delete
  transf      Delete



# END HERE





}






#----------------------------------------------------------------------
#
# RuleBasedSegmentationApplyBayesian
#
#----------------------------------------------------------------------

proc RuleBasedSegmentationApplyBayesian {} {
#    --------------
#Variable to Use
# 1. RuleBasedSegmentation(OutputName) ==> SubVolume generated in first step
#==> Get from: Volume(DLPFCSubVolumeID))
# 2. RuleBasedSegmentation(LabelVolumeIn) ==> Labelmap created by user in
#Editor ==> Get from: Volume($volID2,vol)

# Output from Bayesian filter should be "RuleBasedSegmentation(OutputName2)"

#   RuleBasedSegmentation(LabelVolumeIn) NEVER GETS SET!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


# ITK STUFF #
# 1. how to write a vtkITKBayesianClassifierImageFilter class ?
# 2. once that is done, we will put it into vtkITK/cxx (and add it to
#CMakeListLocal.txt) OR vtkSemiSegmenor/cxx


global RuleBasedSegmentation Volume


set RuleBasedSegmentation(filter) CurvatureAnisotropicDiffusionImageFilter
set filter $RuleBasedSegmentation(filter)

set RuleBasedSegmentation($filter,params) SetConductanceParameter 
set param SetConductanceParameter 
set RuleBasedSegmentation($filter,$param) 5 
set RuleBasedSegmentation($filter,$param,type) "scalar"

set RuleBasedSegmentation($filter,$param,text) "Conductance"
set RuleBasedSegmentation($filter,$param,maxmin) "1 10"
set RuleBasedSegmentation($filter,$param,res) 1
set RuleBasedSegmentation($filter,$param,widget) "scale"



# first input
set v1 $RuleBasedSegmentation(DLPFCSubVolumeID)
# later on, we will want a second input
#set v0 $RuleBasedSegmentation(LabelVolumeIn)

#output
set v2 -5


set filter $RuleBasedSegmentation(filter)

#puts $v2
if {$v2 == -5} {
    set name [Volume($v1,node) GetName]
    set v2 [DevCreateNewCopiedVolume $v1 ""  ${name}_filter ]
    set node [Volume($v2,vol) GetMrmlNode]
    Mrml(dataTree) RemoveItem $node
    set nodeBefore [Volume($v1,vol) GetMrmlNode]
    Mrml(dataTree) InsertAfterItem $nodeBefore $node
    MainUpdateMRML
} else {

        set v2name  [Volume($v2,node) GetName]
        set continue [DevOKCancel "Overwrite $v2name?"]
        if {$continue == "cancel"} { return 1 }
        # They say it is OK, so overwrite!
        Volume($v2,node) Copy Volume($v1,node)
    }

    #Caster
    vtkImageCast _cast
    _cast SetOutputScalarTypeToFloat
    _cast SetInput [Volume($v1,vol) GetOutput]
    _cast Update
    #Create Object

    catch "_filter Delete"
    vtkITK$filter _filter

    foreach param $RuleBasedSegmentation($filter,params) {
        switch -exact -- $RuleBasedSegmentation($filter,$param,type) {
            "scalar" {
                _filter $param $RuleBasedSegmentation($filter,$param)
            }
            "darray" {
                catch "vals Delete"
                vtkDoubleArray vals
                foreach val $RuleBasedSegmentation($filter,$param) {
                    vals InsertNextValue $val
                }
                _filter $param vals
                vals Delete
            }
        }
    }
    _filter SetInput [_cast GetOutput]


    RuleBasedSegmentationITKFiltersBeforeUpdate

    _filter AddObserver StartEvent MainStartProgress
    _filter AddObserver EndEvent MainEndProgress
    _filter AddObserver ProgressEvent "MainShowProgress _filter"
    _filter Update

    RuleBasedSegmentationITKFiltersAfterUpdate

    #Assign output
    [Volume($v2,vol) GetOutput] DeepCopy [_filter GetOutput]

    #Disconnect pipeline

    _cast Delete
    _filter SetOutput ""
    _filter Delete


}


#-------------------------------------------------------------------------------
# .PROC ITKFiltersBeforeUpdate
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationITKFiltersBeforeUpdate { } {

    global RuleBasedSegmentation

    switch -exact -- $RuleBasedSegmentation(filter) {

        "GrayscaleGeodesicErodeImageFilter" {
            _filter SetMaskMarkerImage [_filter GetInput]
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC ITKFiltersAfterUpdate
# Does nothing.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationITKFiltersAfterUpdate { } {

}




#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationAddMrmlImage
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationAddMrmlImage {volID resname } {

  global RuleBasedSegmentation Volume

  set newvol [DevCreateNewCopiedVolume $volID ""  $resname ]
  set node [Volume($newvol,vol) GetMrmlNode]
  Mrml(dataTree) RemoveItem $node 
  set nodeBefore [Volume($volID,vol) GetMrmlNode]
  Mrml(dataTree) InsertAfterItem $nodeBefore $node
  MainUpdateMRML

  return $newvol
}

#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationBuildVTK {} {
global RuleBasedSegmentation

vtkMatrix4x4 RuleBasedSegmentation(tmpMatrix)
vtkMatrix4x4 RuleBasedSegmentation(tmp2Matrix)

}

#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc RuleBasedSegmentationEnter {} {
    global RuleBasedSegmentation
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $RuleBasedSegmentation(eventManager)
    
    #Set all slicer windows to have slices orientation
    #MainSlicesSetOrientAll Slices
    
    #Create 3D cube
    foreach t "DLPFC Striatum" {
  RuleBasedSegmentationCreate3DCube $t
    }



    #Change welcome logo if it exits under ./image
    set modulepath $::PACKAGE_DIR_VTKRuleBasedSegmentation/../../../images
    if {[file exist [ExpandPath [file join \
                     $modulepath "welcome.ppm"]]]} {
        image create photo iWelcome \
        -file [ExpandPath [file join $modulepath "welcome.ppm"]]
    }    


}


#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
    
    #Remove 3D Cube       ************************IMPORTANT!!!
    foreach t "DLPFC Striatum" {
  RuleBasedSegmentationDelete3DCube $t
    }
    #Restore standard slicer logo
    image create photo iWelcome \
        -file [ExpandPath [file join gui "welcome.ppm"]]
}

#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationUpdateGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationUpdateGUI {} {
    global RuleBasedSegmentation Volume

    puts "in update GUI for Semi Segmentor module";
    #Hanifa
    #I changed the following line so that the GUI picks up the initial extent origin and spacing
    #values as soon as they are selected on the menu. This also required a change in Developer.tcl
    
    # this is for the DLPFC tab
    DevUpdateNodeSelectButton Volume RuleBasedSegmentation DLPFCVolumeIn DLPFCVolumeIn DevSelectNode 1 0 1 {RuleBasedSegmentationGetInitParams DLPFC}
    DevUpdateNodeSelectButton Volume RuleBasedSegmentation DLPFCLabelVolumeIn DLPFCLabelVolumeIn DevSelectNode 1 0 1 {RuleBasedSegmentationGetInitParams DLPFC}


    # this is for the Striatum tab
    DevUpdateNodeSelectButton Volume RuleBasedSegmentation StriatumLabelVolumeIn StriatumLabelVolumeIn DevSelectNode 1 0 1 {RuleBasedSegmentationGetInitParams Striatum}
  
    DevUpdateNodeSelectButton Volume RuleBasedSegmentation StriatumVolumeIn StriatumVolumeIn DevSelectNode 1 0 1 {RuleBasedSegmentationGetInitParams Striatum}
     
}

#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationGetInitParams
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationGetInitParams {tab} {
    global RuleBasedSegmentation Volume Module    
    
    if {$tab == "DLPFC"} {
  
  set volID $RuleBasedSegmentation(DLPFCVolumeIn)
  ################# I BELIEVE THAT THE PROBLEM WITH NOT SETTING DLPFCLabelVolumeIn is here ###############
  set volID2 $RuleBasedSegmentation(DLPFCLabelVolumeIn)
    } elseif {$tab == "Striatum"} {

  #TODO: we need to deal better with the case where the labelvolumein is set, but not the volumein

  if { $RuleBasedSegmentation(StriatumVolumeIn) == 0 } {
      set volID $RuleBasedSegmentation(StriatumLabelVolumeIn)
  } else {
      set volID $RuleBasedSegmentation(StriatumLabelVolumeIn)
  }
  set volID2 $RuleBasedSegmentation(StriatumLabelVolumeIn)
    }

    set RuleBasedSegmentation(InOrigin) [[Volume($volID,vol) GetOutput] GetOrigin]
    set RuleBasedSegmentation(InSpacing) [[Volume($volID,vol) GetOutput] GetSpacing]
    set RuleBasedSegmentation(InExtent) [[Volume($volID,vol) GetOutput] GetExtent] 
    
    set RuleBasedSegmentation(OutputName) "[Volume($volID,node) GetName]_roi"
    set RuleBasedSegmentation(OutputName2) "[Volume($RuleBasedSegmentation(DLPFCVolumeIn),node) GetName]labelmap_roi"
    
    set Ext $RuleBasedSegmentation(InExtent)
    
    RuleBasedSegmentation(tmpMatrix) DeepCopy [Volume($volID,node) GetWldToIjk]
    
    
    RuleBasedSegmentation(tmpMatrix) Invert RuleBasedSegmentation(tmpMatrix) RuleBasedSegmentation(tmpMatrix)    
    
    set IJKtoRASmin [RuleBasedSegmentation(tmpMatrix) MultiplyPoint [lindex $Ext 0] [lindex $Ext 2] [lindex $Ext 4] 1]
    set IJKtoRASmax [RuleBasedSegmentation(tmpMatrix) MultiplyPoint [lindex $Ext 1] [lindex $Ext 3] [lindex $Ext 5] 1] 
    

    if {$::Module(verbose) == 1} {
        puts $IJKtoRASmin

        puts $IJKtoRASmax
    }
            
    
    #Resort values
    foreach e "0 1 2" {
        if {[lindex $IJKtoRASmin $e] <= [lindex $IJKtoRASmax $e]} {
           lappend RASmin [lindex $IJKtoRASmin $e]
           lappend RASmax [lindex $IJKtoRASmax $e]
        } else {
           lappend RASmax [lindex $IJKtoRASmin $e]
           lappend RASmin [lindex $IJKtoRASmax $e]
        }      
   }
   
    if {$::Module(verbose) == 1} {
        puts $RASmin
        puts $RASmax 
    }

    #### for cube in DLPFC ###############

    set t $tab
    #set fDLPFC $Module(RuleBasedSegmentation,fDLPFC)
    set f $Module(RuleBasedSegmentation,f$t)
    
    if {$volID == $RuleBasedSegmentation($t,Ext3D,volId)} {
  #Nothing to do, same volume
    } else { 
  
  set RuleBasedSegmentation($t,Ext3D,AxMin,min) [lindex $RASmin 2]
  set RuleBasedSegmentation($t,Ext3D,AxMin,max) [lindex $RASmax 2]
  
  set RuleBasedSegmentation($t,Ext3D,AxMax,min) [lindex $RASmin 2]
  set RuleBasedSegmentation($t,Ext3D,AxMax,max) [lindex $RASmax 2]
  
  set RuleBasedSegmentation($t,Ext3D,SagMin,min) [lindex $RASmin 0]
  set RuleBasedSegmentation($t,Ext3D,SagMin,max) [lindex $RASmax 0]
  
  set RuleBasedSegmentation($t,Ext3D,SagMax,min) [lindex $RASmin 0]
  set RuleBasedSegmentation($t,Ext3D,SagMax,max) [lindex $RASmax 0]
  
  set RuleBasedSegmentation($t,Ext3D,CorMin,min) [lindex $RASmin 1]
  set RuleBasedSegmentation($t,Ext3D,CorMin,max) [lindex $RASmax 1]
  
  set RuleBasedSegmentation($t,Ext3D,CorMax,min) [lindex $RASmin 1]
  set RuleBasedSegmentation($t,Ext3D,CorMax,max) [lindex $RASmax 1] 
  
  set RuleBasedSegmentation($t,Ext3D,volId) $volID
  
  if {$t == "Striatum"} {
      foreach type "AxMin AxMax SagMin SagMax CorMin CorMax" {
    $f.f3D.fControl.f$type.s$type configure -from $RuleBasedSegmentation($t,Ext3D,$type,min) -to $RuleBasedSegmentation($t,Ext3D,$type,max)
      }
  } elseif {$t == "DLPFC"} {
      foreach type "AxMin AxMax CorMin CorMax" {
    $f.f3D.fControl.f$type.s$type configure -from $RuleBasedSegmentation($t,Ext3D,$type,min) -to $RuleBasedSegmentation($t,Ext3D,$type,max)
      }
  }
  
  set RuleBasedSegmentation($t,Ext3D,AxMin) [lindex $RASmin 2]
  set RuleBasedSegmentation($t,Ext3D,AxMax) [lindex $RASmax 2]
  set RuleBasedSegmentation($t,Ext3D,SagMax) [lindex $RASmax 0]
  set RuleBasedSegmentation($t,Ext3D,SagMin) [lindex $RASmin 0]
  set RuleBasedSegmentation($t,Ext3D,CorMin) [lindex $RASmin 1]
  set RuleBasedSegmentation($t,Ext3D,CorMax) [lindex $RASmax 1]
    }
    
    
    if {$volID2 == $RuleBasedSegmentation($t,Ext3D,volId2)} {
  #Nothing to do, same volume
    } else { 
  
  set RuleBasedSegmentation($t,Ext3D,AxMin,min) [lindex $RASmin 2]
  set RuleBasedSegmentation($t,Ext3D,AxMin,max) [lindex $RASmax 2]
  
  set RuleBasedSegmentation($t,Ext3D,AxMax,min) [lindex $RASmin 2]
  set RuleBasedSegmentation($t,Ext3D,AxMax,max) [lindex $RASmax 2]
  
  set RuleBasedSegmentation($t,Ext3D,SagMin,min) [lindex $RASmin 0]
  set RuleBasedSegmentation($t,Ext3D,SagMin,max) [lindex $RASmax 0]
  
  set RuleBasedSegmentation($t,Ext3D,SagMax,min) [lindex $RASmin 0]
  set RuleBasedSegmentation($t,Ext3D,SagMax,max) [lindex $RASmax 0]
  
  set RuleBasedSegmentation($t,Ext3D,CorMin,min) [lindex $RASmin 1]
  set RuleBasedSegmentation($t,Ext3D,CorMin,max) [lindex $RASmax 1]
  
  set RuleBasedSegmentation($t,Ext3D,CorMax,min) [lindex $RASmin 1]
  set RuleBasedSegmentation($t,Ext3D,CorMax,max) [lindex $RASmax 1] 
  
  set RuleBasedSegmentation($t,Ext3D,volId2) $volID2
  
  if {$t == "Striatum"} {
      foreach type "AxMin AxMax SagMin SagMax CorMin CorMax" {
    $f.f3D.fControl.f$type.s$type configure -from $RuleBasedSegmentation($t,Ext3D,$type,min) -to $RuleBasedSegmentation($t,Ext3D,$type,max)
      }
  } elseif {$t == "DLPFC"} {
      foreach type "AxMin AxMax CorMin CorMax" {
    $f.f3D.fControl.f$type.s$type configure -from $RuleBasedSegmentation($t,Ext3D,$type,min) -to $RuleBasedSegmentation($t,Ext3D,$type,max)
      }
  }
  
  set RuleBasedSegmentation($t,Ext3D,AxMin) [lindex $RASmin 2]
  set RuleBasedSegmentation($t,Ext3D,AxMax) [lindex $RASmax 2]
  set RuleBasedSegmentation($t,Ext3D,SagMax) [lindex $RASmax 0]
  set RuleBasedSegmentation($t,Ext3D,SagMin) [lindex $RASmin 0]
  set RuleBasedSegmentation($t,Ext3D,CorMin) [lindex $RASmin 1]
  set RuleBasedSegmentation($t,Ext3D,CorMax) [lindex $RASmax 1]
    }
    
    RuleBasedSegmentationRenderCube $t 
    


}

#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationPick3D
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationPick3D { tab type } {

  global RuleBasedSegmentation 
  set RuleBasedSegmentation($tab,Ext3D,$type) [Slicer GetOffset $RuleBasedSegmentation($tab,Ext3D,$type,Id)]
  RuleBasedSegmentationUpdate3DScales $tab 0
}  

#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationUpdate3DScales
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationUpdate3DScales { tab notUsed } {

  global RuleBasedSegmentation Interactor
  
  set Ext $RuleBasedSegmentation(InExtent)
  
  if {$tab == "DLPFC"} {
      set volID $RuleBasedSegmentation(DLPFCVolumeIn)
  } elseif {$tab == "Striatum"} {
      if {$RuleBasedSegmentation(StriatumVolumeIn) == 0} {
    set volID $RuleBasedSegmentation(StriatumLabelVolumeIn)
      } else {
    set volID $RuleBasedSegmentation(StriatumVolumeIn)
      }
  }
  #Convert RAS ext to ijk ext
  
  RuleBasedSegmentation(tmpMatrix) DeepCopy [Volume($volID,node) GetWldToIjk]
    
  #   set val [Volume($volID,node) GetRasToVtkMatrix]
  #  foreach r "0 1 2 3" {
  #     foreach c "0 1 2 3" {
  #       RuleBasedSegmentation(tmpMatrix) SetElement $r $c [lindex $val [expr 4*$r + $c]]
  #      }
  #  }    
  
  set RAStoIJKmin [RuleBasedSegmentation(tmpMatrix) MultiplyPoint $RuleBasedSegmentation($tab,Ext3D,SagMin) $RuleBasedSegmentation($tab,Ext3D,CorMin) $RuleBasedSegmentation($tab,Ext3D,AxMin) 1]
  set RAStoIJKmax [RuleBasedSegmentation(tmpMatrix) MultiplyPoint $RuleBasedSegmentation($tab,Ext3D,SagMax) $RuleBasedSegmentation($tab,Ext3D,CorMax) $RuleBasedSegmentation($tab,Ext3D,AxMax) 1]
  
  
  
  foreach e "0 1 2" {
        if {[lindex $RAStoIJKmin $e] <= [lindex $RAStoIJKmax $e]} {
           lappend IJKmin [lindex $RAStoIJKmin $e]
           lappend IJKmax [lindex $RAStoIJKmax $e]
        } else {
           lappend IJKmax [lindex $RAStoIJKmin $e]
           lappend IJKmin [lindex $RAStoIJKmax $e]
        }      
   }
   
  set RuleBasedSegmentation($tab,Ext3D,Ijk) "[lindex $IJKmin 0] [lindex $IJKmax 0] [lindex $IJKmin 1] [lindex $IJKmax 1] [lindex $IJKmin 2] [lindex $IJKmax 2]"

    if {$::Module(verbose) == 1} {
        puts $RuleBasedSegmentation($tab,Ext3D,Ijk)
    }
  
    if { [info command RuleBasedSegmentation($tab,Ext3D,CubeActor)] == "" } {
        return
    }

  
  if {$RuleBasedSegmentation($tab,Ext3D,RenderCube) == 1} {
                           #[expr [lindex $Ext 1] - $RuleBasedSegmentation(Ext3D,SagMax)] \
               #[expr [lindex $Ext 1] - $RuleBasedSegmentation(Ext3D,SagMin)] \

    RuleBasedSegmentation($tab,Ext3D,Cube) SetBounds \
               $RuleBasedSegmentation($tab,Ext3D,SagMin) $RuleBasedSegmentation($tab,Ext3D,SagMax) \
               $RuleBasedSegmentation($tab,Ext3D,CorMin) $RuleBasedSegmentation($tab,Ext3D,CorMax) \
               $RuleBasedSegmentation($tab,Ext3D,AxMin) $RuleBasedSegmentation($tab,Ext3D,AxMax)
                     
    RuleBasedSegmentation(tmp2Matrix) Identity
    
    RuleBasedSegmentation($tab,Ext3D,CubeXform) SetMatrix RuleBasedSegmentation(tmp2Matrix)
    RuleBasedSegmentation($tab,Ext3D,CubeXform) Inverse
    
    RuleBasedSegmentation($tab,Ext3D,XformFilter) SetTransform RuleBasedSegmentation($tab,Ext3D,CubeXform)
  
    RuleBasedSegmentation($tab,Ext3D,CubeMapper) Update
    RuleBasedSegmentation($tab,Ext3D,OutlineMapper) Update
  
    eval [RuleBasedSegmentation($tab,Ext3D,CubeActor) GetProperty] SetColor $RuleBasedSegmentation($tab,Ext3D,CubeColor)
    eval [RuleBasedSegmentation($tab,Ext3D,CubeActor) GetProperty] SetOpacity $RuleBasedSegmentation($tab,Ext3D,CubeOpacity)
    
    eval [RuleBasedSegmentation($tab,Ext3D,OutlineActor) GetProperty] SetColor $RuleBasedSegmentation($tab,Ext3D,OutlineColor)
    eval [RuleBasedSegmentation($tab,Ext3D,OutlineActor) GetProperty] SetOpacity $RuleBasedSegmentation($tab,Ext3D,OutlineOpacity)  
  
  } else {
    eval [RuleBasedSegmentation($tab,Ext3D,CubeActor) GetProperty] SetOpacity 0
    eval [RuleBasedSegmentation($tab,Ext3D,OutlineActor) GetProperty] SetOpacity 0
  }  

set startX [expr round([lindex $RuleBasedSegmentation($tab,Ext3D,Ijk) 0])]
set endX [expr round([lindex $RuleBasedSegmentation($tab,Ext3D,Ijk) 1])]

#sagittal
set startY [expr round([lindex $RuleBasedSegmentation($tab,Ext3D,Ijk) 2])]
set endY [expr round([lindex $RuleBasedSegmentation($tab,Ext3D,Ijk) 3])]

#coronal
set startZ [expr round([lindex $RuleBasedSegmentation($tab,Ext3D,Ijk) 4])]
set endZ [expr round([lindex $RuleBasedSegmentation($tab,Ext3D,Ijk) 5])]
#puts "$startX $endX $startY $endY $startZ $endZ"



  Render3D 
}



#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationCreate3DCube
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationCreate3DCube {tab} {

  global RuleBasedSegmentation
  
  vtkCubeSource RuleBasedSegmentation($tab,Ext3D,Cube)
  vtkOutlineFilter RuleBasedSegmentation($tab,Ext3D,Outline)
  vtkTubeFilter RuleBasedSegmentation($tab,Ext3D,Tube)
  vtkTransform  RuleBasedSegmentation($tab,Ext3D,CubeXform)
  vtkTransformPolyDataFilter RuleBasedSegmentation($tab,Ext3D,XformFilter)
  vtkPolyDataMapper RuleBasedSegmentation($tab,Ext3D,CubeMapper)
  vtkPolyDataMapper RuleBasedSegmentation($tab,Ext3D,OutlineMapper)
  vtkActor RuleBasedSegmentation($tab,Ext3D,CubeActor)
  vtkActor RuleBasedSegmentation($tab,Ext3D,OutlineActor)
  #Create Pipeline
  
  RuleBasedSegmentation($tab,Ext3D,XformFilter) SetTransform RuleBasedSegmentation($tab,Ext3D,CubeXform)
  RuleBasedSegmentation($tab,Ext3D,XformFilter) SetInput [RuleBasedSegmentation($tab,Ext3D,Cube) GetOutput]
  RuleBasedSegmentation($tab,Ext3D,Outline) SetInput [RuleBasedSegmentation($tab,Ext3D,XformFilter) GetOutput]
  RuleBasedSegmentation($tab,Ext3D,Tube) SetInput [RuleBasedSegmentation($tab,Ext3D,Outline) GetOutput]
  RuleBasedSegmentation($tab,Ext3D,Tube) SetRadius 0.1
  RuleBasedSegmentation($tab,Ext3D,CubeMapper) SetInput [RuleBasedSegmentation($tab,Ext3D,XformFilter) GetOutput]
  RuleBasedSegmentation($tab,Ext3D,CubeActor) SetMapper RuleBasedSegmentation($tab,Ext3D,CubeMapper)
  RuleBasedSegmentation($tab,Ext3D,OutlineMapper) SetInput [RuleBasedSegmentation($tab,Ext3D,Tube) GetOutput]
  RuleBasedSegmentation($tab,Ext3D,OutlineActor) SetMapper RuleBasedSegmentation($tab,Ext3D,OutlineMapper)
  #Set up default Actor properties
  eval "[RuleBasedSegmentation($tab,Ext3D,CubeActor) GetProperty] SetColor" $RuleBasedSegmentation($tab,Ext3D,CubeColor)
  eval "[RuleBasedSegmentation($tab,Ext3D,CubeActor) GetProperty] SetOpacity" 0
  RuleBasedSegmentation($tab,Ext3D,CubeActor) PickableOff
  eval "[RuleBasedSegmentation($tab,Ext3D,OutlineActor) GetProperty] SetColor" $RuleBasedSegmentation($tab,Ext3D,OutlineColor)
  eval "[RuleBasedSegmentation($tab,Ext3D,OutlineActor) GetProperty] SetOpacity" 0
  RuleBasedSegmentation($tab,Ext3D,OutlineActor) PickableOff
  
  
  #Add to the renderer
  MainAddActor RuleBasedSegmentation($tab,Ext3D,CubeActor)
  MainAddActor RuleBasedSegmentation($tab,Ext3D,OutlineActor)

}

#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationDelete3DCube
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationDelete3DCube {tab} {
  
  MainRemoveActor RuleBasedSegmentation($tab,Ext3D,CubeActor)
  MainRemoveActor RuleBasedSegmentation($tab,Ext3D,OutlineActor)
  Render3D
  
  #Delete Objects
  RuleBasedSegmentation($tab,Ext3D,Cube) Delete
  RuleBasedSegmentation($tab,Ext3D,Outline) Delete
  RuleBasedSegmentation($tab,Ext3D,Tube) Delete
  RuleBasedSegmentation($tab,Ext3D,CubeXform) Delete
  RuleBasedSegmentation($tab,Ext3D,XformFilter) Delete
  RuleBasedSegmentation($tab,Ext3D,CubeMapper) Delete
  RuleBasedSegmentation($tab,Ext3D,CubeActor) Delete
  RuleBasedSegmentation($tab,Ext3D,OutlineMapper) Delete
  RuleBasedSegmentation($tab,Ext3D,OutlineActor) Delete
}  

#-------------------------------------------------------------------------------
# .PROC RuleBasedSegmentationRenderCube
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RuleBasedSegmentationRenderCube {tab } {

  global RuleBasedSegmentation
  
  if { $RuleBasedSegmentation($tab,Ext3D,RenderCube) == 1} {
    RuleBasedSegmentationUpdate3DScales $tab 0
  } else {
    eval [RuleBasedSegmentation($tab,Ext3D,CubeActor) GetProperty] SetOpacity 0
    eval [RuleBasedSegmentation($tab,Ext3D,OutlineActor) GetProperty] SetOpacity 0
    Render3D  
  }
}


###################### STRIATUM routines #####################################


proc RuleBasedSegmentation_Striatum_Delineate { } {

global RuleBasedSegmentation Volume


#make cube transparent
#set RuleBasedSegmentation(Striatum,Ext3D,RenderCube) 0
#RuleBasedSegmentationRenderCube Striatum


# inputs: the segmented striatum label map and the markings
# outputs: segmented substructures and the algrithm markings

############## create outputs ###########################

#set name "$RuleBasedSegmentation(Striatum_OutputName)_Marked"

set volID $RuleBasedSegmentation(StriatumLabelVolumeIn)
##set name "RuleBasedSegmentation_Algorithm_Markings"
#set description "Automatic markings from the algorithm that segments the Nucleus Accumbens"

## copiedID is the new id of the volume
#set copiedID [DevCreateNewCopiedVolume $volID $description $name]
##########This is the output name for the markings volume
#set RuleBasedSegmentation(Striatum_AlgoMarking_id) $copiedID 


#MainVolumesCopyData $copiedID $volID "Off"
#Volume($copiedID,node) SetLabelMap    1

#this needs to be called to update the Mrml Tree and the slicer structure
#MainUpdateMRML
#MainVolumesUpdate $copiedID



set description "segmentation of the striatum"

set name "$RuleBasedSegmentation(Striatum_OutputName)_labelmap"

# copiedID is the new id of the volume
set copiedID2 [DevCreateNewCopiedVolume $volID $description $name]
############This is the output name for the labelmap volume
set RuleBasedSegmentation(Striatum_SegResult_id) $copiedID2 

MainVolumesCopyData $copiedID2 $volID "Off"

Volume($copiedID2,node) SetLabelMap    1
#this needs to be called to update the Mrml Tree and the slicer structure
MainUpdateMRML
MainVolumesUpdate $copiedID2



########################################################




# in the markings volume, we have 3 points that we care about:
# - the ACPC point in label color 18 (once)
# - the left intersection and right intersection points in color 17 (per slice)

# scan from z=0 to z = Zsize each slice and find the left/right intersection and ACPC



###########this is where the values for the first and last slice of the labelmap are set###############
#set startZ 129
#set endZ 180

#set startX 92
#set startY 102
#set endX 166
#set endY 147

#axial
set startX [expr round([lindex $RuleBasedSegmentation(Striatum,Ext3D,Ijk) 0])]
set endX [expr round([lindex $RuleBasedSegmentation(Striatum,Ext3D,Ijk) 1])]

#sagittal
set startY [expr round([lindex $RuleBasedSegmentation(Striatum,Ext3D,Ijk) 2])]
set endY [expr round([lindex $RuleBasedSegmentation(Striatum,Ext3D,Ijk) 3])]

#coronal
set startZ [expr round([lindex $RuleBasedSegmentation(Striatum,Ext3D,Ijk) 4])]
set endZ [expr round([lindex $RuleBasedSegmentation(Striatum,Ext3D,Ijk) 5])]


#To write data to a id in a volume
# $scalars SetTuple1 $id $value

# to access the data in a volume (value)
set imagedata [Volume($RuleBasedSegmentation(StriatumVolumeIn),vol) GetOutput]
set scalars [[$imagedata GetPointData] GetScalars]
#set value [$scalars GetTuple1 $id]

#loop over scalars
set xsize [lindex [$imagedata GetDimensions] 0]
set ysize [lindex [$imagedata GetDimensions] 1]
set zsize [lindex [$imagedata GetDimensions] 2]





for {set z $startZ} {$z <= $endZ} {incr z} {
  for {set y $startY} {$y <= $endY} {incr y} {
    for {set x $startX} {$x <= $endX} {incr x} {

    set id [expr $x + $y * ($xsize) + $z * ($xsize * $ysize)]
    set value [$scalars GetTuple1 $id]
    if {$value == 18} {
    # we found acpc plane
    set RuleBasedSegmentation(ACPC,x) $x
    set RuleBasedSegmentation(ACPC,y) $y
    set RuleBasedSegmentation(ACPC,z) $z
    }
     }
  }
}

# these are your bounding boxes

set startLeftX $startX
set endLeftX $RuleBasedSegmentation(ACPC,x)
set startLeftY $startY
set endLeftY $endY
set startLeftZ $startZ
set endLeftZ $endZ

set startRightX $RuleBasedSegmentation(ACPC,x)
set endRightX $endX
set startRightY $startY
set endRightY $endY
set startRightZ $startZ
set endRightZ $endZ

################ work on right

# scalars is now labelmap
# for each slice
# find intersection point (if doesn't exist go to next)

for {set z $startRightZ} {$z <= $endRightZ } {incr z} {

  set RuleBasedSegmentation(intersectionRight,$z,x) 0
  set RuleBasedSegmentation(intersectionRight,$z,y) 0

}


set RuleBasedSegmentation(lastinput) 100000

#Find all of the intersection points
#To access the intersection points   $RuleBasedSegmentation(intersection,$slicenum,x) and $RuleBasedSegmentation(intersection,$slicenum,y)
for {set z $startRightZ} {$z <= $endRightZ } {incr z} {
  for {set y $startRightY} {$y <= $endRightY} {incr y} {
    for {set x $startRightX} {$x <= $endRightX} {incr x} {

    set id [expr $x + $y * ($xsize) + $z * ($xsize * $ysize)]
    set value [$scalars GetTuple1 $id]
            if {$value == 17} {
    # we found intersection on this slice
    set RuleBasedSegmentation(intersectionRight,$z,x) $x
    set RuleBasedSegmentation(intersectionRight,$z,y) $y
      if {$z < $RuleBasedSegmentation(lastinput)} {

        set RuleBasedSegmentation(lastinput) $z

      }
    }
     }
  }
}

set RuleBasedSegmentation(diff,Right) 0


set imagedata [Volume($RuleBasedSegmentation(StriatumLabelVolumeIn),vol) GetOutput]
set scalars [[$imagedata GetPointData] GetScalars]

#set imagedata [Volume($RuleBasedSegmentation(Striatum_AlgoMarking_id),vol) GetOutput]
#set scalars2 [[$imagedata GetPointData] GetScalars]

set imagedata [Volume($RuleBasedSegmentation(Striatum_SegResult_id),vol) GetOutput]
set scalars3 [[$imagedata GetPointData] GetScalars]

#Set all of the label to one value on both sides:  19

for {set z $startZ} {$z <= $endZ} {incr z} {
  for {set y $startY} {$y <= $endY} {incr y} {
    for {set x $startX} {$x <= $endX} {incr x} {
      set id [expr $x + $y * ($xsize) + $z * ($xsize * $ysize)]
      set value [$scalars GetTuple1 $id]
      if {$value != 0} {
        #$scalars2 SetTuple1 $id 19
        $scalars3 SetTuple1 $id 19
      }
    }
  }
}




#Drop from the intersection point down until leave the label
for {set z $startRightZ} {$z <= $endRightZ } {incr z} {

  if {$RuleBasedSegmentation(intersectionRight,$z,x) != 0 } {
    set RuleBasedSegmentation(diff,Right) 0
    set id [expr $RuleBasedSegmentation(intersectionRight,$z,x) + $RuleBasedSegmentation(intersectionRight,$z,y) * ($xsize) + $z * ($xsize * $ysize)]

    while {$RuleBasedSegmentation(diff,Right) >= 0} {   
      set value [$scalars GetTuple1 $id]
      set value2 [$scalars GetTuple1 [expr $id - $xsize]]

              set RuleBasedSegmentation(diff,Right) [expr $value2 - $value]

      set RuleBasedSegmentation(inferior,$z,x) $RuleBasedSegmentation(intersectionRight,$z,x)
      set RuleBasedSegmentation(inferior,$z,y) [expr ($id-$RuleBasedSegmentation(intersectionRight,$z,x)-$z*$xsize*$ysize)/$xsize]
    
      #$scalars2 SetTuple1 $id 17
    
      set id [expr $id - $xsize]

    }

  }

}


# Finding the most lateral and medial points on the ACPC line

set RuleBasedSegmentation(diff,Right) 0
set counter 0


for {set z $startRightZ} {$z <= $endRightZ } {incr z} {

  if {$RuleBasedSegmentation(intersectionRight,$z,x) != 0 } {
    set counter 0
    for {set x $startRightX} {$x <= $endRightX} {incr x} {
  
      set id [expr $x + $RuleBasedSegmentation(ACPC,y) * ($xsize) + $z * ($xsize * $ysize) + 1]

      set value [$scalars GetTuple1 $id]
      set value2 [$scalars GetTuple1 [expr $id - 1]]

      if {[expr $value - $value2] != 0} {
        if {$counter == 0} {
      
          set RuleBasedSegmentation(medial,$z,x) [expr $x + 1]
              
        }
        if {$counter != 0} {

          set RuleBasedSegmentation(lateral,$z,x) $x
        
          set RuleBasedSegmentation(midpoint,$z,x) [expr ($RuleBasedSegmentation(medial,$z,x) + $RuleBasedSegmentation(lateral,$z,x))/2]
                
        }

        set counter [expr $counter + 1]

      }

    }

  }

}


#for {set z $startRightZ} {$z <= $endRightZ } {incr z} {
# if {$RuleBasedSegmentation(intersectionRight,$z,x) != 0 } {
#   set id [expr $RuleBasedSegmentation(midpoint,$z,x) + $RuleBasedSegmentation(ACPC,y) * ($xsize) + $z * ($xsize * $ysize) ]
#   $scalars2 SetTuple1 $id 39
#
#   set id [expr $RuleBasedSegmentation(medial,$z,x) + $RuleBasedSegmentation(ACPC,y) * ($xsize) + $z * ($xsize * $ysize) ]
#   $scalars2 SetTuple1 $id 39
#
#   set id [expr $RuleBasedSegmentation(lateral,$z,x) + $RuleBasedSegmentation(ACPC,y) * ($xsize) + $z * ($xsize * $ysize) ]
#   $scalars2 SetTuple1 $id 39
# }
#
#}



#Find which points are in the ventral striatum, dorsal striatum, and pre/post areas

set imagedata [Volume($RuleBasedSegmentation(StriatumLabelVolumeIn),vol) GetOutput]
set scalars [[$imagedata GetPointData] GetScalars]

set imagedata [Volume($RuleBasedSegmentation(Striatum_SegResult_id),vol) GetOutput]
set scalars2 [[$imagedata GetPointData] GetScalars]

for {set z $startRightZ} {$z <= $endRightZ } {incr z} {
  if {$RuleBasedSegmentation(intersectionRight,$z,x) == 0 } {
    for {set y $startRightY} {$y <= $endRightY} {incr y} {
      for {set x $startRightX} {$x <= $endRightX} {incr x} {

        set id [expr $x + $y * ($xsize) + $z * ($xsize * $ysize)]
        set value [$scalars GetTuple1 $id]

          if {$value != 0} {

            if {$y > $RuleBasedSegmentation(ACPC,y)} {
          
              if {$z <= $RuleBasedSegmentation(ACPC,z)} {
                #The post caudata/putamen
                $scalars2 SetTuple1 $id 12

              }
              if {$z > $RuleBasedSegmentation(ACPC,z)} {
                #The pre caudata/putamen
                $scalars2 SetTuple1 $id 15

              }

            }
            if {$y <= $RuleBasedSegmentation(ACPC,y)} {
              if {$z >= $RuleBasedSegmentation(lastinput)} {
                #The ventral striatum
                $scalars2 SetTuple1 $id 14
              }
              if {$z < $RuleBasedSegmentation(lastinput)} {
                if {$z > $RuleBasedSegmentation(ACPC,z)} {
                  #The pre caudate/putamen
                  $scalars2 SetTuple1 $id 15
                }
                if {$z <= $RuleBasedSegmentation(ACPC,z)} {
                  #The post caudate/putamen
                  $scalars2 SetTuple1 $id 12

                }
              }

            }
    
          }

      }
    }
  }
  if {$RuleBasedSegmentation(intersectionRight,$z,x) != 0 } {
  for {set y $startRightY} {$y <= $endRightY} {incr y} {
    for {set x $startRightX} {$x <= $endRightX} {incr x} {

      set id [expr $x + $y * ($xsize) + $z * ($xsize * $ysize)]
      set value [$scalars GetTuple1 $id]
      
      if {$value != 0} {

        if {$RuleBasedSegmentation(inferior,$z,y) >= $RuleBasedSegmentation(ACPC,y)} {
          if {$y > $RuleBasedSegmentation(ACPC,y)} {
          
            if {$z <= $RuleBasedSegmentation(ACPC,z)} {
              #The post caudate/putamen
              $scalars2 SetTuple1 $id 12

            }
            if {$z > $RuleBasedSegmentation(ACPC,z)} {
              #The pre caudate/putamen
              $scalars2 SetTuple1 $id 15

            }

          }

          if {$y <= $RuleBasedSegmentation(ACPC,y)} {

            #The ventral striatum
            $scalars2 SetTuple1 $id 14

          }
    
        }
  
  
        if {$RuleBasedSegmentation(inferior,$z,y) < $RuleBasedSegmentation(ACPC,y)} {
          
          #point 1 is inferior
          #point 2 is midpoint

          set slope [expr 10^100]

          if {$RuleBasedSegmentation(inferior,$z,x) != $RuleBasedSegmentation(midpoint,$z,x)} {
            set slope [expr (1.000000000000 * $RuleBasedSegmentation(inferior,$z,y)-$RuleBasedSegmentation(ACPC,y))/($RuleBasedSegmentation(inferior,$z,x)-$RuleBasedSegmentation(midpoint,$z,x))]
          }

          set pos_neg [expr 1.00000000000 * $y - $slope * ( $x - $RuleBasedSegmentation(inferior,$z,x) ) - $RuleBasedSegmentation(inferior,$z,y)]

          if {$pos_neg > 0} {

            if {$z <= $RuleBasedSegmentation(ACPC,z)} {
              #The post caudate/putamen
              $scalars2 SetTuple1 $id 12

            }
            if {$z > $RuleBasedSegmentation(ACPC,z)} {
              #The pre caudate/putamen
              $scalars2 SetTuple1 $id 15

            }
        
          }

          if {$pos_neg <= 0} {

            #The ventral striatum
            $scalars2 SetTuple1 $id 14

          }

        }

      }

    }

  }

}}




################ work on Left

set imagedata [Volume($RuleBasedSegmentation(StriatumVolumeIn),vol) GetOutput]
set scalars [[$imagedata GetPointData] GetScalars]

# scalars is now labelmap
# for each slice
# find intersection point (if doesn't exist go to next)

for {set z $startLeftZ} {$z <= $endLeftZ } {incr z} {

  set RuleBasedSegmentation(intersectionLeft,$z,x) 0
  set RuleBasedSegmentation(intersectionLeft,$z,y) 0

}


#Find all of the intersection points
#To access the intersection points   $RuleBasedSegmentation(intersection,$slicenum,x) and $RuleBasedSegmentation(intersection,$slicenum,y)
for {set z $startLeftZ} {$z <= $endLeftZ } {incr z} {
  for {set y $startLeftY} {$y <= $endLeftY} {incr y} {
    for {set x $startLeftX} {$x <= $endLeftX} {incr x} {

    set id [expr $x + $y * ($xsize) + $z * ($xsize * $ysize)]
    set value [$scalars GetTuple1 $id]
            if {$value == 17} {
    # we found intersection on this slice
    set RuleBasedSegmentation(intersectionLeft,$z,x) $x
    set RuleBasedSegmentation(intersectionLeft,$z,y) $y
    }
     }
  }
}

set RuleBasedSegmentation(diff,Left) 0


set imagedata [Volume($RuleBasedSegmentation(StriatumLabelVolumeIn),vol) GetOutput]
set scalars [[$imagedata GetPointData] GetScalars]

#set imagedata [Volume($RuleBasedSegmentation(Striatum_AlgoMarking_id),vol) GetOutput]
#set scalars2 [[$imagedata GetPointData] GetScalars]


#Drop from the intersection point down until leave the label
for {set z $startLeftZ} {$z <= $endLeftZ } {incr z} {

  if {$RuleBasedSegmentation(intersectionLeft,$z,x) != 0 } {
    set RuleBasedSegmentation(diff,Left) 0
    set id [expr $RuleBasedSegmentation(intersectionLeft,$z,x) + $RuleBasedSegmentation(intersectionLeft,$z,y) * ($xsize) + $z * ($xsize * $ysize)]

    while {$RuleBasedSegmentation(diff,Left) >= 0} {    
      set value [$scalars GetTuple1 $id]
      set value2 [$scalars GetTuple1 [expr $id - $xsize]]

              set RuleBasedSegmentation(diff,Left) [expr $value2 - $value]

      set RuleBasedSegmentation(inferior,$z,x) $RuleBasedSegmentation(intersectionLeft,$z,x)
      set RuleBasedSegmentation(inferior,$z,y) [expr ($id-$RuleBasedSegmentation(intersectionLeft,$z,x)-$z*$xsize*$ysize)/$xsize]
    
      #$scalars2 SetTuple1 $id 17
    
      set id [expr $id - $xsize]

    }

  }

}


# Finding the most lateral and medial points on the ACPC line

set RuleBasedSegmentation(diff,Left) 0
set counter 0


for {set z $startLeftZ} {$z <= $endLeftZ } {incr z} {

  if {$RuleBasedSegmentation(intersectionLeft,$z,x) != 0 } {
    set counter 0
    for {set x $startLeftX} {$x <= $endLeftX} {incr x} {
  
      set id [expr $x + $RuleBasedSegmentation(ACPC,y) * ($xsize) + $z * ($xsize * $ysize) + 1]

      set value [$scalars GetTuple1 $id]
      set value2 [$scalars GetTuple1 [expr $id - 1]]

      if {[expr $value - $value2] != 0} {
        if {$counter == 0} {
      
          set RuleBasedSegmentation(medial,$z,x) [expr $x + 1]
              
        }
        if {$counter != 0} {

          set RuleBasedSegmentation(lateral,$z,x) $x
        
          set RuleBasedSegmentation(midpoint,$z,x) [expr ($RuleBasedSegmentation(medial,$z,x) + $RuleBasedSegmentation(lateral,$z,x))/2]
                
        }

        set counter [expr $counter + 1]

      }

    }

  }

}


#for {set z $startLeftZ} {$z <= $endLeftZ } {incr z} {
# if {$RuleBasedSegmentation(intersectionLeft,$z,x) != 0 } {
#   set id [expr $RuleBasedSegmentation(midpoint,$z,x) + $RuleBasedSegmentation(ACPC,y) * ($xsize) + $z * ($xsize * $ysize) ]
#   $scalars2 SetTuple1 $id 39
#
#   set id [expr $RuleBasedSegmentation(medial,$z,x) + $RuleBasedSegmentation(ACPC,y) * ($xsize) + $z * ($xsize * $ysize) ]
#   $scalars2 SetTuple1 $id 39
#
#   set id [expr $RuleBasedSegmentation(lateral,$z,x) + $RuleBasedSegmentation(ACPC,y) * ($xsize) + $z * ($xsize * $ysize) ]
#   $scalars2 SetTuple1 $id 39
# }
#
#}



#Find which points are in the ventral striatum, dorsal striatum, and pre/post areas

set imagedata [Volume($RuleBasedSegmentation(StriatumLabelVolumeIn),vol) GetOutput]
set scalars [[$imagedata GetPointData] GetScalars]

set imagedata [Volume($RuleBasedSegmentation(Striatum_SegResult_id),vol) GetOutput]
set scalars2 [[$imagedata GetPointData] GetScalars]

for {set z $startLeftZ} {$z <= $endLeftZ } {incr z} {
  if {$RuleBasedSegmentation(intersectionLeft,$z,x) == 0 } {
    for {set y $startLeftY} {$y <= $endLeftY} {incr y} {
      for {set x $startLeftX} {$x <= $endLeftX} {incr x} {

        set id [expr $x + $y * ($xsize) + $z * ($xsize * $ysize)]
        set value [$scalars GetTuple1 $id]

          if {$value != 0} {

            if {$y > $RuleBasedSegmentation(ACPC,y)} {
          
              if {$z <= $RuleBasedSegmentation(ACPC,z)} {
                #The post caudata/putamen
                $scalars2 SetTuple1 $id 16

              }
              if {$z > $RuleBasedSegmentation(ACPC,z)} {
                #The pre caudata/putamen
                $scalars2 SetTuple1 $id 11

              }

            }
            if {$y <= $RuleBasedSegmentation(ACPC,y)} {
              if {$z >= $RuleBasedSegmentation(lastinput)} {
                #The ventral striatum
                $scalars2 SetTuple1 $id 13
              }
              if {$z < $RuleBasedSegmentation(lastinput)} {
                if {$z > $RuleBasedSegmentation(ACPC,z)} {
                  #The pre caudate/putamen
                  $scalars2 SetTuple1 $id 11
                }
                if {$z <= $RuleBasedSegmentation(ACPC,z)} {
                  #The post caudate/putamen
                  $scalars2 SetTuple1 $id 16

                }
              }

            }

    
          }

      }
    }
  }
  if {$RuleBasedSegmentation(intersectionLeft,$z,x) != 0 } {
  for {set y $startLeftY} {$y <= $endLeftY} {incr y} {
    for {set x $startLeftX} {$x <= $endLeftX} {incr x} {

      set id [expr $x + $y * ($xsize) + $z * ($xsize * $ysize)]
      set value [$scalars GetTuple1 $id]
      
      if {$value != 0} {

        if {$RuleBasedSegmentation(inferior,$z,y) >= $RuleBasedSegmentation(ACPC,y)} {
          if {$y > $RuleBasedSegmentation(ACPC,y)} {
          
            if {$z <= $RuleBasedSegmentation(ACPC,z)} {
              #The post caudate/putamen
              $scalars2 SetTuple1 $id 16

            }
            if {$z > $RuleBasedSegmentation(ACPC,z)} {
              #The pre caudate/putamen
              $scalars2 SetTuple1 $id 11

            }

          }

          if {$y <= $RuleBasedSegmentation(ACPC,y)} {

            #The ventral striatum
            $scalars2 SetTuple1 $id 13

          }
    
        }
  
  
        if {$RuleBasedSegmentation(inferior,$z,y) < $RuleBasedSegmentation(ACPC,y)} {
          
          #point 1 is inferior
          #point 2 is midpoint

          set slope [expr 10^100]

          if {$RuleBasedSegmentation(inferior,$z,x) != $RuleBasedSegmentation(midpoint,$z,x)} {
            set slope [expr (1.000000000000 * $RuleBasedSegmentation(inferior,$z,y)-$RuleBasedSegmentation(ACPC,y))/($RuleBasedSegmentation(inferior,$z,x)-$RuleBasedSegmentation(midpoint,$z,x))]
          }

          set pos_neg [expr 1.00000000000 * $y - $slope * ( $x - $RuleBasedSegmentation(inferior,$z,x) ) - $RuleBasedSegmentation(inferior,$z,y)]

          if {$pos_neg > 0} {

            if {$z <= $RuleBasedSegmentation(ACPC,z)} {
              #The post caudate/putamen
              $scalars2 SetTuple1 $id 16

            }
            if {$z > $RuleBasedSegmentation(ACPC,z)} {
              #The pre caudate/putamen
              $scalars2 SetTuple1 $id 11

            }
        
          }

          if {$pos_neg <= 0} {

            #The ventral striatum
            $scalars2 SetTuple1 $id 13

          }

        }

      }

    }

  }

}}


puts "DONE!"



}
