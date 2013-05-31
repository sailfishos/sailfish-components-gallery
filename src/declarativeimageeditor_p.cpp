#include "declarativeimageeditor_p.h"

#include <QImage>
#include <QDebug>
#include <QFileInfo>

DeclarativeImageEditorPrivate::DeclarativeImageEditorPrivate(QObject *parent) :
    QObject(parent)
{
}

DeclarativeImageEditorPrivate::~DeclarativeImageEditorPrivate()
{
#ifndef USE_QIMAGE
    if (m_file) {
        delete m_file;
        m_file = 0;
    }
#endif
}

#ifndef USE_QIMAGE
void DeclarativeImageEditorPrivate::releaseImage(QString fileName)
{
    if (m_source == fileName && m_file) {
        delete m_file;
        m_file = 0;
    }
}
#endif

// Run in QtConcurrent::run
void  DeclarativeImageEditorPrivate::crop(const QString &source, const QString &target, const QSizeF &cropSize, const QSizeF &imageSize, const QPointF &position)
{
    QImage sourceImage(source);
    if (!source.isEmpty() && !sourceImage.isNull() && !cropSize.isEmpty() && !imageSize.isEmpty()) {
        QString tmpTarget = target;
        if (tmpTarget.isEmpty()) {
            QFileInfo tmpFile(source);
            QString completeSuffix = tmpFile.completeSuffix();
            QString fullFileBase = source;
            fullFileBase = fullFileBase.left(source.length() - completeSuffix.length());
            QString dot;
            if (fullFileBase.endsWith(".")) {
                fullFileBase = fullFileBase.left(fullFileBase.length() - 1);
                dot = ".";
            }

            // Find name with running number
            int i = 0;
            do {
                tmpTarget = QString(fullFileBase + "_%1" + dot + completeSuffix).arg(i);
                tmpFile.setFile(tmpTarget);
                ++i;
            } while (tmpFile.exists());
        }

        qreal width = sourceImage.width() / imageSize.width() * cropSize.width();
        qreal height = sourceImage.height() / imageSize.height() * cropSize.height();

        qreal x = sourceImage.width() / imageSize.width() * position.x();
        qreal y = sourceImage.height() / imageSize.height() * position.y();
        QRect tmpRect(x, y, width, height);

        QImage scaledImage = sourceImage.copy(tmpRect);
        bool success = scaledImage.save(tmpTarget);
        emit cropped(success);
    } else {
        emit cropped(false);
    }
}
