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

#include "playlistmodel.h"

#include <QtCore/QtGlobal>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtGui/QBrush>
#include <QtGui/QPixmap>
#include <QtMultimedia/QMediaPlaylist>

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_playlist(Q_NULLPTR)
{
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid() ? m_playlist->mediaCount() : 0;
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? ColumnCount : 0;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid()
            && row >= 0 && row < m_playlist->mediaCount()
            && column >= 0 && column < ColumnCount
            ? createIndex(row, column)
            : QModelIndex();
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!m_playlist || !index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        //QVariant value = m_data[index];
        //if (!value.isValid()) {

        if (index.column() == Column::Status) { /* Column Status */
            return QLatin1String("[?]");

        } else if (index.column() == Column::Title) { /* Column Title */
            QUrl location = m_playlist->media(index.row()).canonicalUrl();
            return QFileInfo(location.path()).fileName();

        } else if (index.column() == Column::Length) { /* Column Length */
            return QLatin1String("5:38");

        } else {
            Q_UNREACHABLE();
        }
        //}
        //return value;

    } else if (role == Qt::DecorationRole) {

        if (index.column() == Column::Status) {

            Media::Status s = m_playlist->mediaStatus(index.row());
            switch (s) {
            case Media::Undefined:
                return QPixmap(QLatin1String(":/icons/mini_undef_20x20.png"));
                break;
            case Media::Keep:
                return QPixmap(QLatin1String(":/icons/mini_keep_20x20.png"));
                break;
            case Media::Zap:
                return QPixmap(QLatin1String(":/icons/mini_zap_20x20.png"));
                break;
            case Media::Move:
                return QPixmap(QLatin1String(":/icons/mini_move_20x20.png"));
                break;
            default:
                Q_UNREACHABLE();
                break;
            }

        }

    } else if (role == Qt::ForegroundRole) {
        if (index.column() == Column::Title) {
            bool error = m_playlist->hasMediaError(index.row());
            if (error) {
                return QBrush(Qt::red);
            }
        }
        return QBrush(Qt::black);

    } else if (role == UserData::FullFileName) {
        QUrl location = m_playlist->media(index.row()).canonicalUrl();
        return QDir::toNativeSeparators( location.toLocalFile() );
    }

    return QVariant();
}

// weird...
Playlist *PlaylistModel::playlist() const
{
    return m_playlist;
}

// weird...
void PlaylistModel::setPlaylist(Playlist *playlist)
{
    if (m_playlist) {
        disconnect(m_playlist, SIGNAL(mediaAboutToBeInserted(int,int)), this, SLOT(beginInsertItems(int,int)));
        disconnect(m_playlist, SIGNAL(mediaInserted(int,int)), this, SLOT(endInsertItems()));
        disconnect(m_playlist, SIGNAL(mediaAboutToBeRemoved(int,int)), this, SLOT(beginRemoveItems(int,int)));
        disconnect(m_playlist, SIGNAL(mediaRemoved(int,int)), this, SLOT(endRemoveItems()));
        disconnect(m_playlist, SIGNAL(mediaChanged(int,int)), this, SLOT(changeItems(int,int)));
    }

    beginResetModel();
    m_playlist = playlist;

    if (m_playlist) {
        connect(m_playlist, SIGNAL(mediaAboutToBeInserted(int,int)), this, SLOT(beginInsertItems(int,int)));
        connect(m_playlist, SIGNAL(mediaInserted(int,int)), this, SLOT(endInsertItems()));
        connect(m_playlist, SIGNAL(mediaAboutToBeRemoved(int,int)), this, SLOT(beginRemoveItems(int,int)));
        connect(m_playlist, SIGNAL(mediaRemoved(int,int)), this, SLOT(endRemoveItems()));
        connect(m_playlist, SIGNAL(mediaChanged(int,int)), this, SLOT(changeItems(int,int)));

        connect(m_playlist, SIGNAL(loadFailed()), this, SLOT(onLoadFailed()));
    }

    endResetModel();
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    m_data[index] = value;
    emit dataChanged(index, index);
    return true;
}

void PlaylistModel::beginInsertItems(int start, int end)
{
    m_data.clear();
    beginInsertRows(QModelIndex(), start, end);
}

void PlaylistModel::endInsertItems()
{
    endInsertRows();
}

void PlaylistModel::beginRemoveItems(int start, int end)
{
    m_data.clear();
    beginRemoveRows(QModelIndex(), start, end);
}

void PlaylistModel::endRemoveItems()
{
    endInsertRows();
}

void PlaylistModel::changeItems(int start, int end)
{
    m_data.clear();
    emit dataChanged(index(start,0), index(end, Column::ColumnCount));
}

void PlaylistModel::onLoadFailed()
{
    qDebug() << Q_FUNC_INFO;

}

void PlaylistModel::forceUpdate()
{
    emit dataChanged(index(0,0), index(m_playlist->mediaCount(), Column::ColumnCount));
}
