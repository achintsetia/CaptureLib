/*
 * Written By: Achint Setia
 * Project Name: Image Lab
 * Organization Name: Setac Systems
 * Date: 04052011
*/

#ifndef _CAPTURE_H_
#define _CAPTURE_H_

#include <iostream>

//C Libraries includes
#include <string.h>
#include <pthread.h>

//FFMPEG includes
#include <ffmpeg/avformat.h>
#include <ffmpeg/avcodec.h>

//OpenCV library includes
#include <cv.h>
#include <highgui.h>


using namespace std;

class Capture
{
  public :
    Capture( char* fileName);
    ~Capture();
    virtual unsigned char*  getNextFrame();
    virtual int             getWidth();
    virtual int             getHeight();
    virtual int             getWidthStep();
    virtual int             saveFrame( char* fileName);
    virtual const char*     getStatus();
    virtual void            setStatus( const char* format, ...);
    const char*             getType();

  private :
    char    m_type[32];
};

class VideoCapture : public Capture
{
  public :
    VideoCapture( char* fileName);
    ~VideoCapture();
    unsigned char*  getNextFrame();
    int             getWidth();
    int             getHeight();
    int             getWidthStep();
    int             saveFrame( char* fileName);
    const char*     getStatus();
    void            setStatus( const char* format, ...);

    //Class Specific VideoCapture Functions
    int             seek( float percentage);
    int             getDuration();
    void            play();
    void            pause();
    void            stop();
    float           getFPS();
    int             saveFrame();
    float           getSeekPosition();

  private :
    void reset();

    AVFormatContext *m_FormatCtx;
    AVCodecContext  *m_CodecCtx;
    AVCodec         *m_Codec;
    AVFrame         *m_Frame;
    AVFrame         *m_FrameRGB;
    AVPacket        m_packet;
    int             m_videoStream;
    int             m_frameSize;
    uint8_t         *m_buffer;
    double          m_currTimeStamp;
    double          m_lastTimeStamp;
    double          m_timeBase;
    int64_t         m_durationUSec;

    int             m_playState; //0 if paused and 1 if playing
    int             m_resetFlag; //1 if stopped
    int             m_index;
    char            m_status[256];
    char            m_fileName[256];

    bool            READY_TO_PLAY;
    bool            ERROR_FOUND;
    bool            SEEK_CAPABILITY;
};

class ImageCapture : public Capture
{
  public :
    ImageCapture( char* fileName);
    ~ImageCapture();
    unsigned char*  getNextFrame();
    int             getWidth();
    int             getHeight();
    int             getWidthStep();
    int             saveFrame( char* fileName);
    const char*     getStatus();
    void            setStatus( const char* format, ...);

  private :
    int             m_width;
    int             m_height;
    int             m_widthStep;
    char            m_fileName[256];
    char            m_status[256];
    IplImage*       m_frame;
    bool            FRAME_LODED;
};

enum CamType
{
  ANALOG,
  UNKNOWN
};

class CameraCapture : public Capture
{
  public :
    CameraCapture( char* fileName);
    ~CameraCapture();
    unsigned char*  getNextFrame();
    int             getWidth();
    int             getHeight();
    int             getWidthStep();
    int             saveFrame( char* fileName);
    const char*     getStatus();
    void            setStatus( const char* format, ...);

    //Camera class functions
    int             openCamera();
    int             closeCamera();
    void            startCapture();
    void            stopCapture();

  private :
    char            m_status[256];
    int             m_width;
    int             m_height;
    int             m_widthStep;
    CamType         m_camType;
    IplImage*       m_frame;
    bool            m_captureFlag;
    bool            camOpenFlag;
};

#endif // _CAPTURE_H_
