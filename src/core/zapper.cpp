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

#include "zapper.h"

#include "utils/movetotrash.h"
#include <Core/Player>
#include <Core/Playlist>
#include <Core/ZapperSettings>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtMultimedia/QMediaPlaylist>
#include <QtMultimedia/QMediaContent>


Zapper::Zapper(QObject *parent) : QObject(parent)
  , m_player(Q_NULLPTR)
{

}


/***********************************************************************************
 ***********************************************************************************/
Player *Zapper::player() const
{
    return m_player;
}

void Zapper::setPlayer(Player *player)
{
    m_player = player;
}

/***********************************************************************************
 ***********************************************************************************/
QString Zapper::defaultMediaPath() const
{
    return m_settings.defaultPath;
}

QString Zapper::playlistPath() const
{
    return m_settings.playlistPath;
}

bool Zapper::isAutosave() const
{
    return m_settings.autosaved;
}


/***********************************************************************************
 ***********************************************************************************/
void Zapper::readSettings()
{
    m_settings.readSettings();
}

void Zapper::writeSettings()
{
    m_settings.writeSettings();
}

/***********************************************************************************
 ***********************************************************************************/
void Zapper::processCurrentSong(CommandSettings settings)
{
    qDebug() << Q_FUNC_INFO;

    const int index = m_player->playlist()->currentIndex();
    const QMediaContent media = m_player->playlist()->media(index);
    const QUrl url = media.canonicalUrl();

    if (!url.isLocalFile())
        return;

    switch (settings.command) {
    case Command::MoveToPath:
    {
        const QString source = QDir::toNativeSeparators(url.toLocalFile());
        const QFileInfo fi(source);
        QString destination = settings.path + QDir::separator() + fi.fileName();

        qDebug() << Q_FUNC_INFO <<  source << "to" << destination;

        int i = 0;
        while (QFileInfo::exists(destination)) {
            i++;
            destination = settings.path + QDir::separator() +
                    fi.completeBaseName() + QString(" (%0).").arg(i) +
                    fi.completeSuffix();
        }

        const bool ok = QFile::rename(source, destination);
        if (ok) {
        } else {

            //  m_player->playlist()->next();
            qWarning() << "error : file not moved.";
            return;
        }

    }

        break;
    case Command::MoveToSystemBin:
    {
        // gtk/util.c
        // #include <gio/gio.h> /* g_file_trash () */

        const QString source = QDir::toNativeSeparators(url.toLocalFile());
        qDebug() << Q_FUNC_INFO <<  "deleting " << source;


        m_player->playlist()->removeMedia(index);
        m_player->playlist()->setCurrentIndex(index);

        MoveToTrashImpl(source);

        //     QFile file (url.toLocalFile());
        //     file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        //     if (!file.remove()) {
        //         qDebug() << Q_FUNC_INFO <<  "error";
        //     }
        return;

    }
        /// \todo to be implemented...


        /*
        // DotNet\corefx-master\src\System.IO.FileSystem\src\System\IO\DirectoryInfo.cs
        using System.Collections.Generic;
        using System.Diagnostics;
        using System.Diagnostics.Contracts;
        using System.Security;
        namespace System.IO
        {
            public sealed partial class DirectoryInfo : FileSystemInfo
            {
                    [System.Security.SecuritySafeCritical]
                    public override void Delete()
                    {
                        FileSystem.Current.RemoveDirectory(FullPath, false);
                    }

                    [System.Security.SecuritySafeCritical]
                    public void Delete(bool recursive)
                    {
                        FileSystem.Current.RemoveDirectory(FullPath, recursive);
                    }
              }
        }

         */

        break;
    case Command::NoAction:
    default:
        break;
    }

    if (settings.isImmediat) {

        /// \todo to be implemented...

    }

    if (settings.isDetachable) {
        m_player->playlist()->removeMedia(index);
        m_player->playlist()->setCurrentIndex(index);
    }

}

/***********************************************************************************
 ***********************************************************************************/
void Zapper::keepCurrentSong()
{
    processCurrentSong(m_settings.keepSettings);
}

void Zapper::zapCurrentSong()
{
    processCurrentSong(m_settings.zapSettings);
}

void Zapper::moveCurrentSong()
{
    processCurrentSong(m_settings.moveSettings);
}

/***********************************************************************************
 ***********************************************************************************/
void Zapper::undo()
{
    /// \todo to be implemented...
}

