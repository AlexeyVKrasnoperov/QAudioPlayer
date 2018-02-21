#include "audiobuffer.h"
#include <algorithm>
using namespace std;

AudioBuffer::AudioBuffer(void)
{
    volume = -1;
    restartOffset = -1;
    setCodec("audio/pcm");
    setByteOrder(QAudioFormat::LittleEndian);
    setSampleRate(441000);
    setSampleSize(16);
    setSampleType(QAudioFormat::SignedInt);
    setChannelCount(2);
}

AudioBuffer::AudioBuffer(const AudioBuffer & buffer, QObject *parent): QObject(parent), QAudioFormat(buffer), QByteArray(buffer)
{
    audioFileName = buffer.audioFileName;
    volume = buffer.volume;
    restartOffset = buffer.restartOffset;
    title = buffer.title;
}

AudioBuffer::AudioBuffer(const AudioBuffer & buffer, int size):QAudioFormat(buffer),QByteArray(size,0)
{
    volume = -1;
    restartOffset = -1;
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

bool AudioBuffer::FillScaled(int v, AudioBuffer ** scaled)
{
    if( isEmpty() )
        return false;
    bool isNew = false;
    if( *scaled != 0 )
    {
        if( (*scaled)->size() != size() )
        {
            delete *scaled;
            *scaled = 0;
        }
    }
    if( *scaled == 0 )
    {
        *scaled = new AudioBuffer(*this);
        isNew = true;
    }
    double scale = double(v)/getVolume();
    short * d = (short*) data();
    short * s = (short*) (*scaled)->data();
    int sz = size()/sizeof(short);
    for(int i = 0; i < sz; i++, s++, d++)
    {
        int v = qRound(scale*(*d));
        *s = (v > 0) ? qMin(SHRT_MAX,v) : qMax(SHRT_MIN,v);
    }
    return isNew;
}

float AudioBuffer::getVolume(void)
{
    if( isEmpty() )
        return -1;
    if( volume < 0 )
        volume = 100.*findAbsMax(*this)/float(SHRT_MAX);
    return volume;
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


