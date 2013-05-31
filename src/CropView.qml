/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Raine Mäkeläinen <raine.makelainen@jollamobile.com>
**
****************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Silica.theme 1.0
import Sailfish.Gallery 1.0

Item {
    id: cropView

    property Item splitView
    property real explicitWidth
    property real explicitHeight
    // Aspect ratio as width / height
    property real aspectRatio
    property string aspectRatioType
    property bool isPortrait: width < height
    property bool active
    property bool showTitle: true
    property alias source: zoomableImage.source
    property alias target: editor.target
    property alias scale: zoomableImage.scale

    signal clicked
    signal cropped(bool success)

    function crop() {
        editor.crop(Qt.size(editor.width, editor.height),
                    Qt.size(zoomableImage.contentWidth, zoomableImage.contentHeight),
                    Qt.point(zoomableImage.contentX, zoomableImage.contentY))
    }

    onAspectRatioChanged: zoomableImage.resetImagePosition()
    onIsPortraitChanged: {
        // Reset back to orginal
        aspectRatio = -1.0
    }

    Label {
        id: cropHeader

        //% "Crop"
        text: qsTrId("components_gallery-he-crop")
        height: Theme.itemSizeLarge
        color: Theme.highlightColor
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: active && showTitle ? 1.0 : 0.0
        z: 1

        font {
            pixelSize: Theme.fontSizeLarge
            family: Theme.fontFamilyHeading
        }

        Behavior on opacity { FadeAnimation {} }
    }

    ZoomableImage {
        id: zoomableImage

        function resetImagePosition() {
            if (status != Image.Ready) {
                return
            }

            editor.setSize()
            resetScale()
        }

        boundsBehavior: Flickable.StopAtBounds
        anchors.fill: editor
        initialImageWidth: explicitWidth
        initialImageHeight: explicitHeight
        maximumWidth: 4 * parent.width
        maximumHeight: 4 * parent.height
        minimumWidth: editor.width
        minimumHeight: editor.height
        interactive: active
        onClicked: cropView.clicked()
        onStatusChanged: resetImagePosition()
    }

    ImageEditor {
        id : editor

        // As a function to avoid binding loops
        function setSize() {
            if (!aspectRatio || aspectRatio === -1.0) {
                aspectRatio = zoomableImage.contentWidth / zoomableImage.contentHeight
                aspectRatioType = "original"
            }

            if (isPortrait) {
                var maxWidth = explicitWidth - Theme.itemSizeMedium
                var tmpHeight = maxWidth / aspectRatio
                var maxHeight = explicitHeight - cropHeader.height
                if (tmpHeight > maxHeight) {
                    maxWidth = maxHeight * aspectRatio
                }

                width = maxWidth
                height = width / aspectRatio
            } else {
                maxHeight = explicitHeight
                var tmpWidth = aspectRatio * maxHeight
                maxWidth = explicitWidth - Theme.itemSizeMedium
                if (tmpWidth > maxWidth) {
                    maxHeight = maxWidth / aspectRatio
                }
                height = maxHeight
                width = aspectRatio * height
            }
        }

        anchors.centerIn: parent
        source: zoomableImage.source
        onCropped: cropView.cropped(success)
    }

    DimmedRegion {
        anchors.fill: parent
        color: Theme.highlightDimmerColor
        opacity: active ? 0.5 : 0.0
        target: cropView
        area: Qt.rect(0, 0, cropView.width, cropView.height)
        exclude: [ editor ]

        Behavior on opacity { FadeAnimation {} }
    }
}
