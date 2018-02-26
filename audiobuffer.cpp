#include "audiobuffer.h"
#include <algorithm>
using namespace std;

AudioBuffer::AudioBuffer(void)
{
    setCodec("audio/pcm");
    setByteOrder(QAudioFormat::LittleEndian);
    setSampleRate(44100);
    setSampleSize(16);
    setSampleType(QAudioFormat::SignedInt);
    setChannelCount(2);
}

AudioBuffer::AudioBuffer(const AudioBuffer & buffer, QObject *parent): QObject(parent), QAudioFormat(buffer), QByteArray(buffer)
{
    audioFileName = buffer.audioFileName;
    title = buffer.title;
}

AudioBuffer::AudioBuffer(const AudioBuffer & buffer, int size):QAudioFormat(buffer),QByteArray(size,0)
{
}


AudioBuffer::~AudioBuffer()
{
    clear();
}

char *AudioBuffer::getFramePtr(int n)
{
    if( sampleSize() != 16 )
        return 0;
    if(n >= frameCount() )
        return 0;
    return (data() + bytesForFrames(n));
}

short *AudioBuffer::getFrame(int n)
{
    return (short*)(getFramePtr(n));
}

AudioBuffer * AudioBuffer::selectChannel(int ch)
{
    if( (ch < 0) || (ch > 1) )
        return 0;
    if( sampleSize() != 16 )
        return 0;
    if( channelCount() != 2 )
        return 0;
    if( isEmpty() )
        return 0;
    AudioBuffer *b = new AudioBuffer(*this);
    short * d = (short*) b->data();
    int dst = (ch==0) ? 1 : 0;
    for( int i = 0; i < frameCount(); i++, d+=2 )
        d[dst] = d[ch];
    return b;
}

void AudioBuffer::clearChannel(int ch)
{
    if( (ch < 0) || (ch > 1) )
        return;
    if( sampleSize() != 16 )
        return;
    if( channelCount() != 2 )
        return;
    if( isEmpty() )
        return;
    short * d = (short*) data();
    if( ch == 1 )
        d+=1;
    for( int i = 0; i < frameCount(); i++, d+=2 )
        *d = 0;
}


