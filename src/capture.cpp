/*
 * Written By: Achint Setia
 * Date: 04052011
 */

#include "capture.h"

Capture::Capture( char* fileName)
{
  if( !strncmp( fileName, "/dev/", 5))
  {
    strcpy( m_type, "CAMERA\n");
  }
  else
  {
    char* suffix = strrchr( fileName, '.');
    if ( suffix) suffix += 1;

    if ((suffix != NULL            )  &&
        (!strcasecmp( suffix, "BMP")  ||
        !strcasecmp( suffix, "DIB")   ||
        !strcasecmp( suffix, "JPEG")  ||
        !strcasecmp( suffix, "JPG")   ||
        !strcasecmp( suffix, "PNG")   ||
        !strcasecmp( suffix, "PPM")   ||
        !strcasecmp( suffix, "PGM")   ||
        !strcasecmp( suffix, "PBM")   ||
        !strcasecmp( suffix, "JP2")   ||
        !strcasecmp( suffix, "TIFF")  ||
        !strcasecmp( suffix, "TIF")))
    {
      strcpy( m_type, "IMAGE\n");
    }
    else if( suffix != NULL           &&
        (!strcasecmp( suffix, "AVI")  ||
        !strcasecmp( suffix, "MP4")   ||
        !strcasecmp( suffix, "MP2")   ||
        !strcasecmp( suffix, "MPEG")  ||
        !strcasecmp( suffix, "MPG")   ||
        !strcasecmp( suffix, "3GP")   ||
        !strcasecmp( suffix, "3GP2")  ||
        !strcasecmp( suffix, "M4V")   ||
        !strcasecmp( suffix, "OGG")   ||
        !strcasecmp( suffix, "MOV")))
    {
      strcpy( m_type, "VIDEO\n");
    }
    else
    {
      strcpy( m_type, "UNKNOWN_TYPE\n");
    }
  }
}

Capture::~Capture()
{

}

const char* Capture::getType()
{
  return m_type;
}

unsigned char* Capture::getNextFrame()
{
  fprintf( stderr, "getNextFrame :: This object got assigned to an interface.");
  return NULL;
}
int Capture::getWidth()
{
  fprintf( stderr, "getWidth :: This object got assigned to an interface.");
  return 0;
}
int Capture::getHeight()
{
  fprintf( stderr, "getHeight :: This object got assigned to an interface.");
  return 0;
}
int Capture::getWidthStep()
{
  fprintf( stderr, "getWidthStep :: This object got assigned to an interface.");
  return 0;
}
int Capture::saveFrame( char* fileName)
{
  fprintf( stderr, "saveFrame :: This object got assigned to an interface.");
  return 0;
}
const char* Capture::getStatus()
{
  fprintf( stderr, "getStatus :: This object got assigned to an interface.");
  return NULL;
}
void Capture::setStatus( const char* format, ...)
{
  fprintf( stderr, "setStatus :: This object got assigned to an interface.");
}
