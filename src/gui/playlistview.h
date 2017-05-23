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

#ifndef PLAYLIST_VIEW_H
#define PLAYLIST_VIEW_H

#include <QtWidgets/QWidget>
#include <QtMultimedia/QMediaPlayer>

namespace Ui {
class PlaylistView;
}

class Player;

class PlaylistView : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistView(QWidget *parent = 0);
    ~PlaylistView();

    void setPlayer(Player *player);

Q_SIGNALS:

protected Q_SLOTS:
    void playlistPositionChanged(int);
#ifndef QT_NO_CONTEXTMENU
    void onCustomContextMenuRequested(const QPoint &pos);
#endif
    void onOpenTriggered(const QModelIndex &index);
    void onRemoveTriggered(const QModelIndex &index);
    void onCopyItemTriggered(const QModelIndex &index);
    void onClearTriggered();
    void onOpenPathTriggered(const QModelIndex &index);
    void onReorganizeByNameTriggered();
    void onReorganizeByPathTriggered();
    void onReorganizeRandomTriggered();


private:
    Ui::PlaylistView *ui;
    Player *m_player;
};

#endif // PLAYLIST_VIEW_H
