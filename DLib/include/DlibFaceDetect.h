#ifndef _DLIB_FACEDETECT_
#define _DLIB_FACEDETECT_

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <cv.h>
#include <highgui.h>
#include <dlib/opencv/to_open_cv.h>
#include <dlib/svm_threaded.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <math.h>
#include "dlib/image_processing/object_detector.h"
#include <iostream>
#include <fstream>


using namespace cv;
using namespace std;
using namespace dlib;


class DlibFaceDetect
{
  public:

	std::string imagePath;

	int imageHeight;

	int imageWidth;

	int imageCount=0;

	typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 

	object_detector<image_scanner_type> m_trainedDetector;

	string m_trainingDataXml;

	std::vector<dlib::rectangle> dets;

	frontal_face_detector m_detector;

	DlibFaceDetect();

	DlibFaceDetect(string trainingDataXml);

	void getDetector();

	void loadImage(dlib::array2d<bgr_pixel>& inImage, std::string curPath);

	std::vector<dlib::rectangle> detectFaces(std::string curPath, int faceWidth, int faceHeight);

	std::vector<dlib::rectangle> detectFaces(dlib::array2d<bgr_pixel>& inImage, int faceWidth, int faceHeight);

	cv::Rect convertRectangleToRect(dlib::rectangle inRectangle);

	cv::Mat convertToMat(dlib::array2d<bgr_pixel>& inImage);

	void saveImage(array2d<bgr_pixel>& inImage, std::vector<dlib::rectangle> det, std::string inPath);

	void showImage(dlib::array2d<bgr_pixel>& inImage, std::vector<dlib::rectangle> dets);

};

#endif
