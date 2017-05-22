/* - MuzicZapper - Copyright (C) 2011-2017 Sebastien Vavassori
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "commandsettingspage.h"
#include "ui_commandsettingspage.h"

#include <QtCore/QtGlobal>
#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QDirModel>
#include <QtWidgets/QFileDialog>

CommandSettingsPage::CommandSettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandSettingsPage)
{
    ui->setupUi(this);

    connect(ui->radioCommand_2, SIGNAL(toggled(bool)), ui->pathLineEdit, SLOT(setEnabled(bool)));
    connect(ui->radioCommand_2, SIGNAL(toggled(bool)), ui->browseButton, SLOT(setEnabled(bool)));
    ui->radioCommand_2->toggle();

    QCompleter *completer = new QCompleter(this);
    completer->setModel(new QDirModel(completer));
    ui->pathLineEdit->setCompleter(completer);

    connect(ui->browseButton, SIGNAL(released()), this, SLOT(browse()));
}

CommandSettingsPage::~CommandSettingsPage()
{
    delete ui;
}


void CommandSettingsPage::browse()
{
    QString dir = ui->pathLineEdit->text();
    if (dir.isEmpty()) {
        dir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    }
    QString path = QFileDialog::getExistingDirectory(
                this, tr("Select Directory"), dir,
                QFileDialog::ShowDirsOnly |
                QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        ui->pathLineEdit->setText(path);
    }
}

/***********************************************************************************
 ***********************************************************************************/
CommandSettings CommandSettingsPage::settings() const
{
    CommandSettings res;
    res.command = this->command();
    res.path = this->targetPath();
    res.isImmediat = this->isImmediat();
    res.isDetachable = this->isDetachable();
    return res;
}

void CommandSettingsPage::setSettings(const CommandSettings &value)
{
    this->setCommand(value.command);
    this->setTargetPath(value.path);
    this->setImmediat(value.isImmediat);
    this->setDetachable(value.isDetachable);
}

/***********************************************************************************
 ***********************************************************************************/
Command CommandSettingsPage::command() const
{
    if (ui->radioCommand_3->isChecked()) {
        return Command::MoveToSystemBin;

    } else if (ui->radioCommand_2->isChecked()) {
        return Command::MoveToPath;
    }
    return Command::NoAction;
}

void CommandSettingsPage::setCommand(Command command)
{
    switch (command) {
    case Command::MoveToSystemBin: ui->radioCommand_3->setChecked(true); break;
    case Command::MoveToPath:      ui->radioCommand_2->setChecked(true); break;
    case Command::NoAction:        ui->radioCommand_1->setChecked(true); break;
    default:
        Q_UNREACHABLE();
        break;
    }
}

/***********************************************************************************
 ***********************************************************************************/
QString CommandSettingsPage::targetPath() const
{
    return ui->pathLineEdit->text();
}

void CommandSettingsPage::setTargetPath(const QString &path)
{
    ui->pathLineEdit->setText(path);
}

/***********************************************************************************
 ***********************************************************************************/
bool CommandSettingsPage::isImmediat() const
{
    Q_ASSERT(ui->radioRunOption_1->isChecked() || ui->radioRunOption_2->isChecked() );
    return ui->radioRunOption_1->isChecked();
}

void CommandSettingsPage::setImmediat(bool immediat)
{
    if (immediat) {
        ui->radioRunOption_1->setChecked(true);
    } else {
        ui->radioRunOption_2->setChecked(true);
    }
    Q_ASSERT(ui->radioRunOption_1->isChecked() || ui->radioRunOption_2->isChecked() );
}

/***********************************************************************************
 ***********************************************************************************/
/*!
 * \brief Must detach the media from the playlist.
 */
bool CommandSettingsPage::isDetachable() const
{
    return ui->detachCheckBox->isChecked();
}

void CommandSettingsPage::setDetachable(bool detachable)
{
    ui->detachCheckBox->setChecked(detachable);
}

