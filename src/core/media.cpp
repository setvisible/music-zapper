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

#include "media.h"

#include <QtCore/QJsonObject>

static char STR_UNDEFINED[] = "undefined";
static char STR_KEEP[]      = "keep";
static char STR_ZAP[]       = "zap";
static char STR_MOVE[]      = "move";


Media::Media()
    : fullfilename(QString())
    , status(Status::Undefined)
    , error(NoError)
{

}

Media::Media(const QString  &_fullfilename, const Status _status)
    : fullfilename(_fullfilename)
    , status(_status)
    , error(NoError)
{

}

/* JSON Serialization */
void Media::read(const QJsonObject &json)
{
    fullfilename = json["path"].toString();
    status = stringToStatus( json["status"].toString() );
}

void Media::write(QJsonObject &json) const
{
    json["path"] = fullfilename;
    json["status"] = statusToString( status );
}

bool Media::operator==(const Media &other) const
{
    return (*this).fullfilename == other.fullfilename
            && (*this).status == other.status;

}
bool Media::operator!=(const Media &other) const
{
    return ((*this) == other) ? false : true;
}

Media::Status Media::stringToStatus(const QString &value)
{
    if (value == STR_KEEP) return Media::Keep;
    if (value == STR_ZAP)  return Media::Zap;
    if (value == STR_MOVE) return Media::Move;
    return Media::Undefined;
}

QString Media::statusToString(Media::Status value)
{
    switch (value) {
    case Media::Keep: return STR_KEEP; break;
    case Media::Zap : return STR_ZAP ; break;
    case Media::Move: return STR_MOVE; break;
    case Media::Undefined:
    default:
        break;
    }
    return STR_UNDEFINED;
}

