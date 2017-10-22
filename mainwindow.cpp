/**********************************************************************
 *  MainWindow.cpp
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
#include "ui_mainwindow.h"

#include <QColorDialog>
#include <QFileDialog>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent, QString file) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cmd = new Cmd(this);
    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::cleanup);

    file_name = file;
    this->setWindowTitle(tr("MX Conky"));
    refresh();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// find defined colors in the config file
void MainWindow::parseContent()
{
    QStringList list = file_content.split("\n");
    foreach(QString row, list) {
        if (!row.startsWith("#")) {

            // Deal with colors
            if(row.startsWith("default_color")) {
                setColor(ui->widgetDefaultColor, strToColor(row.section(" ", 1)));
            } else if (row.startsWith("color0")) {
                setColor(ui->widgetColor0, strToColor(row.section(" ", 1)));
            } else if (row.startsWith("color1")) {
                setColor(ui->widgetColor1, strToColor(row.section(" ", 1)));
            } else if (row.startsWith("color2")) {
                setColor(ui->widgetColor2, strToColor(row.section(" ", 1)));
            } else if (row.startsWith("color3")) {
                setColor(ui->widgetColor3, strToColor(row.section(" ", 1)));
            } else if (row.startsWith("color4")) {
                setColor(ui->widgetColor4, strToColor(row.section(" ", 1)));

            // Desktop config
            } else if (row.startsWith("own_window_hints ")) {
                if (row.contains("sticky")) {
                    ui->radioAllDesktops->setChecked(true);
                } else {
                    ui->radioDesktop1->setChecked(true);
                }
            } else if (row.trimmed() == "own_window_hints") { // if empty options, assume default
                ui->radioDesktop1->setChecked(true);

            // Day/Month format
            } else {
                if (row.contains("%A")) {
                    ui->radioButtonDayLong->setChecked(true);
                } else if (row.contains("%a")) {
                    ui->radioButtonDayShort->setChecked(true);
                }
                if (row.contains("%B")) {
                    ui->radioButtonMonthLong->setChecked(true);
                } else if (row.contains("%b")) {
                    ui->radioButtonMonthShort->setChecked(true);
                }
            }
        }
    }
}

bool MainWindow::checkConkyRunning()
{
    if (system("pgrep -x conky > /dev/null 2>&1 ") == 0) {
        ui->buttonToggleOn->setText("Stop");
        ui->buttonToggleOn->setIcon(QIcon::fromTheme("stop"));
        return true;
    } else {
        ui->buttonToggleOn->setText("Run");
        ui->buttonToggleOn->setIcon(QIcon::fromTheme("start"));
        return false;
    }
}

// read config file
bool MainWindow::readFile(QString file_name)
{
    QFile file(file_name);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Count not open file: " << file.fileName();
        return false;
    }
    file_content = file.readAll().trimmed();
    file.close();
    return true;
}

// convert string to QColor
QColor MainWindow::strToColor(QString colorstr)
{
    QColor color(colorstr);
    if (!color.isValid()) { // if color is invalid assume RGB values and add a # in front of the string
        color.setNamedColor("#" + colorstr);
    }
    return color;
}

// refresh windows content
void MainWindow::refresh()
{
    // hide all color frames by default, display only the ones in the config file
    QList<QWidget *> frames;
    frames << ui->frameDefault << ui->frame0 << ui->frame1 << ui->frame2 << ui->frame3 << ui->frame4;
    foreach (QWidget *w, frames) {
        w->hide();
    }
    // draw borders around color widgets
    QList<QWidget *> widgets;
    widgets << ui->widgetDefaultColor << ui->widgetColor0 << ui->widgetColor1 << ui->widgetColor2 << ui->widgetColor3 << ui->widgetColor4;
    foreach (QWidget *w, widgets) {
        w->setStyleSheet("border: 1px solid black");
    }

    QString conky_name = QFileInfo(file_name).fileName();
    ui->buttonChange->setText(conky_name);

    checkConkyRunning();

    QFile(file_name + ".bak").remove();
    QFile::copy(file_name, file_name + ".bak");

    if (readFile(file_name)) {
        parseContent();
    }
    this->adjustSize();
}

// write color change back to the file
void MainWindow::writeColor(QWidget *widget, QColor color)
{
    QString item_name;
    if (widget->objectName() == "widgetDefaultColor") {
        item_name = "default_color";
    } else if (widget->objectName() == "widgetColor0") {
        item_name = "color0";
    } else if (widget->objectName() == "widgetColor1") {
        item_name = "color1";
    } else if (widget->objectName() == "widgetColor2") {
        item_name = "color2";
    } else if (widget->objectName() == "widgetColor3") {
        item_name = "color3";
    } else if (widget->objectName() == "widgetColor4") {
        item_name = "color4";
    }

    QStringList list = file_content.split("\n");
    QStringList new_list;
    foreach(QString row, list) {
        if (row.startsWith(item_name)) {
            row = item_name + " " + color.name().remove('#');
        }
        new_list << row;
    }
    file_content = new_list.join("\n");

    writeFile(file_name, file_content);
}

// write new content to file
void MainWindow::writeFile(QString file_name, QString content)
{
    QFile file(file_name);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << content;
        file.close();
    } else {
        qDebug() << "Error opening file " + file_name  + " for output";
    }
}


// cleanup environment when window is closed
void MainWindow::cleanup()
{
    if(!cmd->terminate()) {
        cmd->kill();
    }
}


// Get version of the program
QString MainWindow::getVersion(QString name)
{
    return cmd->getOutput("dpkg -l "+ name + "| awk 'NR==6 {print $3}'");
}


void MainWindow::pickColor(QWidget *widget)
{
    QColorDialog *dialog = new QColorDialog();
    QColor color = dialog->getColor(widget->palette().color(QWidget::backgroundRole()));
    if (color.isValid()) {
        setColor(widget, color);
        writeColor(widget, color);
    }
}

void MainWindow::setColor(QWidget *widget, QColor color)
{
    widget->parentWidget()->show();
    if (color.isValid()) {
        QPalette pal = palette();
        pal.setColor(QPalette::Background, color);
        widget->setAutoFillBackground(true);
        widget->setPalette(pal);
    }
}


void MainWindow::cmdStart()
{
    setCursor(QCursor(Qt::BusyCursor));
}


void MainWindow::cmdDone()
{
 //   ui->progressBar->setValue(ui->progressBar->maximum());
    setCursor(QCursor(Qt::ArrowCursor));
    cmd->disconnect();
}

// set proc and timer connections
void MainWindow::setConnections()
{
    connect(cmd, &Cmd::started, this, &MainWindow::cmdStart);
    connect(cmd, &Cmd::finished, this, &MainWindow::cmdDone);
}

// About button clicked
void MainWindow::on_buttonAbout_clicked()
{
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About MX Conky"), "<p align=\"center\"><b><h2>" +
                       tr("MX Conky") + "</h2></b></p><p align=\"center\">" + tr("Version: ") + getVersion("mx-conky") + "</p><p align=\"center\"><h3>" +
                       tr("GUI program for configuring Conky in MX Linux") +
                       "</h3></p><p align=\"center\"><a href=\"http://mxlinux.org\">http://mxlinux.org</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) MX Linux") + "<br /><br /></p>");
    msgBox.addButton(tr("License"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::NoRole);
    if (msgBox.exec() == QMessageBox::AcceptRole) {
        system("xdg-open file:///usr/share/doc/mx-conky/license.html");
    }
}

// Help button clicked
void MainWindow::on_buttonHelp_clicked()
{
    QString cmd = QString("xdg-open https://mxlinux.org/user_manual_mx15/mxum.html");
    system(cmd.toUtf8());
}


void MainWindow::on_buttonDefaultColor_clicked()
{
    pickColor(ui->widgetDefaultColor);
}

void MainWindow::on_buttonColor0_clicked()
{
    pickColor(ui->widgetColor0);
}

void MainWindow::on_buttonColor1_clicked()
{
    pickColor(ui->widgetColor1);
}

void MainWindow::on_buttonColor2_clicked()
{
    pickColor(ui->widgetColor2);
}

void MainWindow::on_buttonColor3_clicked()
{
    pickColor(ui->widgetColor3);
}

void MainWindow::on_buttonColor4_clicked()
{
    pickColor(ui->widgetColor4);
}


// start-stop conky
void MainWindow::on_buttonToggleOn_clicked()
{
    if (checkConkyRunning()) {
        system("pkill -x conky");
    } else {
        system("conky -c '" + file_name.toUtf8() + "'&");
    }
    checkConkyRunning();
}


void MainWindow::on_buttonRestore_clicked()
{
    if (QFile(file_name + ".bak").exists()) {
        QFile(file_name).remove();
    }
    if (QFile::copy(file_name + ".bak", file_name)) {
        refresh();
    }
}

void MainWindow::on_buttonEdit_clicked()
{
    QString editor;

    if (QFile("/usr/bin/featherpad").exists()) {
        editor = "featherpad";
    } else if (QFile("/usr/bin/leafpad").exists()) {
        editor = "leafpad";
    } else {
        qDebug() << "could not find either leafpad or featherpad";
        return;
    }

    system(editor.toUtf8() + " '" + file_name.toUtf8() + "'");
    refresh();
}

void MainWindow::on_buttonChange_clicked()
{
    QFileDialog dialog;

    QString selected = dialog.getOpenFileName(0, QObject::tr("Select Conky Manager config file"), QFileInfo(file_name).path());
    if (selected != "") {
        file_name = selected;
    }
    refresh();
}

void MainWindow::on_radioDesktop1_clicked()
{
    bool found = false;
    QStringList list = file_content.split("\n");
    QStringList new_list;
    foreach(QString row, list) {
        if (row.startsWith("own_window_hints ")) {
            row.remove(",sticky");
            row.remove("sticky"); // if first item
            found = true;
        }
        new_list << row;
    }
    if (!found) {
        new_list << "own_window_hints ";
    }

    file_content = new_list.join("\n");

    writeFile(file_name, file_content);
}

void MainWindow::on_radioAllDesktops_clicked()
{
    bool found = false;
    QStringList list = file_content.split("\n");
    QStringList new_list;
    foreach(QString row, list) {
        if (row.startsWith("own_window_hints ")) {;
            row.append(",sticky");
            found = true;
        }
        if (row == "own_window_hints") {
            row.append(" sticky");
            found = true;
        }
        new_list << row;
    }
    if (!found) {
        new_list << "own_window_hints sticky";
    }

    file_content = new_list.join("\n");

    writeFile(file_name, file_content);
}

void MainWindow::on_radioButtonDayLong_clicked()
{
    file_content.replace("%a", "%A");
    writeFile(file_name, file_content);
}

void MainWindow::on_radioButtonDayShort_clicked()
{
    file_content.replace("%A", "%a");
    writeFile(file_name, file_content);
}

void MainWindow::on_radioButtonMonthLong_clicked()
{
    file_content.replace("%b", "%B");
    writeFile(file_name, file_content);
}

void MainWindow::on_radioButtonMonthShort_clicked()
{
    file_content.replace("%B", "%b");
    writeFile(file_name, file_content);
}