#ifndef DECLARATIVEIMAGEEDITOR_H
#define DECLARATIVEIMAGEEDITOR_H

#include <qqml.h>
#include <QQuickItem>
#include <QString>

class DeclarativeImageEditorPrivate;

class DeclarativeImageEditor : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl target READ target WRITE setTarget NOTIFY targetChanged)
public:
    explicit DeclarativeImageEditor(QQuickItem *parent = 0);
    virtual ~DeclarativeImageEditor();

    QUrl source() const;
    void setSource(const QUrl &source);

    QUrl target() const;
    void setTarget(const QUrl &target);

    Q_INVOKABLE void crop(const QSizeF &cropSize, const QSizeF &imageSize, const QPointF &position);

Q_SIGNALS:
    void cropped(bool success);
    void sourceChanged();
    void targetChanged();

private:
    DeclarativeImageEditorPrivate *d_ptr;
    Q_DECLARE_PRIVATE(DeclarativeImageEditor)
};

QML_DECLARE_TYPE(DeclarativeImageEditor)

#endif // DECLARATIVEIMAGEEDITOR_H
