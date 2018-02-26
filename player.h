#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QAudio>
#include <QAudioDeviceInfo>

class QAudioOutput;
class QBuffer;
class AudioBuffer;
class AudioFileLoader;

class Player: public QObject
{
    Q_OBJECT
protected:
    QAudioDeviceInfo outputAudioDeviceInfo;
    AudioFileLoader *fileLoader;
    AudioBuffer  *audioBuffer;
    QAudioOutput *output;
    QBuffer *device;
    bool autoRestart;
    int notifyInterval;
public:
    Player(void);
    virtual ~Player();
    bool setAudioDevice(const QAudioDeviceInfo & info);
    bool open(const QString & name);
    void close(void);
    bool start(qint32 from = 0);
    void stop(void);
    bool seek(qint32 to);
    qint32 getDuration(void);
    qint32 currentTime(void);
    void setNotifyInterval(int ms);
    void setAutoRestart(bool f)
    {
        autoRestart = f;
    }
protected slots:
    void stateChangedSlot(QAudio::State state);
    void notifySlot(void);
    void bufferReadySlot(AudioBuffer *b);
signals:
    void stateChanged(QAudio::State);
    void currentTimeChanged(qint32 time);
    void errorSignal(void);
    void started(void);
    void stoped(void);
    void playerReady(bool ready);
};

#endif // PLAYER_H
