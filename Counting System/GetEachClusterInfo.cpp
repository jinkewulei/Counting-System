#include <iostream>  
#include <iomanip>  
#include <algorithm>
#include <fstream>
#include <vector>
#include "structure.h"
using namespace std;  

vector<Vertex> ver;

//根据叉积排序极角，当极角相等时距离远的点在前面，方便后面删除极角一样的点  
bool cmp(Vertex ver1, Vertex ver2)
{  
    int x1,x2,y1,y2,s;  
    x1 = ver1.x-ver[0].x;  
    y1 = ver1.y-ver[0].y;  
    x2 = ver2.x-ver[0].x;  
    y2 = ver2.y-ver[0].y;  
    s = x1*y2 - x2*y1;  
    if(s>0 || s==0 && x1*x1+y1*y1>x2*x2+y2*y2)  
        return true;  
    else  
        return false;  
}
  
//线段拐向的判断  
//若(p2 - p0) × (p1 - p0) > 0,则p0p1在p1点拐向右侧后得到p1p2。  
//若(p2 - p0) × (p1 - p0) < 0,则p0p1在p1点拐向左侧后得到p1p2。  
//若(p2 - p0) × (p1 - p0) = 0,则p0、p1、p2三点共线  
inline int dig(int x0, int y0, int x1, int y1, int x2, int y2)
{  
    return (x2 - x0) * (y1 - y0) - (y2 - y0) * (x1 - x0);
}
  
//交换  
void Swap(Vertex &ver1, Vertex &ver2)
{  
    Vertex temp;  
    temp.x = ver1.x;  
    temp.y = ver1.y;  
    ver1.x = ver2.x;  
    ver1.y = ver2.y;  
    ver2.x = temp.x;  
    ver2.y = temp.y;  
}

double area()
{
	double s,m;					//s stand for the area 
    int n,i,k,a,b;			//n is the total number of points
    int StackVer[1000];  
    int pVer;  
    int MinY;  

	n = ver.size();
  //  if(n<=2)   
  //  {  
		//s = 0.0;
  //      cout<<"0.0"<<endl;  
		//getchar();
		//getchar();getchar();getchar();getchar();getchar();
  //      return s;  
  //  }  
    s=0;  
    pVer=0;  
    MinY = 0;  
    for (i=1; i<n; i++)  
    {  
        if (ver[i].y<ver[MinY].y || ver[i].y==ver[MinY].y&&ver[i].x<ver[MinY].x)  
        {  
            MinY = i;  
        }  
    }  
          
    Swap(ver[0],ver[MinY]);  
    sort(&ver[0]+1,&ver[0]+n, cmp);  
    k=2;  
    //删除重复的点  
    for(i=2; i<n; i++)  
    {  
        if(dig(ver[0].x, ver[0].y, ver[i-1].x,ver[i-1].y, ver[i].x, ver[i].y) != 0)  
            ver[k++] = ver[i];  
    }  
    //堆栈初始化  
    StackVer[pVer++] = 0;  
    StackVer[pVer++] = 1;  
    StackVer[pVer++] = 2;  
          
    //找到所有极点  
        for(i = 3; i < k; i++)   
        {  
            for(a = StackVer[pVer - 2], b = StackVer[pVer - 1];  
                ! (dig(ver[a].x, ver[a].y, ver[b].x, ver[b].y, ver[i].x, ver[i].y)< 0);  
                a = StackVer[pVer - 2], b = StackVer[pVer - 1])   
                {  
                    pVer--;   //删点  
                }  
                StackVer[pVer++] = i;  //加点  
    }  
          
    //计算面积  
    while(pVer>=2)  
    {  
        a = StackVer[pVer - 2];  
        b = StackVer[pVer - 1];  
        m = dig( ver[0].x, ver[0].y,ver[b].x, ver[b].y,ver[a].x, ver[a].y)/2.0;  
        if(m<0) m=-m;  
        pVer--;  
        s=s+m;  
    }
//    cout<<setiosflags(ios::fixed)<<setprecision(1)<<s<<endl;        

	return s;
}


int GetEachClusterInfo(vector<ClusterPts> * ADDR_ClusteredPts,std::vector<ClusterInfo> * ADDR_EachClusterInfo,const char * img1)
{
	int ClusterNum;
	int k,xsum,ysum,tempnum;

	//Get Total num of cluster befor filter
	int num = (*ADDR_ClusteredPts).size();			
	ClusterNum = (*ADDR_ClusteredPts)[0].category;		//total cluster number
	for(int i=1;i<num;i++)
	{
		if((*ADDR_ClusteredPts)[i].category>ClusterNum)
			ClusterNum = (*ADDR_ClusteredPts)[i].category;
	}
//	cout<<"Total num of cluster befor filter:"<<ClusterNum<<endl;

	//
	IplImage * image = cvLoadImage(img1);
	Vertex temp;
	ClusterInfo TempInfo;
	int x2,y2;
	int R = 0,G = 50,B = 0;
	int situ = 0;
	for(int j=1;j<=ClusterNum;j++)
	{
		CvPoint centerpoint;

		xsum = 0;
		ysum = 0;
		for(k=0;k<num;k++)
		{
			if(j == (*ADDR_ClusteredPts)[k].category)
			{
				xsum += (*ADDR_ClusteredPts)[k].x;
				ysum += (*ADDR_ClusteredPts)[k].y;
				temp.x = (*ADDR_ClusteredPts)[k].x;
				temp.y = (*ADDR_ClusteredPts)[k].y;
				ver.push_back(temp);
			}
		}
		
		tempnum = ver.size();
		if(tempnum>minpts)
		{
			situ++;
			switch (situ)
			{
				case 1:
					R = 255;
					G = 255;
					B = 255;
					break;
				case 2:
					R = 255;
					G = 0;
					B = 0;
					break;
				case 3:
					R = 0;
					G = 0;
					B = 0;
					break;
				case 4:
					R = 0;
					G = 255;
					B = 0;
					break;
				case 5:
					R = 0;
					G = 255;
					B = 255;
					break;
				case 6:
					R = 127;
					G = 127;
					B = 127;
					break;
				case 7:
					R = 0;
					G = 0;
					B = 255;
					break;
				case 8:
					R = 127;
					G = 0;
					B = 127;
					break;
				case 9:
					R = 255;
					G = 255;
					B = 0;
					break;
				case 10:
					R = 255;
					G = 170;
					B = 170;
					break;
			}
			/*if(j%4 == 0)
			{
				R = (R+127)%254;
				G = (R+127)%254;
			}
			if(j%4 == 1)
			{
				R = (R+127)%254;
				B = (R+127)%254;
			}
			if(j%4 == 2)
			{
				B = (R+127)%254;
				G = (R+127)%254;
			}
			if(j%4 == 3)
			{
				R = (R+100)%254;
				G = (R+70)%254;
				B = (R+40)%254;
			}*/

			TempInfo.ClusterId = j;
			TempInfo.number = tempnum;
			TempInfo.xcenter = (int) xsum/tempnum;
			TempInfo.ycenter = (int) ysum/tempnum;
			TempInfo.area = area();
			x2 = 768 - TempInfo.xcenter;
			y2 = 576 - TempInfo.ycenter;
			TempInfo.DisToCorner = sqrt(x2*x2+y2*y2);
			(*ADDR_EachClusterInfo).push_back(TempInfo);
		}

		//==================draw points on the picture group by each cluster==================//
		
		for(int liz=0;liz<tempnum;liz++)
		{
			centerpoint.x = ver[liz].x;
			centerpoint.y = ver[liz].y;
			cvCircle( image, centerpoint ,3 , CV_RGB(R,G,B),1,  8, 3 );
	//		cvSaveImage("p2.bmp",image);
		}
		//==================draw points on the picture group by each cluster==================//

	//	for(int liz=0;liz<5;liz++)
	//	{
	//		centerpoint.x = 768-liz*3;
	//		centerpoint.y = 576-liz*3;
	//		cvCircle( image, centerpoint ,3 , CV_RGB(R,G,B),1,  8, 3 );
	////		cvSaveImage("p2.bmp",image);
	//	}
		ver.clear();
	}
	
	int num4 = (*ADDR_EachClusterInfo).size();
	ofstream EachCluster;
	EachCluster.open("EachClusterInfo.txt",ios::out | ios::app);
	//EachCluster<<"xcenter"<<'\t'<<"ycenter"<<'\t';
	//EachCluster<<"number"<<'\t'<<"Dist"<<'\t'<<"area"<<endl;
	for(int i=0;i<num4;i++)
	{
		//EachCluster<<EachClusterInfo[i].xcenter<<'\t'<<EachClusterInfo[i].ycenter<<'\t';
		EachCluster<<(*ADDR_EachClusterInfo)[i].number<<'\t'<<(*ADDR_EachClusterInfo)[i].DisToCorner<<'\t'<<(*ADDR_EachClusterInfo)[i].area<<'\t'<<endl;
	}
	EachCluster.close();

	/*
	//std::cout<<endl<<"Cluster number:"<<(*ADDR_EachClusterInfo).size()<<endl;
	cvNamedWindow("image1 points",CV_WINDOW_AUTOSIZE);
	cvShowImage("image1 points",image);
	cvWaitKey(0);
	*/
	
	return 0;
}