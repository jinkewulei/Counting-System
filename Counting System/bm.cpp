#include <opencv2\opencv.hpp> 
#include <opencv2\legacy\legacy.hpp>
#include <vector>
#include "structure.h"

using namespace std;

static inline int cmpBlocks(const uchar* A, const uchar* B, int Bstep, CvSize blockSize )
{
	//ע��A�洢����ԭʼͼ���һ����ĻҶ�ֵ
    int x, s = 0;
    for( ; blockSize.height--; A += blockSize.width, B += Bstep )		//ɨ���A��ÿһ�У�����ÿ�ο�A����Ϊ��������B������Ϊͼ����
    {
        for( x = 0; x <= blockSize.width - 4; x += 4 )					//����������������ص� �ڿ�A���B�Ĳ�ľ���ֵ �ĺ�
            s += std::abs(A[x] - B[x]) + std::abs(A[x+1] - B[x+1]) +
                std::abs(A[x+2] - B[x+2]) + std::abs(A[x+3] - B[x+3]);
        for( ; x < blockSize.width; x++ )								//�Դ�����û��ɨ�赽�����������䣬��x���һ�μ�4�Բ������ȵĲ���
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

    CvMat stubA, *srcA = cvGetMat( result1, &stubA );		//stubA�洢PIC 1 ����Ϣ
    CvMat stubB, *srcB = cvGetMat( result2, &stubB );		//stubB�洢PIC 2 ����Ϣ
    CvMat stubx, *velx = cvGetMat( output, &stubx );		//stubx�洢res 1 ����Ϣ

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
    cv::vector<CvPoint> _ss((2 * MaxRange.width + 1) * (2 * MaxRange.height + 1));	//����Ϊƥ�������CvPoint����
    CvPoint* ss = &_ss[0];
    int ss_count = 0;		//Ϊƥ���������

    int blWidth = BlockSize.width, blHeight = BlockSize.height;
    int blSize = blWidth*blHeight;									//����ܴ�С
    int acceptLevel = blSize * SMALL_DIFF;
    int escapeLevel = blSize * BIG_DIFF;

    int i,j;

    cv::vector<uchar> _blockA(blSize + 16);
    uchar* blockA = (uchar*)&_blockA[0];			//һ�������С+16���ֽڵ��ַ���������׵�ַ

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
	for( i = 0; i < min_count; i++ )		//��range�����е�ÿһ�����걣����ss�ṹ��������
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

    int maxX = srcB->cols - BlockSize.width;		//Ŀ��ͼ��Ŀ��
	int maxY = srcB->rows - BlockSize.height;		//Ŀ��ͼ��ĸ߶�
    const uchar* Adata = srcA->data.ptr;			//PIC1�ĻҶ�ֵͷָ�룬����Ϊuchar *
    const uchar* Bdata = srcB->data.ptr;			//PIC2�ĻҶ�ֵͷָ�룬����Ϊuchar *
    int Astep = srcA->step, Bstep = srcB->step;		//PIC1 �� PIC2 �����ֽ�Ϊ��λ������

	//cout<<"++++++++++++++++++++"<<endl;
    // compute the flow
	//i��Ϊ������ĺ����꣬j��Ϊ�������������
	FeaturePoints tempPoint;
	Movpts MovTemp;
	int countOfPts = (*ADDR_inputPoints).size();
	for (int liz=0;liz<countOfPts;liz++)
	{
	//	cout<<liz<<endl;
		i = (*ADDR_inputPoints)[liz].y;
		j = (*ADDR_inputPoints)[liz].x;
		
		float* vx = (float*)(velx->data.ptr + velx->step*i);	//velx��i�е�ͷָ��

		int X1 = j*ShiftSize.width;			//X1Ϊ��ǰ��ƫ��shift�����֮���λ��
		int Y1 = i*ShiftSize.height;		//Y1Ϊ��ǰ��ƫ��shift���߶�֮���λ��
		int X2, Y2;

		int k;
		//cout<<"X1:"<<X1<<'\t'<<"Y1:"<<Y1<<endl;
		for( k = 0; k < blHeight; k++ )		//����BLOCK��ÿһ�У�KΪÿ��ѭ�����е���Ŀ
			memcpy( blockA + k*blWidth, Adata + Astep*(Y1 + k) + X1, blWidth );		//��һ���е�ÿһ�е�ֵ����blockA

		X2 = X1;
		Y2 = Y1;
		int dist = INT_MAX;
		if( 0 <= X2 && X2 <= maxX && 0 <= Y2 && Y2 <= maxY )
			dist = cmpBlocks( blockA, Bdata + Bstep*Y2 + X2, Bstep, BlockSize );	//dist�ǵ�ǰ����A��B��ԭʼλ�õĲ�ֵ

		int countMin = 1;
		int sumx = 0, sumy = 0;		

		if( dist > acceptLevel )
		{
			// do brute-force search
			for( k = 0; k < ss_count; k++ )		//��maxRange��Χ��ÿһ������б���
			{
				int dx = ss[k].x;		//dx��ʾ�ڿ�ƥ��ѭ����xƫ����
				int dy = ss[k].y;		//dy��ʾ�ڿ�ƥ��ѭ����yƫ����
				X2 = X1 + dx;					//X2Ϊ��ǰ�����ڿ�ƥ��ѭ����ƫ�ƺ��ֵ
				Y2 = Y1 + dy;					//Y2Ϊ��ǰ�����ڿ�ƥ��ѭ����ƫ�ƺ��ֵ

				if( !(0 <= X2 && X2 <= maxX && 0 <= Y2 && Y2 <= maxY) )
					continue;

				int tmpDist = cmpBlocks( blockA, Bdata + Bstep*Y2 + X2, Bstep, BlockSize );	//����ƫ�ƺ�ÿ���ԭʼ��Ĳ�ֵ
				if( tmpDist < acceptLevel )		//���maxRange��ĳһ���MADֵС��acceptLevel,��ô��ȡ��㣬���������Ŀ�ƥ��
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
			for( k = 0; k < blHeight; k++ )		//����BLOCK��ÿһ�У�KΪÿ��ѭ�����е���Ŀ
			{
				memcpy( new_block + k*blWidth, Bdata + Astep*(MovTemp.y_cur + k) + MovTemp.x_cur, blWidth );
			}
			CvSize blocksize = BlockSize;
			int x,vec_count=0;
			for( ; blocksize.height--; new_block += blocksize.width)		//ɨ���A��ÿһ�У�����ÿ�ο�A����Ϊ��������B������Ϊͼ����
			{				
				//for(x = 0; x <= blocksize.width - 4; x += 4 )					//����������������ص� �ڿ�A���B�Ĳ�ľ���ֵ �ĺ�
				//{
				//	MovTemp.vec[x] = blockA[x];
				//	MovTemp.vec[x+1] =  blockA[x+1];
				//	MovTemp.vec[x+2] =  blockA[x+2];
				//	MovTemp.vec[x+3] =  blockA[x+3];
				//}					
				//for( ; x < blocksize.width; x++ )								//�Դ�����û��ɨ�赽�����������䣬��x���һ�μ�4�Բ������ȵĲ���
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

