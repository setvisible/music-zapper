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

#ifndef PLAYLIST_MODEL_H
#define PLAYLIST_MODEL_H

#include <Core/Playlist>

#include <QtCore/QAbstractItemModel>

class PlaylistModel : public QAbstractItemModel
{
    Q_OBJECT

    enum Column {
        Status = 0,     ///< Keep, Zap or Move.
        Title,          ///< Title of the track.
        Length,         ///< Track's length in hh:mm:ss.
        ColumnCount     ///< \internal Do not remove. Indicates the last column.
    };

public:
    enum UserData {
        FullFileName = Qt::UserRole + 1
    };

    explicit PlaylistModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // tempo
    Playlist *playlist() const;
    void setPlaylist(Playlist *playlist);
    // tempo

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole);

    void forceUpdate(); // weird..

private Q_SLOTS:
    void beginInsertItems(int start, int end);
    void endInsertItems();
    void beginRemoveItems(int start, int end);
    void endRemoveItems();
    void changeItems(int start, int end);
    void onLoadFailed();

private:
    Playlist *m_playlist;
    QMap<QModelIndex, QVariant> m_data;

};

#endif // PLAYLIST_MODEL_H
