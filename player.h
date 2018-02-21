#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QAudio>

class QAudioOutput;
class QBuffer;
class AudioBuffer;

class Player: public QObject
{
    Q_OBJECT
protected:
    AudioBuffer  *audioBuffer;
    QAudioOutput *output;
    QBuffer *device;
    void close(void);
    qint32 restartOffset;
    bool autoRestart;
    int notifyInterval;
public:
    Player(int ms = -1);
    virtual ~Player();
    void setAutoBuffer(AudioBuffer *b)
    {
        audioBuffer = b;
    }
    bool start(double from_sec = -1);
    bool stop(void);
    bool isPlay(void);
    qint64 currentTime(void);
    void setNotifyInterval(int ms);
    void setAutoRestart(bool f)
    {
        autoRestart = f;
    }
protected slots:
    void stateChangedSlot(QAudio::State state);
    void notifySlot(void);
signals:
    void stateChanged(QAudio::State);
    void currentTimeChanged(qint64 time);
    void errorSignal(void);
    void started(void);
    void stoped(void);
};

#endif // PLAYER_H
