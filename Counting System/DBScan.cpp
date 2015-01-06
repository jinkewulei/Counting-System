#include "structure.h"
#include <math.h>
#include <vector>
#include <fstream>

int GetNeighbours(int i,std::vector<point_data> * ADDR_PtsData,std::vector<int> * ADDR_TempClusterDataId,float radius);
int BuildNewCluster(int * TotalCatagory);
int ExpandCluster(int ID,std::vector<point_data> * ADDR_PtsData,std::vector<int> * ADDR_TempClusterDataId,float radius);

//cluster the points
int dbscan(std::vector<FeaturePoints> * ADDR_finalPoints,std::vector<ClusterPts> * ADDR_ClusteredPts,float radius)
{
	using namespace std;

	int TotalCatagory = 0;			//total cluster number
	vector<int> TempClusterDataId;	//temp cluster package
	vector<point_data> PtsData;		//init feature points in the function dbscan
	int PtsDataSum = (*ADDR_finalPoints).size();
	
	point_data temp;
	temp.category = -1;		//设置数据分类集，-1表示未进行分类,聚类结束仍未-1表示此点是噪声点
	temp.visit = 0;			//设置数据访问属性，0表示未被访问，1表示被访问，2也表示此点为噪声点
	temp.BelongPacket = 0;	//if i-point is not belong the packet,set this parameter as 0,otherwise set 1
	for(int k=0;k<PtsDataSum;k++)
	{
		temp.x = (*ADDR_finalPoints)[k].x;
		temp.y = (*ADDR_finalPoints)[k].y;
		PtsData.push_back(temp);
	}
//	cout<<endl<<PtsData[0].x<<'\t'<<PtsData[0].y<<endl;
//	cout<<endl<<PtsData[PtsDataSum-2].x<<'\t'<<PtsData[PtsDataSum-2].y<<endl;
//	cout<<endl<<PtsData[PtsDataSum-1].x<<'\t'<<PtsData[PtsDataSum-1].y<<endl;

	int ClearPacketID;
	int PacketSize;
	int i,j;

	for(i=0;i<PtsDataSum;i++)
	{
		if(0==PtsData[i].visit)
		{
			int N;
			PtsData[i].visit = 1;
			N = GetNeighbours(i,&PtsData,&TempClusterDataId,radius);
			if (N>=minpts)
			{
				PtsData[i].category = BuildNewCluster(&TotalCatagory);
				ExpandCluster(i,&PtsData,&TempClusterDataId,radius);
			}
			else
			{
				PtsData[i].visit = 2;		//take this point as a "noise"
			}
		}
		//clear packet
		PacketSize = TempClusterDataId.size();
		for(j=0;j<PacketSize;j++)
		{
			ClearPacketID = TempClusterDataId[j];
			PtsData[ClearPacketID].BelongPacket = 0;
		}
		TempClusterDataId.clear();
	}

//	ofstream resss;
//	resss.open("resss.txt");
	
	ClusterPts temp2;
	for(int liz=0;liz<PtsDataSum;liz++)
	{
//		resss<<PtsData[liz].x<<'\t'<<PtsData[liz].y<<'\t'<<PtsData[liz].category<<'\t'<<PtsData[liz].visit<<endl;
		if(PtsData[liz].visit != 2)
		{
			temp2.x = PtsData[liz].x;
			temp2.y = PtsData[liz].y;
			temp2.category = PtsData[liz].category;
			(*ADDR_ClusteredPts).push_back(temp2);
		}
	}
//	resss.close();

	return 0;
}

//expand the cluster of ID
int ExpandCluster(int ID,std::vector<point_data> * ADDR_PtsData,std::vector<int> * ADDR_TempClusterDataId,float radius)
{
//	cout<<endl<<"00000000000000000000000000000 expand "<<ID<<"'s cluster 0000000000000000000000000"<<endl<<endl;
	int NowCluster = (*ADDR_PtsData)[ID].category;
	int PointsOfNeighbour;
	int CurrentPoint;
	int PacketSizeEx;
	
	PacketSizeEx = (*ADDR_TempClusterDataId).size();
	for(int i=0;i<PacketSizeEx;i++)
	{
		PointsOfNeighbour = 0;
		CurrentPoint = (*ADDR_TempClusterDataId)[i];

		if(0 == (*ADDR_PtsData)[CurrentPoint].visit)
		{
			(*ADDR_PtsData)[CurrentPoint].visit = 1;
			PointsOfNeighbour = GetNeighbours(CurrentPoint,ADDR_PtsData,ADDR_TempClusterDataId,radius);

			if(-1 == (*ADDR_PtsData)[CurrentPoint].category)
			{
				(*ADDR_PtsData)[CurrentPoint].category = NowCluster;
			} 
		}
		PacketSizeEx = (*ADDR_TempClusterDataId).size();
	}
	return 0;
}

//get the number of points of this point's neighbour area
int GetNeighbours(int i,std::vector<point_data> * ADDR_PtsData,std::vector<int> * ADDR_TempClusterDataId,float radius)
{
	int n = 0; 
	int count = 0;
	float distance = 30;
	
//	cout<<endl<<"==================== count the"<<i<<" point's neighbour ==================="<<endl<<endl;
	int num = (*ADDR_PtsData).size();
	for(int j=0;j<num;j++)
	{
	//	if(j==i)
	//		continue;

		if( abs((*ADDR_PtsData)[j].x-(*ADDR_PtsData)[i].x)<=radius && abs((*ADDR_PtsData)[j].y-(*ADDR_PtsData)[i].y)<=radius )
		{
			distance =  (float)sqrt(
				 pow((*ADDR_PtsData)[i].x-(*ADDR_PtsData)[j].x,2) + pow((*ADDR_PtsData)[i].y-(*ADDR_PtsData)[j].y,2)  );
			//when a point is near to the kernel,and if it is not exited in the packet,add it
			if(distance <= radius )		
			{
				n++;
		/*		bool NotExitedInPacket = true;
				for(int k=0;k<TempClusterDataId.size();k++)
				{
					if(j == TempClusterDataId[k])
					{
						NotExitedInPacket = false;
						break;
					}
				}
		*/
				if(0 == (*ADDR_PtsData)[j].BelongPacket && j!=i)
				{
					count++;
					(*ADDR_PtsData)[j].BelongPacket = 1;
					(*ADDR_TempClusterDataId).push_back(j);
				}
			}
		}
		else
		{
			continue;
		}
	}

//	if the total added points'qualtity is less than "minpts",delete them from the packet
	n = n-1;	//	delete the point itself
	int nowsize = (*ADDR_TempClusterDataId).size();
	if(n!=0 && n<minpts)
	{
		int nowptid;
		//错误出现在这里，如果点数小于minpts，则不把这些点加入到N,同时对他们其加入到N这个标记进行清除，清除的应该是末尾的数据，而不是从0开始
		for(int i=1;i<=count;i++)
		{
			nowptid = (*ADDR_TempClusterDataId)[nowsize-i];
			(*ADDR_PtsData)[nowptid].BelongPacket = 0; 
		}
		//如果点数小于minpts，则不把这些点加入到N
		(*ADDR_TempClusterDataId).erase((*ADDR_TempClusterDataId).end()-count,(*ADDR_TempClusterDataId).end());
	}
	return n;
}

//build a new cluster,and refresh the "TotalCluster"
int BuildNewCluster(int * TotalCatagory)
{
	return (++(*TotalCatagory));
}