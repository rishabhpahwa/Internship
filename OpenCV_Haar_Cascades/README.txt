/*
*****************************************************
*	 * Copyright: Vehant Technologies (P) Ltd		*
*	 *												*
*	 * Readme.txt									*
*	 *												*
*	 * Initial Author: Rishabh Pahwa				*
*****************************************************
*/

The Project enables a user to detect faces in static images.

>	The src folder contains all the source files.
>	the obj folders stores the intermediate object files for ReadDirectory.cpp and HaarCascades.cpp
>	The lib folder contains the library combining both the object files above.
>	The bin folder contains the final executable.

There are two Source files and one main.cpp in src folder:

>	ReadDirectory.cpp : 
		Enables user to enumerate files in either just the directory or the entire subdirectories recursively.

	Functions Provided:
		>	isDir(String path)				: checks wether the path entered is a valid directory or not.
		>	sortFileNames() 				: Sorts the filenames in lexicographic order stored in the vector which is 
								  			  maintained by the object of the class.
		>	listFileNames(string baseDir
						, bool recursive)	: Takes as input a base directory as a reference directory to enumerate the
											  filenames. 'recursive' parameter tells whether to include the subdirectories
											  while enumeration or not (true : include the subdirectories).

>	HaarCascades.cpp :
		Enables a user to detect faces in images, display them on screen, save them with detected faces to disk and additionally apply
		temporal filtering on a consequtive sequence of frames of a video to decrease the false detection rate of the algorithm.

	Functions Provided:
		>	loadProfileFaceCascade(string 
			ifaceCascadePathProfileFace)	: Provide the path to the Profile Face cascade (xml file). It then detectes profile faces.

		>	loadFrontalFaceCascade(string 
			ifaceCascadePathFrontalFace)	: Provide the path to the Frontal Face cascade (xml file). It then detectes frontal faces.

		>	loadImage(Mat& inImage, string 
						currentPath)		: Loads the image directed to by the currentPath into the inImage Mat object.

		>	getBufferedFrames(vector<string>
		 		inputFileNames, int faceWidth, 
		 			int faceHeight, 
		 			int noOfBufferFrames)	: This function is to be used if you wish to apply temporal filtering on the set of Images
		 									  (Should be a continuous frames of a Video, else temporal filtering would fail miserably).
		 									  This function maintains a buffer of frames (an equal number before and after the frame in 
		 									  consideration) which it would check everytime it wishes to temporally filter the current frame.
		 									  Generates a vector of faces detected natively by the HaarCascades algorithm corresponding to the frames
 											  given as input. noOfBufferFrames should be an odd number, inclusive of the frame you intend to filter.
											  The middle element would be the current frame of concern.
 											  (faceWidth, faceHeight) = Minimum size of the face in the image.
 											  A supportive function is maintainBuffer() : Which is necessary to run everytime a frame is processed.

 		>	maintainBuffer(vector<vector<Rect> >& 
 			bufferedFrames, vector<Rect> det): Removes the first frame data in the buffer and appends the next vector of detected faces (of the 
 											   next frame in the sequence - ie. just after the last frame of the buffer) to the buffer. It is 
 											   mandatory to run this function everytime you process a frame temporally.

 		>	combineDetectedFaces(
 				vector<Rect> face1, 
 					vector<Rect> face2)		: Intended to combine the frontal and profile faces for a frame.

 		>	detectFrontalFaces(string 
 				currentPath, int faceWidth, 
 					int faceHeight)			: Reads the image directed to by the currentPath and detects frontal faces in them and returns the same.
											  (faceWidth, faceHeight) = Minimum size of the face to be detected in the image.

		>	detectProfileFaces(string 
				currentPath, int faceWidth, 
					int faceHeight)			: Reads the image directed to by the currentPath and detects profile faces in them and returns the same.
											  (faceWidth, faceHeight) = Minimum size of the face to be detected in the image.

		>	detectFrontalFaces(Mat inImage, 
			int faceWidth, int faceHeight)	: Detects the frontal faces in the inImage.
											  (faceWidth, faceHeight) = Minimum size of the face in the image.

		>	detectProfileFaces(Mat inImage, 
			int faceWidth, int faceHeight)	: Detects the profile faces in the inImage.
											  (faceWidth, faceHeight) = Minimum size of the face in the image.
		
		>	saveImage(Mat inImage, 
				vector<Rect> det, string 
						inPath="")			: Saves the inImage with the faces described by det in the location described by inPath.
											  If inPath is left empty, the frames with the faces described are saved in the current working
											  directory with consequtive numbers from 1 onwards.

		>	showImage(Mat inImage, 
					vector<Rect> det)		: Shows the inImage with faces overlayed as described by det.