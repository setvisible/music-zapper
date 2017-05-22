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

#ifndef PLAYER_H
#define PLAYER_H

#include <QtCore/QObject>
#include <QtMultimedia/QMediaPlayer>

QT_BEGIN_NAMESPACE
class QMediaPlayer;
class QVideoWidget;
class QJsonObject;
QT_END_NAMESPACE

class Playlist;
class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    //Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(QMediaPlayer::State state READ state NOTIFY stateChanged)

public:
    explicit Player(QObject *parent = Q_NULLPTR);
    ~Player();

    void clear();

    /* JSON Serialization */
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;


    bool isMetaDataAvailable() const;

    QVariant metaData(const QString &key) const;
    QStringList availableMetaData() const;

    void setVideoOutput(QVideoWidget *widget);

    QMediaPlayer::State state() const;

    qint64 duration() const;
    //qint64 position() const;

    int volume() const;
    bool isMuted() const;
    bool isAudioAvailable() const;
    bool isVideoAvailable() const;

    QString errorString() const;

    Playlist *playlist() const;

    void addToPlaylist(const QList<QUrl> urls);

    bool isPlayerAvailable() const;
    QStringList supportedMimeTypes() const;


public Q_SLOTS:
    void play();
    void pause();
    void stop();

    void next();
    void previous();

    //void setPosition(qint64 position);
    void setVolume(int volume);
    void setMuted(bool muted);

    void setPlaybackRate(qreal rate);

    void seek(int seconds);
    void jump(const QModelIndex &index);


Q_SIGNALS:
    void metaDataAvailableChanged(bool available);
    void metaDataChanged();
    void metaDataChanged(const QString &key, const QVariant &value);

    void mediaChanged(const QMediaContent &media);
    void currentMediaChanged(const QMediaContent &media);

    void stateChanged(QMediaPlayer::State newState);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);

    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);

    void volumeChanged(int volume);
    void mutedChanged(bool muted);
    void audioAvailableChanged(bool available);
    void videoAvailableChanged(bool videoAvailable);

    void bufferStatusChanged(int percentFilled);

    void error(QMediaPlayer::Error error);


    void mediaChanged();

private Q_SLOTS:
    void onMediaChanged(int start, int end);

private:
    QMediaPlayer *m_player;
    Playlist *m_playlist;

};

#endif // PLAYER_H
