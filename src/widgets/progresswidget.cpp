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

#include "progresswidget.h"
#include "ui_progresswidget.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>

ProgressWidget::ProgressWidget(QWidget *parent) : QWidget(parent)
  , ui(new Ui::ProgressWidget)
  , m_duration(0)
{
    ui->setupUi(this);

    updateDurationInfo(m_duration / 1000);
    ui->slider->setRange(0, m_duration / 1000);

    connect(ui->slider, SIGNAL(sliderMoved(int)), SIGNAL(sliderMoved(int)));
}

ProgressWidget::~ProgressWidget()
{
    delete ui;
}

/*! \fn void ProgressSlider::sliderMoved(int position)
 *  \brief The signal sliderMoved() is emitted when the user drags the slider.
 */

void ProgressWidget::sliderPressed()
{
    int position = ui->slider->value();
    qDebug() << Q_FUNC_INFO << position;
    emit sliderMoved(position);
}

void ProgressWidget::durationChanged(qint64 duration)
{
    m_duration = duration/1000;
    ui->slider->setMaximum(duration / 1000);
}

void ProgressWidget::positionChanged(qint64 progress)
{
    if (!ui->slider->isSliderDown()) {
        ui->slider->setValue(progress / 1000);
    }
    updateDurationInfo(progress / 1000);
}

void ProgressWidget::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {

        QTime currentTime((currentInfo/3600)%60,
                          (currentInfo/60)%60,
                          (currentInfo%60),
                          (currentInfo*1000)%1000);

        QTime totalTime((m_duration/3600)%60,
                        (m_duration/60)%60,
                        (m_duration%60),
                        (m_duration*1000)%1000);

        QString format = "mm:ss";
        if (m_duration > 3600) {
            format = "hh:mm:ss";
        }
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    ui->durationLabel->setText(tStr);
}


