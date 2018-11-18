#include "ftpget.h"
//using namespace  std;

FtpGet::FtpGet()
{

}

/**
 * @brief 实现命令行下进度条，提示下载进度
 * @param bytesReceived
 * @param bytesTotal
 */
void FtpGet::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    int barLength = 50;
    int percent = int(qreal(bytesReceived) / qreal(bytesTotal) * barLength);
    QString out = "\rPercent: " + QString(percent, '#') + QString(barLength - percent, ' ');
    out += " " + QString::number(bytesReceived) + " / " + QString::number(bytesTotal);
   // std::cout << qPrintable(out) << std::flush;
}
