#include "cmd.h"

#include <QDebug>
#include <QEventLoop>
#include <QProcess>


// util function for getting bash command output
QString getCmdOut(const QString &cmd, bool quiet)
{
    QByteArray output;
    run(cmd, output, quiet);
    return output;
}


bool run(const QString &cmd, QByteArray &output, bool quiet)
{
    if (!quiet) qDebug().noquote() << cmd;
    QProcess proc;
    QEventLoop loop;
    proc.start("/bin/bash", QStringList() << "-c" << cmd);
    proc.waitForFinished();
    output = proc.readAll().trimmed();
    return (proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0);
}

