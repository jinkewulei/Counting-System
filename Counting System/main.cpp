#include <ctime>  
//#include "opencv2/core/core.hpp"  
//#include "opencv2/features2d/features2d.hpp"  
//#include "opencv2/highgui/highgui.hpp"  
//#include "opencv2/calib3d/calib3d.hpp"  
#include <opencv2/nonfree/features2d.hpp>
//#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2\opencv.hpp> 
#include <opencv2\legacy\legacy.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include "structure.h"
#include "mstree.h"

using namespace cv;  
using namespace std;  

const char * img1 = "D:\\Projects\\Counting System-PC\\Counting System\\workspace\\90.bmp";  
const char * img2 = "D:\\Projects\\Counting System-PC\\Counting System\\workspace\\91.bmp";  
//const char * img3 = "D:\\Projects\\Counting System-PC\\Counting System\\workspace\\3.bmp";  
vector<FeaturePoints> FeaturePtsArray;
vector<FeaturePoints> FinalPoints;
vector<ClusterPts> ClusteredPts;
vector<ClusterInfo> EachClusterInfo;
vector<Movpts> Movpts12;
vector<Movpts> Movpts23;
vector<FeaturePoints> NewPts;

int main()
{
	double begin;
	double finish;
	//============================================PIC1-2=======================================//
//	begin = clock();
	surf(img1,img2,&FeaturePtsArray);
//	finish = clock();
//	cout<<"Time of surf:"<<'\t'<<finish-begin<<endl;

	//unsigned int temp;
	//FeaturePoints point;
	//int count = 0;
	//ifstream infile;
	//infile.open("surf.txt");
	//while(infile.good())
	//{
	//	infile>>temp;
	//	if(temp>0)
	//	{
	//		//cout<<dec<<temp<<'\t';
	//		//cout<<hex<<temp<<endl;
	//		//cout<<(temp>>16)<<'\t';
	//		//cout<<(temp & 0x0000FFFF)<<endl;
	//		point.y = (temp>>16);
	//		point.x = (temp & 0x0000FFFF);
	//		FeaturePtsArray.push_back(point);
	//		count++;
	//	}
	//}
	//cout<<"Number of feature points:"<<FeaturePtsArray.size()<<endl;

	begin = clock();
	bm(img1,img2,&FeaturePtsArray,&FinalPoints,&Movpts12);
	finish = clock();
	cout<<"Time of BM:"<<'\t'<<finish-begin<<endl;

	begin = clock();
	float radius = min_span_tree(FinalPoints,alpha);
	finish = clock();
	cout<<"Time of MST:"<<'\t'<<finish-begin<<endl;

	begin = clock();
	dbscan(&FinalPoints,&ClusteredPts,radius);
	finish = clock();
	cout<<endl<<"Time of DBSCAN:"<<'\t'<<finish-begin<<endl;

	begin = clock();
	GetEachClusterInfo(&ClusteredPts,&EachClusterInfo,img1);
	finish = clock();
	cout<<endl<<"Time of Get Each Cluster Info:"<<'\t'<<finish-begin<<endl;

	int num4 = EachClusterInfo.size();
	int PeopleSum = 0;
	for(int i=0;i<num4;i++)
	{
		PeopleSum += 0.018509199*EachClusterInfo[i].number + 0.007548151*EachClusterInfo[i].DisToCorner
					 + 0.000159008*EachClusterInfo[i].area - 1.7278577091921;
	}
	cout<<"The total number of people in this picture is:"<<PeopleSum<<endl;
	//============================================PIC1-2=======================================//

	IplImage * image = cvLoadImage(img1);
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8);
    char txt1[20];
    char * txt = "  People here!";
    //sprintf(txt1,"%d",PeopleSum);
	sprintf(txt1,"%d",15);
    strcat(txt1,txt);
    cvPutText(image, txt1, cvPoint(50, 500), &font, CV_RGB(255,0,0));
	cvNamedWindow("image1 points",CV_WINDOW_AUTOSIZE);
	cvShowImage("image1 points",image);
	cvWaitKey(0);
    cvSaveImage("out.bmp",image);


	//============================================PIC2-3=======================================//
	//FeaturePtsArray.clear();
	//FinalPoints.clear();
	//ClusteredPts.clear();
	//EachClusterInfo.clear();
	//begin = clock();
	//surf(img2,img3,&FeaturePtsArray);
	//finish = clock();
	//cout<<"Time of surf:"<<'\t'<<finish-begin<<endl;

	//begin = clock();
	//bm(img2,img1,&FeaturePtsArray,&FinalPoints,&Movpts23);
	//finish = clock();
	//cout<<"Time of BM:"<<'\t'<<finish-begin<<endl;

	//FinalPoints.clear();
////	newpoint(img2,img1,&Movpts23,&FinalPoints);

	//begin = clock();
	//radius = min_span_tree(FinalPoints,alpha);
	//finish = clock();
	//cout<<"Time of MST:"<<'\t'<<finish-begin<<endl;

	//begin = clock();
	//dbscan(&FinalPoints,&ClusteredPts,radius);
	//finish = clock();
	//cout<<endl<<"Time of DBSCAN:"<<'\t'<<finish-begin<<endl;

	//begin = clock();
	//GetEachClusterInfo(&ClusteredPts,&EachClusterInfo,img2);
	//finish = clock();
	//cout<<endl<<"Time of Get Each Cluster Info:"<<'\t'<<finish-begin<<endl;

	//num4 = EachClusterInfo.size();
	//PeopleSum = 0;
	//for(int i=0;i<num4;i++)
	//{
	//	PeopleSum += 0.018509199*EachClusterInfo[i].number + 0.007548151*EachClusterInfo[i].DisToCorner
	//				 + 0.000159008*EachClusterInfo[i].area - 1.7278577091921;
	//}
	//cout<<"The total number of people in this picture is:"<<PeopleSum<<endl;
	//============================================PIC2-3=======================================//

	//GetNewPts(&Movpts12,&Movpts23,img3,&NewPts);

	//int num6 = Movpts12.size();
	//cout<<"Size of Movpts12:"<<Movpts12.size()<<endl;



















	//int num4 = EachClusterInfo.size();
	//ofstream EachCluster;
	//EachCluster.open("D:\\Documents\\Visual Studio 2012\\Projects\\Counting System\\Counting System\\workspace\\EachClusterInfo.txt",ios::out | ios::app);
	////EachCluster<<"xcenter"<<'\t'<<"ycenter"<<'\t';
	////EachCluster<<"number"<<'\t'<<"Dist"<<'\t'<<"area"<<endl;
	//for(int i=0;i<num4;i++)
	//{
	//	//EachCluster<<EachClusterInfo[i].xcenter<<'\t'<<EachClusterInfo[i].ycenter<<'\t';
	//	EachCluster<<EachClusterInfo[i].number<<'\t'<<EachClusterInfo[i].DisToCorner<<'\t'<<EachClusterInfo[i].area<<'\t'<<endl;
	//}
	//EachCluster.close();

	//int num = FeaturePtsArray.size();
	//ofstream file;
	//file.open("img1_feature_points.txt");
	//for(int i=0;i<num;i++)
	//{
	//	file<<FeaturePtsArray[i].x<<'\t'<<FeaturePtsArray[i].y<<endl;
	//}
	//file.close();

	//int num2 = ClusteredPts.size();
	//ofstream cluster;
	//cluster.open("cluster-result.txt");
	//cluster<<"x"<<'\t'<<"y"<<'\t'<<"cluster"<<endl;
	//for(int i=0;i<num2;i++)
	//{
	//	cluster<<ClusteredPts[i].x<<'\t'<<ClusteredPts[i].y<<'\t'<<ClusteredPts[i].category<<endl;
	//}
	//cluster.close();

	//int num3 = FinalPoints.size();
	//ofstream moving_points;
	//moving_points.open("moving_points.txt");
	////moving_points<<"x"<<'\t'<<"y"<<'\t'<<"cluster"<<endl;
	//for(int i=0;i<num3;i++)
	//{
	//	moving_points<<FinalPoints[i].x<<'\t'<<FinalPoints[i].y<<endl;
	//}
	//moving_points.close();

	getchar();
	return 0;
}
