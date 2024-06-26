#include <QDebug>
#include <QEventLoop>

#include "cmd.h"

Cmd::Cmd(QObject *parent)
    : QProcess(parent)
{
    connect(this, &Cmd::readyReadStandardOutput, [this]() { emit outputAvailable(readAllStandardOutput()); });
    connect(this, &Cmd::readyReadStandardError, [this]() { emit errorAvailable(readAllStandardError()); });
    connect(this, &Cmd::outputAvailable, [this](const QString &out) { out_buffer += out; });
    connect(this, &Cmd::errorAvailable, [this](const QString &out) { out_buffer += out; });
}

bool Cmd::run(const QString &cmd, bool quiet)
{
    out_buffer.clear();
    QString output;
    return run(cmd, output, quiet);
}

QString Cmd::getCmdOut(const QString &cmd, bool quiet)
{
    out_buffer.clear();
    QString output;
    run(cmd, output, quiet);
    return output;
}

bool Cmd::run(const QString &cmd, QString &output, bool quiet)
{
    out_buffer.clear();
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Cmd::done,
            Qt::UniqueConnection);
    if (state() != QProcess::NotRunning) {
        qDebug() << "Process already running:" << program() << arguments();
        return false;
    }
    if (!quiet) {
        qDebug().noquote() << cmd;
    }
    QEventLoop loop;
    connect(this, &Cmd::done, &loop, &QEventLoop::quit);
    start(QStringLiteral("/bin/bash"), {QStringLiteral("-c"), cmd});
    loop.exec();
    output = out_buffer.trimmed();
    return (exitStatus() == QProcess::NormalExit && exitCode() == 0);
}

QString Cmd::getCmdOutUntrimmed(const QString &cmd, bool quiet)
{
    out_buffer.clear();
    QString output;
    runUntrimmed(cmd, output, quiet);
    return output;
}

bool Cmd::runUntrimmed(const QString &cmd, QString &output, bool quiet)
{
    out_buffer.clear();
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Cmd::done,
            Qt::UniqueConnection);
    if (state() != QProcess::NotRunning) {
        qDebug() << "Process already running:" << program() << arguments();
        return false;
    }
    if (!quiet) {
        qDebug().noquote() << cmd;
    }
    QEventLoop loop;
    connect(this, &Cmd::done, &loop, &QEventLoop::quit);
    start(QStringLiteral("/bin/bash"), {QStringLiteral("-c"), cmd});
    loop.exec();
    output = out_buffer;
    return (exitStatus() == QProcess::NormalExit && exitCode() == 0);
}
