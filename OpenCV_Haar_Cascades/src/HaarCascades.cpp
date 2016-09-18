/*
		 * Copyright: Vehant Technologies (P) Ltd
		 */

		/*
		 * HaarCascades.cpp
		 * (Source File)
		 * ------------------
		 * Defines the functions declared in the HaarCascades.h 
		 * It helps in detecting faces in an Image, save the image with faces,
		 * display the image with faces and even apply temporal filtering if the
		 * input images correspond to a continuous video stream from a camera.
		 * 
		 * Initial Author: Rishabh Pahwa
*/

#include "HaarCascades.h"

// Detectes profile faces.
void HaarCascadesFaceDetect::loadProfileFaceCascade(string ifaceCascadePathProfileFace)
{
	faceCascadePathProfileFace = ifaceCascadePathProfileFace;
	if (!profileFaceCascade.load(faceCascadePathProfileFace))
	{
		printf("--(!)Error loading faceCascadeProfileFace. Please check the path provided.\n");
	}
}

// Detects Frontal Faces.
void HaarCascadesFaceDetect::loadFrontalFaceCascade(string ifaceCascadePathFrontalFace)
{
	faceCascadePathFrontalFace = ifaceCascadePathFrontalFace;
	if (!frontFaceCascade.load(faceCascadePathFrontalFace))
	{
		printf("--(!)Error loading faceCascadeFrontalFace. Please check the path provided.\n");
	}
}

// load the image directed to by the currentPath.
void HaarCascadesFaceDetect::loadImage(Mat& inImage, std::string currentPath)
{
	if (currentPath.substr(currentPath.size()-3,3) == "jpg")
		inImage = imread(currentPath);
	else
	{
		perror("The path directed to by the input path doesn't lead to a valid jpg image. Please check.\n");
	}
}

// Generates a vector of faces detected natively by the HaarCascades algorithm corresponding to the frames
// given as input. noOfBufferFrames should be an odd number, inclusive of the frame you intend to filter.
// The middle element would be the current frame of concern.
// (faceWidth, faceHeight) = Minimum size of the face in the image.
std::vector<vector <Rect> > HaarCascadesFaceDetect::getBufferedFrames(std::vector<string> inputFileNames, int faceWidth, int faceHeight, int noOfBufferFrames)
{
	std::vector<std::vector<Rect> > bufferedFrames;

	if (noOfBufferFrames != inputFileNames.size())
	{
		perror("The 'noOfBufferFrames' parameter is inconsistent with the number of Filenames given to be considered in the buffer. The empty buffer is returned.\n");
		return bufferedFrames;
	}
	else if (noOfBufferFrames%2 == 0)
	{
		perror("The 'noOfBufferFrames' parameter should be odd (inclusive of the frame under consideration). The empty buffer is returned.\n");
		return bufferedFrames;
	}
	for (int i=0;i < noOfBufferFrames;i++)
	{
	 	cout << inputFileNames[i] << endl;
		Mat inImage = imread(inputFileNames[i]);
		vector<Rect> det = detectFrontalFaces(inImage, faceWidth, faceHeight);
		if (i <= ((noOfBufferFrames-1)/2))
		{
			saveImage(inImage, det, inputFileNames[i]);
		}
		bufferedFrames.push_back(det);
	}
	return bufferedFrames;
}

// pixelSeparation: How far are two multiple consequtive faces in a frame can be (minimum).
// thresholdCount: By what factor should a face be found in the bufferFrames. (Higher number - should be found in more frames to be considered.)
// thresholdCount = 1; would imply that the face should be found in atleast half of the frames. A higher value would imply that 
// more false detections will be considered as valid faces and preserved. A lower value is stringent with its selection.
// Returns the temporally filtered detected faces Rect vector for the central element of the bufferedFaces.
std::vector <Rect> HaarCascadesFaceDetect::temporalFiltering(vector<vector<Rect> > bufferedFrames, int pixelSeparation, int thresholdCount)
{
	int noOfBufferedFrames = (bufferedFrames.size()-1)/2;

	cv::vector <Rect> curDetections;

	for (int f=0;f<bufferedFrames[noOfBufferedFrames].size();f++)
	{
		Rect temp;
		temp.x=bufferedFrames[noOfBufferedFrames][f].x;
		temp.y=bufferedFrames[noOfBufferedFrames][f].y;
		temp.width=bufferedFrames[noOfBufferedFrames][f].width;
		temp.height=bufferedFrames[noOfBufferedFrames][f].height;
		curDetections.push_back(temp);
	}

	// cout << inputFileNames[i] << endl;
	int countOfNotFound=0;
	int countOfFound=0;    
			
	for (int k=0;k<bufferedFrames[noOfBufferedFrames].size();k++) // iterate through the current frame's faces
	{
		countOfNotFound=0;
		countOfFound=0;    
		for (int j = 0; j< 2*noOfBufferedFrames + 1; j++) // iterate through all the buffered frames
		{
			if (j != noOfBufferedFrames) // skip the current frame
			{  
				for (int l=0;l<bufferedFrames[j].size();l++) // Compare the current frame faces with all the buffered ones.
				{
					float pixSep = sqrt(pow(((bufferedFrames[noOfBufferedFrames])[k].x - (bufferedFrames[j])[l].x),2) + 
																						pow(((bufferedFrames[noOfBufferedFrames])[k].y - (bufferedFrames[j])[l].y),2));
					if (pixSep > pixelSeparation ) // Parameter 2 : Ignore the far away detections (as there may be multiple faces)
					{
						countOfNotFound++;
					} 
					else
					{
						countOfFound++;
					}
				}
			}
		}
		if ((float)countOfNotFound > (float)(countOfFound*thresholdCount))  // Parameter 3. (Lonliness of the face detected.)
		{
			curDetections.erase(curDetections.begin()+k);      
		}
	}
	return curDetections;
}

// Removes the first frame data in the buffer and appends the next vector of detected faces to the buffer.
void HaarCascadesFaceDetect::maintainBuffer(vector<vector<Rect> >& bufferedFrames, vector<Rect> det)
{
	if (bufferedFrames.size()%2!=0)
	{
		bufferedFrames.erase(bufferedFrames.begin());
		bufferedFrames.push_back(det);
	}
	else
		perror("bufferedFrames should be odd in size.\n");
}

// Intended to combine the frontal and profile faces for a frame.
vector<Rect> HaarCascadesFaceDetect::combineDetectedFaces(vector<Rect> face1, vector<Rect> face2)
{
	for(int i=0;i<face2.size();i++)
	{
		face1.push_back(face2[i]);
	}
	return face1;
}

// Reads the image directed to by the currentPath and detects faces in them.
// (faceWidth, faceHeight) = Minimum size of the face in the image.
std::vector<Rect> HaarCascadesFaceDetect::detectFrontalFaces(std::string currentPath, int faceWidth, int faceHeight)
{
	std::vector<Rect> frontalFaces;
	if (faceCascadePathFrontalFace.size() > 0)
	{
		if(currentPath.substr(currentPath.size()-3,3)!="jpg")
		{
			perror("The Input Path doesn't lead to a jpg image. Please check the path provided.\n");
			return frontalFaces;
		}
		else 
		{
			Mat inImage = imread(currentPath);
			if (inImage.rows != 0)
			{
				Mat imageToDetectOn;
				
				float factor_w =(float)faceWidth / (float)80;
				float factor_h =(float)faceHeight / (float)80;
				float x_factor = min(factor_h,factor_w);
				int dim_cols = ceil((float) inImage.cols/ (float)x_factor);
				int dim_rows = ceil((float) inImage.rows/ (float)x_factor);
				Size size(dim_cols, dim_rows);
				resize(inImage,imageToDetectOn,size);

				Mat frame_gray;
				cvtColor(imageToDetectOn, frame_gray, COLOR_BGR2GRAY);
				equalizeHist(frame_gray, frame_gray);

				
				frontFaceCascade.detectMultiScale(frame_gray, frontalFaces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(80, 80));
				// detectMultiScale(input_Image, outputFacesToStore, ScaleFactor, minNeighbours, flags, MinSize, MaxSize)
				// Scale factor: How much the image is reduced in size at each image scale. It creates a series of downscaled images
				// of the same input image (as our detector is of a particular size - inconvenient to change it) so that even smaller faces
				// are detectable by the algorithm.
				// Minimum neighbours: This reflects the quality of the face detected. It considers a face only if so many faces are 
				// caught by the algorithm  in its vicinity. It also helps remove the redundant faces which are detected.
				// Min size: Minimum size of the face which can be detected.

				size_t ic = 0; 
				for (ic = 0; ic < frontalFaces.size(); ic++) // Iterate through all current elements (detected frontal Faces)
				{
					frontalFaces[ic].x = frontalFaces[ic].x*x_factor;
					frontalFaces[ic].y = frontalFaces[ic].y*x_factor;
					frontalFaces[ic].width = (frontalFaces[ic].width)*x_factor;
					frontalFaces[ic].height = (frontalFaces[ic].height)*x_factor;
				}
				return frontalFaces;
			}
			else 
			{
				perror("Unable to read the Image at provided path. Might be corrupted or invalid.");
				return frontalFaces;
			}
		}
	}
	else 
	{
		perror("Frontal face cascade is not loaded. Please run the loadFrontalFaceCascade()"
			" function supplying the path to the frontalFaceCascade to it before calling this"
			"function.\n");
	}
	return frontalFaces;
}

// Detects profile faces directed to by the currentPath and returns the detected faces.
// (faceWidth, faceHeight) = Minimum size of the face in the image.
std::vector<Rect> HaarCascadesFaceDetect::detectProfileFaces(std::string currentPath, int faceWidth, int faceHeight)
{
	std::vector<Rect> profileFaces;
	if (faceCascadePathProfileFace.size() > 0)
	{
		if(currentPath.substr(currentPath.size()-3,3)!="jpg")
		{
			perror("The Input Path doesn't lead to a jpg image. Please check the path provided.\n");
			return profileFaces;
		}
		else 
		{
			Mat inImage = imread(currentPath);
			if (inImage.rows != 0)
			{
				Mat imageToDetectOn;

				float factor_w =(float)faceWidth / (float)80;
				float factor_h =(float)faceHeight / (float)80;
				float x_factor = min(factor_h,factor_w);
				int dim_cols = ceil((float) inImage.cols/ (float)x_factor);
				int dim_rows = ceil((float) inImage.rows/ (float)x_factor);
				Size size(dim_cols, dim_rows);
				resize(inImage,imageToDetectOn,size);

				Mat frame_gray;
				cvtColor(imageToDetectOn, frame_gray, COLOR_BGR2GRAY);
				equalizeHist(frame_gray, frame_gray);

				profileFaceCascade.detectMultiScale(frame_gray, profileFaces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(80, 80));
				// detectMultiScale(input_Image, outputFacesToStore, ScaleFactor, minNeighbours, flags, MinSize, MaxSize)
				// Scale factor: How much the image is reduced in size at each image scale. It creates a series of downscaled images
				// of the same input image (as our detector is of a particular size - inconvenient to change it) so that even smaller faces
				// are detectable by the algorithm.
				// Minimum neighbours: This reflects the quality of the face detected. It considers a face only if so many faces are 
				// caught by the algorithm  in its vicinity. It also helps remove the redundant faces which are detected.
				// Min size: Minimum size of the face which can be detected.

				size_t ic = 0; 
				for (ic = 0; ic < profileFaces.size(); ic++) // Iterate through all current elements (detected frontal Faces)
				{
					profileFaces[ic].x = profileFaces[ic].x*x_factor;
					profileFaces[ic].y = profileFaces[ic].y*x_factor;
					profileFaces[ic].width = (profileFaces[ic].width)*x_factor;
					profileFaces[ic].height = (profileFaces[ic].height)*x_factor;
				}
				return profileFaces;
			}
			else 
			{
				perror("Unable to read the Image at provided path. Might be corrupted or invalid.");
				return profileFaces;
			}
		}
	}
	else
	{
		perror("Profile face cascade is not loaded. Please run the loadProfileFaceCascade()"
			" function supplying the path to the profileFaceCascade to it before calling this"
			"function.\n");
	}
	return profileFaces;
}

// Detects the frontal faces in the inImage.
// (faceWidth, faceHeight) = Minimum size of the face in the image.
std::vector<Rect> HaarCascadesFaceDetect::detectFrontalFaces(Mat inImage, int faceWidth, int faceHeight)
{
	std::vector<Rect> frontalFaces;
	if (faceCascadePathFrontalFace.size() > 0)
	{
		if (inImage.rows != 0)
		{
			Mat imageToDetectOn;

			float factor_w =(float)faceWidth / (float)80;
			float factor_h =(float)faceHeight / (float)80;
			// cout << factor_w << endl << factor_h << endl;
			float x_factor = min(factor_h,factor_w);
			int dim_cols = ceil((float) inImage.cols/ (float)x_factor);
			int dim_rows = ceil((float) inImage.rows/ (float)x_factor);
			// imageToDetectOn.set_size(dim_rows,dim_cols);
			Size size(dim_cols, dim_rows);
			// cout << "dim_rows: " << dim_rows << "dim_cols" << dim_cols <<endl;
			resize(inImage,imageToDetectOn,size);

			Mat frame_gray;
			cvtColor(imageToDetectOn, frame_gray, COLOR_BGR2GRAY);
			equalizeHist(frame_gray, frame_gray);

			
			frontFaceCascade.detectMultiScale(frame_gray, frontalFaces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(80, 80));
			// detectMultiScale(input_Image, outputFacesToStore, ScaleFactor, minNeighbours, flags, MinSize, MaxSize)
			// Scale factor: How much the image is reduced in size at each image scale. It creates a series of downscaled images
			// of the same input image (as our detector is of a particular size - inconvenient to change it) so that even smaller faces
			// are detectable by the algorithm.
			// Minimum neighbours: This reflects the quality of the face detected. It considers a face only if so many faces are 
			// caught by the algorithm  in its vicinity. It also helps remove the redundant faces which are detected.
			// Min size: Minimum size of the face which can be detected.

			size_t ic = 0; 
			for (ic = 0; ic < frontalFaces.size(); ic++) // Iterate through all current elements (detected frontal Faces)
			{
				frontalFaces[ic].x = frontalFaces[ic].x*x_factor;
				frontalFaces[ic].y = frontalFaces[ic].y*x_factor;
				frontalFaces[ic].width = (frontalFaces[ic].width)*x_factor;
				frontalFaces[ic].height = (frontalFaces[ic].height)*x_factor;
			}
			return frontalFaces;
		}
		else
		{
			perror("The Mat Image image entered is empty. Please check the input.\n");
			return frontalFaces;
		}
	}
	else 
	{
		perror("Frontal face cascade is not loaded. Please run the loadFrontalFaceCascade()"
			" function supplying the path to the frontalFaceCascade to it before calling this"
			"function.\n");
	}
	return frontalFaces;
}

// Detects profile faces in the inImage.
// (faceWidth, faceHeight) = Minimum size of the face in the image.
std::vector<Rect> HaarCascadesFaceDetect::detectProfileFaces(Mat inImage, int faceWidth, int faceHeight)
{
	std::vector<Rect> profileFaces;
	if (faceCascadePathProfileFace.size() > 0)
	{
		if (inImage.rows != 0)
		{
			Mat imageToDetectOn;
			
			float factor_w =(float)faceWidth / (float)80;
			float factor_h =(float)faceHeight / (float)80;
			float x_factor = min(factor_h,factor_w);
			int dim_cols = ceil((float) inImage.cols/ (float)x_factor);
			int dim_rows = ceil((float) inImage.rows/ (float)x_factor);
			Size size(dim_cols, dim_rows);
			resize(inImage,imageToDetectOn,size);

			Mat frame_gray;
			cvtColor(imageToDetectOn, frame_gray, COLOR_BGR2GRAY);
			equalizeHist(frame_gray, frame_gray);

			profileFaceCascade.detectMultiScale(frame_gray, profileFaces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(80, 80));
			// detectMultiScale(input_Image, outputFacesToStore, ScaleFactor, minNeighbours, flags, MinSize, MaxSize)
			// Scale factor: How much the image is reduced in size at each image scale. It creates a series of downscaled images
			// of the same input image (as our detector is of a particular size - inconvenient to change it) so that even smaller faces
			// are detectable by the algorithm.
			// Minimum neighbours: This reflects the quality of the face detected. It considers a face only if so many faces are 
			// caught by the algorithm  in its vicinity. It also helps remove the redundant faces which are detected.
			// Min size: Minimum size of the face which can be detected.

			size_t ic = 0; 
			for (ic = 0; ic < profileFaces.size(); ic++) // Iterate through all current elements (detected frontal Faces)
			{
				profileFaces[ic].x = profileFaces[ic].x*x_factor;
				profileFaces[ic].y = profileFaces[ic].y*x_factor;
				profileFaces[ic].width = (profileFaces[ic].width)*x_factor;
				profileFaces[ic].height = (profileFaces[ic].height)*x_factor;
			}
			return profileFaces;
		}
			
	}
	else
	{
		perror("Profile face cascade is not loaded. Please run the loadProfileFaceCascade()"
			" function supplying the path to the profileFaceCascade to it before calling this"
			"function.\n");
	}
	return profileFaces;
}

// Saves the inImage with the faces described by det in the location described by inPath.
void HaarCascadesFaceDetect::saveImage(Mat inImage, std::vector<cv::Rect> det, std::string inPath="")
{
	if (inImage.rows > 0)
	{
		Mat srcImage=inImage.clone();
		if (det.size() > 0)
		{
			for (int j=0;j<det.size();j++)
			{
				Point p1((int)(det[j].x), (int)det[j].y);
				Point p2((int)(det[j].x + det[j].height), (int)(det[j].y + det[j].width));
				
				// Draw the rectangle corresponding to a face on the image.
				cv::rectangle(srcImage, p1, p2, Scalar(0, 255, 0), 2, 8, 0);
			}

			// Save the image marked with faces to the disk.
			if (inPath.size()>4)
			{
				string filename = "";
				stringstream ssfn;
				ssfn << inPath.substr(0,inPath.size()-4) << "_detect.jpg";
				filename = ssfn.str();
				imwrite(filename, srcImage);
			}
			else 
			{
				string filename = "";
				stringstream ssfn;
				ssfn << imageCount << "_detect.jpg";
				filename = ssfn.str();
				imwrite(filename, srcImage);
				imageCount++;
			}
		}
		// else
		// {
		// 	if (inPath.size() > 4)
		// 	{	
		// 		string filename = "";
		// 		stringstream ssfn;
		// 		ssfn << inPath.substr(0,inPath.size()-4)  << "_NothingDetected.jpg";
		// 		filename = ssfn.str();
		// 		imwrite(filename, srcImage);
		// 	}
		// 	else
		// 	{
		// 		string filename = "";
		// 		stringstream ssfn;
		// 		ssfn << imageCount << "_detect.jpg";
		// 		filename = ssfn.str();
		// 		imwrite(filename, srcImage);
		// 		imageCount++;
		// 	}
		// 	cout << "No faces detected. The input rectangle object contains no data. Nothing to save but the image itself (done)." << endl;
		// }
	}
	else
	{
		perror("The input Image is empty. Please check.\n");
	}
}

// Shows the inImage with faces overlayed as described by det.
void HaarCascadesFaceDetect::showImage(Mat inImage, std::vector<cv::Rect> det)
{
	if (inImage.rows <= 0)
	{
		cout << "The Input Image is empty. Please check.\n";
	}
	else
	{
		Mat srcImage = inImage.clone();
		if (det.size() > 0)
		{
			for (int j=0;j<det.size();j++)
			{
				Point p1((int)(det[j].x), (int)det[j].y);
				Point p2((int)(det[j].x + det[j].height), (int)(det[j].y + det[j].width));

				// Draw the rectangle corresponding to a face on the image.
				cv::rectangle(srcImage, p1, p2, Scalar(0, 255, 0), 2, 8, 0);
			}
			imshow("Output Window", srcImage);
			waitKey(0);
		}
		else 
		{
			cout << "The input Rect object is empty. Please check. The original Image is displayed anyways.\n";
			imshow("Output Window", srcImage);
			waitKey(0);
		}
	}
}