import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import styleQml 1.0
import Tuto02DialogQml 1.0

Item {
    id: messageDialogItem
    clip: true

    // c++ class
    DialoggMessageDialogs {
        id: Tuto02DialogQmlMessageDialogs
    }

    ScrollView {
        id: scrollView
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            leftMargin: 12
        }
        ColumnLayout {
            spacing: 8
            Item { Layout.preferredHeight: 4 } // padding
            Label {
                font.bold: true
                text: "Message dialog properties:"
            }
            RowLayout {
                Label {
                    id: customizeTitle
                    text: "Window Title"
                    Layout.alignment: Qt.AlignBaseline
                    font.italic: true
                }
                TextField {
                    id: windowTitleField
                    Layout.alignment: Qt.AlignBaseline
                    Layout.fillWidth: true
                    placeholderText: "Custom Dialog"
                    onTextChanged: Tuto02DialogQmlMessageDialogs.title = windowTitleField.text
                    Component.onCompleted: Tuto02DialogQmlMessageDialogs.title = windowTitleField.placeholderText
                }
            }
            ButtonGroup {
                id: buttonGroup
            }

            // the icon type of the message dialog
            GroupBox {
                id: groupBox
                title: "Icon"
                Row {
                    spacing: 10
                    Layout.fillWidth: true
                    RadioButton {
                        text: "None"
                        checked: false
                        ButtonGroup.group: buttonGroup
                        onCheckedChanged: Tuto02DialogQmlMessageDialogs.icon = (checked ? 0 : Tuto02DialogQmlMessageDialogs.icon)
                    }
                    RadioButton {
                        text: "Info"
                        checked: false
                        ButtonGroup.group: buttonGroup
                        onCheckedChanged: Tuto02DialogQmlMessageDialogs.icon = (checked ? 1 : Tuto02DialogQmlMessageDialogs.icon)
                    }
                    RadioButton {
                        text: "Warning"
                        checked: false
                        ButtonGroup.group: buttonGroup
                        onCheckedChanged: Tuto02DialogQmlMessageDialogs.icon = (checked ? 2 : Tuto02DialogQmlMessageDialogs.icon)
                    }
                    RadioButton {
                        text: "Critical"
                        checked: false
                        ButtonGroup.group: buttonGroup
                        onCheckedChanged: Tuto02DialogQmlMessageDialogs.icon = (checked ? 3 : Tuto02DialogQmlMessageDialogs.icon)
                    }
                    RadioButton {
                        text: "Question"
                        checked: false
                        ButtonGroup.group: buttonGroup
                        onCheckedChanged: Tuto02DialogQmlMessageDialogs.icon = (checked ? 4 : Tuto02DialogQmlMessageDialogs.icon)
                    }

                }
            }

            RowLayout {
                Label {
                    id: customizeMessage
                    text: "Window Message"
                    Layout.alignment: Qt.AlignBaseline
                    font.italic: true
                }
                TextField {
                    id: windowMessageField
                    Layout.alignment: Qt.AlignBaseline
                    Layout.fillWidth: true
                    placeholderText: "Custom Message"
                    onTextChanged: Tuto02DialogQmlMessageDialogs.message = windowMessageField.text
                    Component.onCompleted: Tuto02DialogQmlMessageDialogs.message = windowMessageField.placeholderText
                }
            }
            Label {
                text: "Buttons:"
                font.italic: true
            }
            // update the CheckBox status that represent the list of button for message dialog
            Flow {
                spacing: 8
                Layout.fillWidth: true
                Layout.preferredWidth: messageDialogItem.width - 30
                property bool updating: false
                function updateButtons(button, checked) {
                    if (updating) return
                    updating = true
                    var buttons = 0
                    for (var i = 0; i < children.length; ++i)
                        if (children[i].checked)
                            buttons |= children[i].button
                    Tuto02DialogQmlMessageDialogs.buttons = buttons
                    updating = false
                }

                CheckBox {
                    text: "Cancel"
                    property int button: Dialog.Cancel
                    onCheckedChanged: parent.updateButtons(button, checked)
                }

                CheckBox {
                    text: "No"
                    property int button: Dialog.No
                    onCheckedChanged: parent.updateButtons(button, checked)
                }

                CheckBox {
                    text: "OK"
                    property int button: Dialog.Ok
                    onCheckedChanged: parent.updateButtons(button, checked)
                }

                CheckBox {
                    text: "Yes"
                    property int button: Dialog.Yes
                    onCheckedChanged: parent.updateButtons(button, checked)
                }
            }
            Label {
                id: result
                text: "The result is: " + Tuto02DialogQmlMessageDialogs.result
                font.bold: true
            }
            RowLayout {
                id: buttonRow
                Button {
                    text: "Open"
                    onClicked: Tuto02DialogQmlMessageDialogs.open()
                }
            }
        }
    }
}
