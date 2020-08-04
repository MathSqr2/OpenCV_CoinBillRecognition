#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

Mat src, src_gray;
Mat b_hist, g_hist, r_hist;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
int numberOfNotes = 0;
string fileName;

double calculeMean( const Mat & histograma )
{
    // MEAN and STANDARD DEVIATION
    vector<double> meanValue;
    vector<double> stdDev;
    meanStdDev( histograma, meanValue, stdDev );
    return meanValue[0];
}

void coins (int, void*)
{
  vector<Vec3f> circles;

  /// Apply the Hough Transform to find the circles
  
  HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT,
          2,   // accumulator resolution (size of the image / 2)
          src_gray.rows/8,  // minimum distance between two circles
          120, // Canny high threshold
          100, // minimum number of votes
          25, 120); // min and max radius

  //cout << "Numero de moedas: "<<circles.size()<<"\n";

  /// Draw the circles detected
  for( size_t i = 0; i < circles.size(); i++ )
  {
      // Vamos detetar o centro e o raio de cada circulo a analisar para desenhar na imagem final
      int radius = cvRound(circles[i][2]);
      int center_x = cvRound(circles[i][0]);
      int center_y = cvRound(circles[i][1]);

      // Ponto correspondente ao centro
      Point center(center_x,center_y);
    
      // Rectâgulo que engloba a moeda 
      Rect rect_roi = Rect(center_x-radius,center_y-radius, radius*2, radius*2);

      cout << "Moeda "<<i<<" com raio "<<radius<<" :";

      // Desenhar centro do circulo
      circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );

      // Desenhar rectangulo que engloba o circulo
      rectangle( src, rect_roi, Scalar(0, 255, 0), 2, 8);

      Mat roi_hist (src, rect_roi);

      stringstream stream;
			stream << i;
      imwrite("results/"+fileName+"-moeda-"+stream.str()+".png",roi_hist);

      // Compute histogram
      vector<Mat> bgr_planes;
      split( roi_hist, bgr_planes );

      /// Establish the number of bins
      int histSize = 256;

      /// Set the ranges ( for B,G,R) )
      float range[] = { 0, 256 } ;
      const float* histRange = { range };

      bool uniform = true; bool accumulate = false;

      Mat b_hist, g_hist, r_hist;

      /// Compute the histograms:
      calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
      calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
      calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

      // Draw the histograms for B, G and R
      int hist_w = 512; int hist_h = 400;
      int bin_w = cvRound( (double) hist_w/histSize );

      Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );


      /// Normalize the result to [ 0, histImage.rows ]
      normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
      normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
      normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
      
      /// Desenha cada canal do histograma
			for( int j = 1; j < histSize; j++ )
			{
			  line( histImage, Point( bin_w*(j-1), hist_h - cvRound(b_hist.at<float>(j-1)) ) ,
							   Point( bin_w*(j), hist_h - cvRound(b_hist.at<float>(j)) ),
							   Scalar( 255, 0, 0), 2, 8, 0  );
			  line( histImage, Point( bin_w*(j-1), hist_h - cvRound(g_hist.at<float>(j-1)) ) ,
							   Point( bin_w*(j), hist_h - cvRound(g_hist.at<float>(j)) ),
							   Scalar( 0, 255, 0), 2, 8, 0  );
			  line( histImage, Point( bin_w*(j-1), hist_h - cvRound(r_hist.at<float>(j-1)) ) ,
							   Point( bin_w*(j), hist_h - cvRound(r_hist.at<float>(j)) ),
							   Scalar( 0, 0, 255), 2, 8, 0  );							 
			}


      stringstream histStream;
			histStream << i;
      imwrite("results/"+fileName+"-hist-"+histStream.str()+".png",histImage);

      double meanb = mean(b_hist)[0];
      double meang = mean(g_hist)[0];
      double meanr = mean(r_hist)[0];
      cout << meanb << ", " << meang << ", " << meanr << ".\n";

      //Detecting .02 cent coins
      if ((radius >= 46 && radius <=47) && (
      (meanb >= 16.3 && meanb <= 17.4934) ||
      (meang >= 17.6 && meang <= 18) ||
      (meanr >= 19.8947 && meanr <= 21.9527)))
      {
        stringstream stream;
        stream << "0.02";
        putText(src, stream.str(), center, FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2);
      }
      //Detecting .05 cent coins
      else if((radius >= 51 && radius <=55) && (
      (meanb >= 19.6 && meanb <= 20.2) || 
      (meang >= 15.6 && meang <= 20.2) ||
      (meanr >= 19.8947 && meanr <= 21.9527)))
      {
        stringstream stream;
        stream << "0.05";
        putText(src, stream.str(), center, FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2);
      }
      //Detecting .10 cent coins
      else if((radius >= 48 && radius <=49) && (
      (meanb >= 15.6 && meanb <= 18.4) ||
      (meang >= 16.8 && meang <= 18.8) ||
      (meanr >= 19.8947 && meanr <= 21.9527)))
      {
        stringstream stream;
        stream << "0.10";
        putText(src, stream.str(), center, FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2);
      }
      //Detecting .20 cent coins
      else if((radius >= 54 && radius <=57) && ( 
      (meanb >= 16.4077 && meanb <= 19.8804) || 
      (meang >= 20.1853 && meang <= 21.7645) ||
      (meanr >= 19.8947 && meanr <= 21.9527)))
      {
        stringstream stream;
        stream << "0.20";
        putText(src, stream.str(), center, FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2);
      }
      //Detecting .50 cent coins
      else if((radius >= 58 && radius <=60) && (
      (meanb >= 20.7552 && meanb <= 22.2993) || 
      (meang >= 22.3195 && meang <= 22.9358) ||
      (meanr >= 22.3433 && meanr <= 23.1244)))
      {
        stringstream stream;
        stream << "0.50";
        putText(src, stream.str(), center, FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2);
      }
      //Detecting 1 euro coins
      else if((radius >= 57 && radius <=58) && (
      (meanb >= 17.0212 && meanb <= 20.8697) || 
      (meang >= 21.6024 && meang <= 21.9527) ||
      (meanr >= 21.8817 && meanr <= 22.3433)))
      {
        stringstream stream;
        stream << "1";
        putText(src, stream.str(), center, FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2);
      }
      //Detecting 2 euro coins
      else if((radius >= 60 && radius <=64) && (
      (meanb >= 16.5198 && meanb <= 21.621) ||
      (meang >= 22.9826 && meang <= 24.6866) ||
      (meanr >= 23.1244 && meanr <= 24.6866)))
      {
        stringstream stream;
        stream << "2";
        putText(src, stream.str(), center, FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2);
      }
      //Detecting .05 cent coins
      else
      {
        stringstream stream;
	  		stream << "0";
        putText(src, stream.str(), center, FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2);
      }
   }
  
  stringstream stream;
  stream << "Existem " << circles.size() << " moedas.";
  putText(src, stream.str(), Point(10,30), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2);
  /// Show your results
  namedWindow( "Deteção de moedas", CV_WINDOW_AUTOSIZE );
  imshow( "Deteção de moedas", src );
}

void notes (int, void*)
{
  Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// Detect edges using Threshold
  threshold( src_gray, threshold_output, 0, 150, THRESH_BINARY | THRESH_OTSU);
  /// Find contours
  findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );

  for( size_t i = 0; i < contours.size(); i++ )
  { 
    approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
    boundRect[i] = boundingRect( Mat(contours_poly[i]) );
  }

  /// Draw polygonal contour + bonding rects
  Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  for( size_t i = 0; i< contours.size(); i++ )
  {
        double area = contourArea(contours[i], false);
        if(area > 100000 && area < 300000)
        {
            numberOfNotes++;
            cout << "area: "<< area << "\n";
            cout << "top corner" << boundRect[i].tl() << "\n";
            cout << "bot corner" << boundRect[i].br() << "\n";

            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            drawContours( drawing, contours_poly, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() );
            
            // rectangle that will be used to evaluate its value through histograms
            Rect rect_inside = Rect(boundRect[i].tl().x + boundRect[i].width/4 , boundRect[i].tl().y + boundRect[i].height/4, boundRect[i].width/2, boundRect[i].height/2  );
            
            //rectangle( src, rect_inside, Scalar(255, 255, 0), 2, 8, 0 );
            
            // rectangle corresponding to the boundaries of the currency note
            rectangle( src, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );    

            Mat roi_hist (src, rect_inside);

            // Compute histogram
            vector<Mat> bgr_planes;
            split( roi_hist, bgr_planes );

            /// Establish the number of bins
            int histSize = 256;

            /// Set the ranges ( for B,G,R) )
            float range[] = { 0, 256 } ;
            const float* histRange = { range };

            bool uniform = true; bool accumulate = false;

            /// Compute the histograms:
            calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
            calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
            calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

            // Draw the histograms for B, G and R
            int hist_w = 512; int hist_h = 400;
            int bin_w = cvRound( (double) hist_w/histSize );

            Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

            /// Normalize the result to [ 0, histImage.rows ]
            normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
            normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
            normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
            
            double meanb = mean(b_hist)[0];
            double meang = mean(g_hist)[0];
            double meanr = mean(r_hist)[0];
            cout << "r:" << meanr << ", g:" << meang << ", b:" << meanb << ".\n";   
            
            if(area >= 130000 && area <= 150000)
            {
              stringstream stream;
              stream << "5 euros";
              putText(src, stream.str(), Point(boundRect[i].tl().x + boundRect[i].width /3, boundRect[i].tl().y + boundRect[i].height /2), FONT_HERSHEY_DUPLEX, 1, Scalar(255,0,0), 2); 
            }       
            else if(area >= 150000 && area <= 170000)
            {
              stringstream stream;
              stream << "10 euros";
              putText(src, stream.str(), Point(boundRect[i].tl().x + boundRect[i].width /3, boundRect[i].tl().y + boundRect[i].height /2), FONT_HERSHEY_DUPLEX, 1, Scalar(255,0,0), 2); 
            }
            else if(area >= 170000 && area <= 190000)
            {
              stringstream stream;
              stream << "20 euros";
              putText(src, stream.str(), Point(boundRect[i].tl().x + boundRect[i].width /4, boundRect[i].tl().y + boundRect[i].height /4), FONT_HERSHEY_DUPLEX, 1, Scalar(255,0,0), 2); 
            }
            else if(area >= 190000 && area <= 220000)
            {
              stringstream stream;
              stream << "50 euros";
              putText(src, stream.str(), Point(boundRect[i].tl().x + boundRect[i].width /4, boundRect[i].tl().y + boundRect[i].height /4), FONT_HERSHEY_DUPLEX, 1, Scalar(255,0,0), 2); 
            }
            else if(area >= 220000 && area <= 250000)
            {
              stringstream stream;
              stream << "100 euros";
              putText(src, stream.str(), Point(boundRect[i].tl().x + boundRect[i].width /4, boundRect[i].tl().y + boundRect[i].height /4), FONT_HERSHEY_DUPLEX, 1, Scalar(255,0,0), 2); 
            }
            else if(area >= 250000 && area <= 270000)
            {
              stringstream stream;
              stream << "200 euros";
              putText(src, stream.str(), Point(boundRect[i].tl().x + boundRect[i].width /4, boundRect[i].tl().y + boundRect[i].height /4), FONT_HERSHEY_DUPLEX, 1, Scalar(255,0,0), 2); 
            }
            else if(area >= 270000 && area <= 290000)
            {
              stringstream stream;
              stream << "500 euros";
              putText(src, stream.str(), Point(boundRect[i].tl().x + boundRect[i].width /2, boundRect[i].tl().y + boundRect[i].height /2), FONT_HERSHEY_DUPLEX, 1, Scalar(255,0,0), 2); 
            }
            // stringstream stream;
            // stream << area;
            // putText(src, stream.str(), Point(boundRect[i].tl().x + boundRect[i].width /4, boundRect[i].tl().y + boundRect[i].height /4), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2); 
            
        }
    }
    stringstream stream;
    stream << "Existem " << numberOfNotes << " notas.";
    putText(src, stream.str(), Point(10,60), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,0), 2);
}

/** @function main */
int main(int argc, char** argv)
{
 
  //Size size (800,600);
  fileName = argv[1];

  /// Read the image
  src = imread(fileName, IMREAD_COLOR);
  //resize(src, src, size);
  /// Convert it to gray
  cvtColor( src, src_gray, CV_BGR2GRAY );
  /// Reduce the noise so we avoid false circle detection
  GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );
  
  notes(0, 0);
  coins(0, 0);  

  waitKey(0);
  return 0;
}
