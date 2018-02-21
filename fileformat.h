#ifndef FILEFORMAT_H
#define FILEFORMAT_H

struct FileFormat
{
    QString extension;
    QString description;
    FileFormat(const QString & e, const QString & d) : extension(e), description(d){}
};
typedef vector<FileFormat> FileFormatVector;



#endif // FILEFORMAT_H
