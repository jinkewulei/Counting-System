#include <opencv2\opencv.hpp> 
#include <opencv2\legacy\legacy.hpp>
#include <vector>
#include "structure.h"

using namespace std;

static inline int cmpBlocks(const uchar* A, const uchar* B, int Bstep, CvSize blockSize )
{
	//注意A存储的是原始图像的一个块的灰度值
    int x, s = 0;
    for( ; blockSize.height--; A += blockSize.width, B += Bstep )		//扫描块A的每一行，所以每次块A增量为块行数，B增量则为图像宽度
    {
        for( x = 0; x <= blockSize.width - 4; x += 4 )					//计算此行中所有像素点 在块A与块B的差的绝对值 的和
            s += std::abs(A[x] - B[x]) + std::abs(A[x+1] - B[x+1]) +
                std::abs(A[x+2] - B[x+2]) + std::abs(A[x+3] - B[x+3]);
        for( ; x < blockSize.width; x++ )								//对此行中没有扫描到的像素做补充，即x最后一次加4仍不到块宽度的部分
            s += std::abs(A[x] - B[x]);
    }
    return s;
}

int bm(const char * img1,const char * img2,std::vector<FeaturePoints> * ADDR_inputPoints,std::vector<FeaturePoints> * ADDR_finalPoints,std::vector<Movpts> * ADDR_Movpts)
{
	/*=============================Original pic to gray=============================*/
	IplImage *image1;
	IplImage *image2;
	IplImage *result1;
	IplImage *result2;
	image1 = cvLoadImage(img1,-1);
	image2 = cvLoadImage(img2,-1);
	
	int channel=1;			//image->nChannels;
	int depth=image1->depth;
	CvSize sz;
	sz.width=image1->width;
	sz.height=image1->height;
	result1 = cvCreateImage(sz,depth,channel);
	result2 = cvCreateImage(sz,depth,channel);

	cvCvtColor(image1,result1,CV_BGR2GRAY);
	cvCvtColor(image2,result2,CV_BGR2GRAY);
	cout<<"Original picture size:"<<'\t'<<sz.width<<" "<<sz.height<<endl;

	CvSize size;
	size.width =  (result1->width - BlockSize.width + ShiftSize.width)/ShiftSize.width;
	size.height = (result1->height - BlockSize.height + ShiftSize.height)/ShiftSize.height;
	IplImage* output = cvCreateImage(size, IPL_DEPTH_32F,1);
	/*=============================Original pic to gray=============================*/

    CvMat stubA, *srcA = cvGetMat( result1, &stubA );		//stubA存储PIC 1 的信息
    CvMat stubB, *srcB = cvGetMat( result2, &stubB );		//stubB存储PIC 2 的信息
    CvMat stubx, *velx = cvGetMat( output, &stubx );		//stubx存储res 1 的信息

    if( !CV_ARE_TYPES_EQ( srcA, srcB ))
        CV_Error( CV_StsUnmatchedFormats, "Source images have different formats" );

    CvSize velSize =
    {
        (srcA->width - BlockSize.width + ShiftSize.width)/ShiftSize.width,
        (srcA->height - BlockSize.height + ShiftSize.height)/ShiftSize.height
    };

    if( CV_MAT_TYPE( srcA->type ) != CV_8UC1 ||
        CV_MAT_TYPE( velx->type ) != CV_32FC1 )
        CV_Error( CV_StsUnsupportedFormat, "Source images must have 8uC1 type and "
                                           "destination images must have 32fC1 type" );

//    if( srcA->step != srcB->step || velx->step != vely->step )
 //       CV_Error( CV_BadStep, "two source or two destination images have different steps" );

    const int BIG_DIFF=128;

    // scanning scheme coordinates
    cv::vector<CvPoint> _ss((2 * MaxRange.width + 1) * (2 * MaxRange.height + 1));	//容量为匹配区域的CvPoint数组
    CvPoint* ss = &_ss[0];
    int ss_count = 0;		//为匹配数组计数

    int blWidth = BlockSize.width, blHeight = BlockSize.height;
    int blSize = blWidth*blHeight;									//块的总大小
    int acceptLevel = blSize * SMALL_DIFF;
    int escapeLevel = blSize * BIG_DIFF;

    int i,j;

    cv::vector<uchar> _blockA(blSize + 16);
    uchar* blockA = (uchar*)&_blockA[0];			//一个（块大小+16）字节的字符型数组的首地址

    // Calculate scanning scheme
    int min_count = MaxRange.width;

    // use spiral search pattern
    //
    //     9 10 11 12
    //     8  1  2 13
    //     7  *  3 14
    //     6  5  4 15
    //... 20 19 18 17
    //
	for( i = 0; i < min_count; i++ )		//将range窗口中的每一个坐标保存在ss结构体数组中
    {
        // four cycles along sides
        int x = -i-1, y = x;		
        // upper side
        for( j = -i; j <= i + 1; j++, ss_count++ )
        {
            ss[ss_count].x = ++x;
            ss[ss_count].y = y;
        }
        // right side
        for( j = -i; j <= i + 1; j++, ss_count++ )
        {
            ss[ss_count].x = x;
            ss[ss_count].y = ++y;
        }
        // bottom side
        for( j = -i; j <= i + 1; j++, ss_count++ )
        {
            ss[ss_count].x = --x;
            ss[ss_count].y = y;
        }
        // left side
        for( j = -i; j <= i + 1; j++, ss_count++ )
        {
            ss[ss_count].x = x;
            ss[ss_count].y = --y;
        }
    }

    int maxX = srcB->cols - BlockSize.width;		//目标图像的宽度
	int maxY = srcB->rows - BlockSize.height;		//目标图像的高度
    const uchar* Adata = srcA->data.ptr;			//PIC1的灰度值头指针，类型为uchar *
    const uchar* Bdata = srcB->data.ptr;			//PIC2的灰度值头指针，类型为uchar *
    int Astep = srcA->step, Bstep = srcB->step;		//PIC1 和 PIC2 的以字节为单位的列数

	//cout<<"++++++++++++++++++++"<<endl;
    // compute the flow
	//i改为特征点的横坐标，j改为特征点的纵坐标
	FeaturePoints tempPoint;
	Movpts MovTemp;
	int countOfPts = (*ADDR_inputPoints).size();
	for (int liz=0;liz<countOfPts;liz++)
	{
	//	cout<<liz<<endl;
		i = (*ADDR_inputPoints)[liz].y;
		j = (*ADDR_inputPoints)[liz].x;
		
		float* vx = (float*)(velx->data.ptr + velx->step*i);	//velx第i行的头指针

		int X1 = j*ShiftSize.width;			//X1为当前列偏移shift倍宽度之后的位置
		int Y1 = i*ShiftSize.height;		//Y1为当前行偏移shift倍高度之后的位置
		int X2, Y2;

		int k;
		//cout<<"X1:"<<X1<<'\t'<<"Y1:"<<Y1<<endl;
		for( k = 0; k < blHeight; k++ )		//遍历BLOCK的每一行，K为每次循环的行的数目
			memcpy( blockA + k*blWidth, Adata + Astep*(Y1 + k) + X1, blWidth );		//将一块中的每一行的值赋给blockA

		X2 = X1;
		Y2 = Y1;
		int dist = INT_MAX;
		if( 0 <= X2 && X2 <= maxX && 0 <= Y2 && Y2 <= maxY )
			dist = cmpBlocks( blockA, Bdata + Bstep*Y2 + X2, Bstep, BlockSize );	//dist是当前块在A、B中原始位置的差值

		int countMin = 1;
		int sumx = 0, sumy = 0;		

		if( dist > acceptLevel )
		{
			// do brute-force search
			for( k = 0; k < ss_count; k++ )		//对maxRange范围的每一个点进行遍历
			{
				int dx = ss[k].x;		//dx表示在块匹配循环的x偏移量
				int dy = ss[k].y;		//dy表示在块匹配循环的y偏移量
				X2 = X1 + dx;					//X2为当前坐标在块匹配循环中偏移后的值
				Y2 = Y1 + dy;					//Y2为当前坐标在块匹配循环中偏移后的值

				if( !(0 <= X2 && X2 <= maxX && 0 <= Y2 && Y2 <= maxY) )
					continue;

				int tmpDist = cmpBlocks( blockA, Bdata + Bstep*Y2 + X2, Bstep, BlockSize );	//计算偏移后每块和原始块的差值
				if( tmpDist < acceptLevel )		//如果maxRange中某一点的MAD值小于acceptLevel,那么就取这点，跳出这个点的块匹配
				{
					sumx = dx; sumy = dy;
					countMin = 1;
					break;
				}

				if( tmpDist < dist )
				{
					dist = tmpDist;
					sumx = dx; sumy = dy;
					countMin = 1;
				}
				else if( tmpDist == dist )
				{
					sumx += dx; sumy += dy;
					countMin++;
				}
			}

			if( dist > escapeLevel )
			{
				sumx = 0;
				sumy = 0;
				countMin = 1;
			}
		}

		if(abs(sumx)+abs(sumy)>movjudge)
		{
			tempPoint.x = j;
			tempPoint.y = i;
			(*ADDR_finalPoints).push_back(tempPoint);
			vx[j] = 255-(float)sumx/countMin;

			//================================= Continuous Mov Detect=========================================//
			MovTemp.x_pre = j;
			MovTemp.y_pre = i;
			MovTemp.x_cur = j + sumx;
			MovTemp.y_cur = i + sumy;
			if(	!	(MovTemp.x_cur>=0 && (MovTemp.x_cur <= sz.width-BlockSize.width) 
					&& MovTemp.y_cur>=0 && (MovTemp.y_cur <= sz.height-BlockSize.height)))
				continue;
			cv::vector<uchar> _new_block(blSize + 16);
			uchar* new_block = (uchar*)&_new_block[0];
			for( k = 0; k < blHeight; k++ )		//遍历BLOCK的每一行，K为每次循环的行的数目
			{
				memcpy( new_block + k*blWidth, Bdata + Astep*(MovTemp.y_cur + k) + MovTemp.x_cur, blWidth );
			}
			CvSize blocksize = BlockSize;
			int x,vec_count=0;
			for( ; blocksize.height--; new_block += blocksize.width)		//扫描块A的每一行，所以每次块A增量为块行数，B增量则为图像宽度
			{				
				//for(x = 0; x <= blocksize.width - 4; x += 4 )					//计算此行中所有像素点 在块A与块B的差的绝对值 的和
				//{
				//	MovTemp.vec[x] = blockA[x];
				//	MovTemp.vec[x+1] =  blockA[x+1];
				//	MovTemp.vec[x+2] =  blockA[x+2];
				//	MovTemp.vec[x+3] =  blockA[x+3];
				//}					
				//for( ; x < blocksize.width; x++ )								//对此行中没有扫描到的像素做补充，即x最后一次加4仍不到块宽度的部分
				//	MovTemp.vec[x] = blockA[x];
				for(x=0; x < blocksize.width; x++ )
				{
					MovTemp.vec[vec_count++] = new_block[x];
				}
			}
			(*ADDR_Movpts).push_back(MovTemp);
			//================================= Continuous Mov Detect=========================================//
		}
	}
	
	IplImage * image = cvLoadImage(img1);
	CvPoint centerpoint;
	int tempnum = (*ADDR_Movpts).size();
	for(int liz=0;liz<tempnum;liz++)
		{
			centerpoint.x = (*ADDR_Movpts)[liz].x_pre;
			centerpoint.y = (*ADDR_Movpts)[liz].y_pre;
			cvCircle( image, centerpoint ,3 , CV_RGB(255,0,0),1,  8, 3 );
	//		cvSaveImage("p2.bmp",image);
		}

	/*
	cvNamedWindow("After BM",1);
	cvShowImage("After BM",image);
	cvWaitKey(0);
	*/
	cout<<endl<<"Points number in moving objects:"<<'\t'<<(*ADDR_finalPoints).size()<<endl;

	return 0;
}

