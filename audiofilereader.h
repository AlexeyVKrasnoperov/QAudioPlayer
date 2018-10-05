#ifndef AUDIOFILEREADER_H
#define AUDIOFILEREADER_H

#include <QFileInfo>
#include <QAudioDeviceInfo>
#include "audiofile.h"

class AudioFileReader: public AudioFile
{
protected:
    bool Decode();
    bool DecodePacket(AVPacket & packet);
    const AVFrame* ConvertFrame(const AVFrame* frame);
public:
    AudioFileReader(const QString & fname)
    {
        Read(fname);
    }
    virtual ~AudioFileReader() = default;
    bool Read(const QString & fName);
    static AudioBuffer *CreateBuffer(const QString & fname)
    {
        if( ! QFileInfo::exists(fname) )
            return nullptr;
        AudioFileReader f(fname);
        AudioBuffer *b = f.getBuffer();
        if( (b != nullptr) && b->isEmpty() )
        {            
            delete b;
            b = nullptr;
        }
        return b;
    }
};

class AudioFileLoader: public QThread
{
    Q_OBJECT
protected:
    QString fileName;
    virtual void run()
    {
        emit ready(AudioFileReader::CreateBuffer(fileName));
    }
public:
    AudioFileLoader()
    {
        fileName.clear();
    }
    virtual bool Load(const QString & fName)
    {
        if( isRunning() )
            wait();
        if( ! QFileInfo::exists(fName) )
            return false;
        fileName = fName;
        start();
        return true;
    }
    virtual ~AudioFileLoader()
    {
        if( isRunning() )
            wait();
    }
signals:
    void ready(AudioBuffer *buffer);
};




#endif // AUDIOFILEREADER_H
