#ifndef AUDIOFILEREADER_H
#define AUDIOFILEREADER_H

#include <QFileInfo>
#include "audiofile.h"

class AudioFileReader: public AudioFile
{
protected:
    bool Decode(void);
    bool DecodePacket(AVPacket & packet);
    const AVFrame* ConvertFrame(const AVFrame* frame);
public:
    AudioFileReader(AudioBuffer *b):AudioFile(b){}
    virtual ~AudioFileReader(){}
    bool Read(const QString & fName);
    static AudioBuffer *CreateBuffer(const QString & fName)
    {
        if( ! QFileInfo(fName).exists() )
            return 0;
        AudioBuffer *b = new AudioBuffer();
        AudioFileReader f(b);
        if( ( ! f.Read(fName) ) || b->isEmpty() )
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
        AudioBuffer *buffer = AudioFileReader::CreateBuffer(fileName);
        emit ready(buffer);
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
        if( ! QFileInfo(fName).exists() )
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
