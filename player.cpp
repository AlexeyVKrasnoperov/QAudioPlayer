#include "player.h"
#include <QBuffer>
#include <QAudioOutput>
#include <QIODevice>
#include <QDebug>
#include "audiobuffer.h"
#include "audiofilereader.h"

Player::Player(void)
{
    volume = 1;
    notifyInterval = -1;
    autoRestart = false;
    outputAudioDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
}

Player::~Player(void)
{
    close();
}

QString Player::getFileName(void)
{
    return ( originalAudioBuffer.isNull() ) ? QString() :  originalAudioBuffer.data()->objectName();
}


void Player::setVolume(int v)
{
    if( v >= 0 )
        volume = 0.01*v;
    if( ! audioOutputDevice.isNull() )
        audioOutputDevice->setVolume(volume);
}

bool Player::open(const QString & name)
{
    if( fileLoader.isNull())
    {
        fileLoader.reset(new AudioFileLoader());
        connect(fileLoader.data(),SIGNAL(ready(AudioBuffer*)),this,SLOT(bufferReadySlot(AudioBuffer*)));
    }
    return fileLoader->Load(name);
}


void Player::close(void)
{
    if( ! audioOutputDevice.isNull() )
    {
        if( audioOutputDevice->state() != QAudio::StoppedState )
            audioOutputDevice->stop();
        audioOutputDevice.reset();
    }
    audioBufferDevice.reset();
    originalAudioBuffer.reset();
    fileLoader.reset();
    emit currentTimeChanged(0);
    emit playerReady(false);
}

bool Player::start(qint32 from)
{
    if( audioOutputDevice.isNull() || audioBufferDevice.isNull() )
        return false;
    if( from != 0 )
        seek(from);
    audioOutputDevice->start(audioBufferDevice.data());
    return true;
}

void Player::notifySlot(void)
{
    emit currentTimeChanged(currentTime());
}

void Player::stop(void)
{
    if( audioOutputDevice.isNull() )
        return;
    if( audioOutputDevice->state() == QAudio::ActiveState )
        audioOutputDevice->stop();
}

qint32 Player::currentTime(void)
{
    return ( audioBufferDevice.isNull() || audioOutputDevice.isNull() ) ? -1 : audioOutputDevice->format().durationForBytes(audioBufferDevice->pos())/1000;
}

qint32 Player::getDuration(void)
{
    return ( originalAudioBuffer.isNull() ) ? -1 : originalAudioBuffer->duration()/1000;
}

void Player::setNotifyInterval(int ms)
{
    notifyInterval = ms;
    if( ! audioOutputDevice.isNull() )
        audioOutputDevice->setNotifyInterval(notifyInterval);
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
        if( autoRestart )
        {
            seek(0);
            audioOutputDevice->start(audioBufferDevice.data());
        }
        else
            audioOutputDevice->stop();
        break;
    }
    case QAudio::StoppedState:
    {
        if (audioOutputDevice->error() != QAudio::NoError)
            emit errorSignal();
        emit stoped();
        break;
    }
    default:
        break;
    }
}

void Player::bufferReadySlot(AudioBuffer * original)
{
    close(); // clear all
    originalAudioBuffer.reset(original);
    emit playerReady(init());
}

bool Player::init(void)
{
    if( originalAudioBuffer.isNull() )
        return false;
    audioOutputDevice.reset( new QAudioOutput(outputAudioDeviceInfo,*originalAudioBuffer.data()) ) ;
//    audioOutputDevice.reset( new QAudioOutput(outputAudioDeviceInfo,outputAudioDeviceInfo.preferredFormat()) ) ;
    qDebug() << outputAudioDeviceInfo.preferredFormat();

    if( audioOutputDevice.isNull() )
        return false;
    audioBufferDevice.reset(new QBuffer(originalAudioBuffer.data(),this));
    if( ! audioBufferDevice->open(QIODevice::ReadOnly) )
    {
        audioBufferDevice.reset();
        audioOutputDevice.reset();
        return false;
    }
    audioBufferDevice->seek(0);
    setNotifyInterval(notifyInterval);
    setVolume();
    connect(audioOutputDevice.data(),SIGNAL(stateChanged(QAudio::State)),this,SIGNAL(stateChanged(QAudio::State)));
    connect(audioOutputDevice.data(),SIGNAL(stateChanged(QAudio::State)),this,SLOT(stateChangedSlot(QAudio::State)));
    connect(audioOutputDevice.data(),SIGNAL(notify()),this,SLOT(notifySlot()));
    return true;
}



bool Player::seek(qint32 to)
{
    if( originalAudioBuffer.isNull() || audioBufferDevice.isNull() )
        return false;
    return audioBufferDevice->seek(qMin(originalAudioBuffer->bytesForDuration(to*1000),originalAudioBuffer->size()));
}

void Player::setAudioDevice(const QAudioDeviceInfo & info)
{
    if( info.isNull() )
        return;
    outputAudioDeviceInfo = info;
    if( originalAudioBuffer.isNull() )
    {
        emit playerReady(false);
        return;
    }
    //
    bool restart = false;
    qint32 pos   = 0;
    if( ! audioOutputDevice.isNull() )
    {
        restart = (audioOutputDevice->state() == QAudio::ActiveState);
        if( restart )
            audioOutputDevice->stop();
        pos = currentTime();
    }
    //
    emit playerReady(init());
    //
    seek(pos);
    if( restart )
        start();
}
