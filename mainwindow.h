#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ftpmanager.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void OnUploadProgress(qint64 bytesSent, qint64 bytesTotal);

    void upLoadError(QNetworkReply::NetworkError errorCode);

    void OnFinishRelay();

private:
    Ui::MainWindow *ui;
    FtpManager *ftp;
    QNetworkReply *reply;
};

#endif // MAINWINDOW_H
