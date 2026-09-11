import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
	id: window

	required property var gameListModel
	required property var errorNotifier

	width: 960
	height: 640
	visible: true
	title: qsTr("Emulator Manager")

	ColumnLayout {
		anchors.centerIn: parent
		spacing: 8

		Label {
			Layout.alignment: Qt.AlignHCenter
			font.pixelSize: 28
			text: qsTr("Emulator Manager")
		}

		Label {
			Layout.alignment: Qt.AlignHCenter
			text: qsTr("Games loaded: %1").arg(window.gameListModel.count)
		}
	}

	Pane {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		visible: window.errorNotifier.active

		RowLayout {
			anchors.fill: parent

			Label {
				Layout.fillWidth: true
				text: window.errorNotifier.message
				wrapMode: Text.Wrap
			}

			Button {
				text: qsTr("Dismiss")
				onClicked: window.errorNotifier.Clear()
			}
		}
	}
}
