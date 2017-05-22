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

#include "slider.h"

#include <QtGui/QWheelEvent>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleOptionSlider>

/*!
 * \class Slider
 * \brief A generic slider.
 *
 * Some boring features from the QSlider have been removed,
 * like the boring mousePressEvent and the boring wheelEvent.
 *
 */

Slider::Slider(QWidget *parent) : QSlider(parent)
{
}

Slider::Slider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent)
{
}

/*!
 * \reimp
 */
void Slider::mousePressEvent(QMouseEvent *event)
{
    if ((event->button() & style()->styleHint(QStyle::SH_Slider_PageSetButtons)) == event->button()) {
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QStyle::SubControl pressedControl
                = style()->hitTestComplexControl(QStyle::CC_Slider,
                                                 &opt, event->pos(), this);
        if (pressedControl == QStyle::SC_SliderGroove) {
            const QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
            const QPoint center = sliderRect.center() - sliderRect.topLeft();
            // to take half of the slider off for the setSliderPosition call we use the center - topLeft

            int pressValue = _q_pixelPosToRangeValue(_q_pick(event->pos() - center));
            setSliderPosition( pressValue );
            emit sliderMoved( pressValue );
            triggerAction(SliderMove);
            setRepeatAction(SliderNoAction);
            update();
            event->accept();
        }
    } else {
        event->ignore();
        return;
    }
    QSlider::mousePressEvent(event);
}

void Slider::mouseReleaseEvent(QMouseEvent *event)
{
    QSlider::mouseReleaseEvent(event);
}

void Slider::mouseMoveEvent(QMouseEvent *event)
{
    QSlider::mouseMoveEvent(event);
}


#ifndef QT_NO_WHEELEVENT
/*!
 * \reimp
 */
void Slider::wheelEvent(QWheelEvent *event)
{
    // It is very important that we ignore() the event
    // because we do not handle it,
    // so that the widget's parent can interpret it.
    //
    // Indeed, when we increase the volume with the mouse wheel
    // when the mouse is on the ProgressSlider, we do not want
    // the ProgressSlider to move the song's position ;)
    //
    event->ignore();
}
#endif

/***********************************************************************************
 * HACKS
 ***********************************************************************************/
int Slider::_q_pixelPosToRangeValue(int pos) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect gr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    int sliderMin, sliderMax, sliderLength;

    if (orientation() == Qt::Horizontal) {
        sliderLength = sr.width();
        sliderMin = gr.x();
        sliderMax = gr.right() - sliderLength + 1;
    } else {
        sliderLength = sr.height();
        sliderMin = gr.y();
        sliderMax = gr.bottom() - sliderLength + 1;
    }
    return QStyle::sliderValueFromPosition(opt.minimum, opt.maximum, pos - sliderMin,
                                           sliderMax - sliderMin, opt.upsideDown);
}

inline int Slider::_q_pick(const QPoint &pt) const
{
    return orientation() == Qt::Horizontal ? pt.x() : pt.y();
}

