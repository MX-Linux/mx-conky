/**********************************************************************
 *  mxconky.h
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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QDir>

#include "cmd.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0, QString file = "");
    ~MainWindow();

    QString file_name;
    bool modified;

    void parseContent();
    void pickColor(QWidget *widget);
    void refresh();
    void saveBackup();
    void setColor(QWidget *widget, QColor color);
    void writeColor(QWidget *widget, QColor color);
    void writeFile(QString file_name, QString content);

    bool checkConkyRunning();
    bool readFile(QString file_name);

    QColor strToColor(QString colorstr);

public slots:

private slots:
    void cleanup();
    void cmdStart();
    void cmdDone();
    void setConnections();
    void on_buttonAbout_clicked();
    void on_buttonHelp_clicked();
    void on_buttonDefaultColor_clicked();
    void on_buttonColor0_clicked();
    void on_buttonColor1_clicked();
    void on_buttonColor2_clicked();
    void on_buttonColor3_clicked();
    void on_buttonColor4_clicked();
    void on_buttonToggleOn_clicked();
    void on_buttonRestore_clicked();
    void on_buttonEdit_clicked();
    void on_buttonChange_clicked();
    void on_radioDesktop1_clicked();
    void on_radioAllDesktops_clicked();
    void on_radioButtonDayLong_clicked();
    void on_radioButtonDayShort_clicked();
    void on_radioButtonMonthLong_clicked();
    void on_radioButtonMonthShort_clicked();

    void on_buttonCM_clicked();

    void closeEvent(QCloseEvent *);
private:
    Ui::MainWindow *ui;
    Cmd *cmd;
    QTimer *timer;
    QString file_content;
};


#endif

