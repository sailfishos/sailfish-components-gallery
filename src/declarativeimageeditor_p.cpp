// SPDX-FileCopyrightText: 2013-2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024-2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

#include "declarativeimageeditor_p.h"

#include <QImage>
#include <QImageReader>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QRgb>

#ifndef DESKTOP
#include <quillmetadata-qt5/QuillMetadata>
#endif

#include <cmath>

DeclarativeImageEditorPrivate::DeclarativeImageEditorPrivate(QObject *parent)
    : QObject(parent)
{
}

DeclarativeImageEditorPrivate::~DeclarativeImageEditorPrivate()
{
}

#ifndef DESKTOP
QString DeclarativeImageEditorPrivate::save(QImage &image, const QString &source, const QString &target,
                                            const QByteArray &format)
{
    QString tmpFile = uniqueFilePath(source);
    if (!tmpFile.isEmpty() && !image.save(tmpFile, format.constData())) {
        qWarning() << Q_FUNC_INFO << "Failed to save image";
        QFile::remove(tmpFile);
        return QString();
    }

    QuillMetadata md(source);
    if (md.hasExif()) {
        md.removeEntry(QuillMetadata::Tag_Orientation);
        md.setEntry(QuillMetadata::Tag_ImageWidth, image.width());
        md.setEntry(QuillMetadata::Tag_ImageHeight, image.height());
        md.setEntry(QuillMetadata::Tag_Timestamp, QDateTime::currentDateTime().toString(Qt::ISODate));

        // Copy the metadata into the new file
        if (!md.write(tmpFile)) {
            qWarning() << Q_FUNC_INFO << "Failed to write metadata";
            QFile::remove(tmpFile);
            return QString();
        }
    }

    QFileInfo info(source);
    QString targetFile = target;
    if (target.isEmpty() || !QFile::exists(tmpFile)) {
        targetFile = uniqueFilePath(source, info.canonicalPath());
    }
    if (targetFile.isEmpty()) {
        QFile::remove(tmpFile);

        return QString();
    }

    if (tmpFile != targetFile) {

        if (QFile::exists(targetFile)) {
            QFile::remove(targetFile);
        }
        if (!QFile::copy(tmpFile, targetFile)) {
            QFile::remove(tmpFile);
            QFile::remove(targetFile);
            return QString();
        }
        QFile::remove(tmpFile);
    }

    return targetFile;
}
#endif

QString DeclarativeImageEditorPrivate::uniqueFilePath(const QString &sourceFilePath, const QString &path)
{
    if (sourceFilePath.isEmpty() || !QFile::exists(sourceFilePath)) {
        qWarning() << Q_FUNC_INFO << sourceFilePath << "Doesn't exist or then the path is empty!";
        return QString();
    }

    if (path.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "'path' argument is empty!";
        return QString();
    }

    QFileInfo fileInfo(sourceFilePath);

    // Construct target temp file path first:
    QDir dir(path);
    QStringList prevFiles = dir.entryList(QStringList() << fileInfo.baseName() + QLatin1String("*"), QDir::Files);
    int count = prevFiles.count();

    // Create temp file with increasing index in a file name e.g.
    // /var/temp/img_001_0.jpg, /var/temp/img_001_1.jpg
    // In a case there already is a file with the same filename
    QString filePath = dir.absolutePath() + QDir::separator();
    QString suffix = fileInfo.suffix().isEmpty() ? QString()
                                                 : (QLatin1String(".") + fileInfo.suffix());
    QString fileName = fileInfo.baseName() +
                        QLatin1String("_") +
                        QString::number(count) +
                        suffix;

    // This makes sure that we don't generate a file name which already exists. E.g. there are files:
    // img_001_0, img_001_1, img_001_2 and img_001 gets deleted. Then this code would generate a
    // filename img_001_2 which already exists
    while (prevFiles.contains(fileName)) {
        ++count;
        fileName = fileInfo.baseName() +
                    QLatin1String("_") +
                    QString::number(count) +
                    suffix;
    }

    return filePath + fileName;
}

void DeclarativeImageEditorPrivate::rotate(const QString &source, const QString &target, int rotation)
{
#ifndef DESKTOP
    // Just handle all the angles as positive angles
    if (rotation < 0) {
        rotation = (360 + rotation);
    }

    // Scale down large images before rotating them.
    QImageReader reader(source);
    reader.setAutoTransform(true);
    QSize scaledSize = reader.size();
    if (scaledSize.width() > 3264 || scaledSize.height() > 3264) {
        scaledSize = scaledSize.scaled(3264, 3264, Qt::KeepAspectRatio);
    }

    reader.setScaledSize(scaledSize);
    QByteArray format = reader.format();
    QImage img = reader.read();

    if (img.isNull()) {
        qWarning() << "Failed to read image to rotate" << reader.errorString();
        emit rotated(false);
        return;
    }

    QTransform x;
    x.rotate((rotation) % 360);
    img = img.transformed(x);

    QString targetFile = save(img, source, target, format);
    emit rotated(!targetFile.isEmpty(), targetFile);
#endif
}

// Run in QtConcurrent::run
void  DeclarativeImageEditorPrivate::crop(const QString &source, const QString &target, const QSizeF &cropSize,
                                          const QSizeF &imageSize, const QPointF &position)
{
#ifndef DESKTOP
    QImageReader reader(source);
    reader.setAutoTransform(true);

    if (reader.canRead() && !cropSize.isEmpty() && !imageSize.isEmpty()) {
        QByteArray format = reader.format();
        QSize scaledSize = reader.size();

        // QImageReader::autoTransform doesn't transform the size or clip rect.
        if (reader.transformation() & QImageIOHandler::TransformationRotate90) {
            scaledSize.transpose();
        }

        if (scaledSize.width() > 3264 || scaledSize.height() > 3264) {
            scaledSize = scaledSize.scaled(3264, 3264, Qt::KeepAspectRatio);
        }

        const qreal scaleX = scaledSize.width() / imageSize.width();
        qreal scaleY = scaledSize.height() / imageSize.height();

        QRect cropRect(
                    qRound(position.x() * scaleX),
                    qRound(position.y() * scaleY),
                    qRound(cropSize.width() * scaleX),
                    qRound(cropSize.height() * scaleY));


        if (reader.transformation() & QImageIOHandler::TransformationMirror) {
            cropRect.moveLeft(scaledSize.width() - cropRect.x() - cropRect.width());
        }

        if (reader.transformation() & QImageIOHandler::TransformationFlip) {
            cropRect.moveTop(scaledSize.height() - cropRect.y() - cropRect.height());
        }

        if (reader.transformation() & QImageIOHandler::TransformationRotate90) {
            cropRect = QRect(cropRect.y(), scaledSize.width() - cropRect.x() - cropRect.width(),
                             cropRect.height(), cropRect.width());

            scaledSize.transpose();
        }

        reader.setScaledSize(scaledSize);
        reader.setScaledClipRect(cropRect);

        QImage croppedImage = reader.read();

        QString tmpTarget = target;
        if (tmpTarget.isEmpty()) {
            QFileInfo info(source);
            tmpTarget = uniqueFilePath(source, info.canonicalPath());
        }

        // If this is an avatar we might need to create the path first.
        QString targetPath = tmpTarget.left(tmpTarget.lastIndexOf("/"));
        if (!QDir().mkpath(targetPath)) {
           qWarning() << Q_FUNC_INFO << "failed to create target path";
           emit cropped(false);
           return;
        }

        QString targetFile = save(croppedImage, source, target, format);
        emit cropped(!targetFile.isEmpty(), targetFile);
    } else {
        emit cropped(false);
    }
#endif
}

void DeclarativeImageEditorPrivate::adjustLevels(const QString &source, const QString &target,
                                                 double brightness, double contrast)
{
#ifndef DESKTOP
    // Scale down large images before adjusting them.
    QImageReader reader(source);
    QByteArray format = reader.format();
    reader.setAutoTransform(true);

    QSize scaledSize = reader.size();
    if (scaledSize.width() > 3264 || scaledSize.height() > 3264) {
        scaledSize = scaledSize.scaled(3264, 3264, Qt::KeepAspectRatio);
    }

    reader.setScaledSize(scaledSize);
    QImage img = reader.read();
    if (img.format() != QImage::Format_RGB32 && img.format() != QImage::Format_ARGB32) {
        img = img.convertToFormat(QImage::Format_ARGB32);
    }

    // Note: manual processing to match QtGraphicalEffects (there is an suitable filter provided
    // by quillimagefilter-qt5, but it uses a different algorithm whose results do not match...)
    //
    // The shader is:
    //  void main() {
    //      highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
    //      pixelColor.rgb /= max(1.0/256.0, pixelColor.a);
    //      highp float c = 1.0 + contrast;
    //      highp float contrastGainFactor = 1.0 + c * c * c * c * step(0.0, contrast);
    //      pixelColor.rgb = ((pixelColor.rgb - 0.5) * (contrastGainFactor * contrast + 1.0)) + 0.5;
    //      pixelColor.rgb = mix(pixelColor.rgb, vec3(step(0.0, brightness)), abs(brightness));
    //      gl_FragColor = vec4(pixelColor.rgb * pixelColor.a, pixelColor.a) * qt_Opacity;
    //  }
    const bool brightnessModification(brightness != 0.0);
    const bool contrastModification(contrast != 0.0);
    const float intScalar = 256.0f;
    const float fpScalar = 1.0f / intScalar;
    const double c = 1.0 + contrast;
    const float contrastGainFactor = 1.0f + static_cast<float>(std::pow(c, 4)) * (contrast < 0.0 ? 0.0f : 1.0f);
    const float contrastScalar = contrastGainFactor * contrast + 1.0f;
    const float brightnessLimit = (brightness < 0.0 ? 0.0f : 1.0f);
    const float brightnessFraction = std::abs(brightness);
    const float brightnessComplement = 1.0f - brightnessFraction;
    const float brightnessAdjustment = brightnessLimit * brightnessFraction;

    QRgb *it = reinterpret_cast<QRgb *>(img.bits());
    for (QRgb *end = it + (img.width() * img.height()); it != end; ++it) {
        QRgb &pixel(*it);

        const float alpha = qAlpha(pixel) * fpScalar;
        const float alphaScalar = std::max(fpScalar, alpha);

        float components[3] = { qRed(pixel) * fpScalar, qGreen(pixel) * fpScalar, qBlue(pixel) * fpScalar };
        for (int i = 0; i < 3; ++i) {
            float &component(components[i]);
            component /= alphaScalar;

            if (contrastModification) {
                component = (component - 0.5f) * contrastScalar + 0.5f;
            }
            if (brightnessModification) {
                // mix(x,y,a) is defined as: x*(1-a) + y*a
                component = component * brightnessComplement + brightnessAdjustment;
            }

            // Note: I don't know why this is required, but it must happen precisely at this point,
            // otherwise combined modifications of brightness and contrast fail:
            component = std::min(1.0f, std::max(0.0f, component));

            component *= alpha;
        }

        pixel = qRgba(components[0] * intScalar,
                      components[1] * intScalar,
                      components[2] * intScalar,
                      alpha * intScalar);
    }

    QString targetFile = save(img, source, target, format);
    emit levelsAdjusted(!targetFile.isEmpty(), targetFile);
#endif
}
