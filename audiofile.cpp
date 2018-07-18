extern "C" {
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}
#include "audiofile.h"
#include <QDebug>
#include <QAudioDeviceInfo>
#include <QAudioFormat>

bool AudioFile::ffmpegInit = false;
FileFormatVector AudioFile::audioFormats;
QStringList AudioFile::audioFileFilters;

AVFrame * AudioFile::alloc_audio_frame(AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples)
{
    AVFrame *f = av_frame_alloc();
    if( f )
    {
        f->format = sample_fmt;
        f->channel_layout = channel_layout;
        f->channels = av_get_channel_layout_nb_channels(channel_layout);
        f->sample_rate = sample_rate;
        f->nb_samples = nb_samples;
        if (nb_samples)
            if(av_frame_get_buffer(f, 0) < 0)
                av_frame_free(&f);
    }
    return f;
}

void AudioFile::initAudioFileFilters(void)
{
    if( ! audioFileFilters.empty() )
        return;
    if( audioFormats.empty() )
    {
        audioFormats.push_back(FileFormat("mp2","MPEG-1/2 Audio Layer II"));
        audioFormats.push_back(FileFormat("mp3","MPEG-1/2 Audio Layer III"));
        audioFormats.push_back(FileFormat("wma","Windows Media Audio"));
        audioFormats.push_back(FileFormat("mp4","MPEG-4 Part 14"));
        audioFormats.push_back(FileFormat("flac","Free Lossless Audio Codec"));
        audioFormats.push_back(FileFormat("mka","Matroska"));
        audioFormats.push_back(FileFormat("wav","Wave Audio"));
        audioFormats.push_back(FileFormat("asf","Advanced Systems Format"));
        audioFormats.push_back(FileFormat("flv","Flash Video"));
        audioFormats.push_back(FileFormat("aiff","Audio Interchange File Format"));
        audioFormats.push_back(FileFormat("tta","True Audio"));
        audioFormats.push_back(FileFormat("wv","WavPack"));
        audioFormats.push_back(FileFormat("aac","Advanced Audio Coding"));
//        audioFormats.push_back(FileFormat("m4a","Advanced Audio Coding"));
        audioFormats.push_back(FileFormat("ogg","Ogg Vorbis"));
    }
    QString allFormats;
    for( size_t i = 0; i < audioFormats.size(); i++ )
    {
        QString filter(QObject::tr("Файл формата "));
        //filter += QString(audioFileFormats[i].extension).toUpper();
        filter += audioFormats[i].description;
        filter += QString(" (*.%1)").arg(audioFormats[i].extension);
        audioFileFilters << filter;
        if( ! allFormats.isEmpty() )
            allFormats.append(" ");
        allFormats += QString("*.%1").arg(audioFormats[i].extension);
    }
    allFormats.prepend(QObject::tr("Все поддерживаемые форматы ("));
    allFormats.append(QObject::tr(")"));
    audioFileFilters << allFormats;
}

const QStringList & AudioFile::getAudioFileFilters(QString & selected)
{
    if( audioFileFilters.empty() )
        initAudioFileFilters();
    //
    if( ! selected.isEmpty() )
    {
        QString found;
        for( int i = 0; i < int(audioFormats.size()); i++ )
        {
            if( selected.toUpper() == audioFormats[i].extension.toUpper() )
            {
                found = audioFileFilters[i];
                break;
            }
        }
        selected = (found.isEmpty()) ? audioFileFilters.front() : found;
    }
    else
        selected = audioFileFilters.front();
    //
    return audioFileFilters;
}

const QString & AudioFile::getSelectedAudioFileSuffix(const QString & selected)
{
    if( ! selected.isEmpty() )
    {
        for( int i = 0; i < int(audioFileFilters.size()); i++ )
        {
            if( i < int(audioFormats.size()) )
            {
                if( selected == audioFileFilters[i] )
                    return audioFormats[i].extension;
            }
        }
    }
    return audioFormats.front().extension;
}

AudioFile::AudioFile(AudioBuffer *b):buffer(b)
{
    if( ! ffmpegInit )
    {
        av_log_set_level(AV_LOG_TRACE);
        av_register_all();
        ffmpegInit = true;
    }
    iframe = 0;
    oframe = 0;
    codecContext  = 0;
    formatContext = 0;
    swr_ctx = 0;
    //
    if( buffer == 0 )
        buffer = new AudioBuffer();
//        buffer = new AudioBuffer( QAudioDeviceInfo::defaultOutputDevice().preferredFormat() );
}

AudioFile::~AudioFile()
{
    release();
}

void AudioFile::release(void)
{
    if( codecContext != 0 )
    {
        avcodec_free_context(&codecContext);
        codecContext = 0;
    }
    if( formatContext != 0 )
    {
//        if( formatContext->iformat != 0 )
//            avformat_close_input(&formatContext);
//        else
        avformat_free_context(formatContext);
        formatContext = 0;
    }
    if( iframe != 0 )
        av_frame_free(&iframe);
    if( oframe != 0 )
        av_frame_free(&oframe);
    if( swr_ctx )
        swr_free(&swr_ctx);
}
