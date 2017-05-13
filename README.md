# MusicZapper

**MusicZapper** is a **classic music player**, that offers the opportunity to **zap the music files** whenever you want.


## Description

**MusicZapper** is a media player very similar to [MPC-HC](https://mpc-hc.org/ "https://mpc-hc.org/") or [VLC media player](http://www.videolan.org/ "http://www.videolan.org/"). 

However it has 3 pushbuttons:

- `Keep`
- `Zap`
- `Move`


Each button performs a unique action on the music file currently played. The actions are fully customizable (go to `Edit` > `Preferences`).

__For example:__

You are listening a music file (*.FLAC, *.MP3, *.OGG...), then you click on:

- `Keep`: lets the music file *as is*, and plays the next music file in the playlist.
- `Zap`: tags the file as *zapped*, or moves it to a special directory. You could listen it another day.
- `Move`: moves the file to the trash bin and plays the next file.

And so on.

Then, little by little, **MusicZapper** organizes your music files.

It's useful to save gigabytes of space.

## Development

### Source code organisation

Quick explanation:

    /include        Contains the sources headers

    /src            Contains the sources
        /core           Core objects (non GUI)
        /gui            GUI objects (specific to MusicZapper)
        /widgets        GUI generic objects (not specific to MusicZapper)
        /icons          Resource
        /tutorial       Help resource

    /test           Contains unit tests
        /shared         Shared resource for tests


### Build

1. Download `MusicZapper-src.tar.gz`.

2. Unzip:

        $ tar zxvf MusicZapper-src.tar.gz
        $ cd MusicZapper-src/

3. Build it with [QtCreator](https://www.qt.io/developers/ "https://www.qt.io/developers/").

     Compile and run `MusicZapper.pro`.


### Deployment

#### Linux

No specific action.

#### Windows

To deploy the application on Windows, we must make sure that the relevant DLLs and the executable are in the same directory.

1. Go to the `Release` build directory.

        $ mkdir MusicZapper-v1.0-win32-release
        $ cd .\MusicZapper-v1.0-win32-release
        $ cp D:\path\to\MusicZapper.exe .\

2. Use the [Qt's Windows Deployment Tool](http://doc.qt.io/qt-5/windows-deployment.html#the-windows-deployment-tool "http://doc.qt.io/qt-5/windows-deployment.html#the-windows-deployment-tool").

        $ windeployqt .\

3. Zip and publish.

        $ tar MusicZapper-v1.0-win32-release/



## License

The code is released under the [MIT License](LICENSE "LICENSE").
