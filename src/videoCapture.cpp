/*
 * Written By: Achint Setia
 * Project Name: Image Lab
 * Organization Name: Setac Systems
 * Date: 04052011
*/

#include <string.h>
#include "capture.h"

VideoCapture::VideoCapture( char* fileName) : Capture( fileName)
{
  READY_TO_PLAY = false;
  ERROR_FOUND = false;

  // Register all formats and codecs
  av_register_all();

  strcpy( m_fileName, fileName);

  // Open video file
  if( av_open_input_file( &m_FormatCtx, fileName, NULL, 0, NULL)!=0)
  {
    fprintf( stderr, "Couldn't Open File: %s\n", fileName);// Couldn't open file
    setStatus( "Couldn't Open File : %s\n", fileName);
    ERROR_FOUND = true;
    return;
  }

  // Retrieve stream information
  if( av_find_stream_info( m_FormatCtx) < 0)
  {
    fprintf( stderr, "Couldn't Find Stream Information\n");// Couldn't find stream information
    setStatus( "Couldn't Find Stream Information.\n");
    ERROR_FOUND = true;
    return;

  }

  // Dump information about file onto standard error
  dump_format(m_FormatCtx, 0, fileName, 0);

  // Find the first video stream
  m_videoStream=-1;
  for( int i=0; i < m_FormatCtx->nb_streams; i++)
  {
    if( m_FormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
    {
      m_videoStream=i;
      break;
    }
  }

  if( m_videoStream==-1)
  {
    fprintf( stderr, "Don't Find a Video Stream\n");// Didn't find a video stream
    setStatus( "Couldn't Find Video Stream.\n");
    ERROR_FOUND = true;
    return;
  }

  // Get a pointer to the codec context for the video stream
  m_CodecCtx = m_FormatCtx->streams[m_videoStream]->codec;

  // Find the decoder for the video stream
  m_Codec = avcodec_find_decoder( m_CodecCtx->codec_id);
  if( m_Codec == NULL)
  {
    fprintf( stderr, "Unsupported codec!\n"); // Codec not found codec
    setStatus( "Couldn't Find Supported Codec.\n");
    ERROR_FOUND = true;
    return;

  }

  // Open codec
  if( avcodec_open( m_CodecCtx, m_Codec)<0)
  {
    fprintf( stderr, "Couldn't Open Codec.\n"); // Could not open codec
    setStatus( "Couldn't Open Codec.\n");
    ERROR_FOUND = true;
    return;

  }

  // Allocate video frame
  m_Frame = avcodec_alloc_frame();

  // Allocate an AVFrame structure
  m_FrameRGB = avcodec_alloc_frame();

  if(m_FrameRGB==NULL)
  {
    fprintf( stderr, "Couldn't Allocate Frame.\n");
    setStatus( "Couldn't Allocate Frame.\n");
    ERROR_FOUND = true;
    return;
  }

  // Determine required m_buffer size and allocate m_buffer
  m_frameSize = avpicture_get_size(PIX_FMT_RGB24, m_CodecCtx->width,
      m_CodecCtx->height);
  m_buffer = (uint8_t *) av_malloc( m_frameSize * sizeof(uint8_t));

  // Assign appropriate parts of m_buffer to image planes in m_FrameRGB
  // Note that m_FrameRGB is an AVFrame, but AVFrame is a superset
  // of AVPicture
  avpicture_fill((AVPicture *)m_FrameRGB, m_buffer, PIX_FMT_RGB24,
      m_CodecCtx->width, m_CodecCtx->height);

  setStatus( "File Ready to Play : %s.\n", fileName);
  READY_TO_PLAY = true;
  ERROR_FOUND = false;

  if ( m_FormatCtx->duration != (int) AV_NOPTS_VALUE)
    m_durationUSec = m_FormatCtx->duration;
  else
    m_durationUSec = 0;

  m_index = 0;
  m_resetFlag = 1;
  m_playState = 0;
  m_currTimeStamp = 0.0;
  m_lastTimeStamp= 0.0;
  m_timeBase = av_q2d( m_FormatCtx->streams[m_videoStream]->time_base);

  //Getting m_lastTimeStampto calculate relative time position
  play();
  seek( 100.00);
  float i = 0.0;
  int count = 0;
  SEEK_CAPABILITY = true;

  while ((( getNextFrame() == NULL) || ( m_currTimeStamp == 0.0)) && ( count < 15))
  {
    seek(100.00 - i);
    i += 0.01;
    count++;
  }

  if (( count >= 15) && ( m_currTimeStamp == 0.0))
    SEEK_CAPABILITY = false;

  stop();
  m_lastTimeStamp= m_currTimeStamp;
  m_currTimeStamp = 0.0;
}

VideoCapture::~VideoCapture()
{
  READY_TO_PLAY = false;

  // Free the RGB image
  av_free(m_buffer);
  av_free(m_FrameRGB);

  // Free the YUV frame
  av_free(m_Frame);

  // Close the codec
  avcodec_close(m_CodecCtx);

  // Close the video file
  av_close_input_file(m_FormatCtx);
}

unsigned char* VideoCapture::getNextFrame()
{
  if ( m_resetFlag == 0)
  {
    if ( m_playState == 1)
    {
      int frameFinished = 0;
      bool finishedRead = true;
      while( av_read_frame( m_FormatCtx, &m_packet) >= 0)
      {
        finishedRead = false;
        // Is this a m_packet from the video stream?
        if( m_packet.stream_index == m_videoStream)
        {
          if ( m_packet.pts >= 0)
          {
            m_currTimeStamp = (double) m_packet.pts * m_packet.duration * m_timeBase;
          }
          else if (m_packet.dts >= 0)
          {
            m_currTimeStamp = (double) m_packet.dts * m_packet.duration * m_timeBase;
          }
          else
            m_currTimeStamp = 0.0;

          // Decode video frame
          avcodec_decode_video( m_CodecCtx, m_Frame, &frameFinished,
              m_packet.data, m_packet.size);

          // Did we get a video frame?
          if( frameFinished)
          {
            // Convert the image from its native format to RGB
            img_convert((AVPicture *)m_FrameRGB, PIX_FMT_BGR24,
                (AVPicture*)m_Frame, m_CodecCtx->pix_fmt, m_CodecCtx->width,
                m_CodecCtx->height);

            m_index++;

            av_free_packet(&m_packet);
            break;
          }
        }

        // Free the m_packet that was allocated by av_read_frame
        av_free_packet(&m_packet);
      }

      setStatus( "Playing Video, Seek Location :%03.2f %%.\n", getSeekPosition());
      if ( finishedRead == true)
        stop();
      return m_buffer;
    }
    else if ( m_playState == 0)
    {
      setStatus( "Paused Video, Seek Location :%03.2f %%.\n", getSeekPosition());
      return m_buffer;
    }
  }
  setStatus( "Set Video to Play State First.\n");
  return NULL;
}

int VideoCapture::getWidth()
{
  if ( READY_TO_PLAY == true)
    return m_CodecCtx->width;
  else
  {
    setStatus( "Wrong Width Returned as Previously Error Occured.\n");
    return -1;
  }
}

int VideoCapture::getHeight()
{
  if ( READY_TO_PLAY == true)
    return m_CodecCtx->height;
  else
  {
    setStatus( "Wrong Height Returned as Previously Error Occured.\n");
    return -1;
  }
}

int VideoCapture::getWidthStep()
{
  if ( READY_TO_PLAY == true)
    return 3*m_CodecCtx->width;
  else
  {
    setStatus( "Wrong WidthStep Returned as Previously Error Occured.\n");
    return -1;
  }
}

int VideoCapture::saveFrame( char* fileName)
{
  if ( READY_TO_PLAY == true)
  {
    FILE *pFile;
    int  y;

    int width = m_CodecCtx->width;
    int height= m_CodecCtx->height;

    // Open file
    pFile=fopen( fileName, "w");
    if(pFile==NULL)
      return 0;

    // Write header
    fprintf( pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++)
      fwrite( m_FrameRGB->data[0]+y*m_FrameRGB->linesize[0], 1, width*3, pFile);

    // Close file
    fclose(pFile);
    return 1;
  }
  else
  {
    setStatus( "Couldn't Save File %s as Previously Error Occured.\n", fileName);
    return 0;
  }
}

int VideoCapture::seek( float percentage)
{
  if (SEEK_CAPABILITY == true)
  {
    if ( READY_TO_PLAY == true)
    {
      if (( percentage <= 100) && ( percentage >= 0))
      {
        int64_t currPos = av_gettime();
        int64_t desiredPos = percentage * m_durationUSec / 100.0;

        desiredPos = av_rescale_q( desiredPos, AV_TIME_BASE_Q,
            m_FormatCtx->streams[m_videoStream]->time_base);

        if ( desiredPos >= currPos)
        {
          if( av_seek_frame( m_FormatCtx, m_videoStream, desiredPos,
                AVSEEK_FLAG_ANY) < 0)
          {
            fprintf(stderr, "%s: error while seeking\n", m_FormatCtx->filename);
            setStatus("Error While Seeking.\n");
            return 0;
          }
        }
        else
        {
          if( av_seek_frame( m_FormatCtx, m_videoStream, desiredPos,
                AVSEEK_FLAG_BACKWARD) < 0)
          {
            fprintf(stderr, "%s: error while seeking\n", m_FormatCtx->filename);
            setStatus("Error While Seeking.\n");
            return 0;
          }
        }
        return 1;
      }
      else
      {
        fprintf( stderr, "Can't Perform Seek(), Value out of range.\n");
        setStatus("Can't Perform Seek(), Value out of range\n");
        return 0;
      }
    }
    else
    {
      setStatus( "Can't Perform Seek(), as Previously Error Occured.\n");
      return -1;
    }
  }
  else
  {
    setStatus( "Can't Perform Seek With This Video.\n");
    return -1;
  }
}

int VideoCapture::getDuration()
{
  if ( READY_TO_PLAY == true)
  {
    if ( m_durationUSec > 0)
      return m_durationUSec / AV_TIME_BASE;
    else
    {
      setStatus( "No Duration Available.\n");
      return 0;
    }
  }
  else
  {
    setStatus( "Can't Return Duration as Previously Error.\n");
    return -1;
  }
}

void VideoCapture::play()
{
  if ( READY_TO_PLAY == true)
  {
    m_resetFlag = 0;
    m_playState = 1;
    setStatus( "Playing File : %s.\n", m_fileName);
  }
  else
  {
    setStatus("Can't Play File as Previously Occured Error.\n");
  }
}

void VideoCapture::pause()
{
  if ( READY_TO_PLAY == true)
  {
    m_playState = 0;
    setStatus( "Paused File : %s.\n", m_fileName);
  }
}

void VideoCapture::stop()
{
  if ( READY_TO_PLAY == true)
  {
    m_resetFlag = 1;
    m_playState = 0;
    reset();
    setStatus( "Stopped File : %s.\n", m_fileName);
  }
}

float VideoCapture::getFPS()
{
  if (READY_TO_PLAY == true)
  {
    AVStream* st = m_FormatCtx->streams[m_videoStream];
    if(st->r_frame_rate.den && st->r_frame_rate.num)
      return av_q2d( st->r_frame_rate);
    else
      return (1 / av_q2d(st->codec->time_base));
  }
  else
  {
    setStatus( "Can't Return FPS as Previously Error Occured.\n");
    return 0;
  }
}

int VideoCapture::saveFrame()
{
  char szFilename[32];

  // Open file
  sprintf(szFilename, "frame%d.ppm", m_index);
  return saveFrame( szFilename);
}

void VideoCapture::reset()
{
  int count = 0;
  if ( SEEK_CAPABILITY == true)
  {
    while ( !seek( 0.0) && ( count < 5))
    {
      count++;
    }
  }
}

const char* VideoCapture::getStatus()
{
  return m_status;
}

void VideoCapture::setStatus( const char* format, ...)
{
  /* have a variable argument list */
  va_list vAList;

  /* initialise the argument list */
  va_start( vAList, format);

  vsprintf( m_status, format, vAList);
  va_end( vAList);
}

float VideoCapture::getSeekPosition()
{
  if ( SEEK_CAPABILITY == true)
  {
    if ( READY_TO_PLAY == true)
    {
      return m_currTimeStamp / m_lastTimeStamp* 100.00;
    }
    else
    {
      setStatus( "Cant Return Seek Position as Previously Error Occured.\n");
      return -1;
    }
  }
  else
  {
    setStatus( "Can't Perform Seek With This Video.\n");
    return -1;
  }
}
