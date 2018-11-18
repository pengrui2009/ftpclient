#ifndef FTPGET_H
#define FTPGET_H
#include <QWidget>

class FtpGet
{
public:
    FtpGet();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // FTPGET_H
