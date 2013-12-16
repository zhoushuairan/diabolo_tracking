#include "shapeDetection_OpenCV.h"


int detectCircle(ofImage trace){
  //Mat src, src_gray;
	Mat src(trace.getWidth(), trace.getHeight(), CV_8UC1, trace.getPixels());
  /// Read the image
  //src = imread( argv[1], 1 );

  //if( !src.data )
  //  { return -1; }

  /// Convert it to gray
  //cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Reduce the noise so we avoid false circle detection
  //GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

  vector<Vec3f> circles;

  /// Apply the Hough Transform to find the circles
  HoughCircles( src, circles, CV_HOUGH_GRADIENT, 1, src.rows/8, 200, 100, 0, 0 );

  /// Draw the circles detected
  for( size_t i = 0; i < circles.size(); i++ )
  {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      ofSetColor(255, 0, 0);
      ofCircle(300 + circles[i][0], 300 + circles[i][1], radius);
      // circle center
      //circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      //circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
   }

  /// Show your results
  //namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
  //imshow( "Hough Circle Transform Demo", src );

  //waitKey(0);
}