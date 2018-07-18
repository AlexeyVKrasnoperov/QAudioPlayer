#ifndef AUDIOFILEREADER_H
#define AUDIOFILEREADER_H

#include <QFileInfo>
#include <QAudioDeviceInfo>
#include "audiofile.h"
#include <QDebug>

class AudioFileReader: public AudioFile
{
protected:
    bool Decode(void);
    bool DecodePacket(AVPacket & packet);
    const AVFrame* ConvertFrame(const AVFrame* frame);
public:
    AudioFileReader(const QString & fname):AudioFile(0)
    {
        Read(fname);
    }
    virtual ~AudioFileReader(){}
    bool Read(const QString & fName);
    static AudioBuffer *CreateBuffer(const QString & fname)
    {
        if( ! QFileInfo::exists(fname) )
            return 0;
        AudioFileReader f(fname);
        AudioBuffer *b = f.getBuffer();
        if( (b != 0) && b->isEmpty() )
        {            
            delete b;
            b = 0;
        }
        return b;
    }
};

class AudioFileLoader: public QThread
{
    Q_OBJECT
protected:
    QString fileName;
    virtual void run(void)
    {
        emit ready(AudioFileReader::CreateBuffer(fileName));
    }
public:
    AudioFileLoader(void)
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
