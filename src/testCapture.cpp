#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "capture.h"

using namespace std;

Capture* capture = NULL;
int seekVal;

void onSeekChanged( int a)
{
  VideoCapture* temp = (VideoCapture*) capture;
  temp->seek( seekVal);
}

int main( int argc, char** argv)
{
  if ( argc == 2 )
  {
    cout << "Opening " << argv[1] << endl;
    capture = new Capture( argv[1]);

    char type[32];
    strcpy( type, capture->getType());
    cout << "Type :" << type << endl;

    float fps = 100;

    cvNamedWindow( "Video", 1);

    if ( !strncmp( type, "VIDEO", 5))
    {
      delete capture;
      seekVal = 0;
      capture = new VideoCapture( argv[1]);

      VideoCapture* temp = (VideoCapture*) capture;

      cout << "Width : " <<  temp->getWidth() << endl;
      cout << "Height : " <<  temp->getHeight() << endl;
      cout << "Duration : " << temp->getDuration() << endl;
      cout << "FPS : " << temp->getFPS() << endl;

      //fps = temp->getFPS();
      fps = 25;

      cvCreateTrackbar("Seek", "Video", &seekVal, 100, onSeekChanged);
    }
    else if ( !strncmp( type, "IMAGE", 5))
    {
      delete capture;
      capture = new ImageCapture( argv[1]);
      cout << "Width : " <<  capture->getWidth() << endl;
      cout << "Height : " <<  capture->getHeight() << endl;
    }
    else if ( !strncmp( type, "CAMERA", 6))
    {
      delete capture;
      capture = new CameraCapture( argv[1]);

      CameraCapture* temp = (CameraCapture*) capture;
      if( !temp->openCamera())
        cout << capture->getStatus();
      else
        cout << capture->getStatus();

    }
    else
    {
      fprintf( stderr, "Couldn't Not Detect Type of Source");
      fprintf( stderr, " or No Handeller Available\n");
      return 1;
    }

    if ( !(strncmp( capture->getType(), "VIDEO", 5)))
    {
      VideoCapture* temp = (VideoCapture*) capture;
      temp->play();
      cout << capture->getStatus();
    }
    else if( !strncmp( capture->getType(), "CAMERA", 6))
    {
      CameraCapture* temp = (CameraCapture*) capture;
      temp->startCapture();
      cout << capture->getStatus();
    }

    IplImage* image = NULL;

    if (( capture->getWidth() > 0) && (capture->getHeight() > 0))
    {
      image = cvCreateImageHeader( cvSize(capture->getWidth(),
              capture->getHeight()), IPL_DEPTH_8U, 3);
    }
    else
    {
      cout << capture->getStatus();
      return 0;
    }

    static char key;
    for( int i = 0; ;)
    {
      image->imageData = (char*) capture->getNextFrame();
      if ( image->imageData == NULL)
      {
        break;
      }
      cvShowImage( "Video", image);

      key = cvWaitKey((int) 1000.0 / fps);
      if (( key == '\n') && ( i == 0))
      {
        i = 1;
        if ( !(strncmp( capture->getType(), "VIDEO", 5)))
        {
          VideoCapture* temp = (VideoCapture*) capture;
          temp->pause();
        }
      }
      else if (( key == '\n') && ( i == 1))
      {
        i = 0;
        if ( !(strncmp( capture->getType(), "VIDEO", 5)))
        {
          VideoCapture* temp = (VideoCapture*) capture;
          temp->play();
        }
      }
      else if( key == 27)
      {
        break;
      }
      cout << "Type : " << capture->getType() << " " << capture->getStatus();
    }

    if( !strncmp( capture->getType(), "CAMERA", 6))
    {
      CameraCapture* temp = (CameraCapture*) capture;
      temp->stopCapture();
      temp->closeCamera();
    }
    delete capture;
  }
  else
  {
    cout << "Usage :" << argv[0] << " <video/image filename or camera device>" << endl;
  }
  return 0;
}
