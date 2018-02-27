#ifndef AUDIOFILE_H
#define AUDIOFILE_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#include <vector>
using namespace  std;
#include <QString>
#include <QAudioFormat>
#include <QThread>
#include "audiobuffer.h"
#include "fileformat.h"

class AudioFile
{
public:
    static const QStringList & getAudioFileFilters( QString & selected );
    static const QString & getSelectedAudioFileSuffix(const QString & selected);
protected:
    static bool ffmpegInit;
    static void initAudioFileFilters(void);
    static FileFormatVector audioFormats;
    static QStringList audioFileFilters;
    //
    AudioBuffer *buffer;
    QString    fileName;
    //
    AVFrame *iframe;
    AVFrame *oframe;
    AVCodecContext  *codecContext;
    AVFormatContext *formatContext;
    SwrContext * swr_ctx;
    //
    AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);
    //   
    AVSampleFormat getAVSampleFormat(AudioBuffer *b)
    {
        int ssb = b->sampleSize()/8;
        if( ssb == 1 )
        {
            if( b->sampleType() == QAudioFormat::UnSignedInt )
                return AV_SAMPLE_FMT_U8;
        }
        else if( ssb == 2 )
        {
            if( b->sampleType() == QAudioFormat::SignedInt )
                return AV_SAMPLE_FMT_S16;
        }
        else if( ssb == 4 )
        {
            if( b->sampleType() == QAudioFormat::Float )
                return AV_SAMPLE_FMT_FLT;
            else if( b->sampleType() == QAudioFormat::SignedInt )
                return AV_SAMPLE_FMT_S32;
        }
        return AV_SAMPLE_FMT_NONE;
    }
    //
    int getAVChannelLayout(AudioBuffer *b)
    {
        return (b->channelCount() == 1) ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;
    }
    //
    QAudioFormat::SampleType getSampleType(AVSampleFormat format)
    {
        if( (format == AV_SAMPLE_FMT_U8) || (format == AV_SAMPLE_FMT_U8P) )
            return QAudioFormat::UnSignedInt;
        else if( (format == AV_SAMPLE_FMT_FLT) || (format == AV_SAMPLE_FMT_FLTP) )
            return QAudioFormat::Float;
        return QAudioFormat::SignedInt;
    }
    //
    int getSampleSize(AVSampleFormat format)
    {
        if( (format == AV_SAMPLE_FMT_U8) || (format == AV_SAMPLE_FMT_U8P) )
            return 8;
        else if( (format == AV_SAMPLE_FMT_S16 ) || (format == AV_SAMPLE_FMT_S16P ) )
            return 16;
        else if( (format == AV_SAMPLE_FMT_S32 ) || (format == AV_SAMPLE_FMT_S32P ) )
            return 32;
        else if( (format == AV_SAMPLE_FMT_FLT) || (format == AV_SAMPLE_FMT_FLTP) )
            return sizeof(float);
        else if( (format == AV_SAMPLE_FMT_DBL) || (format == AV_SAMPLE_FMT_DBLP) )
            return sizeof(double);
        return 0;
    }
    //
    virtual void release(void);
    //
public:
    AudioFile(AudioBuffer *b = 0);
    virtual ~AudioFile();
};

#endif // AUDIOFILE_H
