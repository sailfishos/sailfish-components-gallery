/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Raine Mäkeläinen <raine.makelainen@jollamobile.com>
**
****************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0

SplitViewDialog {
    id: aspectRatioDialog

    onDone: {
         if (result == DialogResult.Accepted) {
             cropView.crop()
         }
         pageStack.pop()
    }

    onSplitOpenedChanged: _verifyPageIndicatorVisibility(aspectRatioDialog)

    background: SilicaListView {
        anchors.fill: parent

        header: DialogHeader {
            dialog: aspectRatioDialog
        }

        delegate: LabelItem {
            text: model.text
            //: Label that is shown for currently selected aspect ratio.
            //% "Aspect ratio"
            sectionLabel: qsTrId("components_gallery-li-aspect_ratio")
            selected: cropView.aspectRatioType == model.type

            onClicked: {
                cropView.aspectRatio = model.ratio
                cropView.aspectRatioType = model.type
                aspectRatioDialog.splitOpen = !aspectRatioDialog.splitOpen
            }
        }

        model: AspectRatioModel {}
    }

    Component.onCompleted: _verifyPageIndicatorVisibility(aspectRatioDialog)
}
