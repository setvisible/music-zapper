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

#include <QtTest/QtTest>
#include <QtCore/QDebug>

#include <Core/Media>

class tst_Media : public QObject
{
    Q_OBJECT
private slots:
    void test_null();
    void test_constructor_url();
    void test_constructor_url_status();
    void test_copy();
    void test_assign();
    void test_equals();
};

void tst_Media::test_null()
{
    Media media;

    QCOMPARE(media.fullfilename, QString());
    QCOMPARE(media.status, Media::Undefined);
    QCOMPARE(media.error, Media::NoError);
}


void tst_Media::test_constructor_url()
{
    Media media(QLatin1String("http://example.com/movie.mov"));

    QCOMPARE(media.fullfilename, QLatin1String("http://example.com/movie.mov"));
}

void tst_Media::test_constructor_url_status()
{
    Media media(QLatin1String("http://example.com/movie.mov"), Media::Keep);

    QCOMPARE(media.fullfilename, QLatin1String("http://example.com/movie.mov"));
    QCOMPARE(media.status, Media::Keep);
}

void tst_Media::test_copy()
{
    Media media1(Media(QLatin1String("http://example.com/movie.mov")));
    Media media2(media1);

    QVERIFY(media1 == media2);
}

void tst_Media::test_assign()
{
    Media media1(Media(QLatin1String("http://example.com/movie.mov")));
    Media media2;
    Media media3;

    media2 = media1;
    QVERIFY(media2 == media1);

    media2 = media3;
    QVERIFY(media2 == media3);
}

void tst_Media::test_equals()
{
    Media media1;
    Media media2;
    Media media3(Media(QLatin1String("http://example.com/movie.mov")));
    Media media4(Media(QLatin1String("http://example.com/movie.mov")));
    Media media5(Media(QLatin1String("file:///some/where/over/the/rainbow.mp3")));

    // null == null
    QCOMPARE(media1 == media2, true);
    QCOMPARE(media1 != media2, false);

    // null != something
    QCOMPARE(media1 == media3, false);
    QCOMPARE(media1 != media3, true);

    // equiv
    QCOMPARE(media3 == media4, true);
    QCOMPARE(media3 != media4, false);

    // not equiv
    QCOMPARE(media4 == media5, false);
    QCOMPARE(media4 != media5, true);
}


QTEST_APPLESS_MAIN(tst_Media)

#include "tst_media.moc"

