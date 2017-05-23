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

#include "player.h"

#include <Core/Playlist>

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>
#include <QtCore/QModelIndex>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>

Player::Player(QObject *parent) : QObject(parent)
  , m_player(new QMediaPlayer(this))
  , m_playlist(new Playlist(this))
{

    m_player->setPlaylist(m_playlist);

    connect(m_player, SIGNAL(metaDataAvailableChanged(bool)), SIGNAL(metaDataAvailableChanged(bool)));
    connect(m_player, SIGNAL(metaDataChanged()), SIGNAL(metaDataChanged()));
    connect(m_player, SIGNAL(metaDataChanged(QString,QVariant)), SIGNAL(metaDataChanged(QString,QVariant)));


    connect(m_player, SIGNAL(mediaChanged(QMediaContent)), SIGNAL(mediaChanged(QMediaContent)));
    connect(m_player, SIGNAL(currentMediaChanged(QMediaContent)), this, SLOT(onCurrentMediaChanged(QMediaContent)));

    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)),
            SIGNAL(stateChanged(QMediaPlayer::State)));
    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    connect(m_player, SIGNAL(durationChanged(qint64)), SIGNAL(durationChanged(qint64)));
    connect(m_player, SIGNAL(positionChanged(qint64)), SIGNAL(positionChanged(qint64)));

    connect(m_player, SIGNAL(volumeChanged(int)), SIGNAL(volumeChanged(int)));
    connect(m_player, SIGNAL(mutedChanged(bool)), SIGNAL(mutedChanged(bool)));
    connect(m_player, SIGNAL(audioAvailableChanged(bool)), SIGNAL(audioAvailableChanged(bool)));
    connect(m_player, SIGNAL(videoAvailableChanged(bool)), SIGNAL(videoAvailableChanged(bool)));

    connect(m_player, SIGNAL(bufferStatusChanged(int)), SIGNAL(bufferStatusChanged(int)));

    connect(m_player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onError(QMediaPlayer::Error)));

    connect(m_playlist, SIGNAL(mediaInserted(int,int)), this, SLOT(onMediaChanged(int,int)));
    connect(m_playlist, SIGNAL(mediaRemoved(int,int)), this, SLOT(onMediaChanged(int,int)));
    connect(m_playlist, SIGNAL(mediaChanged(int,int)), this, SLOT(onMediaChanged(int,int)));

}

Player::~Player()
{

}

void Player::clear()
{
    m_playlist->clear();
}


Playlist *Player::playlist() const
{
    return m_playlist;
}

/***********************************************************************************
 ***********************************************************************************/
/* JSON Serialization */
void Player::read(const QJsonObject &json)
{
    m_playlist->read(json);
}

void Player::write(QJsonObject &json) const
{
    m_playlist->write(json);
}


/***********************************************************************************
 ***********************************************************************************/
/*! \internal
 * \brief Check for ".m3u" playlists.
 */
static bool isPlaylist(const QUrl &url)
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void Player::addToPlaylist(const QList<QUrl> urls)
{
    foreach (const QUrl &url, urls) {
        if (isPlaylist(url))
            m_playlist->load(url);
        else
            m_playlist->addMedia(url);
    }
}

/***********************************************************************************
 ***********************************************************************************/
void Player::play()
{
    m_player->play();
}

void Player::pause()
{
    m_player->pause();
}

void Player::stop()
{
    m_player->stop();
}

void Player::next()
{
    m_playlist->next();
}

void Player::previous()
{
    /*
     * Go to previous track if we are within the first 5 seconds of playback.
     * Otherwise, seek to the beginning.
     */
    if (m_player->position() <= 5000)
        m_playlist->previous();
    else
        m_player->setPosition(0);
}

/***********************************************************************************
 ***********************************************************************************/
bool Player::isPlayerAvailable() const
{
    return m_player->isAvailable();
    m_player->supportedMimeTypes();
}

QStringList Player::supportedMimeTypes() const
{
    return m_player->supportedMimeTypes();
}


/***********************************************************************************
 ***********************************************************************************/
QString Player::errorString() const
{
    return m_player->errorString();
}

/***********************************************************************************
 ***********************************************************************************/
bool Player::isMetaDataAvailable() const
{
    return m_player->isMetaDataAvailable();
}

QVariant Player::metaData(const QString &key) const
{
    return m_player->metaData(key);
}

QStringList Player::availableMetaData() const
{
    return m_player->availableMetaData();
}

/***********************************************************************************
 ***********************************************************************************/
//qint64 Player::position() const
//{
//    return m_player->duration();
//}
//
//void Player::setPosition(qint64 position)
//{
//    m_player->setPosition(position);
//}

qint64 Player::duration() const
{
    return m_player->position();
}

QMediaPlayer::State Player::state() const
{
    return m_player->state();
}

void Player::setVideoOutput(QVideoWidget *widget)
{
    m_player->setVideoOutput(widget);
}

/***********************************************************************************
 ***********************************************************************************/
int Player::volume() const
{
    return m_player->volume();
}

void Player::setVolume(int volume)
{
    m_player->setVolume(volume);
}

/***********************************************************************************
 ***********************************************************************************/

bool Player::isMuted() const
{
    return m_player->isMuted();
}

void Player::setMuted(bool muted)
{
    m_player->setMuted(muted);
}

bool Player::isAudioAvailable() const
{
    return m_player->isAudioAvailable();
}
bool Player::isVideoAvailable() const
{
    return m_player->isVideoAvailable();
}


void Player::setPlaybackRate(qreal rate)
{
    m_player->setPlaybackRate(rate);
}

/***********************************************************************************
 ***********************************************************************************/
void Player::seek(int seconds)
{
    m_player->setPosition(seconds * 1000);
}

void Player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        m_playlist->setCurrentIndex(index.row());
        m_player->play();
    }
}

/***********************************************************************************
 ***********************************************************************************/
void Player::onMediaChanged(int /*start*/, int /*end*/)
{
    /* It simplifies the signal signature  */
    /* by removing the useless arguments.  */
    emit mediaChanged();
}

/***********************************************************************************
 ***********************************************************************************/
static inline Media::Error toMediaError(QMediaPlayer::Error e)
{
    switch (e) {
    case QMediaPlayer::NoError:              return Media::NoError; break;
    case QMediaPlayer::ResourceError:        return Media::ResourceError; break;
    case QMediaPlayer::FormatError:          return Media::FormatError; break;
    case QMediaPlayer::NetworkError:         return Media::NetworkError; break;
    case QMediaPlayer::AccessDeniedError:    return Media::AccessDeniedError; break;
    case QMediaPlayer::ServiceMissingError:  return Media::ServiceMissingError; break;
    default:
        Q_UNREACHABLE();
        break;
    }
    return Media::NoError;
}

void Player::onCurrentMediaChanged(const QMediaContent &media)
{
    m_playlist->setMediaError(m_playlist->currentIndex(), Media::NoError);
    emit currentMediaChanged(media);
}

void Player::onError(QMediaPlayer::Error e)
{
    m_playlist->setMediaError(m_playlist->currentIndex(), toMediaError(e));
    emit error(e);
}
