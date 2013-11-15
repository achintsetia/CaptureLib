/*
 * Written By: Achint Setia
 * Date: 04052011
 */

#include "capture.h"

pthread_t g_thread;
pthread_attr_t g_attr;
pthread_mutex_t g_frameLock;
volatile bool g_captureFlag;
IplImage* g_frame = NULL;
CvCapture* g_capture;

void *captureThreadFunc(void *t);

CameraCapture::CameraCapture( char* fileName) : Capture( fileName)
{
  m_width = 0;
  m_height = 0;
  m_widthStep = 0;
  m_frame = NULL;
  g_captureFlag = false;
  m_captureFlag = false;
  camOpenFlag = false;
  setStatus("");

  if( !strncmp( fileName, "/dev/", 5))
  {
    m_camType = ANALOG;
  }
  else
  {
    m_camType = UNKNOWN;
  }
}

CameraCapture::~CameraCapture()
{
  cvReleaseCapture( &g_capture);
  m_width = 0;
  m_height = 0;
  m_widthStep = 0;
  g_captureFlag = false;
  m_captureFlag = false;
  camOpenFlag     = false;
  if ( m_frame != NULL)
    cvReleaseImage( &m_frame);

  if( g_frame != NULL)
    cvReleaseImage( &g_frame);
}

int CameraCapture :: openCamera()
{
  g_capture = cvCaptureFromCAM(-1);
  if( !g_capture)
  {
    setStatus("Not able to open camera:%s\n", this->getType());
    return -1;
  }
  else
  {
    camOpenFlag = true;
    return 0;
  }
}

int CameraCapture :: closeCamera()
{
  camOpenFlag = false;
  g_captureFlag = false;
  return 0;
}

void CameraCapture :: startCapture()
{
  if( m_camType == UNKNOWN)
  {
    setStatus("Could not start capturing, UNKNOWN Camera\n");
    m_captureFlag = false;
    g_captureFlag = false;
    return;
  }

  m_captureFlag = true;
  g_captureFlag = true;


  if( !camOpenFlag)
  {
    setStatus("Open the camera first and then start capture\n");
    return;
  }

  int errNo = 0;
  long t = 0;

  /* Initialize and set thread detached attribute */
  pthread_attr_init( &g_attr);
  pthread_attr_setdetachstate( &g_attr, PTHREAD_CREATE_JOINABLE);
  pthread_mutex_init( &g_frameLock, NULL);

  errNo = pthread_create( &g_thread, &g_attr, captureThreadFunc, (void *)t);

  if( errNo)
  {
    setStatus( "Could not start capturing, ERROR is :%d\n", errNo);
  }


  while ( !g_frame)
  {
    usleep(10000);
  }

  m_width     = g_frame->width;
  m_height    = g_frame->height;
  m_widthStep = g_frame->widthStep;

  setStatus( "Capturing started with width :%d, height :%d, widthstep :%d\n",
              m_width, m_height, m_widthStep);
}

void CameraCapture :: stopCapture()
{
  m_captureFlag = false;
  g_captureFlag = false;

  if( m_camType != UNKNOWN)
  {
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy( &g_attr);

    void* status;
    int errNo = pthread_join( g_thread, &status);
    if( errNo)
    {
      setStatus( "ERROR; return code from pthread_create() is :%d\n", errNo);
    }
    pthread_mutex_destroy( &g_frameLock);
  }
}

unsigned char* CameraCapture::getNextFrame()
{
  if( m_frame == NULL)
  {
    pthread_mutex_lock (&g_frameLock);
    m_frame = cvCloneImage( g_frame);
    pthread_mutex_unlock (&g_frameLock);
  }
  else
  {
    pthread_mutex_lock (&g_frameLock);
    m_widthStep = g_frame->widthStep;
    m_frame->widthStep = m_widthStep;
    cvCopy( g_frame, m_frame, NULL);
    pthread_mutex_unlock (&g_frameLock);
  }
  return (unsigned char*) m_frame->imageData;
}

int CameraCapture::getWidth()
{
  if( m_captureFlag)
    return m_width;
  else
  {
    return 0;
    setStatus( "Width unknown untill capturing starts\n");
  }
}

int CameraCapture::getHeight()
{
  if( m_captureFlag)
    return m_height;
  else
  {
    return 0;
    setStatus( "Height unknown untill capturing starts\n");
  }
}

int CameraCapture::getWidthStep()
{
  if( m_captureFlag)
    return m_widthStep;
  else
  {
    return 0;
    setStatus( "WidthStep unknown untill capturing starts\n");
  }
}

int CameraCapture::saveFrame( char* fileName)
{
  cvSaveImage( fileName, m_frame);
  setStatus("Image Saved: %s", fileName);
  return 0;
}

const char* CameraCapture::getStatus()
{
  return m_status;
}

void CameraCapture::setStatus( const char* format, ...)
{
  /* have a variable argument list */
  va_list vAList;

  /* initialise the argument list */
  va_start( vAList, format);

  vsprintf( m_status, format, vAList);
  va_end( vAList);
}

void *captureThreadFunc(void *t)
{
  long tid = (long) t;
  cout << "Thread " << tid << " starting..." << endl;

  while ( g_captureFlag == true)
  {
    pthread_mutex_lock ( &g_frameLock);
    //Capture frame from camera
    g_frame = cvQueryFrame( g_capture);
    pthread_mutex_unlock ( &g_frameLock);
    usleep(1000);
  }

  pthread_exit((void*) t);
}
