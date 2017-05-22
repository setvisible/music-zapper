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

#include "playercontrols.h"
#include "ui_playercontrols.h"


PlayerControls::PlayerControls(QWidget *parent) : QWidget(parent)
  , ui(new Ui::PlayerControls)
  , m_playerState(QMediaPlayer::StoppedState)
  , m_playerMuted(false)
{
    ui->setupUi(this);

    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(false);

    ui->volumeSlider->setRange(0, 100);

    connect(ui->playButton, SIGNAL(clicked()), this, SIGNAL(play()));
    connect(ui->pauseButton, SIGNAL(clicked()), this, SIGNAL(pause()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SIGNAL(stop()));
    connect(ui->nextButton, SIGNAL(clicked()), this, SIGNAL(next()));
    connect(ui->previousButton, SIGNAL(clicked()), this, SIGNAL(previous()));
    connect(ui->commuteButton, SIGNAL(clicked()), this, SIGNAL(commute()));
    connect(ui->muteButton, SIGNAL(clicked()), this, SLOT(muteClicked()));
    connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SIGNAL(changeVolume(int)));

    /// \todo implement QComboBox rateBox
    /// rateBox = new QComboBox(this);
    /// rateBox->addItem("0.5x", QVariant(0.5));
    /// rateBox->addItem("1.0x", QVariant(1.0));
    /// rateBox->addItem("2.0x", QVariant(2.0));
    /// rateBox->setCurrentIndex(1);
    ///
    /// connect(rateBox, SIGNAL(activated(int)), SLOT(updateRate()));

    ui->playButton->setCursor(Qt::PointingHandCursor);
    ui->pauseButton->setCursor(Qt::PointingHandCursor);
    ui->stopButton->setCursor(Qt::PointingHandCursor);
    ui->previousButton->setCursor(Qt::PointingHandCursor);
    ui->rateIncrButton->setCursor(Qt::PointingHandCursor);
    ui->rateDecrButton->setCursor(Qt::PointingHandCursor);
    ui->nextButton->setCursor(Qt::PointingHandCursor);
    ui->commuteButton->setCursor(Qt::PointingHandCursor);
    ui->muteButton->setCursor(Qt::PointingHandCursor);
    ui->volumeSlider->setCursor(Qt::PointingHandCursor);
}

PlayerControls::~PlayerControls()
{
    delete ui;
}


/***********************************************************************************
 ***********************************************************************************/
QMediaPlayer::State PlayerControls::state() const
{
    return m_playerState;
}

void PlayerControls::setState(QMediaPlayer::State state)
{
    if (state != m_playerState) {
        m_playerState = state;

        switch (state) {
        case QMediaPlayer::StoppedState:
            //ui->stopButton->setEnabled(false);
            ui->playButton->setEnabled(true);
            //ui->pauseButton->setEnabled(false);
            ui->stopButton->setChecked(true);
            ui->playButton->setChecked(false);
            ui->pauseButton->setChecked(false);
            break;
        case QMediaPlayer::PlayingState:
            ui->stopButton->setEnabled(true);
            //ui->playButton->setEnabled(false);
            ui->pauseButton->setEnabled(true);
            ui->stopButton->setChecked(false);
            ui->playButton->setChecked(true);
            ui->pauseButton->setChecked(false);
            break;
        case QMediaPlayer::PausedState:
            ui->stopButton->setEnabled(true);
            ui->playButton->setEnabled(true);
            //ui->pauseButton->setEnabled(false);
            ui->stopButton->setChecked(false);
            ui->playButton->setChecked(false);
            ui->pauseButton->setChecked(true);
            break;
        }
    }
}

int PlayerControls::volume() const
{
    return ui->volumeSlider ? ui->volumeSlider->value() : 0;
}

void PlayerControls::setVolume(int volume)
{
    if (ui->volumeSlider)
        ui->volumeSlider->setValue(volume);
}

bool PlayerControls::isMuted() const
{
    return m_playerMuted;
}

void PlayerControls::setMuted(bool muted)
{
    if (muted != m_playerMuted) {
        m_playerMuted = muted;
        ui->muteButton->setIcon(
                    QIcon(muted ? QLatin1String(":/icons/mute_on_20x20.png")
                                : QLatin1String(":/icons/mute_off_20x20.png")));
    }
}

void PlayerControls::muteClicked()
{
    emit changeMuting(!m_playerMuted);
}

qreal PlayerControls::playbackRate() const
{
    /// \todo implement playbackRate()
    /// return ui->rateBox->itemData(rateBox->currentIndex()).toDouble();
    return 1.0;
}

void PlayerControls::setPlaybackRate(float rate)
{
    Q_UNUSED(rate)
    /// \todo implement setPlaybackRate()
    ///  for (int i = 0; i < ui->rateBox->count(); ++i) {
    ///      if (qFuzzyCompare(rate, float(ui->rateBox->itemData(i).toDouble()))) {
    ///          ui->rateBox->setCurrentIndex(i);
    ///          return;
    ///      }
    ///  }
    ///
    ///  ui->rateBox->addItem(QString("%1x").arg(rate), QVariant(rate));
    ///  ui->rateBox->setCurrentIndex(rateBox->count() - 1);
}

void PlayerControls::updateRate()
{
    emit changeRate(playbackRate());
}
