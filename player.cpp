#include "player.h"
#include <QBuffer>
#include <QAudioOutput>
#include <QIODevice>
#include "audiobuffer.h"
#include "audiofilereader.h"
#include <QDebug>

Player::Player(void)
{
    output = 0;
    device = 0;
    audioBuffer = 0;
    fileLoader  = 0;
    notifyInterval = -1;
    autoRestart = false;
    outputAudioDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
}

Player::~Player(void)
{
    close();
}

bool Player::open(const QString & name)
{
    if( fileLoader == 0 )
    {
        fileLoader = new AudioFileLoader();
        connect(fileLoader,SIGNAL(ready(AudioBuffer*)),this,SLOT(bufferReadySlot(AudioBuffer*)));
    }
    return fileLoader->Load(name);
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
    if( audioBuffer != 0 )
    {
        delete audioBuffer;
        audioBuffer = 0;
    }
    if( fileLoader != 0 )
    {
        delete fileLoader;
        fileLoader = 0;
    }
    emit currentTimeChanged(0);
    emit playerReady(false);
}

bool Player::start(qint32 from)
{
    if( output == 0 )
        return false;
    if( from != 0 )
        seek(from);
    output->start(device);
    return true;
}

void Player::notifySlot(void)
{
    emit currentTimeChanged(currentTime());
}

void Player::stop(void)
{
    if( output == 0 )
        return;
    if( output->state() == QAudio::ActiveState )
        output->stop();
}

qint32 Player::currentTime(void)
{
    if( (device == 0) || (output == 0) )
        return 0;
    return output->format().durationForBytes(device->pos())/1000;
}

qint32 Player::getDuration(void)
{
    if( audioBuffer == 0 )
        return -1;
    return audioBuffer->duration()/1000;
}

void Player::setNotifyInterval(int ms)
{
    notifyInterval = ms;
    if( output != 0 )
        output->setNotifyInterval(notifyInterval);
}


void Player::stateChangedSlot(QAudio::State state)
{
    qDebug() << state;
    switch (state)
    {
    case QAudio::ActiveState:
        emit started();
        break;
    case QAudio::IdleState:
    {
        if( autoRestart )
        {
            seek(0);
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
        emit stoped();
        break;
    }
    default:
        break;
    }
}

void Player::bufferReadySlot(AudioBuffer * a)
{
    close(); // clear all
    if( a == 0 )
        return;
    qDebug() << outputAudioDeviceInfo.deviceName();
    output = new QAudioOutput(outputAudioDeviceInfo,*a);
    if( output == 0 )
    {
        delete a;
        return;
    }
    device = new QBuffer(a,this);
    if( ! device->open(QIODevice::ReadOnly) )
    {
        delete a;
        delete device;
        device = 0;
        delete output;
        output = 0;
        return;
    }
    if( notifyInterval > 0 )
        output->setNotifyInterval(notifyInterval);
    connect(output,SIGNAL(stateChanged(QAudio::State)),this,SIGNAL(stateChanged(QAudio::State)));
    connect(output,SIGNAL(stateChanged(QAudio::State)),this,SLOT(stateChangedSlot(QAudio::State)));
    connect(output,SIGNAL(notify()),this,SLOT(notifySlot()));
    audioBuffer = a;
    seek(0);
    emit playerReady(true);
}

bool Player::seek(qint32 to)
{
    if( (device == 0) || (audioBuffer == 0) )
        return false;
    return device->seek(audioBuffer->bytesForDuration(to*1000));
}

bool Player::setAudioDevice(const QAudioDeviceInfo & info)
{
    if( info.isNull() )
        return false;
    outputAudioDeviceInfo = info;
    return true;
}
