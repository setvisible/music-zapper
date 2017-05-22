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

#include "progressslider.h"

#include <QtWidgets/QStyle>

ProgressSlider::ProgressSlider(QWidget *parent) : Slider(parent)
{
    this->stylize();
}

ProgressSlider::ProgressSlider(Qt::Orientation orientation, QWidget *parent)
    : Slider(orientation, parent)
{
    this->stylize();
}

void ProgressSlider::stylize()
{    
    this->setCursor(Qt::PointingHandCursor);
    this->setOrientation(Qt::Horizontal);

    QString sheet("  QSlider::groove:horizontal {                           "
                  "      border-style: inset;                               "
                  "      border-width: 1px;                                 "
                  "      border-color: darkgrey;                            "
                  "      background-color: white;                           "
                  "      height: 5px;                                       "
                  "      margin-left: 7px 0;                                "
                  "      margin-right: 7px 0;                               "
                  "      padding-left: -6px;                                "
                  "      padding-right: -6px;                               "
                  "  }                                                      "
                  "                                                         "
                  "  QSlider::handle:horizontal {                           "
                  "      image: url(://icons/progress_cursor_13x15.png);    "
                  "      margin: -7px 0;                                    "
                  "  }" );

    this->setStyleSheet(sheet);
}

