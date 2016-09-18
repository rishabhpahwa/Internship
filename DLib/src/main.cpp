/*
     * Copyright: Vehant Technologies (P) Ltd
     */

    /*
     * main.cpp
     * (Source File)
     * ------------------
     * To test the DlibFaceDetect code on a set of images, this main file was
     * created. Provide as a command line argument the directory path to images 
     * and it'll recursively traverse the directory for images and detect faces
     * in them.
     *
     * Initial Author: Rishabh Pahwa
*/


#include <iostream>
#include <time.h>
#include "ReadDirectory.h"
#include "DlibFaceDetect.h"


using namespace dlib;
using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{ 
  if (argc <= 1)
  {
    cout << "Please provide the directory path to the images as a command line"
    " argument to the program and re-run it." << endl;
    return 0;
  }

  ReadDirectory* readDir = new ReadDirectory();
  std::vector<string> inputFileNames = readDir->listFiles(argv[1],true);
  inputFileNames = readDir->sortFileNames();

  DlibFaceDetect* FD = new DlibFaceDetect("/mnt/lprsResults0/Newfd/facedetect/DLib_day/data_4874.xml"); // "/mnt/lprsResults0/Newfd/facedetect/DLib_day/data_4874.xml"
  FD->getDetector();
  clock_t tStart = clock();

  for (int i=0; i < inputFileNames.size(); i++)
  {
    if (inputFileNames[i].substr(inputFileNames[i].size()-3,3) == "jpg")
    {
      cout << inputFileNames[i] << endl;
      // FD->detectAndDisplay(inputFileNames[i].c_str());
      dlib::array2d<bgr_pixel> inputImage;
      FD->loadImage(inputImage, inputFileNames[i]);
      clock_t cStart = clock();
      std::vector <dlib::rectangle> detectedFaces = FD->detectFaces(inputImage,40,40);
      printf("Time taken: %.2fs\n", 
                   (double)(clock() - cStart)/CLOCKS_PER_SEC);

      // Temporal filtering.


      FD->saveImage(inputImage, detectedFaces, inputFileNames[i]);
    }
  }
  printf("Time taken by DlibFaceDetect: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  return 0;
}
