/*
		 * Copyright: Vehant Technologies (P) Ltd
		 */

		/*
		 * main.cpp
		 * (Source File)
		 * ------------------
		 * To test the HaarCascades code on a set of images, this main file was
		 * created. Provide as a command line argument the directory path to images 
		 * and it'll recursively traverse the directory for images and detect faces
		 * in them.
		 *
		 * Initial Author: Rishabh Pahwa
*/


#include <iostream>
#include <time.h>
#include "HaarCascades.h"
#include "ReadDirectory.h"
#include <math.h>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		cout << "Please provide the directory path to the images as a command line"
		" argument to the program and re-run it. You may optionally provide no of bufferFrames for temporalFiltering as 2nd Argument." << endl;
		return 0;
	}
	int buffersize=0;
	if (argv[2])
	{
		buffersize=atoi(argv[2]);
	}

		ReadDirectory* readDir = new ReadDirectory();
		std::vector<string> inputFileNames = readDir->listFiles(argv[1],true);
		inputFileNames=readDir->sortFileNames();

		HaarCascadesFaceDetect* FD = new HaarCascadesFaceDetect(); 
		// Provide path to haarcascade_frontalface_alt.xml and | or haarcascade_profileface.xml in the above constructor call.
		// string faceCascade_name = "/opt/opencv-2.4.2/share/OpenCV/haarcascades/haarcascade_profileface.xml"; // /opt/opencv-2.4.2/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml
		FD->loadFrontalFaceCascade("/opt/opencv-2.4.2/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml");  // /opt/opencv-2.4.2/share/OpenCV/lbpcascades/lbpcascade_frontalface.xml/
		// clock_t tStart = clock();
		
		vector<string>::const_iterator first = inputFileNames.begin();
		vector<string>::const_iterator last = inputFileNames.begin() + buffersize;
		vector<string> newVec(first, last);

		vector< vector<Rect> > bufferFaces = FD->getBufferedFrames(newVec, 40, 40, buffersize);

		for (int i = buffersize; i<inputFileNames.size();i++)
		{
			cout << inputFileNames[i] << endl;
			if (inputFileNames[i].substr(inputFileNames[i].size()-3,3) == "jpg")
			{
				Mat curImage;
				FD->loadImage(curImage, inputFileNames[i]);
				std::vector<Rect> dets = FD->detectFrontalFaces(curImage, 40, 40);

				std::vector<Rect> bufDet = FD->temporalFiltering(bufferFaces, 30, 1.5);
				FD->saveImage(imread(inputFileNames[i-(buffersize+1)/2]), bufDet, inputFileNames[i-(buffersize+1)/2]);

				FD->maintainBuffer(bufferFaces, dets);
			}
		}
		// printf("Time taken by OpenCV HaarCascades: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
		int c = waitKey(10);

	if (27 == char(c)){
		return 0;
	}

	return 0;
}