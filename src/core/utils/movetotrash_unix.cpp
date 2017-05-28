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

//
// Source:
// http://stackoverflow.com/questions/17964439/move-files-to-trash-recycle-bin-in-qt
//

#include "movetotrash.h"

/// \todo implement MoveToTrashImpl() for linux

#ifdef Q_OS_LINUX

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>

bool TrashInitialized = false;
QString TrashPath;
QString TrashPathInfo;
QString TrashPathFiles;

void MoveToTrashImpl(const QString &filename)
{
#ifdef QT_GUI_LIB
    if( !TrashInitialized ){
        QStringList paths;
        const char* xdg_data_home = getenv( "XDG_DATA_HOME" );
        if( xdg_data_home ){
            qDebug() << "XDG_DATA_HOME not yet tested";
            QString xdgTrash( xdg_data_home );
            paths.append( xdgTrash + "/Trash" );
        }
        QString home = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
        paths.append( home + "/.local/share/Trash" );
        paths.append( home + "/.trash" );
        foreach( QString path, paths ){
            if( TrashPath.isEmpty() ){
                QDir dir( path );
                if( dir.exists() ){
                    TrashPath = path;
                }
            }
        }
        if( TrashPath.isEmpty() ) {
            qDebug() << "Cant detect trash folder" ;
            /// \todo Implement a exception ?
            /// throw MuzicZapper::Core::Exception( "Cant detect trash folder" );
        }
        TrashPathInfo = TrashPath + "/info";
        TrashPathFiles = TrashPath + "/files";
        if( !QDir( TrashPathInfo ).exists() || !QDir( TrashPathFiles ).exists() ) {
            qDebug() << "Trash doesnt looks like FreeDesktop.org Trash specification" ;
            /// \todo Implement a exception ?
            /// throw MuzicZapper::Core::Exception( "Trash doesnt looks like FreeDesktop.org Trash specification" );
        }
        TrashInitialized = true;
    }

    QFileInfo original( filename );
    if( !original.exists() ) {
        qDebug() << "File doesnt exists, cant move to trash" ;
        /// \todo Implement a exception ?
        /// throw MuzicZapper::Core::Exception( "File doesnt exists, cant move to trash" );
    }

    QString info;
    info += "[Trash Info]\nPath=";
    info += original.absoluteFilePath();
    info += "\nDeletionDate=";
    info += QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzzZ");
    info += "\n";

    QString trashname = original.fileName();
    QString infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
    QString filepath = TrashPathFiles + "/" + trashname;

    int nr = 1;
    while( QFileInfo( infopath ).exists() || QFileInfo( filepath ).exists() ){
        nr++;
        trashname = original.baseName() + "." + QString::number( nr );
        if( !original.completeSuffix().isEmpty() ){
            trashname += QString( "." ) + original.completeSuffix();
        }
        infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
        filepath = TrashPathFiles + "/" + trashname;
    }

    QDir dir;
    if( !dir.rename( original.absoluteFilePath(), filepath ) ){
        qDebug() << "move to trash failed" ;
        /// \todo Implement a exception ?
        /// throw MuzicZapper::Core::Exception( "move to trash failed" );
    }

    /* Write the .trashinfo file */
    QFile file(infopath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << info << "\n";

#else
    Q_UNUSED( filename );
    qDebug() << "Trash in server-mode not supported" ;
    /// \todo Implement a exception ?
    /// throw MuzicZapper::Core::Exception( "Trash in server-mode not supported" );
#endif
}
#endif
