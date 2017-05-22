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

#include "playlistview.h"
#include "ui_playlistview.h"

#include <Core/Player>
#include <Core/Playlist>
#include <Core/PlaylistModel>

#include <QtGui/QClipboard>
#include <QtWidgets/QMenu>


PlaylistView::PlaylistView(QWidget *parent) : QWidget(parent)
  , ui(new Ui::PlaylistView)
  , m_player(Q_NULLPTR)
{
    ui->setupUi(this);
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);

#ifndef QT_NO_CONTEXTMENU
    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(onCustomContextMenuRequested(QPoint)));
#endif
}

PlaylistView::~PlaylistView()
{
    delete ui;
}

/***********************************************************************************
 ***********************************************************************************/
void PlaylistView::setPlayer(Player *player)
{
    if (m_player) {
        disconnect(ui->listView, SIGNAL(activated(QModelIndex)), m_player, SLOT(jump(QModelIndex)));
        disconnect(m_player->playlist(), SIGNAL(currentIndexChanged(int)),
                   this, SLOT(playlistPositionChanged(int)));
    }

    m_player = player;

    if (m_player) {
        connect(ui->listView, SIGNAL(activated(QModelIndex)), m_player, SLOT(jump(QModelIndex)));
        connect(m_player->playlist(), SIGNAL(currentIndexChanged(int)),
                this, SLOT(playlistPositionChanged(int)));

        ui->listView->setModel(m_player->playlist()->model());

        const int i = m_player->playlist()->currentIndex();
        const QModelIndex index = m_player->playlist()->model()->index(i, 0);
        ui->listView->setCurrentIndex(index);
    }

    if (!m_player->isPlayerAvailable()) {
        ui->listView->setEnabled(false);
    }
}


void PlaylistView::playlistPositionChanged(int currentItem)
{
    Q_ASSERT(m_player);
    ui->listView->setCurrentIndex(m_player->playlist()->model()->index(currentItem, 0));
}

/***********************************************************************************
 ***********************************************************************************/
#ifndef QT_NO_CONTEXTMENU
void PlaylistView::onCustomContextMenuRequested(const QPoint &pos)
{
    /// \todo const bool resultsAvailable = m_filterModel->hasResults();
    /// \todo const bool enabled = !m_testRunning && resultsAvailable;

    const QModelIndex clicked = ui->listView->indexAt(pos);
    const bool enabled = clicked.isValid();

    QMenu menu(this);

    QAction *action = new QAction(tr("Open"), &menu);
    action->setEnabled(enabled);
    connect(action, &QAction::triggered, [this, clicked] () { onOpenTriggered(clicked); });
    menu.addAction(action);

    action = new QAction(tr("Remove"), &menu);
    action->setEnabled(enabled);
    connect(action, &QAction::triggered, [this, clicked] () { onRemoveTriggered(clicked); });
    menu.addAction(action);

    menu.addSeparator();

    action = new QAction(tr("Clear"), &menu);
    connect(action, &QAction::triggered, this, &PlaylistView::onClearTriggered);
    menu.addAction(action);

    menu.addSeparator();

    action = new QAction(tr("Copy Path to Clipboard"), &menu);
    menu.addAction(action);
    action->setShortcut(QKeySequence(QKeySequence::Copy));
    action->setEnabled(enabled);
    connect(action, &QAction::triggered, [this, clicked] () { onCopyItemTriggered(clicked); });

    action = new QAction(tr("Open the path"), &menu);
    action->setEnabled(enabled);
    connect(action, &QAction::triggered, [this, clicked] () { onOpenPathTriggered(clicked); });
    menu.addAction(action);

    menu.addSeparator();

    action = new QAction(tr("Reorganize by Name"), &menu);
    connect(action, &QAction::triggered, this, &PlaylistView::onReorganizeByNameTriggered);
    menu.addAction(action);

    action = new QAction(tr("Reorganize by Path"), &menu);
    connect(action, &QAction::triggered, this, &PlaylistView::onReorganizeByPathTriggered);
    menu.addAction(action);

    action = new QAction(tr("Random"), &menu);
    connect(action, &QAction::triggered, this, &PlaylistView::onReorganizeRandomTriggered);
    menu.addAction(action);

    menu.exec(ui->listView->mapToGlobal(pos));
}
#endif

/***********************************************************************************
 ***********************************************************************************/
void PlaylistView::onOpenTriggered(const QModelIndex &index)
{
    Q_ASSERT(m_player);
    Q_ASSERT(index.isValid());
    m_player->playlist()->setCurrentIndex( index.row() );
    m_player->play();
}

void PlaylistView::onRemoveTriggered(const QModelIndex &index)
{
    Q_ASSERT(m_player);
    Q_ASSERT(index.isValid());
    m_player->playlist()->removeMedia( index.row() );
}


void PlaylistView::onClearTriggered()
{
    Q_ASSERT(m_player);
    m_player->playlist()->clear();
}


void PlaylistView::onCopyItemTriggered(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());

    /// \todo implement FILTER ?
    ///
    /// const TestResult result = m_filterModel->testResult(index);
    ///
    const QString result = ui->listView->model()->data(index, PlaylistModel::FullFileName).toString();
    QApplication::clipboard()->setText(result);
}

void PlaylistView::onOpenPathTriggered(const QModelIndex &index)
{
    Q_UNUSED(index)
    /// \todo to be implemented
}

void PlaylistView::onReorganizeByNameTriggered()
{
    /// \todo to be implemented
}

void PlaylistView::onReorganizeByPathTriggered()
{
    /// \todo to be implemented
}

void PlaylistView::onReorganizeRandomTriggered()
{
    /// \todo
}

