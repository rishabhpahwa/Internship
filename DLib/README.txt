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
>	the obj folders stores the intermediate object files for ReadDirectory.cpp and DlibFaceDetect.cpp
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

>	DlibFaceDetect.cpp :
		Enables a user to detect faces in images, display them on screen, save them with detected faces to disk, train your own
		face detector and use it inturn to detect faces.

	Functions Provided:
		>	constructor << DlibFaceDetect() : The Default constructor which if called ensures that Dlibs default face detector is used.
		>	constructor << DlibFaceDetect(
						   string pathToXml): The parameterized constructor which if called with a valid path to a XML file having 
											  training data trains a HOG detector with the data and hence uses it to detect faces in
											  images while the same object is alive.

		>	getDetector() 					: loads the detector into the object depending on whether data for training was supplied to 
											  the constructor while creating the object or not. If no data was supplied, it loads dlibs
											  default frontal_face_detector. If a valid path to genuine training data was supplied,
											  it first trains on the provided data, creates a object detector and loads it.

		>	loadImage(array2d<bgr_pixel>
					inImagestring curPath)	: Loads the image in array2d format of dlib into inImage from the path provided as input.

		>	*detectFaces(string curPath,
			int faceWidth, int faceHeight)  : Generates a vector of faces detected by Dlibs native frontal face detector or the trained detector
											  (if training was done proir to detecting faces) corresponding to the inImage directed by the curPath
											  parameter. noOfBufferFrames should be an odd number, inclusive of the frame you intend to filter.
											  The middle element would be the current frame of concern.
											  (faceWidth, faceHeight) = Minimum size of the face in the image.
		
		>	*detectFaces(array2d<bgr_pixel>
					inImage, int faceWidth, 
						int faceHeight)		: Generates a vector of faces detected by Dlibs native frontal face detector or the trained detector
											  (if training was done proir to detecting faces) corresponding to the inImage directed by the curPath
											  parameter. noOfBufferFrames should be an odd number, inclusive of the frame you intend to filter.
											  The middle element would be the current frame of concern.
											  (faceWidth, faceHeight) = Minimum size of the face in the image.
		
		>	convertRectangleToRect(
				rectangle inRectangle)		: Convert the dlib Rectangle object to opencv Rect object and return the same.
		
		>	convertToMat(array2d<bgr_pixel>&
						 inImage)			: convert the input array2d object to Mat.
		
		>	saveImage(array2d<bgr_pixel>& 
				inImage, vector<rectangle> 
				det, string inPath="")		: Save the image overlayed by the rectangles (dets) to disk at the path provided. If inPath is
											  left blank, image is saved to the current directory named as successive numbers starting with 0.
		
		>	showImage(array2d<bgr_pixel>& 
			inImage, vector<rectangle> dets): Show the inImage to the window with overlayed rectangles (faces).


>	main.cpp
		Provides a sample test envionment for the project. You may modify the code and play with the functions just to see what it does
		on different inputs.


- Author:
	Rishabh Pahwa.
