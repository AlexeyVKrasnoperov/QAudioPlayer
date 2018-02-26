#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <QByteArray>
#include <QAudioFormat>
#include <QMutex>

class AudioBuffer: public QObject, public QAudioFormat, public QByteArray
{
    Q_OBJECT
protected:
    QMutex  mutex;
    QString audioFileName;
    QString title;
public:
    AudioBuffer(void);
    AudioBuffer(const AudioBuffer & buffer, QObject *parent = Q_NULLPTR);
    AudioBuffer(const AudioBuffer & buffer, int size);
    virtual ~AudioBuffer();
    const QString & getFileName(void)
    {
        return audioFileName;
    }
    void setFileName(const QString & fname)
    {
        audioFileName = fname;
    }
    void clearFileName(void)
    {
        audioFileName.clear();
    }
    inline qint64 duration()
    {
        return durationForBytes(size());
    }
    inline qint32 frameCount(void)
    {
        return size()/bytesPerFrame();
    }
    short *getFrame(int n);
    char  *getFramePtr(int n);
    void SaveAppend(const QByteArray & ba)
    {
        QMutexLocker locker(&mutex);
        append(ba);
    }
    QMutex  & getMutex()
    {
        return mutex;
    }
    //
    AudioBuffer * selectChannel(int ch);
    //
    void clearChannel(int ch);
    //
    const QString & getTitle(void)
    {
        return title;
    }
    void setTitle(const QString & t)
    {
        title = t;
    }
    //
    short findAbsMax(const QByteArray & array)
    {
        if( array.isEmpty() )
            return -1;
        int sz = array.size();
        if( sz%bytesPerFrame() != 0 )
            return -1;
        if( sampleSize() != 16 )
            return -1;
        sz /= 2;
        const short * d = (const short*) array.data();
        short max = qAbs(*d++);
        for(int i = 1; i < sz; i++)
            max = qMax(max,qAbs(*d++));
        return max;
    }
};

Q_DECLARE_METATYPE(AudioBuffer*)

#endif // AUDIOBUFFER_H
