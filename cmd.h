#ifndef CMD_H
#define CMD_H

#include <QString>

bool run(const QString &cmd, QByteArray& output, bool quiet = false);
QString getCmdOut(const QString &cmd, bool quiet = false);

#endif // CMD_H
