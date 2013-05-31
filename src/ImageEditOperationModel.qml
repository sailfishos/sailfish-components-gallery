/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Raine Mäkeläinen <raine.makelainen@jollamobile.com>
**
****************************************************************************/

import QtQuick 2.0

ListModel {
    function _operation(index) {
        if (_operation["list"] === undefined) {
            _operation.list = [
                        /*
                        {
                            //: Rotate the image right edit operation (clockwise)
                            //% "Rotate right"
                            text: qsTrId("components_gallery-li-rotate_right"),
                            type: "rotateRight",
                            icon: "image://theme/icon-m-backup"
                        },

                        {
                            //: Rotate the image left edit operation (counter clockwise)
                            //% "Rotate left"
                            text: qsTrId("components_gallery-li-rotate_left"),
                            type: "rotateLeft",
                            icon: "image://theme/icon-m-backup"
                        },

                        {
                            //: Flip the image horizontally
                            //% "Flip horizontally"
                            text: qsTrId("components_gallery-li-flip_horizontally"),
                            type: "flipHorizontally",
                            icon: "image://theme/icon-m-backup"
                        },

                        {
                            //: Flip the image vertically
                            //% "Flip vertically"
                            text: qsTrId("components_gallery-li-flip_vertically"),
                            type: "flipVertically",
                            icon: "image://theme/icon-m-backup"
                        },*/

                        {
                            //: Crop the image. This opens separete cropping view
                            //% "Crop"
                            text: qsTrId("components_gallery-li-crop"),
                            type: "crop",
                            icon: "image://theme/icon-m-backup"
                        }
                    ]
        }
        return _operation.list[index]
    }

    Component.onCompleted: {
        var index = 0
        for (; index < 1; ++index) {
            append(_operation(index))
        }
    }
}
