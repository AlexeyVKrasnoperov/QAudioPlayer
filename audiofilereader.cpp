#include "audiofilereader.h"
#include <QDebug>

bool AudioFileReader::Read(const QString & fName)
{
    if( buffer->isValid() || ! buffer->isEmpty() )
        return false;
    fileName = fName;
    if( ! Decode() )
    {
        release();
        return false;
    }
    if( buffer->isEmpty() )
        return false;
    buffer->setFileName(fileName);
    return true;
}


bool AudioFileReader::Decode(void)
{
    if( avformat_open_input(&formatContext, fileName.toLocal8Bit(), NULL, NULL) != 0)
        return false;
    if (avformat_find_stream_info(formatContext, NULL) < 0)
        return false;
    AVCodec* cdc = nullptr;
    int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &cdc, 0);
    if( streamIndex < 0 )
        return false;
    AVStream *audioStream = formatContext->streams[streamIndex];
    codecContext = audioStream->codec;
    codecContext->codec = cdc;
    if( avcodec_open2(codecContext, codecContext->codec, NULL) != 0)
        return false;
    qDebug() << codecContext->channel_layout;
    if( codecContext->channel_layout == 0 )
    {
        if( codecContext->channels == 1 )
            codecContext->channel_layout = AV_CH_LAYOUT_MONO;
        else
            codecContext->channel_layout = AV_CH_LAYOUT_STEREO;
    }
    //
    //  Init QAudioFormat format
    //
    buffer->setCodec("audio/pcm");
    buffer->setByteOrder(QAudioFormat::LittleEndian);
    buffer->setSampleRate(codecContext->sample_rate);
    buffer->setSampleSize(getSampleSize(codecContext->sample_fmt));
    buffer->setSampleType(getSampleType(codecContext->sample_fmt));
    buffer->setChannelCount((codecContext->channel_layout == AV_CH_LAYOUT_MONO) ? 1 : 2);
    //
    AVPacket readingPacket;
    av_init_packet(&readingPacket);
    bool rv = true;
    while( rv && (av_read_frame(formatContext, &readingPacket) == 0) )
    {
        if(readingPacket.stream_index == audioStream->index)
            rv = DecodePacket(readingPacket);
        av_free_packet(&readingPacket);
    }
    //
    if (codecContext->codec->capabilities & CODEC_CAP_DELAY)
    {
        av_init_packet(&readingPacket);
        rv = DecodePacket(readingPacket);
    }
    //
    codecContext = 0;
    release();
    return rv ;
}

bool AudioFileReader::DecodePacket(AVPacket & packet)
{
    if( iframe == 0 )
    {
        iframe = av_frame_alloc();
        if( ! iframe )
            return false;
    }
    AVPacket decodingPacket = packet;
    while(decodingPacket.size > 0)
    {
        int gotFrame = 0;
        int result = avcodec_decode_audio4(codecContext, iframe, &gotFrame, &decodingPacket);
        if( (result >= 0) && gotFrame)
        {
            decodingPacket.size -= result;
            decodingPacket.data += result;
            //
            const AVFrame *f = ConvertFrame(iframe);
            if( f != 0 )
                buffer->append((const char *)f->data[0],f->linesize[0]);
            else
                return false;
        }
        else
        {
            decodingPacket.size = 0;
            decodingPacket.data = nullptr;
        }
    }
    return true;
}

const AVFrame * AudioFileReader::ConvertFrame(const AVFrame* frame)
{
    if( (getAVSampleFormat(buffer) == codecContext->sample_fmt) &&
            (buffer->channelCount() == codecContext->channels) &&
            (buffer->sampleRate() == codecContext->sample_rate ) )
        return frame;
    int nb_samples = av_rescale_rnd(frame->nb_samples,buffer->sampleRate(),codecContext->sample_rate,AV_ROUND_UP);
    if( oframe != 0 )
    {
        if( oframe->nb_samples < nb_samples )
            av_frame_free(&oframe);
    }
    if( oframe == 0 )
    {
        oframe = alloc_audio_frame(getAVSampleFormat(buffer),getAVChannelLayout(buffer),
                                   buffer->sampleRate(),nb_samples);
        if( oframe == 0 )
            return 0;
    }
    memset(oframe->data[0],0,oframe->linesize[0]);
    if( swr_ctx == 0 )
    {
        swr_ctx = swr_alloc_set_opts(NULL,
                                     getAVChannelLayout(buffer), getAVSampleFormat(buffer),buffer->sampleRate(),
                                     codecContext->channel_layout,codecContext->sample_fmt,codecContext->sample_rate,
                                     0, NULL);
        if( ! swr_ctx )
            return 0;
        if( swr_init(swr_ctx) < 0)
        {
            swr_free(&swr_ctx);
            return 0;
        }
    }
    //
    int got_samples = swr_convert(swr_ctx,oframe->data,nb_samples,(const uint8_t **)iframe->data,iframe->nb_samples);
    if( got_samples < 0 )
        return 0;
    //
    while( got_samples > 0 )
    {
        got_samples = swr_convert(swr_ctx,oframe->data,nb_samples,0,0);
    }
    return oframe;
}

