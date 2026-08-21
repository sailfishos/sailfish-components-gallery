// SPDX-FileCopyrightText: 2013-2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024-2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Gallery 1.0

GridItem {
    id: thumbnail

    property url source
    property string mimeType: model && model.mimeType ? model.mimeType : ""
    property int size: GridView.view.cellSize
    property bool selected

    width: size
    contentHeight: size
    opacity: down && selected && !menuOpen ? 0.8 : 1.0
    // _keyFocused has to be repeated here: GridItem already carries it, but
    // replacing the binding would drop it, and the highlight below is the
    // only thing that paints over an opaque thumbnail.
    highlighted: down || menuOpen || selected || _keyFocused

    HighlightItem {
        z: 1
        active: thumbnail.highlighted && !thumbnail.menuOpen
        anchors.fill: parent
    }
}
