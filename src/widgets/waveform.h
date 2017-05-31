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

#ifndef WAVE_FORM_H
#define WAVE_FORM_H

#include <QtMultimedia/QAudioFormat>
#include <QtGui/QPixmap>
#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>

/**
 * Widget which displays a section of the audio waveform.
 *
 * The waveform is rendered on a set of QPixmaps which form a group of tiles
 * whose extent covers the widget.  As the audio position is updated, these
 * tiles are scrolled from left to right; when the left-most tile scrolls
 * outside the widget, it is moved to the right end of the tile array and
 * painted with the next section of the waveform.
 */
class WaveForm : public QWidget
{
    Q_OBJECT

public:
    explicit WaveForm(QWidget *parent = 0);
    ~WaveForm();

    // QWidget
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    void initialize(const QAudioFormat &format, qint64 audioBufferSize, qint64 windowDurationUs);
    void reset();

    void setAutoUpdatePosition(bool enabled);

public slots:
    void bufferChanged(qint64 position, qint64 length, const QByteArray &buffer);
    void audioPositionChanged(qint64 position);

private:
    static const int NullIndex = -1;

    void deletePixmaps();

    /*
     * (Re)create all pixmaps, repaint and update the display.
     * Triggers an update();
     */
    void createPixmaps(const QSize &newSize);

    /*
     * Update window position.
     * Triggers an update().
     */
    void setWindowPosition(qint64 position);

    /*
     * Base position of tile
     */
    qint64 tilePosition(int index) const;

    /*
     * Structure which identifies a point within a given
     * tile.
     */
    struct TilePoint
    {
        TilePoint(int idx = 0, qint64 pos = 0, qint64 pix = 0)
            :   index(idx), positionOffset(pos), pixelOffset(pix)
        { }

        // Index of tile
        int     index;

        // Number of bytes from start of tile
        qint64  positionOffset;

        // Number of pixels from left of corresponding pixmap
        int     pixelOffset;
    };

    /*
     * Convert position in m_buffer into a tile index and an offset in pixels
     * into the corresponding pixmap.
     *
     * \param position  Offset into m_buffer, in bytes

     * If position is outside the tile array, index is NullIndex and
     * offset is zero.
     */
    TilePoint tilePoint(qint64 position) const;

    /*
     * Convert offset in bytes into a tile into an offset in pixels
     * within that tile.
     */
    int tilePixelOffset(qint64 positionOffset) const;

    /*
     * Convert offset in bytes into the window into an offset in pixels
     * within the widget rect().
     */
    int windowPixelOffset(qint64 positionOffset) const;

    /*
     * Paint all tiles which can be painted.
     * \return true iff update() was called
     */
    bool paintTiles();

    /*
     * Paint the specified tile
     *
     * \pre Sufficient data is available to completely paint the tile, i.e.
     *      m_dataLength is greater than the upper bound of the tile.
     */
    void paintTile(int index);

    /*
     * Move the first n tiles to the end of the array, and mark them as not
     * painted.
     */
    void shuffleTiles(int n);

    /*
     * Reset tile array
     */
    void resetTiles(qint64 newStartPos);

private:
    qint64                  m_bufferPosition;
    qint64                  m_bufferLength;
    QByteArray              m_buffer;

    qint64                  m_audioPosition;
    QAudioFormat            m_format;

    bool                    m_active;

    QSize                   m_pixmapSize;
    QVector<QPixmap*>       m_pixmaps;

    struct Tile {
        // Pointer into parent m_pixmaps array
        QPixmap*            pixmap;

        // Flag indicating whether this tile has been painted
        bool                painted;
    };

    QVector<Tile>           m_tiles;

    // Length of audio data in bytes depicted by each tile
    qint64                  m_tileLength;

    // Position in bytes of the first tile, relative to m_buffer
    qint64                  m_tileArrayStart;

    qint64                  m_windowPosition;
    qint64                  m_windowLength;
};

#endif // WAVE_FORM_H
