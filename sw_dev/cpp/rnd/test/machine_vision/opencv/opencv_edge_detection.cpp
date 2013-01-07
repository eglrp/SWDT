//#include "stdafx.h"
#define CV_NO_BACKWARD_COMPATIBILITY
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <list>


namespace {
namespace local {


void canny(const cv::Mat &gray, cv::Mat &edge)
{
#if 0
	// down-scale and up-scale the image to filter out the noise
	cv::Mat blurred;
	cv::pyrDown(gray, blurred);
	cv::pyrUp(blurred, edge);
#else
	cv::blur(gray, edge, cv::Size(3, 3));
#endif

	// run the edge detector on grayscale
	const int lowerEdgeThreshold = 30, upperEdgeThreshold = 50;
	const bool useL2 = true;
	cv::Canny(edge, edge, lowerEdgeThreshold, upperEdgeThreshold, 3, useL2);
}

}  // namespace local
}  // unnamed namespace


namespace opencv {

void edge_detection()
{
	std::list<std::string> filenames;
#if 0
	filenames.push_back("machine_vision_data\\opencv\\pic1.png");
	filenames.push_back("machine_vision_data\\opencv\\pic2.png");
	filenames.push_back("machine_vision_data\\opencv\\pic3.png");
	filenames.push_back("machine_vision_data\\opencv\\pic4.png");
	filenames.push_back("machine_vision_data\\opencv\\pic5.png");
	filenames.push_back("machine_vision_data\\opencv\\pic6.png");
	filenames.push_back("machine_vision_data\\opencv\\stuff.jpg");
	filenames.push_back("machine_vision_data\\opencv\\synthetic_face.png");
	filenames.push_back("machine_vision_data\\opencv\\puzzle.png");
	filenames.push_back("machine_vision_data\\opencv\\fruits.jpg");
	filenames.push_back("machine_vision_data\\opencv\\lena_rgb.bmp");
	filenames.push_back("machine_vision_data\\opencv\\hand_01.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_05.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_24.jpg");
#elif 1
	filenames.push_back("machine_vision_data\\opencv\\hand_01.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_02.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_03.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_04.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_05.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_06.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_07.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_08.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_09.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_10.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_11.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_12.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_13.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_14.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_15.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_16.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_17.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_18.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_19.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_20.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_21.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_22.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_23.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_24.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_25.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_26.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_27.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_28.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_29.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_30.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_31.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_32.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_33.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_34.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_35.jpg");
	//filenames.push_back("machine_vision_data\\opencv\\hand_36.jpg");
#elif 0
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_01.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_02.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_03.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_04.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_05.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_06.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_07.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_08.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_09.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_10.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_11.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_12.jpg");
	filenames.push_back("machine_vision_data\\opencv\\simple_hand_13.jpg");
#endif

	const std::string windowName1("edge detection - original");
	const std::string windowName2("edge detection - processed");
	cv::namedWindow(windowName1, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(windowName2, cv::WINDOW_AUTOSIZE);

	//
	for (std::list<std::string>::iterator it = filenames.begin(); it != filenames.end(); ++it)
    {
		const cv::Mat img = cv::imread(*it, CV_LOAD_IMAGE_COLOR);
		if (img.empty())
		{
			std::cout << "fail to load image file: " << *it << std::endl;
			continue;
		}

		cv::Mat gray;
		if (1 == img.channels())
			img.copyTo(gray);
		else
			cv::cvtColor(img, gray, CV_BGR2GRAY);
			//cv::cvtColor(img, gray, CV_RGB2GRAY);

		cv::Mat edge;
		local::canny(gray, edge);

		cv::Mat cedge;
		img.copyTo(cedge, edge);
		//cedge = cedge > 0;

		//
		cv::imshow(windowName1, img);
		cv::imshow(windowName2, cedge);

		const unsigned char key = cv::waitKey(0);
		if (27 == key)
			break;
	}

	cv::destroyWindow(windowName1);
	cv::destroyWindow(windowName2);
}

}  // namespace opencv