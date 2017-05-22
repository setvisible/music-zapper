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

#ifndef PLAYER_WIDGET_H
#define PLAYER_WIDGET_H

#include <QtMultimedia/QMediaPlayer>
#include <QtWidgets/QWidget>

namespace Ui {
class PlayerWidget;
}

class Player;
class PlaylistModel;

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = 0);
    ~PlayerWidget();

    Player *player() const;
    void setPlayer(Player *mediaPlayer);

Q_SIGNALS:
    void fullScreenChanged(bool fullScreen);    
    void mediaChanged();
    void commute();
    void stopped();

public Q_SLOTS:
    void scrollVolume(int numSteps);

private Q_SLOTS:
    void metaDataChanged();
    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(int progress);
    void displayErrorMessage();

private:
    Ui::PlayerWidget *ui;

    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);

    Player *m_player;
    PlaylistModel *m_playlistModel;
    
    QString m_trackInfo;
    QString m_statusInfo;
};

#endif // PLAYER_WIDGET_H
