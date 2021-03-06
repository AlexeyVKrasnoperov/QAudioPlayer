#include "audiobuffer.h"
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <algorithm>
using namespace std;

AudioBuffer::AudioBuffer(const QAudioFormat & format, QObject *parent):QObject(parent),QAudioFormat(format)
{
    setCodec(QStringLiteral("audio/pcm"));
}

AudioBuffer::~AudioBuffer() = default;
