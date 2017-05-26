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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QFileInfo>
#include <QtWidgets/QMainWindow>

namespace Ui {
class MainWindow;
}

class CommandSettings;
class Player;
class Zapper;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool savePlaylistFile(const QString &path);
    bool loadPlaylistFile(const QString &path);

    bool isAlwaysOnTop() const;

protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
#endif


private Q_SLOTS:
    void newFile();
    bool save();
    bool saveAs();
    void open();

    void addMedias();

    void viewMinimal();
    void viewNormal();
    void viewFull();
    void commute();

    void setFullScreen(bool enabled);
    void setAlwaysOnTop(bool enabled);

    void showPreferences();
    void showTutorial();

    void play();
    void pause();
    void stop();
    void next();
    void previous();

    void about();

    bool maybeSave();

    void setDirty();
    void setClean();

    void onStackedWidgetChanged(int index);
    void onPlayerVideoAvailableChanged(bool available);

private:
    Ui::MainWindow *ui;
    Player *m_player;
    Zapper *m_zapper;

    bool m_dirty;
    bool m_physicalFile;
    QFileInfo m_currentFile;

    inline QString niceFileName() const;
    inline bool isExampleFile() const;
    inline bool isPhysicalFile() const;

    void readSettings();
    void writeSettings();

    void createActions();
    void createMenus();

    void autoload();
    bool autosave();

    QString askSaveFileName(const QString &fileFilter, const QString &title = tr("Save As"));
    QString askOpenFileName(const QString &fileFilter, const QString &title = tr("Open"));

    void setChecked(QAction *action);


    void initializeVideo();
};

#endif // MAINWINDOW_H
