// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtTest 1.0
import QtQuick 2.6
import Sailfish.Silica 1.0
import Sailfish.Gallery 1.0
import "utils.js" as Utils

Item {
    width: screen.width; height: screen.height

    property var playButton

    VideoPoster {
        id: poster

        anchors.fill: parent
        overlayMode: true
    }

    SignalSpy {
        id: togglePlaySpy
        target: poster
        signalName: "togglePlay"
    }

    SignalSpy {
        id: posterClickedSpy
        target: poster
        signalName: "clicked"
    }

    TestCase {
        name: "VideoPoster"
        when: windowShown

        function init() {
            playButton = Utils.findChild(poster, "IconButton")
            poster.playing = false
            poster.overlayMode = true
            togglePlaySpy.clear()
            posterClickedSpy.clear()
        }

        function test_playByKey() {
            verify(playButton != null)

            playButton.forceActiveFocus()
            keyClick(Qt.Key_Return)
            compare(togglePlaySpy.count, 1)
        }

        function test_focusFollowsPlayback() {
            verify(playButton != null)
            playButton.forceActiveFocus()

            // Playback hides the controls: the poster holds the focus so that
            // Ok can still bring them back.
            poster.playing = true
            poster.overlayMode = false
            tryCompare(poster, "activeFocus", true)

            poster.overlayMode = true
            tryCompare(playButton, "activeFocus", true)
        }
    }
}
