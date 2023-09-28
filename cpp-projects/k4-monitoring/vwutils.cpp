#include "vwutils.h"

// std
#include <format>

VideoWriter::VideoWriter(const std::string &filename, float fps, int fwidth, int fheight, bool iscolor, const std::string &encoderName, std::int64_t bitrate){

    int error = avio_open(&m_ioContext, filename.c_str(), AVIO_FLAG_WRITE);
	if (error<0){
		av_strerror(error, ebuf, sizeof(ebuf));
        std::cerr << std::format("Could not open output file: {}", ebuf) << "\n";
        return;
	}

    m_filepath      = filename;
    m_frameWidth    = fwidth;
    m_frameHeight   = fheight;
    m_iscolor       = iscolor;
    m_fps           = fps;

    if(!setup_frame()){
        return;
    }
    if(!setup_encoder(encoderName, bitrate)){
        return;
    }
    if(!add_video_stream()){
        return;
    }
    if(!get_ready_to_write()){
        return;
    }

    m_initialized = true;
}

auto VideoWriter::get_ready_to_write() -> bool{

    std::cout << "###################### get_ready_to_write\n";

    std::cout << "avcodec_open2:\n";
    if (avcodec_open2(m_codecContext, NULL, NULL) < 0){
		std::cerr << "Could not open codec context\n";
        return false;
	}

    std::cout << "avcodec_parameters_from_context:\n";
    if (avcodec_parameters_from_context(m_stream->codecpar, m_codecContext)<0){
		std::cerr << "Could not copy params\n";
        return false;
	}

    std::cout << "av_packet_alloc:\n";
    m_packet = av_packet_alloc();
    std::cout << "avformat_write_header:\n";
    avformat_write_header(m_formatContext, NULL);

    return true;
}

auto VideoWriter::setup_frame() -> bool{

    std::cout << "###################### setup_frame\n";

    // generate YUV420P frame
    m_frame=av_frame_alloc();
    if (!m_frame){
		std::cerr << "Could not allocate frame. What is this even? smh\n";
        return false;
	}
    m_frame->format = AV_PIX_FMT_YUV420P;
    m_frame->width  = m_frameWidth;
    m_frame->height = m_frameHeight;
    if (av_frame_get_buffer(m_frame, 32)<0){
		std::cerr << "Yo frame so fat, the system ran out of memory!\n";
        return false;
	}

    // generate bgr24 frame
    m_bgrFrame = av_frame_alloc();
    if(!m_bgrFrame){
		std::cerr << "Could not allocate bgr\n";
        return false;
	}

    m_bgrFrame->format=AV_PIX_FMT_BGR24;
    m_bgrFrame->width=m_frameWidth;
    m_bgrFrame->height=m_frameHeight;
    if(av_frame_get_buffer(m_bgrFrame, 32)<0){
		std::cerr << "Could not allocate frame buffer for bgr\n";
        return false;
	}

    // init converter
    m_converter=sws_getContext(
        m_frameWidth,            // width of the source image
        m_frameHeight,           // height of the source image
        AV_PIX_FMT_BGR24,       // source image format
        m_frameWidth,            // width of the destination image
        m_frameHeight,           // height of the destination image
        AV_PIX_FMT_YUV420P,     // destination image format
        SWS_BILINEAR,           // flags specify which algorithm and options to use for rescaling
        NULL, NULL, NULL
    );

    if (!m_converter){
		std::cerr << "Cannot initialize converter\n";
        return false;
	}

    return true;
}

auto VideoWriter::setup_encoder(const std::string &encoderName, std::int64_t bitrate) -> bool{

    std::cout << "###################### setup_encoder\n";

	//setup encoder and its context and properties
    m_encoder = avcodec_find_encoder_by_name(encoderName.c_str()); // AV_CODEC_ID_H264
//    m_encoder = avcodec_find_encoder_by_name("libopenh264"); // AV_CODEC_ID_H264
//    m_encoder = avcodec_find_encoder_by_name("hevc_amf"); // AV_CODEC_ID_HEVC
//    m_encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
//    m_encoder = avcodec_find_encoder(AV_CODEC_ID_HEVC);

    // AV_CODEC_ID_H264
    //  support pixel format: AV_PIX_FMT_YUV420P

    if (m_encoder == nullptr){
        std::cerr  << "Could not load encoder\n";
        return false;
	}else{

        std::cout << "Successfully found encoder: " << ""
            "\n\tLong name: "     << m_encoder->long_name <<
            "\n\tEncoder name: "  << m_encoder->name <<
            "\n\tWrapper name: "  << m_encoder->wrapper_name <<
            "\n\tID: "            << m_encoder->id <<
            "\n\tType: "          << m_encoder->type <<
            "\n\tmax_lowres: "    << (int)m_encoder->max_lowres <<
            "\n\tcapabilities: "  << m_encoder->capabilities <<
            "\n";

        int id = 0;
        if(m_encoder->supported_samplerates != NULL){
            std::cout << "Sample rates:\n";
            while(m_encoder->supported_samplerates[id] != 0){
                std::cout << "[" << m_encoder->supported_samplerates[id] << "]";
                ++id;
            }
            std::cout << "\n";
        }else{
            std::cout << "Unknow samplerates.\n";
        }

        id = 0;
        if(m_encoder->supported_framerates != NULL){
            std::cout << "Framerates:\n";
            while(m_encoder->supported_framerates[id].den != 0 && m_encoder->supported_framerates[id].num != 0){
                std::cout << "[" << m_encoder->supported_framerates[id].den << "]";
                ++id;
            }
            std::cout << "\n";
        }else{
            std::cout << "Unknow framerates.\n";
        }

        id = 0;
        if(m_encoder->sample_fmts != NULL){
            std::cout << "Sample formats:\n";
            while(m_encoder->sample_fmts[id] != -1){
                std::cout << "[" << m_encoder->sample_fmts[id] << "]";
                ++id;
            }
            std::cout << "\n";
        }else{
            std::cout << "Unknow sample formats.\n";
        }

        id = 0;
        if(m_encoder->pix_fmts != NULL){
            std::cout << "Pixels formats:\n";
            while(m_encoder->pix_fmts[id] != -1){
                std::cout << "[" << m_encoder->pix_fmts[id] << "]";
                ++id;
            }
            std::cout << "\n";
        }else{
            std::cout << "Unknow pixels formats.\n";
        }


		/* H264 qsv supports: 0, 23, 160 ==> YUV420P, NV12, YUV440P12BE
		H265 qsv supports: 0, 23, 160, 28, 161 ==> all above and BGRA, AYUV64LE */


	}




    return true;
}

auto VideoWriter::add_video_stream() -> bool{

    std::cout << "###################### add_video_stream\n";

//    /**
//     * Allocate an AVFormatContext for an output format.
//     * avformat_free_context() can be used to free the context and
//     * everything allocated by the framework within it.
//     *
//     * @param ctx           pointee is set to the created format context,
//     *                      or to NULL in case of failure
//     * @param oformat       format to use for allocating the context, if NULL
//     *                      format_name and filename are used instead
//     * @param format_name   the name of output format to use for allocating the
//     *                      context, if NULL filename is used instead
//     * @param filename      the name of the filename to use for allocating the
//     *                      context, may be NULL
//     *
//     * @return  >= 0 in case of success, a negative AVERROR code in case of
//     *          failure
//     */
//    int avformat_alloc_output_context2(AVFormatContext **ctx, const AVOutputFormat *oformat,
//                                       const char *format_name, const char *filename);

    m_formatContext = avformat_alloc_context();
    if (!m_formatContext){
        std::cerr << "Could not create output format context\n";
        return false;
	}
    m_formatContext->pb      = m_ioContext;
    m_formatContext->oformat = av_guess_format(NULL, m_filepath.c_str(), NULL);
    if(m_formatContext->oformat == NULL){
        std::cerr << "Could no find format from filepath: " << m_filepath << "\n";
        return false;
    }

    std::cout << "GUESS FORMAT:\n";
    std::cout << "name: " << m_formatContext->oformat->name << "\n";
    std::cout << "long_name: " << m_formatContext->oformat->long_name << "\n";
    std::cout << "mime_type: " << m_formatContext->oformat->mime_type << "\n";
    std::cout << "extensions: " << m_formatContext->oformat->extensions << "\n";
    std::cout << "audio_codec: " << m_formatContext->oformat->audio_codec << "\n";
    std::cout << "video_codec: " << m_formatContext->oformat->video_codec << "\n";
    std::cout << "subtitle_codec: " << m_formatContext->oformat->subtitle_codec << "\n";
    std::cout << "flags: " << m_formatContext->oformat->flags << "\n";


    //    * can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER,
    //    * AVFMT_GLOBALHEADER, AVFMT_NOTIMESTAMPS, AVFMT_VARIABLE_FPS,
    //    * AVFMT_NODIMENSIONS, AVFMT_NOSTREAMS, AVFMT_ALLOW_FLUSH,
    //    * AVFMT_TS_NONSTRICT, AVFMT_TS_NEGATIVE

//    const_cast<AVOutputFormat*>(m_formatContext->oformat)->video_codec = m_encoder->id;

    m_stream = avformat_new_stream(m_formatContext, NULL); //allocate new stream
    m_stream->time_base =AVRational{
        1,//100,                        // Numerator
        static_cast<int>(m_fps)//static_cast<int>(100*m_fps)  // Denominator
    };

    //       codecCtx->time_base.num = 1;
    //       codecCtx->time_base.den = fps;

    m_codecContext = avcodec_alloc_context3(m_encoder);
    if (!m_codecContext){
        std::cerr << "Could not allocate context for encoder\n";
        return false;
    }
    std::cout  << "CODEC: " << m_codecContext->codec->long_name << " " << m_codecContext->profile <<  "\n";

    m_codecContext->time_base   = m_stream->time_base;
    m_codecContext->width       = m_frameWidth;
    m_codecContext->height      = m_frameHeight;
    m_codecContext->gop_size    = 10; /* emit one intra frame every ten frames */ //12; //is this a typical value?
    m_codecContext->pix_fmt     = AV_PIX_FMT_YUV420P;
    m_codecContext->flags       |= AV_CODEC_FLAG_GLOBAL_HEADER;
//    m_codecContext->profile     = 0;
//    m_codecContext->bit_rate = m_frameWidth*m_frameHeight*m_fps*8 * 1;

    // test
    m_codecContext->skip_frame  = AVDISCARD_BIDIR;

    // allocate context for this encoder and set its properties
//    m_codecContext->bit_rate    = bitrate;

//    m_codecContext = m_frameWidth*m_frameHeight * stride * fps * 8;



//    int res = avformat_alloc_output_context2(&mOutputCtx, NULL, "mov", name);
//        if (res < 0) {
//            mError = "out of memory";
//            return;
//        }

//        AVCodec *codec = avcodec_find_encoder(CODEC_ID_QTRLE);
//        if (codec == NULL) {
//            mError = "codec not found";
//            return;
//        }

//    AVStream* stream = avformat_new_stream(mOutputCtx, codec);
//       if (stream == NULL) {
//           mError = "out of memory";
//           return;
//       }

//       AVCodecContext *codecCtx = stream->codec;

//       avcodec_get_context_defaults3(codecCtx, codec);

//       /* put sample parameters */
//       codecCtx->bit_rate = height * stride * fps * 8;
//       /* resolution must be a multiple of two */
//       assert(((width & 3) | (height & 3)) == 0);
//       codecCtx->width = width;
//       codecCtx->height = height;
//       /* frames per second */
//       codecCtx->time_base.num = 1;
//       codecCtx->time_base.den = fps;
//       codecCtx->gop_size = 10; /* emit one intra frame every ten frames */
//       codecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;

//       switch (fmt) {
//           case aggCanvas::Gray8_Blend:
//               codecCtx->pix_fmt = PIX_FMT_GRAY8;
//               break;
//           case aggCanvas::FF24_Blend:
//               codecCtx->pix_fmt = PIX_FMT_RGB24;
//               break;
//           case aggCanvas::FF_Blend:
//               codecCtx->pix_fmt = PIX_FMT_ARGB;
//               break;
//           default:
//               mError = "unknown pixel format";
//               return;
//       }

//       if (avcodec_open2(codecCtx, codec, NULL) < 0) {
//           mError = "could not open codec";
//           return;
//       }




    return true;
}

auto VideoWriter::write(uint8_t *data) -> bool{

    if(!m_initialized){
        std::cerr << "Video writer not initialized.\n";
        return false;
    }

    av_frame_make_writable(m_bgrFrame);
    av_frame_make_writable(m_frame);
    for (int jj = 0; jj < m_frameWidth*m_frameHeight * 3; ++jj){
//        m_bgrFrame->data[0][jj] = data[jj];
        m_bgrFrame->data[0][jj] = *data;
        data++;
	}

    if(sws_scale(
            m_converter, (const uint8_t * const *)
            m_bgrFrame->data,
            m_bgrFrame->linesize,
            0,
            m_frameHeight,
            m_frame->data,
            m_frame->linesize
        )!=m_frameHeight){
		std::cerr << "Error in scaling\n";
        return false;
	}

    m_frame->pts=pts;
    std::cout << "pts: " << pts << "\n";

    int ret=avcodec_send_frame(m_codecContext, m_frame);
	if (ret<0){
		std::cerr << "Error submitting frame for encoding\n";
        return false;
	}

    printf("Frame number %lld\n", m_codecContext->frame_num);

	while (ret>=0){

        ret = avcodec_receive_packet(m_codecContext, m_packet);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            std::cerr<< "Unknown error in encoding video frame\n";
            return false;
		}else if (ret<0){
			std::cerr << "Problem\n";
            return false;
		}
        std::cout << "av_packet_rescale_ts\n";
        av_packet_rescale_ts(m_packet, m_codecContext->time_base, m_stream->time_base);
        m_packet->stream_index = m_stream->index;
        m_packet->pts          = pts++;

        std::cout << "av_interleaved_write_frame " << m_packet->pts << " " << m_packet->duration << "\n";
        ret = av_interleaved_write_frame(m_formatContext, m_packet);
        if (ret<0){
            std::cerr << "Error writing video frame\n";
            return false;
		}
	}

	return true;
}


VideoWriter::~VideoWriter(){

        av_packet_free(&m_packet);

        avcodec_free_context(&m_codecContext);
        av_frame_free(&m_frame);
        av_frame_free(&m_bgrFrame);
        sws_freeContext(m_converter);
        avformat_free_context(m_formatContext);
        //avresample_free(&ost->avr);
}



//#define MAX_AUDIO_PACKET_SIZE (128 * 1024)


//bool VideoEncoder::InitFile(std::string& inputFile, std::string& container)
//{
//	bool res = false;

//	const char * filename = inputFile.c_str();
//	outputFilename = inputFile;

//	// Initialize libavcodec
//	av_register_all();

//	if (container == std::string("auto"))
//	{
//		// Create format
//		pOutFormat = av_guess_format(NULL, filename, NULL);
//	}
//	else
//	{
//		// use contanier
//		pOutFormat = av_guess_format(container.c_str(), NULL, NULL);
//	}

//	if (pOutFormat)
//	{
//		// allocate context
//		pFormatContext = avformat_alloc_context();
//		if (pFormatContext)
//		{
//			pFormatContext->oformat = pOutFormat;
//			memcpy(pFormatContext->filename, filename, min(strlen(filename),
//				sizeof(pFormatContext->filename)));

//			// Add video and audio stream
//			pVideoStream   = AddVideoStream(pFormatContext, pOutFormat->video_codec);
//			pAudioStream   = AddAudioStream(pFormatContext, pOutFormat->audio_codec);

//			// Set the output parameters (must be done even if no
//			// parameters).
//			{
//				av_dump_format(pFormatContext, 0, filename, 1);

//				// Open Video and Audio stream
//				res = false;
//				if (pVideoStream)
//				{
//					res = OpenVideo(pFormatContext, pVideoStream);
//				}

//				res = OpenAudio(pFormatContext, pAudioStream);

//				if (res && !(pOutFormat->flags & AVFMT_NOFILE))
//				{
//					if (avio_open(&pFormatContext->pb, filename, AVIO_FLAG_WRITE) < 0)
//					{
//						res = false;
//						printf("Cannot open file\n");
//					}
//				}

//				if (res)
//				{
//					// Write file header.
//					avformat_write_header(pFormatContext, NULL);
//					res = true;
//				}
//			}
//		}
//	}

//	if (!res)
//	{
//		Free();
//		printf("Cannot init file\n");
//	}

//	return res;
//}


//bool VideoEncoder::AddFrame(AVFrame* frame, const char* soundBuffer, int soundBufferSize)
//{
//	bool res = true;
//	int nOutputSize = 0;
//	AVCodecContext *pVideoCodec = NULL;

//	if (pVideoStream && frame && frame->data[0])
//	{
//		pVideoCodec = pVideoStream->codec;

//		if (NeedConvert())
//		{
//			// RGB to YUV420P.
//			if (!pImgConvertCtx)
//			{
//				pImgConvertCtx = sws_getContext(pVideoCodec->width, pVideoCodec->height,
//					PIX_FMT_RGB24,
//					pVideoCodec->width, pVideoCodec->height,
//					pVideoCodec->pix_fmt,
//					SWS_BICUBLIN, NULL, NULL, NULL);
//			}
//		}

//		// Allocate picture.
//		pCurrentPicture = CreateFFmpegPicture(pVideoCodec->pix_fmt, pVideoCodec->width,
//			pVideoCodec->height);

//		if (NeedConvert() && pImgConvertCtx)
//		{
//			// Convert RGB to YUV.
//			sws_scale(pImgConvertCtx, frame->data, frame->linesize,
//				0, pVideoCodec->height, pCurrentPicture->data, pCurrentPicture->linesize);
//		}

//		res = AddVideoFrame(pCurrentPicture, pVideoStream->codec);

//		// Free temp frame
//		av_free(pCurrentPicture->data[0]);
//		av_free(pCurrentPicture);
//		pCurrentPicture = NULL;
//	}

//	// Add sound
//	if (soundBuffer && soundBufferSize > 0)
//	{
//		res = AddAudioSample(pFormatContext, pAudioStream, soundBuffer, soundBufferSize);
//	}

//	return res;
//}


//bool VideoEncoder::Finish()
//{
//	bool res = true;
//	// Todo: Maybe you need write audio samples from audioBuffer to file before cloasing.

//	if (pFormatContext)
//	{
//		av_write_trailer(pFormatContext);
//		Free();
//	}

//	if (audioBuffer)
//	{
//		delete[] audioBuffer;
//		audioBuffer = NULL;
//	}

//	return res;
//}


//void VideoEncoder::Free()
//{
//	bool res = true;

//	if (pFormatContext)
//	{
//		// close video stream
//		if (pVideoStream)
//		{
//			CloseVideo(pFormatContext, pVideoStream);
//		}

//		// close audio stream.
//		if (pAudioStream)
//		{
//			CloseAudio(pFormatContext, pAudioStream);
//		}

//		// Free the streams.
//		for(size_t i = 0; i < pFormatContext->nb_streams; i++)
//		{
//			av_freep(&pFormatContext->streams[i]->codec);
//			av_freep(&pFormatContext->streams[i]);
//		}

//		if (!(pFormatContext->flags & AVFMT_NOFILE) && pFormatContext->pb)
//		{
//			avio_close(pFormatContext->pb);
//		}

//		// Free the stream.
//		av_free(pFormatContext);
//		pFormatContext = NULL;
//	}
//}

//AVFrame * VideoEncoder::CreateFFmpegPicture(AVPixelFormat pix_fmt, int nWidth, int nHeight)
//{
//	AVFrame *picture     = NULL;
//	uint8_t *picture_buf = NULL;
//	int size;

//	picture = avcodec_alloc_frame();
//	if ( !picture)
//	{
//		printf("Cannot create frame\n");
//		return NULL;
//	}

//	size = avpicture_get_size(pix_fmt, nWidth, nHeight);

//	picture_buf = (uint8_t *) av_malloc(size);

//	if (!picture_buf)
//	{
//		av_free(picture);
//		printf("Cannot allocate buffer\n");
//		return NULL;
//	}

//	avpicture_fill((AVPicture *)picture, picture_buf,
//		pix_fmt, nWidth, nHeight);

//	return picture;
//}


//bool VideoEncoder::OpenVideo(AVFormatContext *oc, AVStream *pStream)
//{
//	AVCodec *pCodec;
//	AVCodecContext *pContext;

//	pContext = pStream->codec;

//	// Find the video encoder.
//	pCodec = avcodec_find_encoder(pContext->codec_id);
//	if (!pCodec)
//	{
//		printf("Cannot found video codec\n");
//		return false;
//	}

//	// Open the codec.
//	if (avcodec_open2(pContext, pCodec, NULL) < 0)
//	{
//		printf("Cannot open video codec\n");
//		return false;
//	}

//	pVideoEncodeBuffer = NULL;
//	if (!(pFormatContext->oformat->flags & AVFMT_RAWPICTURE))
//	{
//		/* allocate output buffer */
//		nSizeVideoEncodeBuffer = 10000000;
//		pVideoEncodeBuffer = (uint8_t *)av_malloc(nSizeVideoEncodeBuffer);
//	}

//	return true;
//}


//void VideoEncoder::CloseVideo(AVFormatContext *pContext, AVStream *pStream)
//{
//	avcodec_close(pStream->codec);
//	if (pCurrentPicture)
//	{
//		if (pCurrentPicture->data)
//		{
//			av_free(pCurrentPicture->data[0]);
//			pCurrentPicture->data[0] = NULL;
//		}
//		av_free(pCurrentPicture);
//		pCurrentPicture = NULL;
//	}

//	if (pVideoEncodeBuffer)
//	{
//		av_free(pVideoEncodeBuffer);
//		pVideoEncodeBuffer = NULL;
//	}
//	nSizeVideoEncodeBuffer = 0;
//}


//bool VideoEncoder::NeedConvert()
//{
//	bool res = false;
//	if (pVideoStream && pVideoStream->codec)
//	{
//		res = (pVideoStream->codec->pix_fmt != PIX_FMT_RGB24);
//	}
//	return res;
//}


//AVStream *VideoEncoder::AddVideoStream(AVFormatContext *pContext, AVCodecID codec_id)
//{
//	AVCodecContext *pCodecCxt = NULL;
//	AVStream *st    = NULL;

//	st = avformat_new_stream(pContext, 0);
//	if (!st)
//	{
//		printf("Cannot add new vidoe stream\n");
//		return NULL;
//	}

//	pCodecCxt = st->codec;
//	pCodecCxt->codec_id = (AVCodecID)codec_id;
//	pCodecCxt->codec_type = AVMEDIA_TYPE_VIDEO;
//	pCodecCxt->frame_number = 0;
//	// Put sample parameters.
//	pCodecCxt->bit_rate = 2000000;
//	// Resolution must be a multiple of two.
//	pCodecCxt->width  = W_VIDEO;
//	pCodecCxt->height = H_VIDEO;
//	/* time base: this is the fundamental unit of time (in seconds) in terms
//	of which frame timestamps are represented. for fixed-fps content,
//	timebase should be 1/framerate and timestamp increments should be
//	identically 1. */
//	pCodecCxt->time_base.den = 25;
//	pCodecCxt->time_base.num = 1;
//	pCodecCxt->gop_size = 12; // emit one intra frame every twelve frames at most

//	pCodecCxt->pix_fmt = PIX_FMT_YUV420P;
//	if (pCodecCxt->codec_id == AV_CODEC_ID_MPEG2VIDEO)
//	{
//		// Just for testing, we also add B frames
//		pCodecCxt->max_b_frames = 2;
//	}
//	if (pCodecCxt->codec_id == AV_CODEC_ID_MPEG1VIDEO)
//	{
//		/* Needed to avoid using macroblocks in which some coeffs overflow.
//		This does not happen with normal video, it just happens here as
//		the motion of the chroma plane does not match the luma plane. */
//		pCodecCxt->mb_decision = 2;
//	}

//	// Some formats want stream headers to be separate.
//	if(pContext->oformat->flags & AVFMT_GLOBALHEADER)
//	{
//		pCodecCxt->flags |= CODEC_FLAG_GLOBAL_HEADER;
//	}

//	return st;
//}


//AVStream * VideoEncoder::AddAudioStream(AVFormatContext *pContext, AVCodecID codec_id)
//{
//	AVCodecContext *pCodecCxt = NULL;
//	AVStream *pStream = NULL;

//	// Try create stream.
//	pStream = avformat_new_stream(pContext, NULL);
//	if (!pStream)
//	{
//		printf("Cannot add new audio stream\n");
//		return NULL;
//	}

//	// Codec.
//	pCodecCxt = pStream->codec;
//	pCodecCxt->codec_id = codec_id;
//	pCodecCxt->codec_type = AVMEDIA_TYPE_AUDIO;
//	// Set format
//	pCodecCxt->bit_rate    = 128000;
//	pCodecCxt->sample_rate = 44100;
//	pCodecCxt->channels    = 1; // mono
//	pCodecCxt->sample_fmt  = AV_SAMPLE_FMT_S16;

//	nSizeAudioEncodeBuffer = 4 * MAX_AUDIO_PACKET_SIZE;
//	if (pAudioEncodeBuffer == NULL)
//	{
//		pAudioEncodeBuffer = (uint8_t * )av_malloc(nSizeAudioEncodeBuffer);
//	}

//	// Some formats want stream headers to be separate.
//	if(pContext->oformat->flags & AVFMT_GLOBALHEADER)
//	{
//		pCodecCxt->flags |= CODEC_FLAG_GLOBAL_HEADER;
//	}

//	return pStream;
//}


//bool VideoEncoder::OpenAudio(AVFormatContext *pContext, AVStream *pStream)
//{
//	AVCodecContext *pCodecCxt = NULL;
//	AVCodec *pCodec = NULL;
//	pCodecCxt = pStream->codec;

//	// Find the audio encoder.
//	pCodec = avcodec_find_encoder(pCodecCxt->codec_id);
//	if (!pCodec)
//	{
//		printf("Cannot open audio codec\n");
//		return false;
//	}

//	// Open it.
//	if (avcodec_open2(pCodecCxt, pCodec, NULL) < 0)
//	{
//		printf("Cannot open audio codec\n");
//		return false;
//	}

//	if (pCodecCxt->frame_size <= 1)
//	{
//		// Ugly hack for PCM codecs (will be removed ASAP with new PCM
//		// support to compute the input frame size in samples.
//		audioInputSampleSize = nSizeAudioEncodeBuffer / pCodecCxt->channels;
//		switch (pStream->codec->codec_id)
//		{
//		case AV_CODEC_ID_PCM_S16LE:
//		case AV_CODEC_ID_PCM_S16BE:
//		case AV_CODEC_ID_PCM_U16LE:
//		case AV_CODEC_ID_PCM_U16BE:
//			audioInputSampleSize >>= 1;
//			break;
//		default:
//			break;
//		}
//		pCodecCxt->frame_size = audioInputSampleSize;
//	}
//	else
//	{
//		audioInputSampleSize = pCodecCxt->frame_size;
//	}

//	return true;
//}


//void VideoEncoder::CloseAudio(AVFormatContext *pContext, AVStream *pStream)
//{
//	avcodec_close(pStream->codec);
//	if (pAudioEncodeBuffer)
//	{
//		av_free(pAudioEncodeBuffer);
//		pAudioEncodeBuffer = NULL;
//	}
//	nSizeAudioEncodeBuffer = 0;
//}


//bool VideoEncoder::AddVideoFrame(AVFrame * pOutputFrame, AVCodecContext *pVideoCodec)
//{
//	bool res = false;

//	if (pFormatContext->oformat->flags & AVFMT_RAWPICTURE)
//	{
//		// Raw video case. The API will change slightly in the near
//		// future for that.
//		AVPacket pkt;
//		av_init_packet(&pkt);

//		pkt.flags |= AV_PKT_FLAG_KEY;
//		pkt.stream_index = pVideoStream->index;
//		pkt.data= (uint8_t *) pOutputFrame;
//		pkt.size= sizeof(AVPicture);

//		res = av_interleaved_write_frame(pFormatContext, &pkt);
//		res = true;
//	}
//	else
//	{
//		// Encode
//		AVPacket packet;
//		packet.data = pVideoEncodeBuffer;
//		packet.size = nSizeVideoEncodeBuffer;

//		int nOutputSize = 0;
//		// Encode frame to packet.
//		int error = avcodec_encode_video2(pVideoCodec, &packet, pOutputFrame, &nOutputSize);
//		if (!error && nOutputSize > 0)
//		{
//			AVPacket pkt;
//			av_init_packet(&pkt);

//			if (pVideoCodec->coded_frame->pts != AV_NOPTS_VALUE)
//			{
//				pkt.pts = AV_NOPTS_VALUE;
//			}
//			else
//			{
//				int a = 0;
//			}

//			if(pVideoCodec->coded_frame->key_frame)
//			{
//				pkt.flags |= AV_PKT_FLAG_KEY;
//			}
//			pkt.stream_index = pVideoStream->index;
//			pkt.data         = pVideoEncodeBuffer;
//			pkt.size         = packet.size;

//			// Write packet with frame.
//			res = (av_interleaved_write_frame(pFormatContext, &pkt) == 0);
//		}
//		else
//		{
//			res = false;
//		}
//	}

//	return res;
//}


//bool VideoEncoder::AddAudioSample(AVFormatContext *pFormatContext, AVStream *pStream,
//								  const char* soundBuffer, int soundBufferSize)
//{
//	AVCodecContext *pCodecCxt = NULL;
//	bool res = true;

//	pCodecCxt       = pStream->codec;

//	// Size of packet on bytes.
//	// FORMAT s16
//	DWORD packSizeInSize = soundBufferSize;

//	int nCountSamples    = soundBufferSize / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

//	// Add current audio frame to previos.
//	memcpy(audioBuffer + nAudioBufferSizeCurrent, soundBuffer, soundBufferSize);
//	nAudioBufferSizeCurrent += soundBufferSize;

//	int nCurrentSize    = nAudioBufferSizeCurrent;
//	int nWriteSamples   = 0;
//	BYTE * pSoundBuffer = (BYTE *)audioBuffer;

//	while (nCurrentSize >= packSizeInSize)
//	{
//		AVFrame*  pAudioFrame = NULL;

//		pAudioFrame = avcodec_alloc_frame();

//		// Audio frame should be equal or smaller pCodecCxt->frame_size.
//		pAudioFrame->nb_samples = min(pCodecCxt->frame_size / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16), nCountSamples);
//		int nBufferShift        = nWriteSamples * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
//		int nCurrentBufferSize  = pAudioFrame->nb_samples * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

//		if (avcodec_fill_audio_frame(pAudioFrame, 1,
//			AV_SAMPLE_FMT_S16,
//			(uint8_t *)pSoundBuffer,
//			nCurrentBufferSize, 1) != 0)
//		{
//			res = false;
//			break;
//		}

//		AVPacket pkt;
//		av_init_packet(&pkt);

//		pkt.flags |= AV_PKT_FLAG_KEY;
//		pkt.stream_index = pStream->index;
//		pkt.data = pAudioEncodeBuffer;
//		pkt.size = nSizeAudioEncodeBuffer;

//		int nOutputSize = 0;
//		// Create encoded packet from audio frame.
//		int error   = avcodec_encode_audio2(pCodecCxt, &pkt, pAudioFrame, &nOutputSize);

//		if (!error && nOutputSize > 0)
//		{
//			if (pCodecCxt->coded_frame && pCodecCxt->coded_frame->pts != AV_NOPTS_VALUE)
//			{
//				pkt.pts = av_rescale_q(pCodecCxt->coded_frame->pts, pCodecCxt->time_base, pStream->time_base);
//			}

//			pkt.stream_index = pStream->index;

//			// Write the compressed frame in the media file.
//			if (av_interleaved_write_frame(pFormatContext, &pkt) != 0)
//			{
//				res = false;
//				break;
//			}
//		}

//		nCurrentSize  -= nCurrentBufferSize;
//		pSoundBuffer  += nCurrentBufferSize;

//		nWriteSamples += pAudioFrame->nb_samples;
//		avcodec_free_frame(&pAudioFrame);
//	}

//	// save excess
//	memcpy(audioBuffer, audioBuffer + nAudioBufferSizeCurrent - nCurrentSize, nCurrentSize);
//	nAudioBufferSizeCurrent = nCurrentSize;

//	return res;
//}

//FFmpeg 5.1.3
//Toggle main menu visibility

//    Main Page
//    Related Pages
//    Modules
//    Data Structures
//    Files
//    Examples

//    doc
//    examples

//transcoding.c
//Go to the documentation of this file.
///*
// * Copyright (c) 2010 Nicolas George
// * Copyright (c) 2011 Stefano Sabatini
// * Copyright (c) 2014 Andrey Utkin
// *
// * Permission is hereby granted, free of charge, to any person obtaining a copy
// * of this software and associated documentation files (the "Software"), to deal
// * in the Software without restriction, including without limitation the rights
// * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// * copies of the Software, and to permit persons to whom the Software is
// * furnished to do so, subject to the following conditions:
// *
// * The above copyright notice and this permission notice shall be included in
// * all copies or substantial portions of the Software.
// *
// * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// * THE SOFTWARE.
// */

///**
// * @file
// * API example for demuxing, decoding, filtering, encoding and muxing
// * @example transcoding.c
// */

//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavfilter/buffersink.h>
//#include <libavfilter/buffersrc.h>
//#include <libavutil/channel_layout.h>
//#include <libavutil/opt.h>
//#include <libavutil/pixdesc.h>

//static AVFormatContext *ifmt_ctx;
//static AVFormatContext *ofmt_ctx;
//typedef struct FilteringContext {
//    AVFilterContext *buffersink_ctx;
//    AVFilterContext *buffersrc_ctx;
//    AVFilterGraph *filter_graph;

//    AVPacket *enc_pkt;
//    AVFrame *filtered_frame;
//} FilteringContext;
//static FilteringContext *filter_ctx;

//typedef struct StreamContext {
//    AVCodecContext *dec_ctx;
//    AVCodecContext *enc_ctx;

//    AVFrame *dec_frame;
//} StreamContext;
//static StreamContext *stream_ctx;

//static int open_input_file(const char *filename)
//{
//    int ret;
//    unsigned int i;

//    ifmt_ctx = NULL;
//    if ((ret = avformat_open_input(&ifmt_ctx, filename, NULL, NULL)) < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
//        return ret;
//    }

//    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
//        return ret;
//    }

//    stream_ctx = av_calloc(ifmt_ctx->nb_streams, sizeof(*stream_ctx));
//    if (!stream_ctx)
//        return AVERROR(ENOMEM);

//    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
//        AVStream *stream = ifmt_ctx->streams[i];
//        const AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
//        AVCodecContext *codec_ctx;
//        if (!dec) {
//            av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
//            return AVERROR_DECODER_NOT_FOUND;
//        }
//        codec_ctx = avcodec_alloc_context3(dec);
//        if (!codec_ctx) {
//            av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
//            return AVERROR(ENOMEM);
//        }
//        ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
//                   "for stream #%u\n", i);
//            return ret;
//        }
//        /* Reencode video & audio and remux subtitles etc. */
//        if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
//                || codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
//            if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
//                codec_ctx->framerate = av_guess_frame_rate(ifmt_ctx, stream, NULL);
//            /* Open decoder */
//            ret = avcodec_open2(codec_ctx, dec, NULL);
//            if (ret < 0) {
//                av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
//                return ret;
//            }
//        }
//        stream_ctx[i].dec_ctx = codec_ctx;

//        stream_ctx[i].dec_frame = av_frame_alloc();
//        if (!stream_ctx[i].dec_frame)
//            return AVERROR(ENOMEM);
//    }

//    av_dump_format(ifmt_ctx, 0, filename, 0);
//    return 0;
//}

//static int open_output_file(const char *filename)
//{
//    AVStream *out_stream;
//    AVStream *in_stream;
//    AVCodecContext *dec_ctx, *enc_ctx;
//    const AVCodec *encoder;
//    int ret;
//    unsigned int i;

//    ofmt_ctx = NULL;
//    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, filename);
//    if (!ofmt_ctx) {
//        av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
//        return AVERROR_UNKNOWN;
//    }


//    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
//        out_stream = avformat_new_stream(ofmt_ctx, NULL);
//        if (!out_stream) {
//            av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
//            return AVERROR_UNKNOWN;
//        }

//        in_stream = ifmt_ctx->streams[i];
//        dec_ctx = stream_ctx[i].dec_ctx;

//        if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
//                || dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
//            /* in this example, we choose transcoding to same codec */
//            encoder = avcodec_find_encoder(dec_ctx->codec_id);
//            if (!encoder) {
//                av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
//                return AVERROR_INVALIDDATA;
//            }
//            enc_ctx = avcodec_alloc_context3(encoder);
//            if (!enc_ctx) {
//                av_log(NULL, AV_LOG_FATAL, "Failed to allocate the encoder context\n");
//                return AVERROR(ENOMEM);
//            }

//            /* In this example, we transcode to same properties (picture size,
//             * sample rate etc.). These properties can be changed for output
//             * streams easily using filters */
//            if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
//                enc_ctx->height = dec_ctx->height;
//                enc_ctx->width = dec_ctx->width;
//                enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
//                /* take first format from list of supported formats */
//                if (encoder->pix_fmts)
//                    enc_ctx->pix_fmt = encoder->pix_fmts[0];
//                else
//                    enc_ctx->pix_fmt = dec_ctx->pix_fmt;
//                /* video time_base can be set to whatever is handy and supported by encoder */
//                enc_ctx->time_base = av_inv_q(dec_ctx->framerate);
//            } else {
//                enc_ctx->sample_rate = dec_ctx->sample_rate;
//                ret = av_channel_layout_copy(&enc_ctx->ch_layout, &dec_ctx->ch_layout);
//                if (ret < 0)
//                    return ret;
//                /* take first format from list of supported formats */
//                enc_ctx->sample_fmt = encoder->sample_fmts[0];
//                enc_ctx->time_base = (AVRational){1, enc_ctx->sample_rate};
//            }

//            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
//                enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

//            /* Third parameter can be used to pass settings to encoder */
//            ret = avcodec_open2(enc_ctx, encoder, NULL);
//            if (ret < 0) {
//                av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
//                return ret;
//            }
//            ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
//            if (ret < 0) {
//                av_log(NULL, AV_LOG_ERROR, "Failed to copy encoder parameters to output stream #%u\n", i);
//                return ret;
//            }

//            out_stream->time_base = enc_ctx->time_base;
//            stream_ctx[i].enc_ctx = enc_ctx;
//        } else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
//            av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
//            return AVERROR_INVALIDDATA;
//        } else {
//            /* if this stream must be remuxed */
//            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
//            if (ret < 0) {
//                av_log(NULL, AV_LOG_ERROR, "Copying parameters for stream #%u failed\n", i);
//                return ret;
//            }
//            out_stream->time_base = in_stream->time_base;
//        }

//    }
//    av_dump_format(ofmt_ctx, 0, filename, 1);

//    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
//        ret = avio_open(&ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", filename);
//            return ret;
//        }
//    }

//    /* init muxer, write output file header */
//    ret = avformat_write_header(ofmt_ctx, NULL);
//    if (ret < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
//        return ret;
//    }

//    return 0;
//}

//static int init_filter(FilteringContext* fctx, AVCodecContext *dec_ctx,
//        AVCodecContext *enc_ctx, const char *filter_spec)
//{
//    char args[512];
//    int ret = 0;
//    const AVFilter *buffersrc = NULL;
//    const AVFilter *buffersink = NULL;
//    AVFilterContext *buffersrc_ctx = NULL;
//    AVFilterContext *buffersink_ctx = NULL;
//    AVFilterInOut *outputs = avfilter_inout_alloc();
//    AVFilterInOut *inputs  = avfilter_inout_alloc();
//    AVFilterGraph *filter_graph = avfilter_graph_alloc();

//    if (!outputs || !inputs || !filter_graph) {
//        ret = AVERROR(ENOMEM);
//        goto end;
//    }

//    if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
//        buffersrc = avfilter_get_by_name("buffer");
//        buffersink = avfilter_get_by_name("buffersink");
//        if (!buffersrc || !buffersink) {
//            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
//            ret = AVERROR_UNKNOWN;
//            goto end;
//        }

//        snprintf(args, sizeof(args),
//                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
//                dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
//                dec_ctx->time_base.num, dec_ctx->time_base.den,
//                dec_ctx->sample_aspect_ratio.num,
//                dec_ctx->sample_aspect_ratio.den);

//        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
//                args, NULL, filter_graph);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
//            goto end;
//        }

//        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
//                NULL, NULL, filter_graph);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
//            goto end;
//        }

//        ret = av_opt_set_bin(buffersink_ctx, "pix_fmts",
//                (uint8_t*)&enc_ctx->pix_fmt, sizeof(enc_ctx->pix_fmt),
//                AV_OPT_SEARCH_CHILDREN);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
//            goto end;
//        }
//    } else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
//        char buf[64];
//        buffersrc = avfilter_get_by_name("abuffer");
//        buffersink = avfilter_get_by_name("abuffersink");
//        if (!buffersrc || !buffersink) {
//            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
//            ret = AVERROR_UNKNOWN;
//            goto end;
//        }

//        if (dec_ctx->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC)
//            av_channel_layout_default(&dec_ctx->ch_layout, dec_ctx->ch_layout.nb_channels);
//        av_channel_layout_describe(&dec_ctx->ch_layout, buf, sizeof(buf));
//        snprintf(args, sizeof(args),
//                "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=%s",
//                dec_ctx->time_base.num, dec_ctx->time_base.den, dec_ctx->sample_rate,
//                av_get_sample_fmt_name(dec_ctx->sample_fmt),
//                buf);
//        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
//                args, NULL, filter_graph);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
//            goto end;
//        }

//        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
//                NULL, NULL, filter_graph);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
//            goto end;
//        }

//        ret = av_opt_set_bin(buffersink_ctx, "sample_fmts",
//                (uint8_t*)&enc_ctx->sample_fmt, sizeof(enc_ctx->sample_fmt),
//                AV_OPT_SEARCH_CHILDREN);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
//            goto end;
//        }

//        av_channel_layout_describe(&enc_ctx->ch_layout, buf, sizeof(buf));
//        ret = av_opt_set(buffersink_ctx, "ch_layouts",
//                         buf, AV_OPT_SEARCH_CHILDREN);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
//            goto end;
//        }

//        ret = av_opt_set_bin(buffersink_ctx, "sample_rates",
//                (uint8_t*)&enc_ctx->sample_rate, sizeof(enc_ctx->sample_rate),
//                AV_OPT_SEARCH_CHILDREN);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
//            goto end;
//        }
//    } else {
//        ret = AVERROR_UNKNOWN;
//        goto end;
//    }

//    /* Endpoints for the filter graph. */
//    outputs->name       = av_strdup("in");
//    outputs->filter_ctx = buffersrc_ctx;
//    outputs->pad_idx    = 0;
//    outputs->next       = NULL;

//    inputs->name       = av_strdup("out");
//    inputs->filter_ctx = buffersink_ctx;
//    inputs->pad_idx    = 0;
//    inputs->next       = NULL;

//    if (!outputs->name || !inputs->name) {
//        ret = AVERROR(ENOMEM);
//        goto end;
//    }

//    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_spec,
//                    &inputs, &outputs, NULL)) < 0)
//        goto end;

//    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
//        goto end;

//    /* Fill FilteringContext */
//    fctx->buffersrc_ctx = buffersrc_ctx;
//    fctx->buffersink_ctx = buffersink_ctx;
//    fctx->filter_graph = filter_graph;

//end:
//    avfilter_inout_free(&inputs);
//    avfilter_inout_free(&outputs);

//    return ret;
//}

//static int init_filters(void)
//{
//    const char *filter_spec;
//    unsigned int i;
//    int ret;
//    filter_ctx = av_malloc_array(ifmt_ctx->nb_streams, sizeof(*filter_ctx));
//    if (!filter_ctx)
//        return AVERROR(ENOMEM);

//    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
//        filter_ctx[i].buffersrc_ctx  = NULL;
//        filter_ctx[i].buffersink_ctx = NULL;
//        filter_ctx[i].filter_graph   = NULL;
//        if (!(ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO
//                || ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO))
//            continue;


//        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
//            filter_spec = "null"; /* passthrough (dummy) filter for video */
//        else
//            filter_spec = "anull"; /* passthrough (dummy) filter for audio */
//        ret = init_filter(&filter_ctx[i], stream_ctx[i].dec_ctx,
//                stream_ctx[i].enc_ctx, filter_spec);
//        if (ret)
//            return ret;

//        filter_ctx[i].enc_pkt = av_packet_alloc();
//        if (!filter_ctx[i].enc_pkt)
//            return AVERROR(ENOMEM);

//        filter_ctx[i].filtered_frame = av_frame_alloc();
//        if (!filter_ctx[i].filtered_frame)
//            return AVERROR(ENOMEM);
//    }
//    return 0;
//}

//static int encode_write_frame(unsigned int stream_index, int flush)
//{
//    StreamContext *stream = &stream_ctx[stream_index];
//    FilteringContext *filter = &filter_ctx[stream_index];
//    AVFrame *filt_frame = flush ? NULL : filter->filtered_frame;
//    AVPacket *enc_pkt = filter->enc_pkt;
//    int ret;

//    av_log(NULL, AV_LOG_INFO, "Encoding frame\n");
//    /* encode filtered frame */
//    av_packet_unref(enc_pkt);

//    ret = avcodec_send_frame(stream->enc_ctx, filt_frame);

//    if (ret < 0)
//        return ret;

//    while (ret >= 0) {
//        ret = avcodec_receive_packet(stream->enc_ctx, enc_pkt);

//        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//            return 0;

//        /* prepare packet for muxing */
//        enc_pkt->stream_index = stream_index;
//        av_packet_rescale_ts(enc_pkt,
//                             stream->enc_ctx->time_base,
//                             ofmt_ctx->streams[stream_index]->time_base);

//        av_log(NULL, AV_LOG_DEBUG, "Muxing frame\n");
//        /* mux encoded frame */
//        ret = av_interleaved_write_frame(ofmt_ctx, enc_pkt);
//    }

//    return ret;
//}

//static int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index)
//{
//    FilteringContext *filter = &filter_ctx[stream_index];
//    int ret;

//    av_log(NULL, AV_LOG_INFO, "Pushing decoded frame to filters\n");
//    /* push the decoded frame into the filtergraph */
//    ret = av_buffersrc_add_frame_flags(filter->buffersrc_ctx,
//            frame, 0);
//    if (ret < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
//        return ret;
//    }

//    /* pull filtered frames from the filtergraph */
//    while (1) {
//        av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
//        ret = av_buffersink_get_frame(filter->buffersink_ctx,
//                                      filter->filtered_frame);
//        if (ret < 0) {
//            /* if no more frames for output - returns AVERROR(EAGAIN)
//             * if flushed and no more frames for output - returns AVERROR_EOF
//             * rewrite retcode to 0 to show it as normal procedure completion
//             */
//            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//                ret = 0;
//            break;
//        }

//        filter->filtered_frame->pict_type = AV_PICTURE_TYPE_NONE;
//        ret = encode_write_frame(stream_index, 0);
//        av_frame_unref(filter->filtered_frame);
//        if (ret < 0)
//            break;
//    }

//    return ret;
//}

//static int flush_encoder(unsigned int stream_index)
//{
//    if (!(stream_ctx[stream_index].enc_ctx->codec->capabilities &
//                AV_CODEC_CAP_DELAY))
//        return 0;

//    av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
//    return encode_write_frame(stream_index, 1);
//}

//int main(int argc, char **argv)
//{
//    int ret;
//    AVPacket *packet = NULL;
//    unsigned int stream_index;
//    unsigned int i;

//    if (argc != 3) {
//        av_log(NULL, AV_LOG_ERROR, "Usage: %s <input file> <output file>\n", argv[0]);
//        return 1;
//    }

//    if ((ret = open_input_file(argv[1])) < 0)
//        goto end;
//    if ((ret = open_output_file(argv[2])) < 0)
//        goto end;
//    if ((ret = init_filters()) < 0)
//        goto end;
//    if (!(packet = av_packet_alloc()))
//        goto end;

//    /* read all packets */
//    while (1) {
//        if ((ret = av_read_frame(ifmt_ctx, packet)) < 0)
//            break;
//        stream_index = packet->stream_index;
//        av_log(NULL, AV_LOG_DEBUG, "Demuxer gave frame of stream_index %u\n",
//                stream_index);

//        if (filter_ctx[stream_index].filter_graph) {
//            StreamContext *stream = &stream_ctx[stream_index];

//            av_log(NULL, AV_LOG_DEBUG, "Going to reencode&filter the frame\n");

//            av_packet_rescale_ts(packet,
//                                 ifmt_ctx->streams[stream_index]->time_base,
//                                 stream->dec_ctx->time_base);
//            ret = avcodec_send_packet(stream->dec_ctx, packet);
//            if (ret < 0) {
//                av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
//                break;
//            }

//            while (ret >= 0) {
//                ret = avcodec_receive_frame(stream->dec_ctx, stream->dec_frame);
//                if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
//                    break;
//                else if (ret < 0)
//                    goto end;

//                stream->dec_frame->pts = stream->dec_frame->best_effort_timestamp;
//                ret = filter_encode_write_frame(stream->dec_frame, stream_index);
//                if (ret < 0)
//                    goto end;
//            }
//        } else {
//            /* remux this frame without reencoding */
//            av_packet_rescale_ts(packet,
//                                 ifmt_ctx->streams[stream_index]->time_base,
//                                 ofmt_ctx->streams[stream_index]->time_base);

//            ret = av_interleaved_write_frame(ofmt_ctx, packet);
//            if (ret < 0)
//                goto end;
//        }
//        av_packet_unref(packet);
//    }

//    /* flush filters and encoders */
//    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
//        /* flush filter */
//        if (!filter_ctx[i].filter_graph)
//            continue;
//        ret = filter_encode_write_frame(NULL, i);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Flushing filter failed\n");
//            goto end;
//        }

//        /* flush encoder */
//        ret = flush_encoder(i);
//        if (ret < 0) {
//            av_log(NULL, AV_LOG_ERROR, "Flushing encoder failed\n");
//            goto end;
//        }
//    }

//    av_write_trailer(ofmt_ctx);
//end:
//    av_packet_free(&packet);
//    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
//        avcodec_free_context(&stream_ctx[i].dec_ctx);
//        if (ofmt_ctx && ofmt_ctx->nb_streams > i && ofmt_ctx->streams[i] && stream_ctx[i].enc_ctx)
//            avcodec_free_context(&stream_ctx[i].enc_ctx);
//        if (filter_ctx && filter_ctx[i].filter_graph) {
//            avfilter_graph_free(&filter_ctx[i].filter_graph);
//            av_packet_free(&filter_ctx[i].enc_pkt);
//            av_frame_free(&filter_ctx[i].filtered_frame);
//        }

//        av_frame_free(&stream_ctx[i].dec_frame);
//    }
//    av_free(filter_ctx);
//    av_free(stream_ctx);
//    avformat_close_input(&ifmt_ctx);
//    if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
//        avio_closep(&ofmt_ctx->pb);
//    avformat_free_context(ofmt_ctx);

//    if (ret < 0)
//        av_log(NULL, AV_LOG_ERROR, "Error occurred: %s\n", av_err2str(ret));

//    return ret ? 1 : 0;
//}
//Generated by doxygen 1.9.4
