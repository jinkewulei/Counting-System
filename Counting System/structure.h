#ifndef STRUCTURE_H_
#define STRUCTURE_H_
#include <opencv2\opencv.hpp> 

#define BlockSizeWidth 7
#define BlockSizeHeight 7
#define MaxrangeWidth 15
#define MaxrangeHeight 15

//BM parameters
const int movjudge = 5;					//�ж��Ƿ�Ϊ�˶������ֵ��ʹ�õ��ǽ�������
const int SMALL_DIFF=8;						//�ж�һ�����Ƿ�Ϊ��ֹ��ĳ�ʼ������һ������������߲��Լ���ʱ����Ҫ�Ķ����ֵ
const CvSize BlockSize = cvSize(BlockSizeWidth,BlockSizeHeight);		//����������������Ŀ�͸�
const CvSize MaxRange = cvSize(MaxrangeWidth,MaxrangeHeight);		//ƥ������ĵ����͸�
const CvSize ShiftSize = cvSize(1,1);		//���ű�������Ҫ���׸����ֵ


//DBScan parameters
const float alpha = 2.5;			//The number calculated by minimum spanning tree
const int minpts = 5;			//The minimum number of each circle
//const float radius = 1*alpha;	//radius of dbscan,fix length by a parameter

struct FeaturePoints
{
	int x;
	int y;
};

struct ClusterPts
{
	int x;
	int y;
	int category;
};

//structure for continuous pictures
const int VecSize = BlockSizeWidth * BlockSizeHeight;
struct Movpts
{
	int x_pre;
	int y_pre;
	int x_cur;
	int y_cur;
	int vec[VecSize];	//moving point's vortor in current pic(pic 2)
};

struct ClusterInfo
{
	int ClusterId;
	int number;
	float xcenter;
	float ycenter;
	double area;
	double DisToCorner;
};

//counting area
struct Vertex
{
	int x;
	int y;
};

struct point_data
{
	int x;
	int y;
	int category;
	int visit;
	int BelongPacket;
};

int surf (const char * img1,const char * img2,std::vector<FeaturePoints> * ADDR_FeaturePtsArray);
int bm (const char * img1,const char * img2,std::vector<FeaturePoints> * ADDR_inputPoints,std::vector<FeaturePoints> * ADDR_finalPoints,std::vector<Movpts> * ADDR_Movpts);
int dbscan(std::vector<FeaturePoints> * ADDR_finalPoints,std::vector<ClusterPts> * ADDR_ClusteredPts,float radius);
int GetEachClusterInfo(std::vector<ClusterPts> * ADDR_ClusteredPts,std::vector<ClusterInfo> * ADDR_EachClusterInfo,const char * img1);
int GetNewPts(std::vector<Movpts> * ADDR_Movpts12,std::vector<Movpts> * ADDR_Movpts23,const char * img3,std::vector<FeaturePoints> * ADDR_NewPts);
//attention:img1<-->img2
int newpoint(const char * img2,const char * img1,std::vector<Movpts> * ADDR_inputPoints,std::vector<FeaturePoints> * ADDR_Movpts);

#endif