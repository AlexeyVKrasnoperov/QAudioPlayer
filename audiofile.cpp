extern "C" {
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}
#include "audiofile.h"
#include <QAudioDeviceInfo>
#include <QAudioFormat>

bool AudioFile::ffmpegInit = false;
FileFormatVector AudioFile::audioFormats;
QStringList AudioFile::audioFileFilters;

void AudioFile::initAudioFileFilters()
{
    if( ! audioFileFilters.empty() )
        return;
    if( audioFormats.empty() )
    {
        audioFormats.push_back(FileFormat(QStringLiteral("mp2"),QStringLiteral("MPEG-1/2 Audio Layer II")));
        audioFormats.push_back(FileFormat(QStringLiteral("mp3"),QStringLiteral("MPEG-1/2 Audio Layer III")));
        audioFormats.push_back(FileFormat(QStringLiteral("wma"),QStringLiteral("Windows Media Audio")));
        audioFormats.push_back(FileFormat(QStringLiteral("mp4"),QStringLiteral("MPEG-4 Part 14")));
        audioFormats.push_back(FileFormat(QStringLiteral("flac"),QStringLiteral("Free Lossless Audio Codec")));
        audioFormats.push_back(FileFormat(QStringLiteral("mka"),QStringLiteral("Matroska")));
        audioFormats.push_back(FileFormat(QStringLiteral("wav"),QStringLiteral("Wave Audio")));
        audioFormats.push_back(FileFormat(QStringLiteral("asf"),QStringLiteral("Advanced Systems Format")));
        audioFormats.push_back(FileFormat(QStringLiteral("flv"),QStringLiteral("Flash Video")));
        audioFormats.push_back(FileFormat(QStringLiteral("aiff"),QStringLiteral("Audio Interchange File Format")));
        audioFormats.push_back(FileFormat(QStringLiteral("tta"),QStringLiteral("True Audio")));
        audioFormats.push_back(FileFormat(QStringLiteral("wv"),QStringLiteral("WavPack")));
        audioFormats.push_back(FileFormat(QStringLiteral("aac"),QStringLiteral("Advanced Audio Coding")));
        audioFormats.push_back(FileFormat(QStringLiteral("m4a"),QStringLiteral("Advanced Audio Coding")));
        audioFormats.push_back(FileFormat(QStringLiteral("ogg"),QStringLiteral("Ogg Vorbis")));
    }
    QString allFormats;
    for( const auto & format : audioFormats)
    {
        QString filter(QObject::tr("Audio format "));
        //filter += QString(audioFileFormats[i].extension).toUpper();
        filter += format.description;
        filter += QStringLiteral(" (*.");
        filter += format.extension;
        filter += QStringLiteral(")");
        audioFileFilters << filter;
        if( ! allFormats.isEmpty() )
            allFormats.append(" ");
        allFormats += QStringLiteral("*.");
        allFormats += format.extension;
    }
    allFormats.prepend(QObject::tr("All supported formats ("));
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
        for( size_t i = 0; i < audioFormats.size(); i++ )
        {
            if( selected.toUpper() == audioFormats[i].extension.toUpper() )
            {
                found = audioFileFilters.at(int(i));
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
        for( size_t i = 0; i < size_t(audioFileFilters.size()); i++ )
        {
            if( i < audioFormats.size() )
            {
                if( selected == audioFileFilters.at(int(i)) )
                    return audioFormats[i].extension;
            }
        }
    }
    return audioFormats.front().extension;
}

AudioFile::AudioFile()
{
    if( ! ffmpegInit )
    {
        av_log_set_level(AV_LOG_QUIET);
        ffmpegInit = true;
    }
    buffer = nullptr;
    iframe = nullptr;
    codecContext  = nullptr;
    formatContext = nullptr;
}

AudioFile::~AudioFile()
{
    release();
}

void AudioFile::release()
{
    if( codecContext != nullptr )
    {
        avcodec_free_context(&codecContext);
        codecContext = nullptr;
    }
    if( formatContext != nullptr )
    {
        avformat_free_context(formatContext);
        formatContext = nullptr;
    }
    if( iframe != nullptr )
        av_frame_free(&iframe);
}

AudioBuffer * AudioFile::Convert(AudioBuffer * in, QAudioFormat & oFormat)
{
    if( (in == nullptr) || ! oFormat.isValid() || in->isEmpty() )
        return nullptr;
    if( *in == oFormat )
        return in;
    if( ! ffmpegInit )
    {
        av_log_set_level(AV_LOG_QUIET);
        ffmpegInit = true;
    }
    SwrContext * swr_ctx = swr_alloc_set_opts(nullptr,getAVChannelLayout(oFormat), getAVSampleFormat(oFormat),
                                              oFormat.sampleRate(),getAVChannelLayout(*in),
                                              getAVSampleFormat(*in),in->sampleRate(),0, nullptr);
    if( ! swr_ctx )
        return nullptr;
    if( swr_init(swr_ctx) < 0)
    {
        swr_free(&swr_ctx);
        return nullptr;
    }
    auto nb_samples = int(av_rescale_rnd(int64_t(in->frameCount()),int64_t(oFormat.sampleRate()),int64_t(in->sampleRate()),AV_ROUND_UP));
    auto *out = new AudioBuffer(oFormat);
    out->resize(nb_samples*out->sampleSize()/8*out->channelCount());
    out->fill(0);
    auto *out_samples = reinterpret_cast<uint8_t*>(out->data());
    auto *in_samples  = reinterpret_cast<const uint8_t*>(in->data());
    int got_samples = swr_convert(swr_ctx,&out_samples,nb_samples,&in_samples,in->frameCount());
    if( got_samples < 0 )
    {
        delete out;
        swr_free(&swr_ctx);
        return nullptr;
    }
    while( got_samples > 0 )
        got_samples = swr_convert(swr_ctx,&out_samples,nb_samples,nullptr,0);
    swr_free(&swr_ctx);
    out->setObjectName(in->objectName());
    return out;
}
