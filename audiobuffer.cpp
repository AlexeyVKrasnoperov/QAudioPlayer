#include "audiobuffer.h"
#include <algorithm>
using namespace std;

//AudioBuffer::AudioBuffer(void)
//{
//    setCodec("audio/pcm");
//    setByteOrder(QAudioFormat::LittleEndian);
//    setSampleRate(44100);
//    setSampleSize(16);
//    setSampleType(QAudioFormat::SignedInt);
//    setChannelCount(2);
//}

AudioBuffer::AudioBuffer(const QAudioFormat & format, QObject *parent):QObject(parent),QAudioFormat(format)
{
}

AudioBuffer::AudioBuffer(const AudioBuffer & buffer, QObject *parent): QObject(parent), QAudioFormat(buffer), QByteArray(buffer)
{
    audioFileName = buffer.audioFileName;
    title = buffer.title;
}

AudioBuffer::~AudioBuffer()
{
    clear();
}

