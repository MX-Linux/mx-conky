/**********************************************************************
 *  main.cpp
 **********************************************************************
 * Copyright (C) 2017 MX Authors
 *
 * Authors: Adrian
 *          MX Linux <http://mxlinux.org>
 *
 * This file is part of mx-conky.
 *
 * mx-conky is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mx-conky is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mx-conky.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include "mainwindow.h"
#include <unistd.h>
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QIcon>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QSettings>

#include <cmd.h>
#include <versionnumber.h>


// return the config file used for the newest conky process
QString getRunningConky()
{
    Cmd cmd;
    return cmd.getCmdOut("pgrep -xan conky | cut -d' ' -f4-", true);
}

QString openFile(QDir dir)
{
    QFileDialog dialog;
    QString file_name = getRunningConky();

    if (!file_name.isEmpty()) {
        return file_name;
    }

    QString selected = dialog.getOpenFileName(nullptr, QObject::tr("Select Conky Manager config file"), dir.path());
    if (!selected.isEmpty()) {
        return selected;
    }
    return QString();
}

void messageUpdate()
{
    Cmd cmd;
    VersionNumber current_version = cmd.getCmdOut("dpkg -l mx-conky-data | awk 'NR==6 {print $3}'", true);

    QSettings settings("mx-conky");

    QString ver = settings.value("data-version").toByteArray();
    VersionNumber recorded_version = ver;

    QString title = QObject::tr("Conky Data Update");
    QString message = QObject::tr("The MX Conky data set has been updated. <p><p>\
                                  Copy from the folder where it is located <a href=\"/usr/share/mx-conky-data/themes\">/usr/share/mx-conky-data/themes</a> \
                                  whatever you wish to your Home hidden conky folder <a href=\"%1/.conky\">~/.conky</a>. \
                                  Be careful not to overwrite any conkies you have changed.").arg(QDir::homePath());

    if (recorded_version.toString() == "" || current_version > recorded_version) {
        settings.setValue("data-version", current_version.toString());
        QMessageBox::information(nullptr, title, message);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon::fromTheme("mx-conky"));

    QTranslator qtTran;
    qtTran.load(QString("qt_") + QLocale::system().name());
    a.installTranslator(&qtTran);

    QTranslator appTran;
    appTran.load(QString("mx-conky_") + QLocale::system().name(), "/usr/share/mx-conky/locale");
    a.installTranslator(&appTran);

    if (system("dpkg -s conky-manager | grep -q 'Status: install ok installed'") != 0 &&
               system("dpkg -s conky-manager2 | grep -q 'Status: install ok installed'" ) != 0) {
        QMessageBox::critical(nullptr, QObject::tr("Error"),
                              QObject::tr("Could not find conky-manager, please install it before running mx-conky"));
        return 1;
    }

    if (getuid() != 0) {

        QString dir = QDir::homePath() + "/.conky";
        if (!QDir(dir).exists()) {
            QDir().mkdir(dir);
        }

        messageUpdate();

        // copy the mx-conky-data themes to the default folder
        system("cp -rn /usr/share/mx-conky-data/themes/* " + dir.toUtf8());

        QString file;
        if (qApp->arguments().length() >= 2 && QFile::exists(qApp->arguments().at(1))) {
            file = qApp->arguments().at(1);
        } else {
            file = openFile(dir);
        }
        if (file.isEmpty()) {
            return 1;
        }

        MainWindow w(nullptr, file);
        w.show();
        return a.exec();

    } else {
        QApplication::beep();
        QMessageBox::critical(nullptr, QString::null,
                              QObject::tr("You must run this program as normal user"));
        return 1;
    }
}
