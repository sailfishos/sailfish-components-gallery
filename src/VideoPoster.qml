// SPDX-FileCopyrightText: 2013-2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024-2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.Thumbnailer 1.0

/*!
  \inqmlmodule Sailfish.Gallery
*/
Item {
    id: root

    signal togglePlay

    property url source
    property string mimeType

    property bool playing
    property bool loaded
    property alias busy: busyIndicator.running
    property alias status: poster.status

    property real contentWidth: width
    property real contentHeight: height

    property bool overlayMode
    property bool transpose
    readonly property bool error: !!poster.errorLabel
    readonly property bool down: videoMouse.pressed && videoMouse.containsMouse

    signal clicked
    signal doubleClicked

    function displayError() {
        poster.errorLabel = errorLabelComponent.createObject(poster)
    }

    implicitWidth: poster.implicitWidth
    implicitHeight: poster.implicitHeight

    onSourceChanged: {
        if (poster.errorLabel) {
            poster.errorLabel.destroy()
            poster.errorLabel = null
        }
    }

    MouseArea {
        id: videoMouse

        anchors {
            fill: parent
            margins: Theme.paddingLarge // don't react near display edges
        }
        onClicked: clickDelay.restart()
        onDoubleClicked: {
            clickDelay.stop()
            root.doubleClicked()
        }
        Timer {
            id: clickDelay
            interval: 200
            onTriggered: root.clicked()
        }
    }

    // Poster
    Thumbnail {
        id: poster

        property var errorLabel

        anchors.centerIn: parent

        width: !transpose ? root.contentWidth : root.contentHeight
        height: !transpose ? root.contentHeight : root.contentWidth

        sourceSize.width: Screen.height
        sourceSize.height: Screen.height

        source: root.source
        mimeType: root.mimeType

        priority: Thumbnail.HighPriority
        fillMode: Thumbnail.PreserveAspectFit
        opacity: !loaded ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator {} }

        visible: !loaded
        rotation: transpose ? (implicitHeight > implicitWidth ? 270 : 90)  : 0
    }

    BusyIndicator {
        id: busyIndicator

        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
    }

    IconButton {
        id: playButton

        anchors.centerIn: parent
        width: icon.width
        height: icon.height
        enabled: !busy && (overlayMode || !playing) && !root.error
        opacity: enabled ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator {} }

        icon.color: Theme.lightPrimaryColor
        onClicked: togglePlay()

        // Playback disables the button, which leaves Ok nothing to click. Holding
        // the focus on the poster lets Ok click it, as a tap does, to bring the
        // controls back, and the button takes it again once it reappears.
        onEnabledChanged: {
            if (!enabled && activeFocus) {
                root.forceActiveFocus()
            } else if (enabled && root.activeFocus) {
                forceActiveFocus()
            }
        }

        Binding	{
            target: playButton.icon
            when: overlayMode || !playing // avoid flicker to pause icon when pressing play
            property: "source"
            value: "image://theme/icon-video-overlay-" + (playing ?  "pause" : "play")
        }
    }
    Component {
        id: errorLabelComponent

        Rectangle {
            anchors.fill: parent
            color: Theme.rgba(Theme.overlayBackgroundColor, Theme.highlightBackgroundOpacity)

            opacity: 0
            FadeAnimator on opacity { from: 0; to: 1 }
            InfoLabel {
                //% "Oops, can't load the video"
                text: qsTrId("components_gallery-la-video-loading-failed")
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
