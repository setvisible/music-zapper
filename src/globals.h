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

#ifndef GLOBALS_H
#define GLOBALS_H

#include "builddefs.h"
#include <QtCore/QString>

const QString STR_APPLICATION_NAME("MuzicZapper");
const QLatin1String STR_APPLICATION_VERSION("0.1 beta");
const QLatin1String STR_APPLICATION_DATE("2011-2017");
const QString STR_APPLICATION_AUTHOR("Sébastien Vavassori");

/*
 * Remark: the "Application Organization Name"
 * is the string that appears in the Windows registry.
 * It should contain only ASCII characters for maximizing
 * compatibility with Windows 7/8/10 and following.
 */
const QString STR_APPLICATION_ORGANIZATION("Sebastien Vavassori");


const QString STR_DISTANT_HELP("http://github.com/setvisible/muzic-zapper/src/tutorial/help-tutorial.html");


/* Something like "2017-05-20_15:36:58" */
const QString STR_APPLICATION_BUILD =
        QString("%1-%2-%3_%4:%5:%6")
        .arg(BUILD_YEAR, 4, 10, QChar('0'))
        .arg(BUILD_MONTH, 2, 10, QChar('0'))
        .arg(BUILD_DAY, 2, 10, QChar('0'))
        .arg(BUILD_HOUR, 2, 10, QChar('0'))
        .arg(BUILD_MIN, 2, 10, QChar('0'))
        .arg(BUILD_SEC, 2, 10, QChar('0'));


const QString STR_APP_QUICK_DESCRIPTION("MultiMedia Player with Zapping Buttons");

const QLatin1String STR_DEFAULT_PLAYLIST_MZF("[New Playlist]");
const QLatin1String STR_AUTOSAVED_PLAYLIST("[Auto-saved]");
const QLatin1String STR_MUSICZAPPER_PLAYLIST_MZF("MuzicZapper Playlist (*.mzf)");
const QLatin1String STR_MUSICZAPPER_PLAYLIST("MuzicZapper Playlist");


#endif // GLOBALS_H
