#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int coord_x[4];
int coord_y[4];
int cnt = 0;
IplImage* temp;
IplImage* result;
void MouseEvent(int,int,int,int,void*);
int main(void)
{
    IplImage* image;
    image = cvLoadImage("./QrCode.jpg");
    if(!image)
    {
        cout << "Can not find this file" << endl;
        return 1;
    }
    cvNamedWindow("origin",CV_WINDOW_AUTOSIZE);
    setMouseCallback("origin",MouseEvent,image);
    cvShowImage("origin",image);
    cvWaitKey(0);
    cvReleaseImage(&image);
    cvDestroyWindow("origin");
}

void MouseEvent(int event,int x,int y,int flags,void* param )
{
    
    if(event == CV_EVENT_LBUTTONDOWN)
    {
        cout << "x : " << x << endl;
        cout << "y : " << y << endl;
        coord_x[cnt] = x;
        coord_y[cnt] = y;
        cnt++;
    }
    if(cnt == 4)
    {
        IplImage* image = (IplImage*)param;
        //IplImage* temp;
        
        CvPoint2D32f srcQuad[4];
        CvPoint2D32f dstQuad[4];
        CvMat* warp_matrix = cvCreateMat(3, 3, CV_32F);
        
        int left,right;
        int top,bottom;
        left = coord_x[0] < coord_x[3] ? coord_x[0]:coord_x[3];
        right = coord_x[1] > coord_x[2] ? coord_x[1]:coord_x[2];
        top = coord_y[0] < coord_y[1] ? coord_y[0]:coord_y[1];
        bottom = coord_y[2] < coord_y[3] ? coord_y[2]:coord_y[3];
        temp = cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_8U,3);
        
        for(int i = top ; i < bottom ; i++)
        {
            for(int j = left ; j < right*3 ; j+=3)
            {
                
                ((uchar*)(temp->imageData + i*temp->widthStep))[j] =  ((uchar*)(image->imageData + i*image->widthStep))[j];
                ((uchar*)(temp->imageData + i*temp->widthStep))[j+1] =  ((uchar*)(image->imageData + i*image->widthStep))[j+1];
                ((uchar*)(temp->imageData + i*temp->widthStep))[j+2] =  ((uchar*)(image->imageData + i*image->widthStep))[j+2];
                /*
                CvScalar s;
                s = cvGet2D(image,i,j);
                cvSet2D(temp,i,j,s);
                */
            }
        }
        //create result image object
        result = cvCreateImage(cvSize(right,right),IPL_DEPTH_8U,3);
        
        srcQuad[0].x = coord_x[0]; //src Top left
        srcQuad[0].y = coord_y[0];
        srcQuad[1].x = coord_x[1]; //src Top right
        srcQuad[1].y = coord_y[1];
        srcQuad[2].x = coord_x[3]; //src Bottom left
        srcQuad[2].y = coord_y[3];
        srcQuad[3].x = coord_x[2]; //src Bot right
        srcQuad[3].y = coord_y[2];

        dstQuad[0].x = 0; //dst Top left
        dstQuad[0].y = 0;
        dstQuad[1].x = right; //dst Top right
        dstQuad[1].y = 0;
        dstQuad[2].x = 0; //dst Bottom left
        dstQuad[2].y = right;
        dstQuad[3].x = right; //dst Bot right
        dstQuad[3].y = right;
        
        cvGetPerspectiveTransform(
            srcQuad,
            dstQuad,
            warp_matrix
        );
       
        cvWarpPerspective(temp, result, warp_matrix);
        
        cnt  = 0;
        cvNamedWindow("Result",0);
        cvResizeWindow("Result",400,400);
        cvShowImage("Result",result);
        cvWaitKey(0);
        cvReleaseImage(&result);
        cvDestroyWindow("Result");
    }

}
