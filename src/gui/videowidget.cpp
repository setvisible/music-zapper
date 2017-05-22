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

#include "videowidget.h"

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QBitmap>
#include <QtGui/QPixmap>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>


VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent)
  , m_thumb(new QLabel(this))
  , m_video(new MyVideoWidget(this))
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_thumb);
    layout->addWidget(m_video);
    layout->setMargin(0);
    this->setLayout(layout);

    QString sheet("QLabel { background-color: black; }" );
    m_thumb->setStyleSheet(sheet);

    setThumbVisible(true);
}

void VideoWidget::setThumbImage(const QString &image)
{
    QPixmap pixmap(image);
    m_thumb->setPixmap(pixmap);
    m_thumb->setAlignment(Qt::AlignCenter);
}

void VideoWidget::setThumbVisible(bool visible)
{
    m_thumb->setVisible(visible);
    m_video->setVisible(!visible);
}

QVideoWidget *VideoWidget::videoWidget() const
{
    return m_video;
}

/***********************************************************************************
 ***********************************************************************************/

MyVideoWidget::MyVideoWidget(QWidget *parent) : QVideoWidget(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black);
    setPalette(p);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void MyVideoWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen()) {
        setFullScreen(false);
        event->accept();
    } else if (event->key() == Qt::Key_Enter && event->modifiers() & Qt::Key_Alt) {
        setFullScreen(!isFullScreen());
        event->accept();
    } else {
        QVideoWidget::keyPressEvent(event);
    }
}

void MyVideoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    setFullScreen(!isFullScreen());
    event->accept();
}

void MyVideoWidget::mousePressEvent(QMouseEvent *event)
{
    QVideoWidget::mousePressEvent(event);
}

