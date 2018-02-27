#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <QByteArray>
#include <QAudioFormat>
#include <QMutex>

class AudioBuffer: public QObject, public QAudioFormat, public QByteArray
{
    Q_OBJECT
protected:
    QString audioFileName;
    QString title;
public:
    AudioBuffer(const QAudioFormat & format = QAudioFormat(), QObject *parent = Q_NULLPTR);
    AudioBuffer(const AudioBuffer & buffer, QObject *parent = Q_NULLPTR);
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
    //
    const QString & getTitle(void)
    {
        return title;
    }
    void setTitle(const QString & t)
    {
        title = t;
    }
};

Q_DECLARE_METATYPE(AudioBuffer*)

#endif // AUDIOBUFFER_H
