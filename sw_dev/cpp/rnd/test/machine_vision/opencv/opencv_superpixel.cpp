//#include "stdafx.h"
#define CV_NO_BACKWARD_COMPATIBILITY
#include <opencv2/ximgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>


namespace {
namespace local {

void lsc(const std::string& img_filename)
{
	const std::string windowName("superpixel - SLIC");

	cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

	const cv::Mat &img = cv::imread(img_filename, cv::IMREAD_COLOR);
	//const cv::Mat &img = cv::imread(img_filename, cv::IMREAD_GRAYSCALE);
	if (img.empty())
	{
		std::cout << "fail to load image file: " << img_filename << std::endl;
		return;
	}

	// Linear Spectral Clustering (LSC) superpixels algorithm.
	const int region_size = 10;
	const float ratio = 0.075f;
	cv::Ptr<cv::ximgproc::SuperpixelLSC> superpixel = cv::ximgproc::createSuperpixelLSC(img, region_size, ratio);

	// Calculate the superpixel segmentation.
	const int num_iterations = 10;
	superpixel->iterate(num_iterations);

	//
	cv::Mat superpixel_label;
	superpixel->getLabels(superpixel_label);

	cv::Mat superpixel_contour_mask;
	const bool thick_line = true;
	superpixel->getLabelContourMask(superpixel_contour_mask, thick_line);

	// Output result.
	cv::imshow(windowName, superpixel_label);

	cv::waitKey(0);

	cv::destroyAllWindows();
}

void seeds(const std::string& img_filename)
{
	const std::string windowName("superpixel - SLIC");

	cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

	const cv::Mat &img = cv::imread(img_filename, cv::IMREAD_COLOR);
	//const cv::Mat &img = cv::imread(img_filename, cv::IMREAD_GRAYSCALE);
	if (img.empty())
	{
		std::cout << "fail to load image file: " << img_filename << std::endl;
		return;
	}

	// Superpixels Extracted via Energy-Driven Sampling (SEEDS) superpixels algorithm.
	const int image_width = img.cols;
	const int image_height = img.rows;
	const int image_channels = img.channels();
	const int num_superpixels = 200;
	const int num_levels = 3;
	const int prior = 2;
	const int histogram_bins = 5;
	const bool double_step = false;
	cv::Ptr<cv::ximgproc::SuperpixelSEEDS> superpixel = cv::ximgproc::createSuperpixelSEEDS(image_width, image_height, image_channels, num_superpixels, num_levels, prior, histogram_bins, double_step);

	// Calculate the superpixel segmentation.
	const int num_iterations = 10;
	superpixel->iterate(img, num_iterations);

	//
	cv::Mat superpixel_label;
	superpixel->getLabels(superpixel_label);

	cv::Mat superpixel_contour_mask;
	const bool thick_line = true;
	superpixel->getLabelContourMask(superpixel_contour_mask, thick_line);

	// Output result.
	cv::imshow(windowName, superpixel_label);

	cv::waitKey(0);

	cv::destroyAllWindows();
}

void slic(const std::string& img_filename)
{
	const std::string windowName("superpixel - SLIC");

	cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

	const cv::Mat &img = cv::imread(img_filename, cv::IMREAD_COLOR);
	//const cv::Mat &img = cv::imread(img_filename, cv::IMREAD_GRAYSCALE);
	if (img.empty())
	{
		std::cout << "fail to load image file: " << img_filename << std::endl;
		return;
	}

	// Simple Linear Iterative Clustering (SLIC) superpixels algorithm.
	const int algorithm = cv::ximgproc::SLICO;
	const int region_size = 10;
	const float ruler = 10.0f;
	cv::Ptr<cv::ximgproc::SuperpixelSLIC> superpixel = cv::ximgproc::createSuperpixelSLIC(img, algorithm, region_size, ruler);

	// Calculate the superpixel segmentation.
	const int num_iterations = 10;
	superpixel->iterate(num_iterations);

	//
	cv::Mat superpixel_label;
	superpixel->getLabels(superpixel_label);

	cv::Mat superpixel_contour_mask;
	const bool thick_line = true;
	superpixel->getLabelContourMask(superpixel_contour_mask, thick_line);

	// Output result.
	cv::imshow(windowName, superpixel_label);

	cv::waitKey(0);

	cv::destroyAllWindows();
}

}  // namespace local
}  // unnamed namespace

namespace my_opencv {

void superpixel()
{
	const std::string img_filename("./data/machine_vision/vlfeat");

	local::lsc(img_filename);
	local::seeds(img_filename);
	local::slic(img_filename);
}

}  // namespace my_opencv
