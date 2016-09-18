#ifndef _HAAR_CASCADES_FACE_DETECT_
#define _HAAR_CASCADES_FACE_DETECT_

#include <cv.h>
#include <highgui.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

class HaarCascadesFaceDetect
{
	public:
	
	CascadeClassifier frontFaceCascade;
	CascadeClassifier profileFaceCascade;
	string faceCascadePathFrontalFace="";
	string faceCascadePathProfileFace="";


	// string imagePath="";
	int imageCount=0;
	// int imageWidth;
	// int imageHeight;
	// std::vector<Rect> frontalFaces;
	// Mat currentImage;

	
	// Detectes frontal and profile faces.
	
	// Input the path to the image to detect faces in. It save a copy of the image in the same directory
	// as the input image with the faces demarkated (if found).
	
	// Only detectes frontal face features. Provide the path to the cascade file as input argument.
	void loadProfileFaceCascade(string ifaceCascadePathProfileFace);
	
	void loadFrontalFaceCascade(string ifaceCascadePathFrontalFace);
	
	void loadImage(Mat& inImage, std::string currentPath);

	std::vector<vector <Rect> > getBufferedFrames(std::vector<string> inputFileNames, int faceWidth, int faceHeight, int noOfBufferFrames);

	std::vector <Rect> temporalFiltering(vector<vector<Rect> > bufferedFrames, int pixelSeparation, int thresholdCount);

	void maintainBuffer(vector<vector<Rect> >& bufferedFrames, vector<Rect> det);

	vector<Rect> combineDetectedFaces(vector<Rect> face1, vector<Rect> face2);

	std::vector<Rect> detectFrontalFaces(std::string currentPath, int faceWidth, int faceHeight);
	
	std::vector<Rect> detectProfileFaces(std::string currentPath, int faceWidth, int faceHeight);
	
	std::vector<Rect> detectFrontalFaces(Mat inImage, int faceWidth, int faceHeight);

	std::vector<Rect> detectProfileFaces(Mat inImage, int faceWidth, int faceHeight);

	void saveImage(Mat inImage, std::vector<cv::Rect> det, std::string inPath);

	void showImage(Mat inImage, std::vector<cv::Rect> det);

};

#endif