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

#include "utils.h"
#include "wavfile.h"

#include <QtCore/qendian.h>
#include <QtCore/QVector>
#include <QtCore/QDebug>

struct chunk
{
    char        id[4];
    quint32     size;
};

struct RIFFHeader
{
    chunk       descriptor;     // "RIFF"
    char        type[4];        // "WAVE"
};

struct WAVEHeader
{
    chunk       descriptor;
    quint16     audioFormat;
    quint16     numChannels;
    quint32     sampleRate;
    quint32     byteRate;
    quint16     blockAlign;
    quint16     bitsPerSample;
};

struct DATAHeader
{
    chunk       descriptor;
};

struct CombinedHeader
{
    RIFFHeader  riff;
    WAVEHeader  wave;
};

WavFile::WavFile(QObject *parent)
    : QFile(parent)
    , m_headerLength(0)
{

}

bool WavFile::open(const QString &fileName)
{
    close();
    setFileName(fileName);
    return QFile::open(QIODevice::ReadOnly) && readHeader();
}

const QAudioFormat &WavFile::fileFormat() const
{
    return m_fileFormat;
}

qint64 WavFile::headerLength() const
{
    return m_headerLength;
}

bool WavFile::readHeader()
{
    seek(0);
    CombinedHeader header;
    bool result = read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader);
    if (result) {
        if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
             || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
                && memcmp(&header.riff.type, "WAVE", 4) == 0
                && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0
                && (header.wave.audioFormat == 1 || header.wave.audioFormat == 0)) {

            // Read off remaining header information
            DATAHeader dataHeader;

            if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader)) {
                // Extended data available
                quint16 extraFormatBytes;
                if (peek((char*)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16))
                    return false;
                const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
                if (read(throwAwayBytes).size() != throwAwayBytes)
                    return false;
            }

            if (read((char*)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader))
                return false;

            // Establish format
            if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
                m_fileFormat.setByteOrder(QAudioFormat::LittleEndian);
            else
                m_fileFormat.setByteOrder(QAudioFormat::BigEndian);

            int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
            m_fileFormat.setChannelCount(qFromLittleEndian<quint16>(header.wave.numChannels));
            m_fileFormat.setCodec("audio/pcm");
            m_fileFormat.setSampleRate(qFromLittleEndian<quint32>(header.wave.sampleRate));
            m_fileFormat.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));
            m_fileFormat.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
        } else {
            result = false;
        }
    }
    m_headerLength = pos();
    return result;
}
