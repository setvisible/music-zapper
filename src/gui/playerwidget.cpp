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

#include "playerwidget.h"
#include "ui_playerwidget.h"

#include <Core/Player>
#include <Core/Playlist>
#include <Core/PlaylistModel>

#include <QtMultimedia/QMediaMetaData>
#include <QtWidgets/QMessageBox>

PlayerWidget::PlayerWidget(QWidget *parent): QWidget(parent)
  , ui(new Ui::PlayerWidget)
  , m_player(Q_NULLPTR)
{
    ui->setupUi(this);

    ui->coverLabel->setText(QString());
    ui->statusLabel->setText(QString());

    ui->progressWidget->setMinimumHeight(0);

}

PlayerWidget::~PlayerWidget()
{
    delete ui;
}

/***********************************************************************************
 ***********************************************************************************/
/*! \brief Each step represents a volume change by 5%.
 */
void PlayerWidget::scrollVolume(int numSteps)
{
    Q_ASSERT(m_player);
    int volume = m_player->volume(); // from 0 to 100
    volume += 5 * numSteps;
    m_player->setVolume(volume);
}


/***********************************************************************************
 ***********************************************************************************/
Player *PlayerWidget::player() const
{
    return m_player;
}

void PlayerWidget::setPlayer(Player *mediaPlayer)
{
    Q_ASSERT(mediaPlayer);
    Q_ASSERT(m_player == Q_NULLPTR);

    m_player = mediaPlayer;

    connect(m_player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(m_player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));

    connect(m_player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));

    connect(ui->progressWidget, SIGNAL(sliderMoved(int)), m_player, SLOT(seek(int)));
    connect(m_player, SIGNAL(durationChanged(qint64)), ui->progressWidget, SLOT(durationChanged(qint64)));
    connect(m_player, SIGNAL(positionChanged(qint64)), ui->progressWidget, SLOT(positionChanged(qint64)));


    ui->controls->setState(m_player->state());
    ui->controls->setVolume(m_player->volume());
    ui->controls->setMuted(ui->controls->isMuted());

    connect(ui->controls, SIGNAL(play()), m_player, SLOT(play()));
    connect(ui->controls, SIGNAL(pause()), m_player, SLOT(pause()));
    connect(ui->controls, SIGNAL(stop()), m_player, SLOT(stop()));
    connect(ui->controls, SIGNAL(next()), m_player, SLOT(next()));
    connect(ui->controls, SIGNAL(previous()), m_player, SLOT(previous()));
    connect(ui->controls, SIGNAL(changeVolume(int)), m_player, SLOT(setVolume(int)));
    connect(ui->controls, SIGNAL(changeMuting(bool)), m_player, SLOT(setMuted(bool)));
    connect(ui->controls, SIGNAL(changeRate(qreal)), m_player, SLOT(setPlaybackRate(qreal)));
    connect(ui->controls, SIGNAL(commute()), SIGNAL(commute()));

    connect(ui->controls, SIGNAL(stop()), SIGNAL(stopped()));

    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)),
            ui->controls, SLOT(setState(QMediaPlayer::State)));
    connect(m_player, SIGNAL(volumeChanged(int)), ui->controls, SLOT(setVolume(int)));
    connect(m_player, SIGNAL(mutedChanged(bool)), ui->controls, SLOT(setMuted(bool)));

    ui->fullScreenButton->setCheckable(true);

    if (!m_player->isPlayerAvailable()) {
        QMessageBox::warning(this,
                             tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"\
                                "Please check the media service plugins are installed."));
        ui->controls->setEnabled(false);
        ui->fullScreenButton->setEnabled(false);
    }

    ui->coverLabel->setText(QString());

    metaDataChanged();


}


/***********************************************************************************
 ***********************************************************************************/
void PlayerWidget::metaDataChanged()
{
    Q_ASSERT(m_player);
    if (m_player->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                     .arg(m_player->metaData(QMediaMetaData::AlbumArtist).toString())
                     .arg(m_player->metaData(QMediaMetaData::Title).toString()));

        if (ui->coverLabel) {
            QUrl url = m_player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

            ui->coverLabel->setPixmap(!url.isEmpty()
                                      ? QPixmap(url.toString())
                                      : QPixmap());
        }
    }
}


void PlayerWidget::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);

    // handle status message
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Media Stalled"));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}

void PlayerWidget::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia ||
            status == QMediaPlayer::BufferingMedia ||
            status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void PlayerWidget::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%").arg(progress));
}

void PlayerWidget::setTrackInfo(const QString &info)
{
    m_trackInfo = info;
    if (!m_statusInfo.isEmpty())
        ui->statusLabel->setText(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
    else
        ui->statusLabel->setText(m_trackInfo);
}

void PlayerWidget::setStatusInfo(const QString &info)
{
    m_statusInfo = info;
    if (!m_statusInfo.isEmpty())
        ui->statusLabel->setText(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
    else
        ui->statusLabel->setText(m_trackInfo);
}

void PlayerWidget::displayErrorMessage()
{
    Q_ASSERT(m_player);
    setStatusInfo(m_player->errorString());
}

