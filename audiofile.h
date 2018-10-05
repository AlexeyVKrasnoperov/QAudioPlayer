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
    static void initAudioFileFilters();
    static FileFormatVector audioFormats;
    static QStringList audioFileFilters;
    //
    AudioBuffer *buffer = nullptr;
    QString    fileName;
    //
    AVFrame *iframe;
    AVCodecContext  *codecContext;
    AVFormatContext *formatContext;
    //
    static AVSampleFormat getAVSampleFormat(QAudioFormat & b)
    {
        int ssb = b.sampleSize()/8;
        if( ssb == 1 )
        {
            if( b.sampleType() == QAudioFormat::UnSignedInt )
                return AV_SAMPLE_FMT_U8;
        }
        else if( ssb == 2 )
        {
            if( b.sampleType() == QAudioFormat::SignedInt )
                return AV_SAMPLE_FMT_S16;
        }
        else if( ssb == 4 )
        {
            if( b.sampleType() == QAudioFormat::Float )
                return AV_SAMPLE_FMT_FLT;
            else if( b.sampleType() == QAudioFormat::SignedInt )
                return AV_SAMPLE_FMT_S32;
        }
        return AV_SAMPLE_FMT_NONE;
    }
    //
    static int getAVChannelLayout(QAudioFormat & b)
    {
        return (b.channelCount() == 1) ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;
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
    virtual void release();
    //
public:
    AudioFile();
    virtual ~AudioFile();
    AudioBuffer * getBuffer()
    {
        return buffer;
    }
    static AudioBuffer * Convert(AudioBuffer * in, QAudioFormat & oFormat);
};

#endif // AUDIOFILE_H
