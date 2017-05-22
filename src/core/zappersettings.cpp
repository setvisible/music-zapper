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

#include "zappersettings.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>

static char STR_DEFAULT_PATH[]  = "defaultPath";
static char STR_PLAYLIST_PATH[] = "playlistPath";
static char STR_GROUP_KEEP[]    = "keep";
static char STR_GROUP_ZAP[]     = "zap";
static char STR_GROUP_MOVE[]    = "move";
static char STR_COMMAND[]       = "command";
static char STR_PATH[]          = "path";
static char STR_IMMEDIAT[]      = "immediat";
static char STR_DETACHABLE[]    = "detachable";
static char STR_AUTOSAVE[]      = "autosave";


ZapperSettings::ZapperSettings()
    : keepSettings(CommandSettings())
    , zapSettings(CommandSettings())
    , moveSettings(CommandSettings())
    , playlistPath(QString())
    , autosaved(false)
{

}

void ZapperSettings::readSettings()
{
    QSettings settings;

    QString dir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    QString defPath  = QDir::toNativeSeparators(QString("%0/MuzicZapper/").arg(dir));
    QString playlist = QDir::toNativeSeparators(QString("%0/MuzicZapper/playlist.mzf").arg(dir));
    QString keepPath = QDir::toNativeSeparators(QString("%0/MuzicZapper/Saved").arg(dir));
    QString zapPath  = QDir::toNativeSeparators(QString("%0/MuzicZapper/Zapped").arg(dir));
    QString movePath = QDir::toNativeSeparators(QString("%0/MuzicZapper/Deleted").arg(dir));


    settings.beginGroup(STR_GROUP_KEEP);
    keepSettings.command = (Command)settings.value(STR_COMMAND   , 1).toInt();
    keepSettings.path             = settings.value(STR_PATH      , keepPath).toString();
    keepSettings.isImmediat       = settings.value(STR_IMMEDIAT  , true).toBool();
    keepSettings.isDetachable     = settings.value(STR_DETACHABLE, true).toBool();
    settings.endGroup();

    settings.beginGroup(STR_GROUP_ZAP);
    zapSettings.command = (Command)settings.value(STR_COMMAND   , 0).toInt();
    zapSettings.path             = settings.value(STR_PATH      , zapPath).toString();
    zapSettings.isImmediat       = settings.value(STR_IMMEDIAT  , true).toBool();
    zapSettings.isDetachable     = settings.value(STR_DETACHABLE, false).toBool();
    settings.endGroup();

    settings.beginGroup(STR_GROUP_MOVE);
    moveSettings.command = (Command)settings.value(STR_COMMAND   , 2).toInt();
    moveSettings.path             = settings.value(STR_PATH      , movePath).toString();
    moveSettings.isImmediat       = settings.value(STR_IMMEDIAT  , true).toBool();
    moveSettings.isDetachable     = settings.value(STR_DETACHABLE, true).toBool();
    settings.endGroup();

    defaultPath = settings.value(STR_DEFAULT_PATH, defPath).toString();
    playlistPath = settings.value(STR_PLAYLIST_PATH, playlist).toString();
    autosaved = settings.value(STR_AUTOSAVE, true).toBool();

}

void ZapperSettings::writeSettings()
{
    QSettings settings;

    settings.beginGroup(STR_GROUP_KEEP);
    settings.setValue(STR_COMMAND   , (int)keepSettings.command);
    settings.setValue(STR_PATH      ,      keepSettings.path);
    settings.setValue(STR_IMMEDIAT  ,      keepSettings.isImmediat);
    settings.setValue(STR_DETACHABLE,      keepSettings.isDetachable);
    settings.endGroup();

    settings.beginGroup(STR_GROUP_ZAP);
    settings.setValue(STR_COMMAND   , (int)zapSettings.command);
    settings.setValue(STR_PATH      ,      zapSettings.path);
    settings.setValue(STR_IMMEDIAT  ,      zapSettings.isImmediat);
    settings.setValue(STR_DETACHABLE,      zapSettings.isDetachable);
    settings.endGroup();

    settings.beginGroup(STR_GROUP_MOVE);
    settings.setValue(STR_COMMAND   , (int)moveSettings.command);
    settings.setValue(STR_PATH      ,      moveSettings.path);
    settings.setValue(STR_IMMEDIAT  ,      moveSettings.isImmediat);
    settings.setValue(STR_DETACHABLE,      moveSettings.isDetachable);
    settings.endGroup();

    settings.setValue(STR_DEFAULT_PATH, defaultPath);
    settings.setValue(STR_PLAYLIST_PATH, playlistPath);
    settings.setValue(STR_AUTOSAVE, autosaved);

}

