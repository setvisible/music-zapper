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

#ifdef Q_OS_WIN

#include "windows.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

void MoveToTrashImpl(const QString &filename)
{
    QFileInfo fileinfo( filename );

    if (!fileinfo.exists()) {
        qDebug() << "File doesnt exists, cant move to trash";

        /// \todo Implement a exception ?
        /// throw MuzicZapper::Core::Exception( "File doesnt exists, cant move to trash" );
    }

    WCHAR from[ MAX_PATH ];
    memset( from, 0, sizeof( from ));

    int length = fileinfo.absoluteFilePath().toWCharArray( from );
    Q_ASSERT( 0 <= length && length < MAX_PATH );
    from[length] = '\0';

    SHFILEOPSTRUCT fileop;
    memset( &fileop, 0, sizeof( fileop ) );

    fileop.wFunc = FO_DELETE;
    fileop.pFrom = from;
    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

    int rv = SHFileOperation( &fileop );

    if (rv != 0) {
        qDebug() << rv << QString::number( rv ).toInt( 0, 8 );

        /// \todo Implement a exception ?
        /// throw MuzicZapper::Core::Exception( "move to trash failed" );
    }
}

#endif
