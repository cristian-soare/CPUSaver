//////////////////////////////////////////////////////////////////////////
//																		//
// CPUSaver (C)2013, Cristian-Valeriu Soare 							//
// author: Cristian-Valeriu Soare 										//
// e-mail: soare_cristian16@yahoo.com 									//
// supervisor: Conf. Dr. Eng. Florin Pop								//
// Politehnica University of Bucharest, 								//
// Faculty of Automatic Control and Computer Science					//
// 																		//
// This file is part of CPUSaver.										//
// 																		//
// CPUSaver is free software: you can redistribute it and/or modify		//
// it under the terms of the GNU General Public License as published by	//
// the Free Software Foundation, either version 3 of the License, or 	//
// (at your option) any later version.									//
// 																		//
// CPUSaver is distributed in the hope that it will be useful,			//
// but WITHOUT ANY WARRANTY; without even the implied warranty of		//
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 		//
// GNU General Public License for more details.							//
// 																		//
// You should have received a copy of the GNU General Public License 	//
// along with CPUSaver.  If not, see <http://www.gnu.org/licenses/>.	//
//																		//
//////////////////////////////////////////////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QStandardItemModel>
#include <QDebug>
#include <string>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <csignal>
#include <pwd.h>
#include <unistd.h>

using namespace std;

typedef unsigned int uint;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString s[] = {"Suspend", "Kill", "SlowDown", "Immune"};
    for (int i = 0; i < 3; i++)
        ui->endTypeComboBox->addItem(s[i]);
    for (int i = 0; i < 4; i++)
        ui->exEndComboBox->addItem(s[i]);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// gets pid of cpusaver daemon as string
string MainWindow::getPid()
{
    FILE* pipe = popen("pidof cpusaver", "r");
    char buffer[10];

    fgets(buffer, 10, pipe);
    pclose(pipe);

    string s(buffer);

    return s;
}

// gets configuration file path
string MainWindow::getCfgPath()
{
    string s = getpwuid(getuid())->pw_dir;
    s += "/.cpusconfig";
    return s;
}

// reads configuration file (button)
void MainWindow::on_getcfgButton_clicked()
{
    ifstream f;
    f.open(getCfgPath().c_str());

    if (f.good())
    {
        string s;
        ui->exListWidget->clear();
        while (f >> s)
        {
            if (s == "Threshold")
            {
                int threshold;
                if (f >> threshold) ui->thrSpinbox->setValue(threshold);
                else return;
            }
            else if (s == "EndType")
            {
                if (f >> s) ui->endTypeComboBox->setCurrentText(s.c_str());
                else return;
            }
            else if (s == "PercentSD")
            {
                int percent;
                if (f >> percent) ui->percentSpinBox->setValue(percent);
                else return;
            }
            else if (s == "SpecialList")
            {
                int n;
                if (f >> n)
                {
                    string cmd, et;
                    for (int i = 0; i < n; i++)
                    {
                        f >> cmd >> et;
                        ui->exListWidget->addItem((cmd + " " + et).c_str());
                    }
                }
            }
        }
    }
    else
        QMessageBox::warning(NULL, "Warning !", "Config file could not be opened !");

    f.close();
}

// writes configuration file (button)
void MainWindow::on_setcfgButton_clicked()
{
    ofstream g;
    g.open(getCfgPath().c_str(), ofstream::out | ofstream::trunc);

    g << "Threshold " << ui->thrSpinbox->value() << '\n';
    g << "EndType " << ui->endTypeComboBox->currentText().toStdString() << '\n';
    g << "PercentSD " << ui->percentSpinBox->value() << '\n';
    g << "SpecialList " << ui->exListWidget->count() << '\n';
    for (int i = 0; i < ui->exListWidget->count(); i++)
        g << ui->exListWidget->item(i)->text().toStdString() << '\n';
    g << '\n';

    g.close();
}

// adds exception to exception list (button)
void MainWindow::on_addExButton_clicked()
{
    ui->exListWidget->addItem(ui->exLineEdit->text() + " " + ui->exEndComboBox->currentText());
}

// removes selected exception from list
void MainWindow::on_remExButton_clicked()
{
    delete ui->exListWidget->item(ui->exListWidget->currentRow());
}

// checks if daemon is running
void MainWindow::on_isRunningButton_clicked()
{
    string pid = getPid();
    if (atoi(pid.c_str()) > 0)
    {
        string s = "Yes, pid: ";
        s += pid;
        QMessageBox::information(NULL, "Is daemon running ?", s.c_str());
    }
    else
        QMessageBox::information(NULL, "Is daemon running ?", "No");
}

// starts the daemon
void MainWindow::on_startdButton_clicked()
{
    if (atoi(getPid().c_str()) > 0)
    {
        QMessageBox::information(NULL, "Attention !", "Daemon already started !");
        return;
    }

    if (system("cpusaver") == 0) QMessageBox::information(NULL, "Success !", "Daemon started !");
    else QMessageBox::warning(NULL, "Warning !", "Daemon could not start !");
}

// stops the daemon
void MainWindow::on_stopdButton_clicked()
{
    uint pid = (uint)atoi(getPid().c_str());
    if (pid == 0)
    {
        QMessageBox::information(NULL, "Attention !", "Daemon not running !");
        return;
    }

    if (kill(pid, SIGINT) == 0) QMessageBox::information(NULL, "Success !", "Daemon killed !");
    else QMessageBox::warning(NULL, "Warning !", "Daemon could not be killed !");
}
