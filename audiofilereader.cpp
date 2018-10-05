#include "audiofilereader.h"
#include <QtMath>

bool AudioFileReader::Read(const QString & fName)
{
    fileName = fName;
    if( ! Decode() )
    {
        release();
        return false;
    }
    if( (buffer == nullptr) || buffer->isEmpty() )
        return false;
    buffer->setObjectName(fName);
    return true;
}


bool AudioFileReader::Decode()
{
    if( avformat_open_input(&formatContext, fileName.toLocal8Bit(), nullptr, nullptr) != 0)
        return false;
    if (avformat_find_stream_info(formatContext, nullptr) < 0)
        return false;
    AVCodec* cdc = nullptr;
    int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &cdc, 0);
    if( streamIndex < 0 )
        return false;
    AVStream *audioStream = formatContext->streams[streamIndex];
    codecContext = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(codecContext, audioStream->codecpar);
    //
    if( avcodec_open2(codecContext, cdc, nullptr) != 0)
        return false;
    if( codecContext->channel_layout == 0 )
    {
        if( codecContext->channels == 1 )
            codecContext->channel_layout = AV_CH_LAYOUT_MONO;
        else
            codecContext->channel_layout = AV_CH_LAYOUT_STEREO;
    }
    if( buffer != nullptr )
    {
        delete buffer;
        buffer = nullptr;
    }
    //
    if( buffer == nullptr )
    {
        buffer = new AudioBuffer();
        buffer->setByteOrder(QAudioFormat::LittleEndian);
        buffer->setSampleRate(codecContext->sample_rate);
        int bytesPerSample = av_get_bytes_per_sample(codecContext->sample_fmt);
        buffer->setSampleSize(8*bytesPerSample);
        buffer->setSampleType(getSampleType(codecContext->sample_fmt));
        buffer->setChannelCount(codecContext->channels);
        int bps = bytesPerSample * codecContext->channels;
        int d = bps*qCeil(1.02*double(formatContext->duration*codecContext->sample_rate) / double(AV_TIME_BASE));
        buffer->reserve(d);
    }
    //
    AVPacket readingPacket;
    av_init_packet(&readingPacket);
    bool rv = true;
    while( rv && (av_read_frame(formatContext, &readingPacket) == 0) )
    {
        if(readingPacket.stream_index == audioStream->index)
            rv = DecodePacket(readingPacket);
        av_packet_unref(&readingPacket);
    }
    //
    if (cdc->capabilities & AV_CODEC_CAP_DELAY)
    {
        av_init_packet(&readingPacket);
        rv = DecodePacket(readingPacket);
        av_packet_unref(&readingPacket);
    }
    buffer->squeeze();
    release();
    return rv ;
}

bool AudioFileReader::DecodePacket(AVPacket & packet)
{
    if( iframe == nullptr )
    {
        iframe = av_frame_alloc();
        if( ! iframe )
            return false;
    }
    if( avcodec_send_packet(codecContext, &packet) < 0 )
        return false;
    forever
    {
        int ret = avcodec_receive_frame(codecContext,iframe);
        if (ret == AVERROR(EAGAIN) ) //|| ret == AVERROR_EOF)
            break;
        if (ret < 0)
            return false;
        if( av_sample_fmt_is_planar(codecContext->sample_fmt) )
        {
            int data_size = av_get_bytes_per_sample(codecContext->sample_fmt);
            for(int i = 0; i < iframe->nb_samples; i++)
                for(int ch = 0; ch < codecContext->channels; ch++)
                    buffer->append(reinterpret_cast<const char*>(iframe->data[ch] + data_size*i), data_size);
        }
        else
        {
            int data_size = av_samples_get_buffer_size(nullptr,iframe->channels,iframe->nb_samples,codecContext->sample_fmt,1);
            buffer->append(reinterpret_cast<const char*>(iframe->data[0]),data_size);
        }
        av_frame_unref(iframe);
    }
    return true;
}


