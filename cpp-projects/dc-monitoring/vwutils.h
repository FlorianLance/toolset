
#pragma once

// std
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <inttypes.h>
}

class VideoWriter{

public:

    VideoWriter(const std::string &filename, float fps, int fwidth, int fheight, bool iscolor=true, const std::string &encoderName = "libopenh264", std::int64_t bitrate = 5000000);
	~VideoWriter();
	
    auto write(uint8_t* data) -> bool; //assumed data is given in opencv bgr format
	//there is no need to release the object. It is done automatically

private:

    // setup functions
    auto setup_frame() -> bool;
    auto setup_encoder(const std::string &encoderName, std::int64_t bitrate) -> bool;
    auto add_video_stream() -> bool;
    auto get_ready_to_write() -> bool;

    bool m_initialized = false;
    std::string m_filepath = "";
    float m_fps = 0.f;
    int m_frameWidth = 0;
    int m_frameHeight = 0;
    bool m_iscolor = false;

    // av data
    AVIOContext*        m_ioContext       = nullptr;
    AVCodecContext*     m_codecContext    = nullptr;
    AVFormatContext*    m_formatContext   = nullptr;
    AVStream*           m_stream          = nullptr;
    AVPacket*           m_packet          = nullptr;
    const AVCodec*      m_encoder         = nullptr;
    // # frames
    AVFrame* m_frame                      = nullptr;
    AVFrame* m_bgrFrame                   = nullptr;
    // # sws
    SwsContext* m_converter               = nullptr;


    uint64_t pts=1;
    uint8_t* gray_rgb_data;
	char ebuf[255];
};


//class VideoEncoder
//{
//  private:

//  // output file name
//  std::string     outputFilename;
//  // output format.
//  const AVOutputFormat  *pOutFormat;
//  // format context
//  AVFormatContext *pFormatContext;
//  // video stream context
//  AVStream * pVideoStream;
//  // audio streams context
//  AVStream * pAudioStream;
//  // convert context context
//  struct SwsContext *pImgConvertCtx;
//  // encode buffer and size
//  uint8_t * pVideoEncodeBuffer;
//  int nSizeVideoEncodeBuffer;

//  // audio buffer and size
//  uint8_t * pAudioEncodeBuffer;
//  int nSizeAudioEncodeBuffer;


//  // count of sample
//  int audioInputSampleSize;
//  // current picture
//  AVFrame *pCurrentPicture;

//  // audio buffer. Save rest samples in audioBuffer from previous audio frame.
//  char* audioBuffer;
//  int   nAudioBufferSize;
//  int   nAudioBufferSizeCurrent;

//  public:

//  VideoEncoder()
//  {
//    pOutFormat = NULL;
//    pFormatContext = NULL;
//    pVideoStream = NULL;
//    pImgConvertCtx = NULL;
//    pCurrentPicture = NULL;
//    pVideoEncodeBuffer = NULL;
//    nSizeVideoEncodeBuffer = 0;
//    pAudioEncodeBuffer = NULL;
//    nSizeAudioEncodeBuffer = 0;
//    nAudioBufferSize = 1024 * 1024 * 4;
//    audioBuffer      = new char[nAudioBufferSize];
//    nAudioBufferSizeCurrent = 0;
//  }

//  virtual ~VideoEncoder()
//  {
//    Finish();
//  }

//  // init output file
//  bool InitFile(std::string& inputFile, std::string& container);
//  // Add video and audio data
//  bool AddFrame(AVFrame* frame, const char* soundBuffer, int soundBufferSize);
//  // end of output
//  bool Finish();

//  private:

//  // Add video stream
//  AVStream *AddVideoStream(AVFormatContext *pContext, AVCodecID codec_id);
//  // Open Video Stream
//  bool OpenVideo(AVFormatContext *oc, AVStream *pStream);
//  // Allocate memory
//  AVFrame * CreateFFmpegPicture(AVPixelFormat pix_fmt, int nWidth, int nHeight);
//  // Close video stream
//  void CloseVideo(AVFormatContext *pContext, AVStream *pStream);
//  // Add audio stream
//  AVStream * AddAudioStream(AVFormatContext *pContext, AVCodecID codec_id);
//  // Open audio stream
//  bool OpenAudio(AVFormatContext *pContext, AVStream *pStream);
//  // close audio stream
//  void CloseAudio(AVFormatContext *pContext, AVStream *pStream);
//  // Add video frame
//  bool AddVideoFrame(AVFrame * frame, AVCodecContext *pVideoCodec);
//  // Add audio samples
//  bool AddAudioSample(AVFormatContext *pFormatContext,
//    AVStream *pStream, const char* soundBuffer, int soundBufferSize);
//  // Free resourses.
//  void Free();
//  bool NeedConvert();
//};
