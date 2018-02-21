#include "player.h"
#include <QBuffer>
#include <QAudioOutput>
#include <QIODevice>
#include "audiobuffer.h"

Player::Player(int ms)
{
    output = 0;
    device = 0;
    restartOffset  = -1;
    notifyInterval = ms;
    autoRestart = false;
}

Player::~Player(void)
{
    close();
}

void Player::close(void)
{
    if( output != 0 )
    {
        if( output->state() != QAudio::StoppedState )
            output->stop();
    }
    if( output != 0 )
    {
        delete output;
        output = 0;
    }
    if( device != 0 )
    {
        if( device->isOpen() )
            device->close();
        delete device;
        device = 0;
    }
}

bool Player::start(double from_sec)
{
    close();
    if( audioBuffer == 0 )
        return false;
    restartOffset = audioBuffer->getRestartOffset();
//    output = ProgramSettings::CreateOutputDevice(*b);
    if( output == 0 )
        return false;
    if( notifyInterval > 0 )
        output->setNotifyInterval(notifyInterval);
    connect(output,SIGNAL(stateChanged(QAudio::State)),this,SIGNAL(stateChanged(QAudio::State)));
    connect(output,SIGNAL(stateChanged(QAudio::State)),this,SLOT(stateChangedSlot(QAudio::State)));
    connect(output,SIGNAL(notify()),this,SLOT(notifySlot()));
    device = new QBuffer();
    device->setBuffer(audioBuffer);
    if( ! device->open(QIODevice::ReadOnly) )
    {
        close();
        return false;
    }
    if( from_sec > 0 )
    {
//        if( from_sec >= us2s(b->duration()) )
//            from_sec = 0;
//        device->seek(b->bytesForDuration(s2us(from_sec)));
    }
    output->start(device);
    return (output != 0);
}

void Player::notifySlot(void)
{
    emit currentTimeChanged(currentTime());
}

bool Player::stop(void)
{
    bool rv = isPlay();
    if( output != 0 )
        output->stop();
    return rv;
}

bool Player::isPlay(void)
{
    if( output == 0 )
        return false;
    return (output->state() == QAudio::ActiveState);
}

qint64 Player::currentTime(void)
{
    if( (device == 0) || (output == 0) )
        return 0;
    return output->format().durationForBytes(device->pos());
}

void Player::setNotifyInterval(int ms)
{
    notifyInterval = ms;
    if( output != 0 )
        output->setNotifyInterval(notifyInterval);
}


void Player::stateChangedSlot(QAudio::State state)
{
    switch (state)
    {
    case QAudio::ActiveState:
        emit started();
        break;
    case QAudio::IdleState:
    {
        qint64 offset = restartOffset;
        if( (offset < 0) && autoRestart )
            offset = 0;
        if( offset >= 0 )
        {
            device->seek(offset);
            output->start(device);
        }
        else
            output->stop();
        break;
    }
    case QAudio::StoppedState:
    {
        if (output->error() != QAudio::NoError)
            emit errorSignal();
        close();
        emit stoped();
        break;
    }
    default:
        break;
    }
}
