/*
 * SPDX-FileCopyrightText: 2013-2023 Jolla Ltd.
 * SPDX-FileCopyrightText: 2024-2025 Jolla Mobile Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DECLARATIVEIMAGEMETADATA_H
#define DECLARATIVEIMAGEMETADATA_H

#include <QObject>
#include <QUrl>
#include <QQmlParserStatus>

class DeclarativeImageMetadata : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(int orientation READ orientation NOTIFY orientationChanged)
    Q_PROPERTY(int width READ width NOTIFY widthChanged)
    Q_PROPERTY(int height READ height NOTIFY heightChanged)
    Q_PROPERTY(bool valid READ valid NOTIFY validChanged)
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit DeclarativeImageMetadata(QObject *parent = 0);
    ~DeclarativeImageMetadata();

    void componentComplete();
    void classBegin();

    QUrl source() const;
    void setSource(const QUrl &source);

    bool autoUpdate() const;
    void setAutoUpdate(bool update);

    int orientation() const;
    int width() const;
    int height() const;
    bool valid() const;

Q_SIGNALS:
    void sourceChanged();
    void autoUpdateChanged();
    void orientationChanged();
    void widthChanged();
    void heightChanged();
    void validChanged();
    void hasExifChanged();
    void hasXmpChanged();

private:
    void fileChanged(const QString &fileName);
    void readDimensions(const QString &fileName);

private:
    QUrl m_source;
    int m_orientation;
    int m_width;
    int m_height;
    bool m_autoUpdate;
    bool m_complete;
    bool m_valid;
    bool m_wantDimensions;

    friend class ImageWatcher;
};

#endif // DECLARATIVEIMAGEMETADATA_H
