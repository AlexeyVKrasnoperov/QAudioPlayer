#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <QByteArray>
#include <QAudioFormat>
#include <QMutex>

class AudioBuffer: public QObject, public QAudioFormat, public QByteArray
{
    Q_OBJECT
public:
    AudioBuffer(const QAudioFormat & format = QAudioFormat(), QObject *parent = Q_NULLPTR);
    virtual ~AudioBuffer();
    inline qint64 duration()
    {
        return durationForBytes(size());
    }
    inline qint32 frameCount(void)
    {
        return size()/bytesPerFrame();
    }
};

Q_DECLARE_METATYPE(AudioBuffer*)

#endif // AUDIOBUFFER_H
