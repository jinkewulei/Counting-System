#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <opencv2\opencv.hpp> 
#include <opencv2/nonfree/features2d.hpp>
#include "structure.h"

using namespace cv;  
using namespace std;

int surf(const char * img1,const char * img2,vector<FeaturePoints> * ADDR_FeaturePtsArray)
{
	double begin,finish;
	begin = clock();

	//load images  
    Mat img_object = imread(img1, CV_LOAD_IMAGE_GRAYSCALE );  
//    Mat img_scene = imread(img2, CV_LOAD_IMAGE_GRAYSCALE );  
    if( !img_object.data )//|| !img_scene.data )  
    {  
        std::cout<< " --(!) Error reading images " << std::endl;  
        return -1;  
    }  
  
    //-- Step 1: Detect the keypoints using SURF Detector  
    int minHessian = 20;    //hessian matrix  
  
    SurfFeatureDetector detector( minHessian );  
    std::vector<KeyPoint> keypoints_object, keypoints_scene;  
    detector.detect( img_object, keypoints_object );  
//  detector.detect( img_scene, keypoints_scene );  
  
	finish = clock();
	cout<<"Time of surf:"<<'\t'<<finish-begin<<endl;

	//catch out the featurepoints 
	FeaturePoints tempPoint;
	int num = keypoints_object.size();
	for(int i=0;i<num;i++)
	{
	//	cout<<keypoints_object[i].pt.x<<'\t'<<'\t'<<keypoints_object[i].pt.y<<endl;
		tempPoint.x = (int)(keypoints_object[i].pt.x+0.5);
		tempPoint.y = (int)(keypoints_object[i].pt.y+0.5);
		(*ADDR_FeaturePtsArray).push_back(tempPoint);
		}
	cout<<"Num of feature points:"<<'\t'<<keypoints_object.size()<<endl;

	/*
	//==================draw points on the picture==================//
	IplImage * image = cvLoadImage(img1);
	CvPoint centerpoint;
	int R,G,B;
	R = 255;
	G = 255;
	B = 255;
	for(int liz=0;liz<num;liz++)
	{
		centerpoint.x = (*ADDR_FeaturePtsArray)[liz].x;
		centerpoint.y = (*ADDR_FeaturePtsArray)[liz].y;
		cvCircle( image, centerpoint ,3 , CV_RGB(R,G,B),1,  8, 3 );
		cvSaveImage("circle.bmp",image);
	}
	//==================draw points on the picture==================//
	*/

	/*
    //draw keypoints  
    Mat img_keypoints_object;//,img_keypoints_scene;  
    //memory first  
    drawKeypoints(img_object,keypoints_object,img_keypoints_object,Scalar::all(-1),DrawMatchesFlags::DEFAULT);  
    //drawKeypoints(img_scene,keypoints_scene,img_keypoints_scene,Scalar::all(-1),DrawMatchesFlags::DEFAULT);  
    //then show  
	imshow("surf_keypoints_object",img_keypoints_object);  
	waitKey(0);  
	*/

	return 0;
}
