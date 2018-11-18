#include "ftpmanager.h"
#include <QException>

FtpManager::FtpManager(QString rftpUsesrname,QString rftpUserPwd,QString rftpIP,int rftpPort)
{
    ftpIP =  rftpIP;
    ftpUsesrname = rftpUsesrname;
    ftpPort = rftpPort;
    ftpUserPwd = rftpUserPwd;

}

QString FtpManager::uploadFile(QString path_fileName,QString ftpFileName)
{
    if(ftpIP == "")
    {
        return "请设置ftp服务器信息";
    }
    try{
        sourceFile = new QFile(path_fileName);      //设置文件路径
        sourceFile->open(QIODevice::ReadOnly);  //读取模式
        //by_txt=sourceFile->readAll();   //全部读取,遇到大文件时会内存溢出
        //sourceFile->close();            //关闭文件

        manager = new QNetworkAccessManager;
        ftpUrl = new QUrl();      //设置QUrl数据
        ftpUrl->setScheme("ftp"); //设置URL的类型（或协议）
        ftpUrl->setHost(ftpIP);   //设置主机地址
        ftpUrl->setPort(ftpPort);           //端口
        ftpUrl->setUserName(ftpUsesrname);  //ftp用户名
        ftpUrl->setPassword(ftpUserPwd);   //ftp密码
        ftpUrl->setPath(ftpFileName);//设置路径+文件名,这里为ftp根目录

        replyOrg = manager->put(QNetworkRequest(*ftpUrl), sourceFile);  //上传
    }catch(QException e)
    {
        return "错误："+QString(e.what());
    }

    return "success";
}

QNetworkReply *FtpManager::getRelay()
{
    return replyOrg;
}
