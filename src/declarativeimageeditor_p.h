#ifndef DECLARATIVEIMAGEEDITOR_PRIVATE_H
#define DECLARATIVEIMAGEEDITOR_PRIVATE_H

#include <QObject>
#include <QUrl>

#ifndef USE_QIMAGE
#include <QuillFile>
#endif

class QString;
class QSizeF;
class QPointF;

class DeclarativeImageEditorPrivate : public QObject
{
    Q_OBJECT
public:
    explicit DeclarativeImageEditorPrivate(QObject *parent = 0);
    virtual ~DeclarativeImageEditorPrivate();

    // Member variables
    QUrl m_source;
    QUrl m_target;

#ifdef USE_QIMAGE
public Q_SLOTS:
    void crop(const QString &source, const QString &target, const QSizeF &cropSize, const QSizeF &imageSize, const QPointF &position);
#else
    QuillFile *m_file;

public Q_SLOTS:
    void releaseImage(QString fileName);
#endif

Q_SIGNALS:
    void cropped(bool success);

};

#endif // DECLARATIVEIMAGEEDITOR_PRIVATE_H
