// SPDX-FileCopyrightText: 2013-2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024-2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtTest 1.0
import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Gallery 1.0
import "utils.js" as Utils

Item {
    width: screen.width; height: screen.height

    // Grid needs this, usually it comes from Page
    property bool isPortrait: true
    property var thumbnail

    ImageGridView {
        id: imageGridView
        anchors.fill: parent

        delegate: ThumbnailImage {
            source: url
            mimeType: model.mimeType

        }
        model: testModel
    }

    ListModel {
        id: testModel
        Component.onCompleted: append({"mimeType": "image/jpeg",
                                          "url": "file://" + StandardPaths.pictures + "Default/All_green.jpg" })
    }


    SignalSpy {
        id: clickedSpy
        signalName: "clicked"
    }

    SignalSpy {
        // WORKAROUND: Compared to the other SignalSpy items, this one deals
        // with the fact that MouseArea contains one property and one signal
        // sharing the same name "pressed" - SignalSpy fails with such a target.
        id: pressedSpy_
        target: pressedSpy
        signalName: "targetPressed"

        Connections {
            id: pressedSpy

            target: pressedSpyFakeTarget
            onPressed: targetPressed()
            signal targetPressed()
            property alias count: pressedSpy_.count
            function clear() { pressedSpy_.clear() }
        }

        // Used to suppress warning about nonexisting signal during initialization
        Item {
            id: pressedSpyFakeTarget
            signal pressed()
        }
    }

    SignalSpy {
        id: releasedSpy
        signalName: "released"
    }

    SignalSpy {
        id: pressAndHoldSpy
        signalName: "pressAndHold"
    }

    TestCase {
        name: "ThumbnailImage"
        when: windowShown

        function test_init() {
            isPortrait = true
            thumbnail = Utils.findChild(imageGridView, "ThumbnailImage_QMLTYPE")
            clickedSpy.target = thumbnail
            pressedSpy.target = thumbnail
            releasedSpy.target = thumbnail
            pressAndHoldSpy.target = thumbnail
        }

        function test_thumbnailImage() {
            verify(thumbnail != null)

            compare(thumbnail.width, imageGridView.cellSize)
            compare(thumbnail.height, imageGridView.cellSize)
            compare(thumbnail.source, "file://" + StandardPaths.pictures + "Default/All_green.jpg")
            compare(thumbnail.mimeType, "image/jpeg")

            // Get the actual Thumbnail item from ThumbnailBase
            var nemoThumbnail = thumbnail._thumbnail
            verify(nemoThumbnail != null)
            compare(nemoThumbnail.sourceSize.height, thumbnail.width)
            compare(nemoThumbnail.sourceSize.width, thumbnail.height)

            isPortrait = false
            compare(thumbnail.width, imageGridView.cellSize)
            compare(thumbnail.height, imageGridView.cellSize)
            compare(nemoThumbnail.sourceSize.height, thumbnail.width)
            compare(nemoThumbnail.sourceSize.width, thumbnail.height)
        }

        function test_thumbnailImageClicked() {
            verify(thumbnail != null)

            pressedSpy.clear()

            mouseClick(thumbnail, thumbnail.x, thumbnail.y)
            tryCompare(clickedSpy, "count", 1)
            tryCompare(pressedSpy, "count", 1)
            tryCompare(releasedSpy, "count", 1)
        }

        function test_thumbnailImagePressedReleased() {
            verify(thumbnail != null)

            pressedSpy.clear()
            releasedSpy.clear()

            mousePress(thumbnail, thumbnail.x, thumbnail.y)
            tryCompare(pressedSpy, "count", 1)

            mouseRelease(thumbnail, thumbnail.x, thumbnail.y)
            tryCompare(releasedSpy, "count", 1)
        }
    }
}
