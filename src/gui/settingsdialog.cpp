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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <Core/ZapperSettings>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMessageBox>

static char STR_CURRENT_INDEX[]  = "currentTabPage";

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(clicked(QAbstractButton*)));

    connect(ui->autosaveCheckBox, SIGNAL(toggled(bool)),
            ui->playlistPathLabel, SLOT(setEnabled(bool)));
    connect(ui->autosaveCheckBox, SIGNAL(toggled(bool)),
            ui->playlistPathLineEdit, SLOT(setEnabled(bool)));
    ui->autosaveCheckBox->toggle();

    readSettings();    
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::closeEvent(QCloseEvent *)
{
    /* Write the tab index only */
    QSettings local_settings;
    local_settings.setValue(STR_CURRENT_INDEX, ui->tabWidget->currentIndex());
}

void SettingsDialog::accept()
{
    ensureDirectoryExists(ui->keepPage->settings().path);
    ensureDirectoryExists(ui->zapPage->settings().path);
    ensureDirectoryExists(ui->movePage->settings().path);
    ensureDirectoryExists(ui->defaultMediaPathlineEdit->text());

    /* Write all settings */
    writeSettings();
    QDialog::accept();
}

/***********************************************************************************
 ***********************************************************************************/
void SettingsDialog::clicked(QAbstractButton *button)
{
    const QDialogButtonBox::ButtonRole role = ui->buttonBox->buttonRole(button);
    if (role == QDialogButtonBox::ResetRole) {
        QSettings settings;
        settings.clear();
        readSettings();
    }
}

/***********************************************************************************
 ***********************************************************************************/
void SettingsDialog::readSettings()
{
    ZapperSettings settings;
    settings.readSettings();
    ui->keepPage->setSettings(settings.keepSettings);
    ui->zapPage->setSettings(settings.zapSettings);
    ui->movePage->setSettings(settings.moveSettings);
    ui->defaultMediaPathlineEdit->setText(settings.defaultPath);
    ui->playlistPathLineEdit->setText(settings.playlistPath);
    ui->autosaveCheckBox->setChecked(settings.autosaved);

    QSettings local_settings;
    ui->tabWidget->setCurrentIndex(local_settings.value(STR_CURRENT_INDEX, 0).toInt());
}

void SettingsDialog::writeSettings()
{
    ZapperSettings settings;
    settings.keepSettings = ui->keepPage->settings();
    settings.zapSettings  = ui->zapPage->settings();
    settings.moveSettings = ui->movePage->settings();
    settings.defaultPath = ui->defaultMediaPathlineEdit->text();
    settings.playlistPath = ui->playlistPathLineEdit->text();
    settings.autosaved = ui->autosaveCheckBox->isChecked();
    settings.writeSettings();

    QSettings local_settings;
    local_settings.setValue(STR_CURRENT_INDEX, ui->tabWidget->currentIndex());


}

/***********************************************************************************
 ***********************************************************************************/
void SettingsDialog::ensureDirectoryExists(const QString &path)
{
    if (path.isEmpty())
        return;

    QDir target(path);
    if (target.exists())
        return;

    bool ok = target.mkpath(target.path());
    if (!ok) {
        QMessageBox::warning(
                    this, tr("Directory"),
                    tr("Error: Cannot create the following directory:\n\n%0").arg(path),
                    QMessageBox::Ok);
    } else {
        QMessageBox::information(
                    this, tr("Directory"),
                    tr("The following directory has been created:\n\n%0").arg(path),
                    QMessageBox::Ok);
    }
}


