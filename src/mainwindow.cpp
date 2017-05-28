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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "about.h"
#include "globals.h"
#include <Core/Player>
#include <Core/Zapper>
#include <GUI/PlayerControls>
#include <GUI/PlayerWidget>
#include <GUI/Selector>
#include <GUI/SettingsDialog>
#include <GUI/VideoWidget>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMimeData>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#ifdef Q_OS_WIN
#  include <windows.h>  /* setWindowAlwaysOnTop() */
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_player(new Player(this))
  , m_zapper(new Zapper(this))
  , m_dirty(false)
  , m_physicalFile(false)
{
    ui->setupUi(this);

    this->setWindowTitle(QString("%0 v%1").arg(STR_APPLICATION_NAME).arg(STR_APPLICATION_VERSION));
    this->setWindowIcon(QIcon(":/icons/logo/Movies.ico"));

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->setAcceptDrops(true);

    ui->player->setPlayer(m_player);
    ui->playlistView->setPlayer(m_player);

    m_zapper->setPlayer(m_player);
    m_zapper->readSettings();

    connect(ui->selector, SIGNAL(keepClicked()), m_zapper, SLOT(keepCurrentSong()));
    connect(ui->selector, SIGNAL(zapClicked()),  m_zapper, SLOT(zapCurrentSong()));
    connect(ui->selector, SIGNAL(moveClicked()), m_zapper, SLOT(moveCurrentSong()));
    connect(ui->selector, SIGNAL(undoClicked()), m_zapper, SLOT(undo()));

    connect(m_player, SIGNAL(mediaChanged()), this, SLOT(setDirty()));
    connect(ui->player, SIGNAL(commute()), this, SLOT(commute()));

    initializeVideo();

    createActions();
    createMenus();

    autoload();

    // todo
    ui->action_ClearRecentList->setEnabled(false);

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/***********************************************************************************
 ***********************************************************************************/
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        m_zapper->writeSettings();
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        m_player->addToPlaylist(mimeData->urls());
    } else {
        this->statusBar()->showMessage(tr("Cannot open the file"), 5000);
    }
    event->acceptProposedAction();
}


#ifndef QT_NO_WHEELEVENT
void MainWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        ui->player->scrollVolume(numSteps.y());
    }
    event->accept();
}
#endif


/***********************************************************************************
 ***********************************************************************************/
void MainWindow::newFile()
{    
    if (autosave())
        return;

    if (maybeSave()) {
        m_physicalFile = false;
        m_currentFile = QFileInfo();
        m_currentFile.setFile( STR_DEFAULT_PLAYLIST_MZF );

        // Clear Objects
        m_player->clear();

        // Reset GUI
        viewNormal();
        setClean();
    }
}

bool MainWindow::isExampleFile() const
{
    return m_currentFile.filePath().startsWith(":/");
}

bool MainWindow::isPhysicalFile() const
{
    return m_physicalFile
            && m_currentFile.isFile()
            && !m_currentFile.isSymLink()
            && !isExampleFile();
}

bool MainWindow::save()
{
    if (autosave())
        return true;

    if ( isPhysicalFile() ){
        return savePlaylistFile(m_currentFile.absoluteFilePath());
    } else {
        return saveAs();
    }
}

bool MainWindow::saveAs()
{
    if (autosave())
        return true;

    QString filePath = askSaveFileName(STR_MUSICZAPPER_PLAYLIST_MZF,
                                       STR_MUSICZAPPER_PLAYLIST);
    if (filePath.isEmpty()) {
        return false;
    }
    return savePlaylistFile(filePath);
}

void MainWindow::open()
{
    if (autosave())
        return;

    if (maybeSave()) {
        QString filePath = askOpenFileName(tr("%0;;All files (*.*)")
                                           .arg( STR_MUSICZAPPER_PLAYLIST_MZF ));
        if (!filePath.isEmpty()) {
            if (loadPlaylistFile(filePath)) {
                this->setClean();
            }
        }
    }
}

/***********************************************************************************
 ***********************************************************************************/
void MainWindow::addMedias()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Files"));

    QStringList supportedMimeTypes = m_player->supportedMimeTypes();
    if (!supportedMimeTypes.isEmpty()) {
        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }

    fileDialog.setDirectory(QStandardPaths::standardLocations(
                                QStandardPaths::MusicLocation).value(0, QDir::homePath()));

    if (fileDialog.exec() == QDialog::Accepted) {
        m_player->addToPlaylist(fileDialog.selectedUrls());
        this->statusBar()->showMessage(tr("Media added"), 5000);
    }
}



/***********************************************************************************
 ***********************************************************************************/
void MainWindow::setChecked(QAction *action)
{
    Q_ASSERT(action);
    bool blocked = this->blockSignals(true);
    ui->action_Minimal->setChecked( action == ui->action_Minimal);
    ui->action_Normal->setChecked(  action == ui->action_Normal);
    ui->action_Full->setChecked(    action == ui->action_Full);
    this->blockSignals(blocked);
}

void MainWindow::viewMinimal()
{
    setChecked(ui->action_Minimal);
    ui->player->setVisible(false);
    this->adjustSize();
    this->adjustSize();
}

void MainWindow::viewNormal()
{
    setChecked(ui->action_Normal);
    ui->player->setVisible(true);
    this->adjustSize();
    this->adjustSize();

}

void MainWindow::viewFull()
{
    setChecked(ui->action_Full);
    ui->player->setVisible(true);
    this->adjustSize();
    this->adjustSize();
}

/***********************************************************************************
 ***********************************************************************************/
void MainWindow::setFullScreen(bool enabled)
{
    QWidget *sender = qobject_cast<QWidget *>(this->sender());
    if (sender != (QWidget *)ui->action_Fullscreen) {
        ui->action_Fullscreen->setChecked(enabled);
    }

    if (enabled == isFullScreen())
        return;

    if (enabled) {
        this->showFullScreen();
    } else {
        this->showNormal();
    }
}

void MainWindow::setAlwaysOnTop(bool enabled)
{
    QWidget *sender = qobject_cast<QWidget *>(this->sender());
    if (sender != (QWidget *)ui->action_AlwaysOnTop) {
        ui->action_AlwaysOnTop->setChecked(enabled);
    }

    if (enabled == isAlwaysOnTop())
        return;

    // The following code avoids the flickering of the window.
    // Reference:
    // http://www.qtforum.org/article/37819/ms-windows-qwidget-toggle-stay-on-top-always-on-top.html

#ifdef Q_OS_WIN
    if (enabled) {
        //To make Always on Top use:
        SetWindowPos((HWND)this->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    } else {
        //to turn it off you can use:
        SetWindowPos((HWND)this->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
#else
    Qt::WindowFlags flags = this->windowFlags();
    if (enabled) {
        this->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        this->show();
    } else {
        this->setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        this->show();
    }
#endif
}

bool MainWindow::isAlwaysOnTop() const
{
#ifdef Q_OS_WIN
    DWORD dwExStyle = GetWindowLong((HWND)this->winId(), GWL_EXSTYLE);
    return (bool)((dwExStyle & WS_EX_TOPMOST) != 0);
#else
    return (bool)(this->windowFlags() & Qt::WindowStaysOnTopHint);
#endif
}

/***********************************************************************************
 ***********************************************************************************/
void MainWindow::showPreferences()
{
    bool ontop = isAlwaysOnTop();
    this->setAlwaysOnTop(false);

    SettingsDialog dialog(this);
    dialog.exec();
    m_zapper->readSettings();
    autoload();

    this->setAlwaysOnTop(ontop);
}

void MainWindow::showTutorial()
{
    const QString path = QCoreApplication::applicationDirPath();
    const QString local_help = QString("%0/help-tutorial.html").arg(path);
    const QString distant_help = STR_DISTANT_HELP;

    if (QFileInfo::exists( local_help )) {
        QDesktopServices::openUrl(QUrl(local_help, QUrl::TolerantMode));
    } else {
        QDesktopServices::openUrl(QUrl(distant_help, QUrl::TolerantMode));
    }
}


/***********************************************************************************
 ***********************************************************************************/
void MainWindow::about()
{
    bool ontop = isAlwaysOnTop();
    this->setAlwaysOnTop(false);

    QMessageBox msgBox(QMessageBox::NoIcon, tr("About %0").arg(STR_APPLICATION_NAME), aboutHtml());
    msgBox.exec();

    this->setAlwaysOnTop(ontop);
}

/***********************************************************************************
 ***********************************************************************************/
void MainWindow::play()
{
    m_player->play();
}

void MainWindow::pause()
{
    m_player->pause();
}

void MainWindow::stop()
{
    m_player->stop();
}

void MainWindow::next()
{
    m_player->next();
}

void MainWindow::previous()
{
    m_player->previous();
}

void MainWindow::commute()
{
    // commute
    Q_ASSERT(ui->stackedWidget->count() == 2); // Assume 2 pages
    ui->stackedWidget->setCurrentIndex(1 - ui->stackedWidget->currentIndex());
}


/***********************************************************************************
 ***********************************************************************************/
static bool s_autosave_lock = false;

void MainWindow::autoload()
{
    if (m_zapper->isAutosave()) {
        const QString f = m_zapper->playlistPath();
        if (!QFileInfo::exists(f)) {
            savePlaylistFile( f );  // Ensures the file exists
        }
        loadPlaylistFile( f );
    } else {
        newFile();
    }
}

bool MainWindow::autosave()
{
    if (!m_zapper->isAutosave()) {
        return false;
    }
    if (!s_autosave_lock) {
        s_autosave_lock = true;
        savePlaylistFile( m_zapper->playlistPath() );
        this->setWindowTitle( QString("%0 - %1")
                              .arg(STR_AUTOSAVED_PLAYLIST)
                              .arg(STR_APPLICATION_NAME));
        this->statusBar()->showMessage(tr("Autosaved"), 5000);
        s_autosave_lock = false;
    }
    return true;
}

/***********************************************************************************
 ***********************************************************************************/
bool MainWindow::maybeSave()
{
    if (m_dirty) {
        int ret = QMessageBox::warning(
                    this, tr("Unsaved changes"),
                    tr("The playlist has not been saved.\n\n"
                       "Do you want to save your changes ?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            return save();
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

QString MainWindow::niceFileName() const
{
    if ( isPhysicalFile() ){
        return QDir::toNativeSeparators(m_currentFile.canonicalFilePath());
    }else{
        return m_currentFile.fileName();
    }
}

void MainWindow::setDirty()
{
    if (!m_dirty) {
        m_dirty = true;
    }
    if (!autosave()) {
        this->setWindowTitle( niceFileName() + QString("* - %0").arg(STR_APPLICATION_NAME));
    }
}

void MainWindow::setClean()
{
    m_dirty = false;
    if (!autosave()) {
        this->setWindowTitle( niceFileName() + QString(" - %0").arg(STR_APPLICATION_NAME));
    }
}


/***********************************************************************************
 ***********************************************************************************/
QString MainWindow::askSaveFileName(const QString &fileFilter, const QString &title)
{
    QString suggestedPath;
    if (isPhysicalFile()) {
        suggestedPath = m_currentFile.canonicalFilePath();
    } else {
        suggestedPath =
                QDir::toNativeSeparators(
                    m_zapper->defaultMediaPath()
                    + QDir::separator()
                    + m_currentFile.fileName());
    }
    return QFileDialog::getSaveFileName(this, title, suggestedPath, fileFilter);
}

QString MainWindow::askOpenFileName(const QString &fileFilter, const QString &title)
{
    QString suggestedPath = m_zapper->defaultMediaPath();
    if (isPhysicalFile()){
        suggestedPath = m_currentFile.absolutePath();
    }
    return QFileDialog::getOpenFileName(this, title, suggestedPath, fileFilter);
}


/***********************************************************************************
 ***********************************************************************************/
void MainWindow::readSettings()
{
    QSettings settings;
    if ( !isMaximized() ) {
        this->move(settings.value("Position", QPoint(100, 100)).toPoint());
        this->resize(settings.value("Size", QSize(350, 350)).toSize());
    }
    this->setWindowState( (Qt::WindowStates)settings.value("WindowState", 0).toInt() );
    this->setAlwaysOnTop( settings.value("AlwaysOnTop", false).toBool() );

    ui->action_Fullscreen->setChecked(isFullScreen());
    ui->action_AlwaysOnTop->setChecked(isAlwaysOnTop());
}

void MainWindow::writeSettings()
{
    QSettings settings;

    if( !(isMaximized() | isFullScreen()) ) {
        settings.setValue("Position", this->pos());
        settings.setValue("Size", this->size());
    }
    settings.setValue("WindowState", (int)this->windowState()); // minimized, maximized, active, fullscreen...
    settings.setValue("AlwaysOnTop", this->isAlwaysOnTop() );



    // --------------------------------------------------------------
    // Write also the current version of application in the settings,
    // because it might be used by 3rd-party update manager softwares like
    // FileHippo or Google Updater (aka gup).
    // --------------------------------------------------------------
    settings.setValue("Version", STR_APPLICATION_VERSION );
}

/***********************************************************************************
 ***********************************************************************************/
void MainWindow::createActions()
{
    //! [0] File
    ui->action_New->setShortcuts(QKeySequence::New);
    ui->action_New->setStatusTip(tr("Create a new playlist"));
    connect(ui->action_New, SIGNAL(triggered()), this, SLOT(newFile()));

    ui->action_Open->setShortcuts(QKeySequence::Open);
    ui->action_Open->setStatusTip(tr("Open a playlist from a file"));
    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(open()));

    ui->action_Save->setShortcuts(QKeySequence::Save);
    ui->action_Save->setStatusTip(tr("Save the playlist to current file"));
    connect(ui->action_Save, SIGNAL(triggered()), this, SLOT(save()));

    ui->action_SaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->action_SaveAs->setStatusTip(tr("Save the playlist to a different file"));
    connect(ui->action_SaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

    ui->action_AddMedias->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    ui->action_AddMedias->setStatusTip(tr("Add medias"));
    connect(ui->action_AddMedias, SIGNAL(triggered()), this, SLOT(addMedias()));

    ui->action_Exit->setShortcuts(QKeySequence::Quit);
    ui->action_Exit->setStatusTip(tr("Quit %0").arg(STR_APPLICATION_NAME));
    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(close()));
    //! [0]

    //! [1] View
    ui->action_Minimal->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    ui->action_Minimal->setStatusTip(tr("Minimal"));
    connect(ui->action_Minimal, SIGNAL(triggered()), this, SLOT(viewMinimal()));

    ui->action_Normal->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    ui->action_Normal->setStatusTip(tr("Normal"));
    connect(ui->action_Normal, SIGNAL(triggered()), this, SLOT(viewNormal()));

    ui->action_Full->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
    ui->action_Full->setStatusTip(tr("Full"));
    connect(ui->action_Full, SIGNAL(triggered()), this, SLOT(viewFull()));

    ui->action_ShowPlaylist->setStatusTip(tr("Show/hide playlist"));
    connect(ui->action_ShowPlaylist, SIGNAL(triggered()), this, SLOT(commute()));

    ui->action_Fullscreen->setStatusTip(tr("Fullscreen"));
    connect(ui->action_Fullscreen, SIGNAL(triggered(bool)), this, SLOT(setFullScreen(bool)));

    ui->action_AlwaysOnTop->setStatusTip(tr("Always on Top"));
    connect(ui->action_AlwaysOnTop, SIGNAL(triggered(bool)), this, SLOT(setAlwaysOnTop(bool)));

#ifdef Q_OS_WIN
    ui->action_Preferences->setShortcut(QKeySequence(/*Qt::CTRL +*/ Qt::Key_F10));
#else
    ui->action_Preferences->setShortcuts(QKeySequence::Preferences);
#endif
    ui->action_Preferences->setStatusTip(tr("Open settings"));
    connect(ui->action_Preferences, SIGNAL(triggered()), this, SLOT(showPreferences()));
    //! [1]

    //! [2] Play
    ui->action_Play->setStatusTip(tr("Play"));
    connect(ui->action_Play, SIGNAL(triggered()), this, SLOT(play()));

    ui->action_Pause->setStatusTip(tr("Pause"));
    connect(ui->action_Pause, SIGNAL(triggered()), this, SLOT(play()));

    ui->action_Stop->setStatusTip(tr("Stop"));
    connect(ui->action_Stop, SIGNAL(triggered()), this, SLOT(stop()));
    //! [2]

    //! [3] Navigate
    ui->action_Next->setStatusTip(tr("Next"));
    connect(ui->action_Next, SIGNAL(triggered()), this, SLOT(next()));

    ui->action_Previous->setStatusTip(tr("Previous"));
    connect(ui->action_Previous, SIGNAL(triggered()), this, SLOT(previous()));
    //! [3]

    //! [4] Help
    ui->action_Tutorial->setStatusTip(tr("Quick Tutorial"));
    connect(ui->action_Tutorial, SIGNAL(triggered()), this, SLOT(showTutorial()));

    ui->action_About->setShortcuts(QKeySequence::HelpContents);
    ui->action_About->setStatusTip(tr("About %0").arg(STR_APPLICATION_NAME));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(about()));

    ui->action_AboutQt->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F1));
    ui->action_AboutQt->setStatusTip(tr("About Qt"));
    connect(ui->action_AboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    //! [4]
}

void MainWindow::createMenus()
{

}


/***********************************************************************************
 ***********************************************************************************/
bool MainWindow::savePlaylistFile(const QString &path)
{
    QDir::setCurrent(path);
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        QMessageBox::warning(this, tr("Cannot save file"),
                             tr("Cannot write to file %1:\n%2.")
                             .arg(path)
                             .arg(file.errorString()));
        return false;
    }

    QJsonObject json;
    m_player->write(json);
    QJsonDocument saveDoc(json);
    file.write( saveDoc.toJson() );

    m_physicalFile = true;
    m_currentFile.setFile(path);
    this->statusBar()->showMessage(tr("Playlist saved"), 2000);
    setClean();
    return true;
}

/***********************************************************************************
 ***********************************************************************************/
bool MainWindow::loadPlaylistFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Couldn't open file.");
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(path)
                             .arg(file.errorString()));
        return false;
    }
    QByteArray saveData = file.readAll();
    QJsonParseError ok;
    QJsonDocument loadDoc( QJsonDocument::fromJson(saveData, &ok) );

    if (ok.error != QJsonParseError::NoError) {
        qCritical("Couldn't parse JSON file.");
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot parse the JSON file:\n"
                                "%1\n\n"
                                "At character %2, %3.\n\n"
                                "Operation cancelled.")
                             .arg(path)
                             .arg(ok.offset)
                             .arg(ok.errorString()));
        return false;
    }

    m_player->read(loadDoc.object());
    m_physicalFile = true;
    m_currentFile = path;
    this->statusBar()->showMessage(tr("Playlist loaded"), 5000);
    setClean();
    return true;
}


/***********************************************************************************
 ***********************************************************************************/
/* Video Management */
void MainWindow::initializeVideo()
{
    ui->videoThumbWidget->setThumbImage(QLatin1String(":/icons/video_thumb_48x48.png"));
    ui->videoWidget->setThumbImage(QLatin1String(":/icons/video_thumb_176x43.png"));

    m_player->setVideoOutput(ui->videoThumbWidget->videoWidget());

    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetChanged(int)));
    connect(m_player, SIGNAL(videoAvailableChanged(bool)), this, SLOT(onPlayerVideoAvailableChanged(bool)));


    /// \todo try with this instead ?
    /// connect(m_player, SIGNAL(stop()), ui->videoWidget, SLOT(update()));
    ///
    connect(ui->player, SIGNAL(stopped()), ui->videoThumbWidget, SLOT(update()));
    connect(ui->player, SIGNAL(stopped()), ui->videoWidget, SLOT(update()));


    onStackedWidgetChanged(ui->stackedWidget->currentIndex());
}

void MainWindow::onStackedWidgetChanged(int index)
{
    if (index == 0) {
        m_player->setVideoOutput(ui->videoThumbWidget->videoWidget());
        ui->videoThumbWidget->setThumbVisible(false);
        ui->videoWidget->setThumbVisible(true);
    } else {
        m_player->setVideoOutput(ui->videoWidget->videoWidget());
        ui->videoThumbWidget->setThumbVisible(true);
        ui->videoWidget->setThumbVisible(false);
    }

}

void MainWindow::onPlayerVideoAvailableChanged(bool available)
{
    if (ui->stackedWidget->currentIndex() == 0) {
        ui->videoThumbWidget->setThumbVisible(!available);
        ui->videoWidget->setThumbVisible(true);
    } else {
        ui->videoThumbWidget->setThumbVisible(true);
        ui->videoWidget->setThumbVisible(!available);
    }

    /// \todo
    /// if (!available) {
    ///     disconnect(ui->fullScreenButton, SIGNAL(clicked(bool)), ui->videoWidget->videoWidget(), SLOT(setFullScreen(bool)));
    ///     disconnect(ui->videoWidget->videoWidget(), SIGNAL(fullScreenChanged(bool)), ui->fullScreenButton, SLOT(setChecked(bool)));
    ///     ui->videoWidget->videoWidget()->setFullScreen(false);
    /// } else {
    ///     connect(ui->fullScreenButton, SIGNAL(clicked(bool)), ui->videoWidget->videoWidget(), SLOT(setFullScreen(bool)));
    ///     connect(ui->videoWidget->videoWidget(), SIGNAL(fullScreenChanged(bool)), ui->fullScreenButton, SLOT(setChecked(bool)));
    ///     if (ui->fullScreenButton->isChecked()) {
    ///         ui->videoWidget->videoWidget()->setFullScreen(true);
    ///     }
    /// }

}


