#include <iostream>
#include <stdlib.h>
#include <sstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

 int main( int argc, char** argv )
 {
    VideoCapture cap(0); //capture the video from webcam

    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the web cam" << endl;
         return -1;
    }

    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

  int iLowB = 0;
 int iHighB = 76;

  int iLowG = 0; 
 int iHighG = 76;

  int iLowR = 119;
 int iHighR = 255;

string cmd;
std::ostringstream oss;
std::ostringstream oss1;
int mx=0,my=0;

  //Create trackbars in "Control" window
 createTrackbar("LowB", "Control", &iLowB, 255); //Hue (0 - 179)
 createTrackbar("HighB", "Control", &iHighB, 255);

  createTrackbar("LowG", "Control", &iLowG, 255); //Saturation (0 - 255)
 createTrackbar("HighG", "Control", &iHighG, 255);

  createTrackbar("LowR", "Control", &iLowR, 255);//Value (0 - 255)
 createTrackbar("HighR", "Control", &iHighR, 255);

  int iLastX = -1; 
 int iLastY = -1;

  //Capture a temporary image from the camera
 Mat imgTmp;
 cap.read(imgTmp); 

  //Create a black image with the size as the camera output
 Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;
 
    while (true)
    {
        Mat imgOriginal;

        bool bSuccess = cap.read(imgOriginal); // read a new frame from video

         if (!bSuccess) //if not success, break loop
        {
             cout << "Cannot read a frame from video stream" << endl;
             break;
        }

//    Mat imgHSV;

   //cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
 
  Mat imgThresholded;

   inRange(imgOriginal, Scalar(iLowB, iLowG, iLowR), Scalar(iHighB, iHighG, iHighR), imgThresholded); //Threshold the image
      
  //morphological opening (removes small objects from the foreground)
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

   //morphological closing (removes small holes from the foreground)
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

   //Calculate the moments of the thresholded image
  Moments oMoments = moments(imgThresholded);

  double dM01 = oMoments.m01;
  double dM10 = oMoments.m10;
  double dArea = oMoments.m00;

   // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
  if (dArea > 50000)
  {
   //calculate the position of the ball
   int posX = dM10 / dArea;
   int posY = dM01 / dArea;        
        
   if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
   {
    //Draw a red line from the previous point to the current point
  /*  line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 2);
    cmd = "xdotool mousemove " ;
    mx= ((double(double(iLastX)/640))*1365);
    oss << mx;  
    my= ((double(double(iLastY)/480))*767);
    oss1 <<  my;
    cmd = cmd + oss.str() + " " + oss1.str();
    oss.str("");
    oss.clear();
    oss1.str("");
    oss1.clear();
    cout<<cmd<<endl;
    cout<<iLastX<<endl;
    cout<<iLastY<<endl<<endl;
    */
    const char * c = cmd.c_str();
    system(c);
   }

    iLastX = posX;
   iLastY = posY;
  }

   imshow("Thresholded Image", imgThresholded); //show the thresholded image

   imgOriginal = imgOriginal + imgLines;
  imshow("Original", imgOriginal); //show the original image

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
            cout << "esc key is pressed by user" << endl;
            break; 
       }
    }

   return 0;
}
