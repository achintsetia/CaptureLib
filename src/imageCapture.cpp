/*
 * Written By: Achint Setia
 * Date: 04052011
 */

#include "capture.h"

ImageCapture::ImageCapture( char* fileName) : Capture( fileName)
{
  FRAME_LODED = false;

  m_frame = cvLoadImage( fileName, CV_LOAD_IMAGE_COLOR);

  if ( m_frame == NULL)
    setStatus( "Can't Load Image : %s.\n", fileName);
  else
  {
    FRAME_LODED = true;
    strcpy( m_fileName, fileName);
  }
}

ImageCapture::~ImageCapture()
{
  if (FRAME_LODED == true)
    cvReleaseImage( &m_frame);
}

unsigned char* ImageCapture::getNextFrame()
{
  if ( FRAME_LODED == true)
  {
    setStatus("Showing File : %s.\n", m_fileName);
    return (unsigned char*) m_frame->imageData;
  }
  else
  {
    setStatus("Frame Not Loaded.\n");
    return NULL;
  }
}

int ImageCapture::getWidth()
{
  if ( FRAME_LODED == true)
    return (int) m_frame->width;
  else
    return -1;
}

int ImageCapture::getHeight()
{
  if ( FRAME_LODED == true)
    return (int) m_frame->height;
  else
    return -1;
}

int ImageCapture::getWidthStep()
{
  if ( FRAME_LODED == true)
    return (int) m_frame->widthStep;
  else
    return -1;
}

int ImageCapture::saveFrame( char* fileName)
{
  if ( FRAME_LODED == true)
  {
    cvSaveImage( fileName, m_frame);
    setStatus("Saving Frame : %s.\n", fileName);
    return 1;
  }
  else
  {
    setStatus("Frame Not Loaded. Can't Save.\n");
    return -1;
  }
}

const char* ImageCapture::getStatus()
{
  return m_status;
}

void ImageCapture::setStatus( const char* format, ...)
{
  /* have a variable argument list */
  va_list vAList;

  /* initialise the argument list */
  va_start( vAList, format);

  vsprintf( m_status, format, vAList);
  va_end( vAList);
}
