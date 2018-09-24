#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QAudio>
#include <QAudioDeviceInfo>
#include <QScopedPointer>

class QAudioOutput;
class QBuffer;
class AudioBuffer;
class AudioFileLoader;

class Player: public QObject
{
    Q_OBJECT
protected:
    QAudioDeviceInfo outputAudioDeviceInfo;
    QScopedPointer<AudioBuffer>  originalAudioBuffer;
    QScopedPointer<AudioFileLoader> fileLoader;
    QScopedPointer<QAudioOutput> audioOutputDevice;
    QScopedPointer<QBuffer> audioBufferDevice;
    bool autoRestart;
    int notifyInterval;
    qreal volume;
    bool init();
public:
    Player();
    virtual ~Player();
    bool open(const QString & name);
    void close();
    bool start(qint32 from = 0);
    void stop();
    bool seek(qint32 to);
    qint32 getDuration();
    qint32 currentTime();
    void setNotifyInterval(int ms);
    void setAutoRestart(bool f)
    {
        autoRestart = f;
    }
    //
    QString getFileName();
    //
public slots:
    void setVolume(int v = -1);
    void setAudioDevice(const QAudioDeviceInfo & info);
protected slots:
    void stateChangedSlot(QAudio::State state);
    void notifySlot();
    void bufferReadySlot(AudioBuffer *original);
signals:
    void stateChanged(QAudio::State);
    void currentTimeChanged(qint32 time);
    void errorSignal();
    void started();
    void stoped();
    void playerReady(bool ready);
};

#endif // PLAYER_H
