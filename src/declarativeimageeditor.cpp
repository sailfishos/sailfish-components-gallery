#include "declarativeimageeditor.h"
#include "declarativeimageeditor_p.h"

#ifdef USE_QIMAGE
#include <QImage>
#include <QtConcurrentRun>
#else
#include <Quill>
#include <QuillFile>
#include <QuillImageFilter>
#include <QuillImageFilterFactory>
#endif

#include <QRectF>

DeclarativeImageEditor::DeclarativeImageEditor(QQuickItem *parent) :
    QQuickItem(parent),
    d_ptr(new DeclarativeImageEditorPrivate)
{
    connect(d_ptr, SIGNAL(cropped(bool)), this, SIGNAL(cropped(bool)));
#ifndef USE_QIMAGE
    Quill::setDBusThumbnailingEnabled(false);
#endif

}

DeclarativeImageEditor::~DeclarativeImageEditor()
{
    delete d_ptr;
}

void DeclarativeImageEditor::setSource(const QUrl &url)
{
    Q_D(DeclarativeImageEditor);

    if (d->m_source != url) {
        d->m_source = url;
        emit sourceChanged();
    }
}

QUrl DeclarativeImageEditor::source() const
{
    Q_D(const DeclarativeImageEditor);
    return d->m_source;
}



void DeclarativeImageEditor::setTarget(const QUrl &target)
{
    Q_D(DeclarativeImageEditor);
    if (d->m_target != target) {
        d->m_target = target;
        emit targetChanged();
    }
}

QUrl DeclarativeImageEditor::target() const
{
    Q_D(const DeclarativeImageEditor);
    return d->m_target;
}

void DeclarativeImageEditor::crop(const QSizeF &cropSize, const QSizeF &imageSize, const QPointF &position)
{
    Q_D(DeclarativeImageEditor);
    QString source = d->m_source.toLocalFile();
    QString target = d->m_target.toLocalFile();
#ifdef USE_QIMAGE
    QtConcurrent::run(d, &DeclarativeImageEditorPrivate::crop, source, target, cropSize, imageSize, position);
#else
    if (!d->m_source.isEmpty() && !d->m_target.isEmpty() && target.isValid() && !target.isEmpty()) {
        d->m_file = new QuillFile(d->m_source);
        QuillImageFilter *filter = QuillImageFilterFactory::createImageFilter("org.maemo.crop");
        filter->setOption(QuillImageFilter::CropRectangle, QVariant(targetRect.toRect()));
        d->m_file->runFilter(filter);
        QObject::connect(Quill::instance(), SIGNAL(saved(QString)),
                                 d, SLOT(releaseImage(QString)));

        d->m_file->save();
    }
#endif
}
