#include <iostream>
#include <opencv/cv.h>
#include "opencv/cvaux.h"
#include "opencv/cxmisc.h"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <opencv/highgui.h>

using namespace std;
using namespace cv;
//VARIABLES for CODEBOOK METHOD:
CvBGCodeBookModel* model = 0;
const int NCHANNELS = 3;
bool ch[NCHANNELS]={true,true,true}; // This sets what channels should be adjusted for background bounds


void  detect(IplImage* img_8uc1,IplImage* img_8uc3);

void help(void)
{
    printf("\nLearn background and find foreground using simple average and average difference learning method:\n"
           "\nUSAGE:\nbgfg_codebook [--nframes=300] [movie filename, else from camera]\n"
           "***Keep the focus on the video windows, NOT the consol***\n\n"
           "INTERACTIVE PARAMETERS:\n"
           "\tESC,q,Q  - quit the program\n"
           "\th	- print this help\n"
           "\tp	- pause toggle\n"
           "\ts	- single step\n"
           "\tr	- run mode (single step off)\n"
           "=== AVG PARAMS ===\n"
           "\t-    - bump high threshold UP by 0.25\n"
           "\t=    - bump high threshold DOWN by 0.25\n"
           "\t[    - bump low threshold UP by 0.25\n"
           "\t]    - bump low threshold DOWN by 0.25\n"
           "=== CODEBOOK PARAMS ===\n"
           "\ty,u,v- only adjust channel 0(y) or 1(u) or 2(v) respectively\n"
           "\ta	- adjust all 3 channels at once\n"
           "\tb	- adjust both 2 and 3 at once\n"
           "\ti,o	- bump upper threshold up,down by 1\n"
           "\tk,l	- bump lower threshold up,down by 1\n"
           "\tSPACE - reset the model\n"
           );
}

//
//USAGE:  ch9_background startFrameCollection# endFrameCollection# [movie filename, else from camera]
//If from AVI, then optionally add HighAvg, LowAvg, HighCB_Y LowCB_Y HighCB_U LowCB_U HighCB_V LowCB_V
//

String inttostr(int input)
{
    stringstream ss;
    ss << input;
    return ss.str();
}

/*
int main(void)
{
    Mat frame;
    
    vector<int> compression_params;
    //vector that stores the compression parameters of the image
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    //specify the compression technique
    compression_params.push_back(100); //specify the compression quality
    
    int photocount = 0; //initialize image counter
    String imagename;
    int key;
    
    VideoCapture cap(0); //open camera no.0  0=internal 1=external
    while ((key = waitKey(30)) != 27) //wait 30 milliseconds and check for esc key
    {
        cap >> frame; //save captured image to frame variable
        imshow("Camera", frame); //show image on window named Camera
        if (key == 'c')
        {
            photocount++;// increment image number for each capture
            imshow("Captured", frame);
            
            imagename = "/Users/sartaj10/Desktop/ " + inttostr(photocount) + ".jpg";
            //finalize imagename to save
            
            imwrite(imagename, frame, compression_params);
            
        }
        
    }
}
*/

int main(int argc, char** argv)
{
    int photocount = 0; //initialize image counter
    // start new code
    int p[3];
    //specify the compression technique
    p[0] = CV_IMWRITE_JPEG_QUALITY;
    //specify the compression quality
    p[1] = 100;
    p[2] = 0;

    const char* filename = 0;
    IplImage* rawImage = 0, *yuvImage = 0; //yuvImage is for codebook method
    IplImage *ImaskCodeBook = 0,*ImaskCodeBookCC = 0, *showImageCB = 0;
    CvCapture* capture = 0;
    
    int c, n, nframes = 0;
    int nframesToLearnBG = 300;
    
    model = cvCreateBGCodeBookModel();
    
    //Set color thresholds to default values
    model->modMin[0] = 3;
    model->modMin[1] = model->modMin[2] = 3;
    model->modMax[0] = 10;
    model->modMax[1] = model->modMax[2] = 10;
    model->cbBounds[0] = model->cbBounds[1] = model->cbBounds[2] = 10;
    
    bool pause = false;
    bool singlestep = false;
    
    for( n = 1; n < argc; n++ )
    {
        static const char* nframesOpt = "--nframes=";
        if( strncmp(argv[n], nframesOpt, strlen(nframesOpt))==0 )
        {
            if( sscanf(argv[n] + strlen(nframesOpt), "%d", &nframesToLearnBG) == 0 )
            {
                help();
                return -1;
            }
        }
        else
            filename = argv[n];
    }
    
    if( !filename )
    {
        printf("Capture from camera\n");
        capture = cvCaptureFromCAM( 0 );
    }
    else
    {
        printf("Capture from file %s\n",filename);
        capture = cvCreateFileCapture( filename );
    }
    
    if( !capture )
    {
        printf( "Can not initialize video capturing\n\n" );
        help();
        return -1;
    }
    
    //MAIN PROCESSING LOOP:
    for(;;)
    {
        if( !pause )
        {
            rawImage = cvQueryFrame( capture );
            ++nframes;
            if(!rawImage)
                break;
        }
        if( singlestep )
            pause = true;
        
        //First time:
        if( nframes == 1 && rawImage )
        {
            // CODEBOOK METHOD ALLOCATION
            yuvImage = cvCloneImage(rawImage);
            ImaskCodeBook = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
            ImaskCodeBookCC = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
            showImageCB = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);
            
            cvSet(ImaskCodeBook,cvScalar(255));
            
            cvNamedWindow( "Raw", 1 );
            cvNamedWindow( "ForegroundCodeBook",1);
            cvNamedWindow( "CodeBook_ConnectComp",1);
        }
        
        // If we've got an rawImage and are good to go:
        if( rawImage )
        {
            cvCvtColor( rawImage, yuvImage, CV_BGR2YCrCb );//YUV For codebook method
            //This is where we build our background model
            if( !pause && nframes-1 < nframesToLearnBG  )
                cvBGCodeBookUpdate( model, yuvImage );
            
            if( nframes-1 == nframesToLearnBG  )
                cvBGCodeBookClearStale( model, model->t/2 );
            
            //Find the foreground if any
            if( nframes-1 >= nframesToLearnBG  )
            {
                // Find foreground by codebook method
                cvBGCodeBookDiff( model, yuvImage, ImaskCodeBook );
                // This part just to visualize bounding boxes and centers if desired
                cvCopy(ImaskCodeBook,ImaskCodeBookCC);
                cvSegmentFGMask( ImaskCodeBookCC );
                //bwareaopen_(ImaskCodeBookCC,100);
                cvCopy(ImaskCodeBookCC, showImageCB);
                cvShowImage( "CodeBook_ConnectComp",ImaskCodeBookCC);
                detect(ImaskCodeBookCC,rawImage);
                
                photocount++;// increment image number for each capture
                //cvShowImage("Captured", showImageCB);
                
                String imagename = "/Users/sartaj10/Desktop/Images/" + inttostr(photocount) + ".jpg";
                const char * c = imagename.c_str();
                
                //CvSize size = cvSize(150,150);
                //IplImage *destination = cvCreateImage(size, showImageCB->depth, s howImageCB->nChannels);
                //cvResize(showImageCB, destination);
                //cvSaveImage(c, destination,p);
                cvSaveImage(c, showImageCB, p);

            }
            //Display
            cvShowImage( "Raw", rawImage );
            cvShowImage( "ForegroundCodeBook",ImaskCodeBook);
            
        }

        // User input:
        c = cvWaitKey(10)&0xFF;
        c = tolower(c);
        // End processing on ESC, q or Q
        if(c == 27 || c == 'q')
            break;
        
        /*
        if(c == 'w')
        {
            photocount++;// increment image number for each capture
            cvShowImage("Captured", showImageCB);
            
            String imagename = "/Users/sartaj10/Desktop/Images" + inttostr(photocount) + ".jpg";
            const char * c = imagename.c_str();
            
            //CvSize size = cvSize(150,150);
            //IplImage *destination = cvCreateImage(size, showImageCB->depth, showImageCB->nChannels);
            //cvResize(showImageCB, destination);
            //cvSaveImage(c, destination,p);
            cvSaveImage(c, showImageCB, p);
        }
        */
        
        switch( c )
        {
            case 'h':
                help();
                break;
            case 'p':
                pause = !pause;
                break;
            case 's':
                singlestep = !singlestep;
                pause = false;
                break;
            case 'r':
                pause = false;
                singlestep = false;
                break;
            case ' ':
                cvBGCodeBookClearStale( model, 0 );
                nframes = 0;
                break;
                //CODEBOOK PARAMS
            case 'y': case '0':
            case 'u': case '1':
            case 'v': case '2':
            case 'a': case '3':
            case 'b':
                ch[0] = c == 'y' || c == '0' || c == 'a' || c == '3';
                ch[1] = c == 'u' || c == '1' || c == 'a' || c == '3' || c == 'b';
                ch[2] = c == 'v' || c == '2' || c == 'a' || c == '3' || c == 'b';
                printf("CodeBook YUV Channels active: %d, %d, %d\n", ch[0], ch[1], ch[2] );
                break;
            case 'i': //modify max classification bounds (max bound goes higher)
            case 'o': //modify max classification bounds (max bound goes lower)
            case 'k': //modify min classification bounds (min bound goes lower)
            case 'l': //modify min classification bounds (min bound goes higher)
            {
                uchar* ptr = c == 'i' || c == 'o' ? model->modMax : model->modMin;
                for(n=0; n<NCHANNELS; n++)
                {
                    if( ch[n] )
                    {
                        int v = ptr[n] + (c == 'i' || c == 'l' ? 1 : -1);
                        ptr[n] = CV_CAST_8U(v);
                    }
                    printf("%d,", ptr[n]);
                }
                printf(" CodeBook %s Side\n", c == 'i' || c == 'o' ? "High" : "Low" );
            }
                break;
        }
    }
    
    cvReleaseCapture( &capture );
    cvDestroyWindow( "Raw" );
    cvDestroyWindow( "ForegroundCodeBook");
    cvDestroyWindow( "CodeBook_ConnectComp");
    return 0;
}

void  detect(IplImage* img_8uc1,IplImage* img_8uc3) {
    
    
    //cvNamedWindow( "aug", 1 );
    
    
    //cvThreshold( img_8uc1, img_edge, 128, 255, CV_THRESH_BINARY );
    CvMemStorage* storage = cvCreateMemStorage();
    CvSeq* first_contour = NULL;
    CvSeq* maxitem=NULL;
    double area=0,areamax=0;
    int maxn=0;
    int Nc = cvFindContours(
                            img_8uc1,
                            storage,
                            &first_contour,
                            sizeof(CvContour),
                            CV_RETR_LIST // Try all four values and see what happens
                            );
    int n=0;
    //printf( "Total Contours Detected: %d\n", Nc );
    
    if(Nc>0)
    {
        for( CvSeq* c=first_contour; c!=NULL; c=c->h_next )
        {
            
            //cvCvtColor( img_8uc1, img_8uc3, CV_GRAY2BGR );
            
            area=cvContourArea(c,CV_WHOLE_SEQ );
            
            if(area>areamax)
            {
                areamax=area;
                maxitem=c;
                maxn=n;
            }
            
            
            n++;
            
            
        }
        CvMemStorage* storage3 = cvCreateMemStorage(0);
        //if (maxitem) maxitem = cvApproxPoly( maxitem, sizeof(maxitem), storage3, CV_POLY_APPROX_DP, 3, 1 );
        
        
        
        if(areamax>5000)
        {
            maxitem = cvApproxPoly( maxitem, sizeof(CvContour), storage3, CV_POLY_APPROX_DP, 10, 1 );
            
            CvPoint pt0;
            
            CvMemStorage* storage1 = cvCreateMemStorage(0);
            CvMemStorage* storage2 = cvCreateMemStorage(0);
            CvSeq* ptseq = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvContour),
                                       sizeof(CvPoint), storage1 );
            CvSeq* hull;
            CvSeq* defects;
            
            for(int i = 0; i < maxitem->total; i++ )
            {   CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, maxitem, i );
                pt0.x = p->x;
                pt0.y = p->y;
                cvSeqPush( ptseq, &pt0 );
            }
            hull = cvConvexHull2( ptseq, 0, CV_CLOCKWISE, 0 );
            int hullcount = hull->total;
            
            defects= cvConvexityDefects(ptseq,hull,storage2  );
            
            //printf(" defect no %d \n",defects->total);
            
            
            
            
            CvConvexityDefect* defectArray;
            
            
            int j=0;
            //int m_nomdef=0;
            // This cycle marks all defects of convexity of current contours.
            for(;defects;defects = defects->h_next)
            {
                int nomdef = defects->total; // defect amount
                //outlet_float( m_nomdef, nomdef );
                
                //printf(" defect no %d \n",nomdef);
                
                if(nomdef == 0)
                    continue;
                
                // Alloc memory for defect set.
                //fprintf(stderr,"malloc\n");
                defectArray = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect)*nomdef);
                
                // Get defect set.
                //fprintf(stderr,"cvCvtSeqToArray\n");
                 cvCvtSeqToArray(defects,defectArray, CV_WHOLE_SEQ);
                
                // Draw marks for all defects.
                for(int i=0; i<nomdef; i++)  
                {   printf(" defect depth for defect %d %f \n",i,defectArray[i].depth);
                    cvLine(img_8uc3, *(defectArray[i].start), *(defectArray[i].depth_point),CV_RGB(255,255,0),1, CV_AA, 0 );  
                    cvCircle( img_8uc3, *(defectArray[i].depth_point), 5, CV_RGB(0,0,164), 2, 8,0);  
                    cvCircle( img_8uc3, *(defectArray[i].start), 5, CV_RGB(0,0,164), 2, 8,0);  
                    cvLine(img_8uc3, *(defectArray[i].depth_point), *(defectArray[i].end),CV_RGB(255,255,0),1, CV_AA, 0 );  
                    
                } 
                char txt[]="0";
                txt[0]='0'+nomdef-1;
                CvFont font;
                cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 5, CV_AA);
                cvPutText(img_8uc3, txt, cvPoint(50, 50), &font, cvScalar(0, 0, 255, 0)); 
                
                j++;  
                
                // Free memory.         
                free(defectArray);  
            } 
            
            
            cvReleaseMemStorage( &storage );
            cvReleaseMemStorage( &storage1 );
            cvReleaseMemStorage( &storage2 );
            cvReleaseMemStorage( &storage3 );
            //return 0;
        }
    }
}