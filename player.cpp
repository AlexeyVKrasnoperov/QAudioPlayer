#include "player.h"
#include <QBuffer>
#include <QAudioOutput>
#include <QIODevice>
#include "audiobuffer.h"
#include "audiofilereader.h"

Player::Player()
{
    volume = 1;
    notifyInterval = -1;
    autoRestart = false;
    outputAudioDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
}

Player::~Player()
{
    close();
}

QString Player::getFileName()
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
        connect(fileLoader.data(),&AudioFileLoader::ready,this,&Player::bufferReadySlot);
    }
    return fileLoader->Load(name);
}


void Player::close()
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

void Player::notifySlot()
{
    emit currentTimeChanged(currentTime());
}

void Player::stop()
{
    if( audioOutputDevice.isNull() )
        return;
    if( audioOutputDevice->state() == QAudio::ActiveState )
        audioOutputDevice->stop();
}

qint32 Player::currentTime()
{
    return ( audioBufferDevice.isNull() || audioOutputDevice.isNull() ) ? -1 : audioOutputDevice->format().durationForBytes(audioBufferDevice->pos())/1000;
}

qint32 Player::getDuration()
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
    if( ! outputAudioDeviceInfo.isFormatSupported(*original) )
    {
        QAudioFormat format = outputAudioDeviceInfo.nearestFormat(*original);
        AudioBuffer * b = AudioFile::Convert(original,format);
        if( (b != nullptr) && (b != original) )
        {
            originalAudioBuffer.reset(b);
            delete original;
        }
        else
            originalAudioBuffer.reset(original);
    }
    else
        originalAudioBuffer.reset(original);


    emit playerReady(init());
}

bool Player::init()
{
    if( originalAudioBuffer.isNull() )
        return false;
    audioOutputDevice.reset( new QAudioOutput(outputAudioDeviceInfo,*originalAudioBuffer.data()) ) ;
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
    connect(audioOutputDevice.data(),&QAudioOutput::stateChanged,this,&Player::stateChanged);
    connect(audioOutputDevice.data(),&QAudioOutput::stateChanged,this,&Player::stateChangedSlot);
    connect(audioOutputDevice.data(),&QAudioOutput::notify,this,&Player::notifySlot);
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
