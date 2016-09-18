/*
	 * Copyright: Vehant Technologies (P) Ltd
	 */

	/*
	 * DLibFaceDetect.cpp
	 * (Source File)
	 * ------------------
	 * Reads a set of images (generally a frame dump of a webcam) and shows
	 * and saves the frames in which a face is detected. It uses the Dlib 18.18
	 * library and associated functions. The detectAndDisplay() function does the
	 * operation mentioned above.
	 *
	 * Initial Author: Rishabh Pahwa
*/

#include "DlibFaceDetect.h"

// Default constructor. Used when user intends to use dlib's own frontal_face_detector.
DlibFaceDetect::DlibFaceDetect()
{
	m_trainingDataXml="";
	imagePath = "";
	imageCount=0;
}

// Constructor for training your own face detector.
DlibFaceDetect::DlibFaceDetect(string trainingDataXml)
{
	m_trainingDataXml = trainingDataXml;
	imagePath="";
	imageCount=0;
}

// loads the detector into the object depending on whether data for training was supplied to 
// the constructor while creating the object or not. If no data was supplied, it loads dlib's
// default frontal_face_detector. If a valid path to genuine training data was supplied,
// it first trains on the provided data, creates a object detector and loads it.
void DlibFaceDetect::getDetector()
{
	if (m_trainingDataXml.size()==0)
	{
		m_detector = get_frontal_face_detector();
	}
	else if (m_trainingDataXml.size()>0)
	{
		try
		{
			dlib::array<array2d<unsigned char> > images_train;	// , images_test
			std::vector<std::vector<dlib::rectangle> > face_boxes_train; // , face_boxes_test

			load_image_dataset(images_train, face_boxes_train, m_trainingDataXml);
			
			upsample_image_dataset<pyramid_down<2> >(images_train, face_boxes_train);
			
			// Since human faces are generally left-right symmetric we can increase
			// our training dataset by adding mirrored versions of each image back
			// into images_train.  So this next step doubles the size of our
			// training dataset.  Again, this is obviously optional but is useful in
			// many object detection tasks.
			add_image_left_right_flips(images_train, face_boxes_train);
			cout << "num training images: " << images_train.size() << endl;
			
			typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 
			image_scanner_type scanner;
			
			scanner.set_detection_window_size(80, 80); 
			structural_object_detection_trainer<image_scanner_type> trainer(scanner);
			
			// Set this to the number of processing cores on your machine.
			trainer.set_num_threads(4);  
			
			// The trainer is a kind of support vector machine and therefore has the usual SVM
	        // C parameter.  In general, a bigger C encourages it to fit the training data
	        // better but might lead to overfitting.  You must find the best C value
	        // empirically by checking how well the trained detector works on a test set of
	        // images you haven't trained on.  Don't just leave the value set at 3.  Try a few
	        // different C values and see what works best for your data.
			trainer.set_c(4);
			
			// We can tell the trainer to print it's progress to the console if we want.  
			trainer.be_verbose();
			
			// The trainer will run until the "risk gap" is less than 0.01.  Smaller values
	        // make the trainer solve the SVM optimization problem more accurately but will
	        // take longer to train.  For most problems a value in the range of 0.1 to 0.01 is
	        // plenty accurate.  Also, when in verbose mode the risk gap is printed on each
	        // iteration so you can see how close it is to finishing the training.  
			trainer.set_epsilon(0.01);

			m_trainedDetector = trainer.train(images_train, face_boxes_train);

			// Now that we have a face detector we can test it.  The first statement tests it
        	// on the training data.  It will print the precision, recall, and then average precision.
			cout << "training results: " << test_object_detection_function(m_trainedDetector, images_train, face_boxes_train) << endl;

			// If you have read any papers that use HOG you have probably seen the nice looking
			// "sticks" visualization of a learned HOG detector.  This next line creates a
			// window with such a visualization of our detector.  It should look somewhat like
			// a face.
			// image_window hogwin(draw_fhog(detector), "Learned fHOG detector");

			// Like everything in dlib, you can save your detector to disk using the
			// serialize() function.
			// serialize("face_detector.svm") << m_trainedDetector;

			// Then you can recall it using the deserialize() function.
			object_detector<image_scanner_type> detector2;
			deserialize("face_detector.svm") >> detector2;

			
			// You can see how many separable filters are inside your detector like so:
			// cout << "num filters: "<< num_separable_filters(detector) << endl;
			// You can also control how many filters there are by explicitly thresholding the
			// singular values of the filters like this:
			m_trainedDetector = threshold_filter_singular_values(m_trainedDetector,0.1);
			
		}
		catch (exception& e)
		{
			cout << "\nexception thrown!" << endl;
			cout << e.what() << endl;
		}
	}
}

// Loads the image in array2d format of dlib into inImage from the path provided as input.
void DlibFaceDetect::loadImage(dlib::array2d<bgr_pixel>& inImage, std::string curPath)
{
	imagePath = curPath;
	load_image(inImage, curPath);
	// return tempImg;
}

// Generates a vector of faces detected by Dlib's native frontal face detector or the trained detector
// (if training was done proir to detecting faces) corresponding to the inImage directed by the curPath parameter.
// noOfBufferFrames should be an odd number, inclusive of the frame you intend to filter.
// The middle element would be the current frame of concern.
// (faceWidth, faceHeight) = Minimum size of the face in the image.
std::vector<dlib::rectangle> DlibFaceDetect::detectFaces(std::string curPath, int faceWidth, int faceHeight)
{
	if (m_trainingDataXml.size()>0)
	{
		if (curPath.substr(curPath.size()-3,3) != "jpg")
		{
			perror ("The provided path does not lead to a valid jpg Image. Please check!\n");
			return dets;
		}
		
		array2d<bgr_pixel> inImage;
		load_image(inImage, curPath);

		if (inImage.nc() != 0)
		{
			array2d <bgr_pixel> imageToDetectOn;
			
			float factor_w =(float)faceWidth / (float)80;
			float factor_h =(float)faceHeight / (float)80;
			// cout << factor_w << endl << factor_h << endl;
			float x_factor = min(factor_h,factor_w);
			int dim_cols= ceil((float) inImage.nc()/ (float)x_factor);
			int dim_rows = ceil((float) inImage.nr()/ (float)x_factor);
			imageToDetectOn.set_size(dim_rows,dim_cols);
			dlib::resize_image(inImage, imageToDetectOn);

			dets = m_trainedDetector(imageToDetectOn);
			cout << "No. of faces detected: " << dets.size() << endl;

			for (int i=0;i<dets.size();i++)
			{
				dets[i].set_top(dets[i].top()*x_factor);
				dets[i].set_bottom(dets[i].bottom()*x_factor);
				dets[i].set_left(dets[i].left()*x_factor);
				dets[i].set_right(dets[i].right()*x_factor);
			}

			return dets;

		}
		else
		{
			perror("The Image directed to by the path could not be loaded. The image file may be courrupted or unavailable.\n");
			return dets;
		}	
	}
	else
	{
		if (curPath.substr(curPath.size()-3,3) != "jpg")
		{
			perror ("The provided path does not lead to a valid jpg Image. Please check!\n");
			return dets;
		}
		
		array2d<bgr_pixel> inImage;
		load_image(inImage, curPath);

		if (inImage.nc() != 0)
		{
			array2d <bgr_pixel> imageToDetectOn;
			
			float factor_w =(float)faceWidth / (float)80;
			float factor_h =(float)faceHeight / (float)80;
			// cout << factor_w << endl << factor_h << endl;
			float x_factor = min(factor_h,factor_w);
			int dim_cols= ceil((float) inImage.nc()/ (float)x_factor);
			int dim_rows = ceil((float) inImage.nr()/ (float)x_factor);
			imageToDetectOn.set_size(dim_rows,dim_cols);
			dlib::resize_image(inImage, imageToDetectOn);

			dets = m_detector(imageToDetectOn);

			cout << "No. of faces detected: " << dets.size() << endl;

			for (int i=0;i<dets.size();i++)
			{
				dets[i].set_top(dets[i].top()*x_factor);
				dets[i].set_bottom(dets[i].bottom()*x_factor);
				dets[i].set_left(dets[i].left()*x_factor);
				dets[i].set_right(dets[i].right()*x_factor);
			}

			return dets;

		}
		else
		{
			perror("The Image directed to by the path could not be loaded. The image file may be courrupted or unavailable.\n");
			return dets;
		}
	}
}

// Generates a vector of faces detected by Dlib's native frontal face detector or the trained detector
// (if training was done proir to detecting faces) corresponding to the inImage provided as input.
// noOfBufferFrames should be an odd number, inclusive of the frame you intend to filter.
// The middle element would be the current frame of concern.
// (faceWidth, faceHeight) = Minimum size of the face in the image.
std::vector<dlib::rectangle> DlibFaceDetect::detectFaces(dlib::array2d<bgr_pixel>& inImage, int faceWidth, int faceHeight)
{
	if (m_trainingDataXml.size()>0)
	{
		if (inImage.nc() != 0)
		{
			array2d <bgr_pixel> imageToDetectOn;
			
			float factor_w =(float)faceWidth / (float)80;
			float factor_h =(float)faceHeight / (float)80;
			// cout << factor_w << endl << factor_h << endl;
			float x_factor = min(factor_h,factor_w);
			int dim_cols= ceil((float) inImage.nc()/ (float)x_factor);
			int dim_rows = ceil((float) inImage.nr()/ (float)x_factor);
			imageToDetectOn.set_size(dim_rows,dim_cols);
			dlib::resize_image(inImage, imageToDetectOn);

			dets = m_trainedDetector(imageToDetectOn);

			cout << "No. of faces detected: " << dets.size() << endl;
			
			for (int i=0;i<dets.size();i++)
			{
				dets[i].set_top(dets[i].top()*x_factor);
				dets[i].set_bottom(dets[i].bottom()*x_factor);
				dets[i].set_left(dets[i].left()*x_factor);
				dets[i].set_right(dets[i].right()*x_factor);
			}

			return dets;
		}
		else
		{
			perror("The Image could not be loaded or is invalid (might be possibly NULL)\n");
		}
	}
	else
	{
		if (inImage.nc() != 0)
		{
			array2d <bgr_pixel> imageToDetectOn;
			
			float factor_w =(float)faceWidth / (float)80;
			float factor_h =(float)faceHeight / (float)80;
			// cout << factor_w << endl << factor_h << endl;
			float x_factor = min(factor_h,factor_w);
			int dim_cols= ceil((float) inImage.nc()/ (float)x_factor);
			int dim_rows = ceil((float) inImage.nr()/ (float)x_factor);
			imageToDetectOn.set_size(dim_rows,dim_cols);
			dlib::resize_image(inImage, imageToDetectOn);

			dets = m_detector(imageToDetectOn);

			cout << "No. of faces detected: " << dets.size() << endl;
			
			for (int i=0;i<dets.size();i++)
			{
				dets[i].set_top(dets[i].top()*x_factor);
				dets[i].set_bottom(dets[i].bottom()*x_factor);
				dets[i].set_left(dets[i].left()*x_factor);
				dets[i].set_right(dets[i].right()*x_factor);
			}

			return dets;
		}
		else
		{
			perror("The Image could not be loaded or is invalid (might be possibly NULL)\n");
		}
	}	
	return dets;
}

// Convert the dlib Rectangle object to opencv Rect object and return the same.
cv::Rect DlibFaceDetect::convertRectangleToRect (dlib::rectangle inRectangle)
{
	Rect temp=cv::Rect(cv::Point2i(inRectangle.left(), inRectangle.top()), cv::Point2i(inRectangle.right() + 1, inRectangle.bottom() + 1));
	return temp;
}

// convert the input array2d object to Mat.
cv::Mat DlibFaceDetect::convertToMat(dlib::array2d<bgr_pixel>& inImage)
{
	return toMat(inImage);
}

// Save the image overlayed by the rectangles (dets) to disk at the path provided. If inPath is left blank, image is 
// saved to the current directory named as successive numbers starting with 0.
void DlibFaceDetect::saveImage(array2d<bgr_pixel>& inImage, std::vector<dlib::rectangle> det, std::string inPath="")
{
	Mat srcImage = toMat(inImage);
	if (det.size() > 0)
	{
		for (int j=0;j<det.size();j++)
		{
			// Convert the dlib's Rectangle object to OpenCV's Rect object to make it possible to draw it on the image.
			Rect temp = convertRectangleToRect(det[j]);
			Point p1((int)(temp.x), (int)temp.y);
			Point p2((int)(temp.x + temp.height), (int)(temp.y + temp.width));


			// cout << temp.x << endl << temp.y << endl << (temp.x + temp.height) << endl << (temp.y + temp.width) << endl; 
			
			// Draw the rectangle corresponding to a face on the image.
			cv::rectangle(srcImage, p1, p2, Scalar(0, 255, 0), 2, 8, 0);

			// imshow("output", dstImage);
			// waitKey(0);
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
}


// Show the inImage to the window with overlayed rectangles (faces).
void DlibFaceDetect::showImage(dlib::array2d<bgr_pixel>& inImage, std::vector<dlib::rectangle> dets)
{
	if (inImage.nc() != 0)
	{
		image_window win(inImage);
		win.add_overlay(dets, bgr_pixel(0,0,255));
		cout << "Hit enter to continue...";
		cin.get();
	}
	else
	{
		perror("The image is invalid.\n");
	}
}