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

#include "volumeslider.h"

#include <QtWidgets/QStyle>

VolumeSlider::VolumeSlider(QWidget *parent) : Slider(parent)
{
    this->stylize();
}

VolumeSlider::VolumeSlider(Qt::Orientation orientation, QWidget *parent)
    : Slider(orientation, parent)
{
    this->stylize();
}

void VolumeSlider::stylize()
{
    this->setCursor(Qt::PointingHandCursor);
    this->setOrientation(Qt::Horizontal);

    QString sheet("  QSlider::groove:horizontal {                                      "
                  "      background-image: url(://icons/volume_background_18x49.png);  "
                  "  }                                                                 "
                  "  QSlider::handle:horizontal {                                      "
                  "      image: url(://icons/volume_cursor_10x18.png);                 "
                  "  }" );

    this->setStyleSheet(sheet);

    this->setMinimumSize(49, 18);
    this->setMaximumSize(49, 18);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

