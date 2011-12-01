/*=========================================================================

  Program:        OpenCV support for Visualization Toolkit
  Module:         vtkOpenCVImageActor.h
  Contributor(s): Junichi Tokuda (tokuda@bwh.harvard.edu)

  Copyright (c) Brigham and Women's Hospital, All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOpenCVImageActor - abstract specification for actor
// .SECTION Description

// .SECTION See Also
// vtkImageActor vtkRenderWindow vtkImageActor vtkCamera vtkLight vtkVolume

#ifndef __vtkOpenCVImageActor_h
#define __vtkOpenCVImageActor_h

// VTK header files and prototypes
#include "vtkImageActor.h"

class vtkImageData;
class vtkRenderWindow;
class vtkRenderer;
class vtkMapper;

// OpenCV header files and protoptyes
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/highgui/highgui.hpp"

class VTK_RENDERING_EXPORT vtkOpenCVImageActor : public vtkImageActor
{

public:

  vtkTypeMacro(vtkOpenCVImageActor,vtkImageActor);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkOpenCVImageActor *New();

  // Description:
  // SetUseCameraMatrix(1) activates the use of camera calibration matrix.
  // Note that the camera calibration is not automatically activated
  // after calling ImportCameraCalibrationFile()
  vtkSetMacro(UseCameraMatrix, int);
  vtkGetMacro(UseCameraMatrix, int);
  
  // Description:
  // Set VideoCapture class as video source
  // 'capture' must be opened prior to SetVideoCapture() call.
  int SetVideoCapture(cv::VideoCapture * capture);

  // Description:
  // Capture image and render. If image process handler is set,
  // it will be called after importing the captured image.
  int Capture();

  // Description:
  // Import camera calibration matrix from a camera calibration file in OpenCV's
  // format (*.yml).
  int ImportCameraCalibrationFile(const char * calibFile);

  // Description:
  // Get the size of camera image
  void GetImageSize(unsigned int& width, unsigned int& height);

  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS
  // Support the standard render methods.
  // vtkOpenCVImageActor sets vtk camera parameter before rendering process
  // to fit the camera image into the renderer.
  //virtual int RenderTranslucentPolygonalGeometry(vtkViewport *viewport);
  virtual void Render(vtkRenderer *);

//BTX
protected:
  
  vtkOpenCVImageActor();
  ~vtkOpenCVImageActor();
  
  vtkImageData *            VideoImageData;

  cv::Ptr<cv::VideoCapture> VideoSource;
  cv::Mat                   CaptureImage;
  cv::Mat                   CaptureImageTmp;
  cv::Mat                   RGBImage;
  cv::Mat                   GrayImage;
  cv::Mat                   UndistortedImage;
  cv::Size                  ImageSize;

  // OpenCV camera parameters
  int                       UseCameraMatrix;
  cv::Mat                   CameraMatrix, DistCoeffs;
  cv::Size                  CalibratedImageSize;
  cv::Mat                   Map1, Map2;

  std::vector< cv::Mat >    Map1Array;
  std::vector< cv::Mat >    Map2Array;

  // VTK camera parameters
  double                    ParallelScale;
  double                    FocalPointX;
  double                    FocalPointY;
  double                    FocalPointZ;
  
private:

  vtkOpenCVImageActor(const vtkOpenCVImageActor&);  // Not implemented.
  void operator=(const vtkOpenCVImageActor&);  // Not implemented.

  // Description:
  // To improve the speed of undistortion process, the OpenCVImageActor call
  // cv::initUndistortRectifyMap() once as an initialization process and 
  // cv::remap() in each capture, instead of executing both functions in each
  // capture by calling cv::undistort().
  void InitUndistort(int rows, int cols, cv::InputArray _cameraMatrix,
                     cv::InputArray _distCoeffs,
                     std::vector< cv::Mat >& _map1_array,
                     std::vector< cv::Mat >& _map2_array );

  void FastUndistort( cv::InputArray _src, cv::OutputArray _dst,
                      std::vector< cv::Mat >& _map1_array,
                      std::vector< cv::Mat >& _map2_array,
                      int rows, int cols);
  
//ETX
};

#endif
