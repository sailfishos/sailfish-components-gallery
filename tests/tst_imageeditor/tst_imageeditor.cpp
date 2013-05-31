/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Raine Mäkeläinen <raine.makelainen@jollamobile.com>
**
****************************************************************************/

#include <QtTest>
#include <QImage>
#include <QUrl>
#include <QVariant>

#include "declarativeimageeditor.h"

int colorPalette[16][3] =
{{255, 0, 0}, {255, 0, 0}, {0, 0, 255}, {0, 0, 255},       // red | red | blue | blue
 {255, 0, 0}, {255, 0, 0}, {0, 0, 255}, {0, 0, 255},       // red | red | blue | blue
 {0, 255, 0}, {0, 255, 0}, {255, 255, 0}, {255, 255, 0},   // green | green | yellow | yellow
 {0, 255, 0}, {0, 255, 0}, {255, 255, 0}, {255, 255, 0}};  // green | green | yellow | yellow

class tst_imageeditor : public QObject
{
    Q_OBJECT

public:
    tst_imageeditor();
    ~tst_imageeditor();

    QString sourceImagePath;
    QString targetImagePath;

private Q_SLOTS:
    void initTestCase();
    void crop();

private:
    QUrl createColorPalette();
};

tst_imageeditor::tst_imageeditor() :
    sourceImagePath(QDir::tempPath() + "/test_image.png"),
    targetImagePath(QDir::tempPath() + "/test_cropped.png")
{
}

tst_imageeditor::~tst_imageeditor()
{
    QFile source(sourceImagePath);
    if (source.exists()) {
        source.remove();
    }
    QFile target(targetImagePath);
    if (target.exists()) {
        target.remove();
    }
}

QUrl tst_imageeditor::createColorPalette()
{
    QImage image(QSize(4, 4), QImage::Format_RGB32);
    for (int pixel = 0; pixel < 16; pixel++) {
        image.setPixel(pixel % 4, pixel / 4,
                       qRgb(colorPalette[pixel][0],
                            colorPalette[pixel][1],
                            colorPalette[pixel][2]));
    }
    QString testImage(sourceImagePath);
    image.save(testImage);
    return QUrl::fromLocalFile(testImage);
}

void tst_imageeditor::initTestCase()
{
}

void tst_imageeditor::crop()
{
    QUrl sourceFile = createColorPalette();
    QUrl targetFile = QUrl::fromLocalFile(targetImagePath);

    DeclarativeImageEditor editor;    editor.setSource(sourceFile);
    editor.setTarget(targetFile);

    QSignalSpy croppedSpy(&editor, SIGNAL(cropped(bool)));
    editor.crop(QSizeF(2.0, 2.0), QSizeF(4.0, 4.0), QPointF(1.0, 1.0));
    QTest::qWait(100);
    QCOMPARE(croppedSpy.count(), 1);
    QCOMPARE(qvariant_cast<QVariant>(croppedSpy.at(0).first()).toBool(), true);
    QCOMPARE(QImage(targetImagePath), QImage(sourceImagePath).copy(1, 1, 2, 2));

    croppedSpy.clear();
    editor.crop(QSizeF(1.0, 1.0), QSizeF(4.0, 4.0), QPointF(3.0, 3.0));
    QTest::qWait(100);
    QCOMPARE(croppedSpy.count(), 1);
    QCOMPARE(qvariant_cast<QVariant>(croppedSpy.at(0).first()).toBool(), true);
    QCOMPARE(QImage(targetImagePath), QImage(sourceImagePath).copy(3, 3, 1, 1));
}

QTEST_MAIN(tst_imageeditor)

#include "tst_imageeditor.moc"
