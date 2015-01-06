#include "structure.h"
#include <iostream>
#include <opencv2\opencv.hpp> 
#include <opencv2\legacy\legacy.hpp>
#include <vector>

int GetNewPts(std::vector<Movpts> * ADDR_Movpts12,std::vector<Movpts> * ADDR_Movpts23,const char * img3,std::vector<FeaturePoints> * ADDR_NewPts)
{
	int minmad = 1000;
	int count12 = (*ADDR_Movpts12).size();
	int count23 = (*ADDR_Movpts23).size();
	int i,j,k;
	int block_size = BlockSizeWidth*BlockSizeHeight;
	int x,y,TempPtsVector[BlockSizeWidth*BlockSizeHeight];
	int liz,liz_x,liz_y,sum;
	FeaturePoints TempNewPt;

	for(i=0;i<count23;i++)
	{
		x = (*ADDR_Movpts23)[i].x_cur;
		y = (*ADDR_Movpts23)[i].y_cur;
		TempNewPt.x = x;
		TempNewPt.y = y;
		(*ADDR_NewPts).push_back(TempNewPt);
		k = 0;
		for(j=0;j<block_size;j++)
		{
			TempPtsVector[k] = (*ADDR_Movpts23)[i].vec[k];
			k++;
		}
		for(liz=0;liz<count12;liz++)
		{
			k = 0;
			liz_x = (*ADDR_Movpts12)[liz].x_cur;
			liz_y = (*ADDR_Movpts12)[liz].y_cur;
			if ( abs( x-liz_x ) > BlockSizeWidth || abs( y-liz_y ) > BlockSizeHeight )
				continue;
			sum = 0;
			for(j=0;j<block_size;j++)
			{
				sum += abs(TempPtsVector[k] - (*ADDR_Movpts12)[liz].vec[k]);
				k++;
			}
			if (sum<minmad)
			{
				(*ADDR_NewPts).erase((*ADDR_NewPts).end()-1,(*ADDR_NewPts).end());
				break;
			}
		}
	}

	/*IplImage *image3;
	image3 = cvLoadImage(img3);*/
	IplImage * image3 = cvLoadImage(img3);
	int num = (*ADDR_NewPts).size();
	int lizdraw;
	int R = 0,
		G = 255,
		B = 0;
	CvPoint centerpoint;
	for(lizdraw=0;lizdraw<num;lizdraw++)
	{
		centerpoint.x = (*ADDR_NewPts)[lizdraw].x;
		centerpoint.y = (*ADDR_NewPts)[lizdraw].y;
		cvCircle( image3, centerpoint ,3 , CV_RGB(R,G,B),1,  8, 3 );
		//cvSaveImage("p2.bmp",image3);
	}

	std::cout<<"New points number:"<<(*ADDR_NewPts).size()<<std::endl;
	cvNamedWindow("image1",CV_WINDOW_AUTOSIZE);
	cvShowImage("image1",image3);
	cvWaitKey(0);

	return 0;
}

