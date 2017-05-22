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

#include "playlist.h"

#include <Core/PlaylistModel>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QString>


/*!
 * \class Playlist
 * \brief The class Playlist is a container for a playlist document.
 *
 * Use \a read() and \a write() to serialize to JSON format.
 */

Playlist::Playlist(QObject *parent) : QMediaPlaylist(parent)
  , m_playlistModel(new PlaylistModel(this))
{
    m_playlistModel->setPlaylist(this); // weird...

    connect(this, SIGNAL(mediaInserted(int,int)), SLOT(onMediaInserted(int,int)));
    connect(this, SIGNAL(mediaRemoved(int,int)), SLOT(onMediaRemoved(int,int)));
    connect(this, SIGNAL(mediaChanged(int,int)), SLOT(onMediaChanged(int,int)));

}

/***********************************************************************************
 ***********************************************************************************/
PlaylistModel *Playlist::model() const
{
    return m_playlistModel;
}

/***********************************************************************************
 ***********************************************************************************/
void Playlist::onMediaInserted(int start, int end)
{
    Q_ASSERT(start >= 0);
    Q_ASSERT(start <= end);
    Q_ASSERT(end < mediaCount());

    for (int index = start; index <= end; ++index) {
        const QMediaContent &content = media(index);
        Media item(content.canonicalUrl().path());
        m_medias.insert(index, item);
    }
}

void Playlist::onMediaRemoved(int start, int end)
{
    Q_ASSERT(start >= 0);
    Q_ASSERT(start <= end);
    Q_ASSERT(end < m_medias.count());

    m_medias.erase(m_medias.begin()+start, m_medias.begin()+end+1);
}

void Playlist::onMediaChanged(int /*start*/, int /*end*/)
{
}

/***********************************************************************************
 ***********************************************************************************/
/* JSON Serialization */
void Playlist::read(const QJsonObject &json)
{
    m_medias.clear();

    QJsonArray mediaArray = json["medias"].toArray();
    for (int i = 0; i < mediaArray.size(); ++i) {
        QJsonObject mediaObject = mediaArray[i].toObject();
        Media media;
        media.read(mediaObject);

        QUrl url = QUrl::fromLocalFile(media.fullfilename);
        addMedia(url);
    }
}

void Playlist::write(QJsonObject &json) const
{
    QJsonArray mediaArray;
    foreach (const Media media, m_medias) {
        QJsonObject mediaObject;
        media.write(mediaObject);
        mediaArray.append(mediaObject);
    }
    json["medias"] = mediaArray;
}
