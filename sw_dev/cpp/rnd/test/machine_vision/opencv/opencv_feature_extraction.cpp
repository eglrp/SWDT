//#include "stdafx.h"
//#define CV_NO_BACKWARD_COMPATIBILITY
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/legacy/compat.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
#include <cassert>
#include <cstring>


namespace {
namespace local {

void harris_corner(IplImage *&srcImage, IplImage *grayImage)
{
	const int blockSize = 3;
	const int apertureSize = 7;
	const double k = 0.1;

	IplImage *harrisResponseImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_32F, 1);
	harrisResponseImage->origin = srcImage ->origin;

	cvCornerHarris(grayImage, harrisResponseImage, blockSize, apertureSize, k);

	//
	//cvConvertScale(harrisResponseImage, tmp, 255.0, 0.0);  // 32F -> 8U

	cvReleaseImage(&srcImage);
	srcImage = harrisResponseImage;
}

void strong_corner(IplImage *srcImage, IplImage *grayImage)
{
	const double quality = 0.01;
	const double minDistance = 10;
	const int winSize = 10;  // odd number ???

	const int MAX_FEATURE_COUNT = 10000;

	CvPoint2D32f *features = (CvPoint2D32f *)cvAlloc(MAX_FEATURE_COUNT * sizeof(CvPoint2D32f));

	IplImage *eig = cvCreateImage(cvGetSize(grayImage), grayImage->depth, 1);
	IplImage *temp = cvCreateImage(cvGetSize(grayImage), grayImage->depth, 1);

	int featureCount = MAX_FEATURE_COUNT;
	cvGoodFeaturesToTrack(
		grayImage, eig, temp, features, &featureCount,
		quality, minDistance, NULL, 3, 0, 0.04
	);
	cvFindCornerSubPix(
		grayImage, features, featureCount,
		cvSize(winSize, winSize), cvSize(-1, -1),
		cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03)
	);

	cvReleaseImage(&eig);
	cvReleaseImage(&temp);

	for (int i = 0; i < featureCount; ++i)
	{
		const int r = 3;
		cvCircle(srcImage, cvPoint(cvRound(features[i].x), cvRound(features[i].y)), r, CV_RGB(255,0,0), CV_FILLED, CV_AA, 0);
		cvLine(srcImage, cvPoint(cvRound(features[i].x - r), cvRound(features[i].y)), cvPoint(cvRound(features[i].x + r), cvRound(features[i].y)), CV_RGB(0,255,0), 1, CV_AA, 0);
		cvLine(srcImage, cvPoint(cvRound(features[i].x), cvRound(features[i].y - r)), cvPoint(cvRound(features[i].x), cvRound(features[i].y + r)), CV_RGB(0,255,0), 1, CV_AA, 0);
	}

	cvFree(&features);
}

void surf(IplImage *srcImage, IplImage *grayImage)
{
	const double hessianThreshold = 600;
	const int useExtendedDescriptor = 0;  // 0 means basic descriptors (64 elements each), 1 means extended descriptors (128 elements each)

	CvSeq *keypoints = NULL;
	CvSeq *descriptors = NULL;
	CvMemStorage *storage = cvCreateMemStorage(0);

	cvExtractSURF(grayImage, NULL, &keypoints, &descriptors, storage, cvSURFParams(hessianThreshold, useExtendedDescriptor));

	const int keypointCount = keypoints->total;
	assert(keypointCount == descriptors->total);
	for (int i = 0; i < keypointCount; ++i)
	{
		// SURF point
		const CvSURFPoint *keypoint = (CvSURFPoint *)cvGetSeqElem(keypoints, i);

		// SURF descriptor
		const float *descriptor = (float *)cvGetSeqElem(descriptors, i);

		//cvCircle(srcImage, cvPoint(cvRound(keypoint->pt.x), cvRound(keypoint->pt.y)), 1, CV_RGB(255,0,0), CV_FILLED, CV_AA, 0);
		//cvCircle(srcImage, cvPoint(cvRound(keypoint->pt.x), cvRound(keypoint->pt.y)), 2, CV_RGB(0,0,255), 1, CV_AA, 0);
		const int r = keypoint->size / 10;
		cvCircle(srcImage, cvPoint(cvRound(keypoint->pt.x), cvRound(keypoint->pt.y)), r, CV_RGB(255,0,0), 1, CV_AA, 0);
		cvLine(srcImage, cvPoint(cvRound(keypoint->pt.x), cvRound(keypoint->pt.y)), cvPoint(cvRound(keypoint->pt.x + r * std::cos(keypoint->dir * CV_PI / 180.0)), cvRound(keypoint->pt.y + r * std::sin(keypoint->dir * CV_PI / 180.0))), CV_RGB(0,255,0), 1, CV_AA, 0);
	}

	cvClearMemStorage(storage);
}

void star_keypoint(IplImage *srcImage, IplImage *grayImage)
{
	CvMemStorage *storage = cvCreateMemStorage(0);

	CvSeq *keypoints = cvGetStarKeypoints(grayImage, storage, cvStarDetectorParams(45));

	const int keypointCount = keypoints ? keypoints->total : 0;
	for (int i = 0; i < keypointCount; ++i)
	{
		const CvStarKeypoint *keypoint = (CvStarKeypoint *)cvGetSeqElem(keypoints, i);

		const int r = keypoint->size / 2;
		cvCircle(srcImage, keypoint->pt, r, CV_RGB(255,0,0), 1, CV_AA, 0);
		cvLine(srcImage, cvPoint(keypoint->pt.x + r, keypoint->pt.y + r), cvPoint(keypoint->pt.x - r, keypoint->pt.y - r), CV_RGB(0,255,0), 1, CV_AA, 0);
		cvLine(srcImage, cvPoint(keypoint->pt.x - r, keypoint->pt.y + r), cvPoint(keypoint->pt.x + r, keypoint->pt.y - r), CV_RGB(0,255,0), 1, CV_AA, 0);
	}

	cvClearMemStorage(storage);
}

#if 0
void mser(IplImage *srcImage, IplImage *grayImage)
{
	const CvScalar colors[] =
	{
		{{0,0,255}},
		{{0,128,255}},
		{{0,255,255}},
		{{0,255,0}},
		{{255,128,0}},
		{{255,255,0}},
		{{255,0,0}},
		{{255,0,255}},
		{{255,255,255}},
		{{196,255,255}},
		{{255,255,196}}
	};

	const unsigned char bcolors[][3] =
	{
		{0,0,255},
		{0,128,255},
		{0,255,255},
		{0,255,0},
		{255,128,0},
		{255,255,0},
		{255,0,0},
		{255,0,255},
		{255,255,255}
	};

	IplImage *hsv = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_8U, 3);
	cvCvtColor(srcImage, hsv, CV_BGR2YCrCb);

	CvMSERParams params = cvMSERParams(); //cvMSERParams(5, 60, cvRound(0.2 * grayImage->width * grayImage->height), 0.25, 0.2);
	CvMemStorage *storage= cvCreateMemStorage();
	CvSeq *contours = NULL;
	double t = (double)cvGetTickCount();
	cvExtractMSER(hsv, NULL, &contours, storage, params);
	t = cvGetTickCount() - t;

	cvReleaseImage(&hsv);

	std::cout << "MSER extracted " << contours->total << " contours in " << (t / ((double)cvGetTickFrequency() * 1000.0)) << " ms" << std::endl;

	// draw MSER with different color
	//unsigned char *imgptr = (unsigned char *)srcImage->imageData;
	//for (int i = contours->total - 1; i >= 0; --i)
	//{
	//	CvSeq *seq = *(CvSeq **)cvGetSeqElem(contours, i);
	//	for (int j = 0; j < seq->total; ++j)
	//	{
	//		CvPoint *pt = CV_GET_SEQ_ELEM(CvPoint, seq, j);
	//		imgptr[pt->x*3+pt->y*srcImage->widthStep] = bcolors[i%9][2];
	//		imgptr[pt->x*3+1+pt->y*srcImage->widthStep] = bcolors[i%9][1];
	//		imgptr[pt->x*3+2+pt->y*srcImage->widthStep] = bcolors[i%9][0];
	//	}
	//}

	// find ellipse ( it seems cvFitEllipse2 have error or sth? )
	// FIXME [check] >> there are some errors. have to compare original source (mser_sample.cpp)
	for (int i = 0; i < contours->total; ++i)
	{
		const CvContour *contour = *(CvContour **)cvGetSeqElem(contours, i);
		const CvBox2D box = cvFitEllipse2(contour);
		//box.angle = (float)CV_PI / 2.0f - box.angle;

		if (contour->color > 0)
			cvEllipseBox(srcImage, box, colors[9], 2, 8, 0);
		else
			cvEllipseBox(srcImage, box, colors[2], 2, 8, 0);
	}

	cvClearMemStorage(storage);
}
#else
// [ref] ${OPENCV_ROOT}/sample/c/mser_sample.cpp
void mser(cv::Mat &srcImage, const cv::Mat &grayImage)
{
	const cv::Scalar colors[] =
	{
		cv::Scalar(0, 0, 255),
		cv::Scalar(0, 128, 255),
		cv::Scalar(0, 255, 255),
		cv::Scalar(0, 255, 0),
		cv::Scalar(255, 128, 0),
		cv::Scalar(255, 255, 0),
		cv::Scalar(255, 0, 0),
		cv::Scalar(255, 0, 255),
		cv::Scalar(255, 255, 255),
		cv::Scalar(196, 255, 255),
		cv::Scalar(255, 255, 196)
	};

	const cv::Vec3b bcolors[] =
	{
		cv::Vec3b(0, 0, 255),
		cv::Vec3b(0, 128, 255),
		cv::Vec3b(0, 255, 255),
		cv::Vec3b(0, 255, 0),
		cv::Vec3b(255, 128, 0),
		cv::Vec3b(255, 255, 0),
		cv::Vec3b(255, 0, 0),
		cv::Vec3b(255, 0, 255),
		cv::Vec3b(255, 255, 255)
	};

	cv::Mat yuv(srcImage.size(), CV_8UC3);
	cv::cvtColor(srcImage, yuv, CV_BGR2YCrCb);

	const int delta = 5;
	const int min_area = 60;
	const int max_area = 14400;
	const float max_variation = 0.25f;
	const float min_diversity = 0.2f;
	const int max_evolution = 200;
	const double area_threshold = 1.01;
	const double min_margin = 0.003;
	const int edge_blur_size = 5;
	cv::MSER mser;

	double t = (double)cv::getTickCount();
	std::vector<std::vector<cv::Point> > contours;
	cv::MSER()(yuv, contours);
	t = cv::getTickCount() - t;

	std::cout << "MSER extracted " << contours.size() << " contours in " << (t / ((double)cv::getTickFrequency() * 1000.0)) << " ms" << std::endl;

	// find ellipse ( it seems cvFitEllipse2 have error or sth? )
	// FIXME [check] >> there are some errors. have to compare original source (mser_sample.cpp)
    for (int i = (int)contours.size() - 1; i >= 0; --i)
	{
        const std::vector<cv::Point> &r = contours[i];
        for (int j = 0; j < (int)r.size(); ++j)
        {
            const cv::Point &pt = r[j];
            srcImage.at<cv::Vec3b>(pt) = bcolors[i % 9];
        }

        // find ellipse (it seems cvfitellipse2 have error or sth?)
        cv::RotatedRect box = cv::fitEllipse(r);

        box.angle = (float)CV_PI / 2 - box.angle;
        cv::ellipse(srcImage, box, colors[10], 2);
	}
}
#endif

}  // namespace local
}  // unnamed namespace

namespace my_opencv {

void feature_extraction()
{
	std::list<std::string> filenames;

#if 0
	filenames.push_back("machine_vision_data\\opencv\\osp_robot_1.jpg");
	filenames.push_back("machine_vision_data\\opencv\\osp_robot_2.jpg");
	filenames.push_back("machine_vision_data\\opencv\\osp_robot_3.jpg");
	filenames.push_back("machine_vision_data\\opencv\\osp_robot_4.jpg");
	filenames.push_back("machine_vision_data\\opencv\\osp_rc_car_1.jpg");
	filenames.push_back("machine_vision_data\\opencv\\osp_rc_car_2.jpg");
	filenames.push_back("machine_vision_data\\opencv\\osp_rc_car_3.jpg");
#elif 1
	filenames.push_back("machine_vision_data\\opencv\\beaver_target.png");
	filenames.push_back("machine_vision_data\\opencv\\melon_target.png");
	filenames.push_back("machine_vision_data\\opencv\\puzzle.png");
	filenames.push_back("machine_vision_data\\opencv\\lena_rgb.bmp");
#elif 0
	filenames.push_back("machine_vision_data\\opencv\\hand_01.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_02.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_03.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_04.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_05.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_06.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_07.jpg");  // error occurred !!!
	filenames.push_back("machine_vision_data\\opencv\\hand_08.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_09.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_10.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_11.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_12.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_13.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_14.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_15.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_16.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_17.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_18.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_19.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_20.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_21.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_22.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_23.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_24.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_25.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_26.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_27.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_28.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_29.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_30.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_31.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_32.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_33.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_34.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_35.jpg");
	filenames.push_back("machine_vision_data\\opencv\\hand_36.jpg");
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

	const char *windowName = "feature extraction";
	cvNamedWindow(windowName, CV_WINDOW_AUTOSIZE);

	//
	for (std::list<std::string>::iterator it = filenames.begin(); it != filenames.end(); ++it)
    {
		IplImage *srcImage = cvLoadImage(it->c_str());
		if (NULL == srcImage)
		{
			std::cout << "fail to load image file" << std::endl;
			return;
		}

		IplImage *grayImage = NULL;
		if (1 == srcImage->nChannels)
			cvCopy(srcImage, grayImage, NULL);
		else
		{
			grayImage = cvCreateImage(cvGetSize(srcImage), srcImage->depth, 1);
#if defined(__GNUC__)
			if (strcasecmp(srcImage->channelSeq, "RGB") == 0)
#elif defined(_MSC_VER)
			if (_stricmp(srcImage->channelSeq, "RGB") == 0)
#endif
				cvCvtColor(srcImage, grayImage, CV_RGB2GRAY);
#if defined(__GNUC__)
			else if (strcasecmp(srcImage->channelSeq, "BGR") == 0)
#elif defined(_MSC_VER)
			else if (_stricmp(srcImage->channelSeq, "BGR") == 0)
#endif
				cvCvtColor(srcImage, grayImage, CV_BGR2GRAY);
			else
				assert(false);
			grayImage->origin = srcImage->origin;
		}

		//
		//local::harris_corner(srcImage, grayImage);
		//local::strong_corner(srcImage, grayImage);
		//local::surf(srcImage, grayImage);
		//local::star_keypoint(srcImage, grayImage);
#if 0
		local::mser(srcImage, grayImage);
#else
		local::mser(cv::Mat(srcImage), cv::Mat(grayImage));
#endif

		//
		cvShowImage(windowName, srcImage);
		cvWaitKey();

		//
		cvReleaseImage(&grayImage);
		cvReleaseImage(&srcImage);
	}

	cvDestroyWindow(windowName);
}

}  // namespace my_opencv
