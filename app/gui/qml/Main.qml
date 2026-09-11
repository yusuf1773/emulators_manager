pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
	id: window
	required property var gameListModel
	required property var errorNotifier
	width: 1000
	height: 680
	minimumWidth: 640
	minimumHeight: 480
	visible: true
	title: qsTr("Emulator Manager — Библиотека")

	function editGame(gameId, gameName, gamePath, gamePlatform) {
		editor.gameId = gameId
		nameField.text = gameName
		pathField.text = gamePath
		platformField.text = gamePlatform
		window.errorNotifier.Clear()
		editor.open()
	}

	header: ToolBar {
		RowLayout {
			anchors.fill: parent
			anchors.margins: 8
			Label {
				text: qsTr("Библиотека · %1").arg(window.gameListModel.count)
				font.pixelSize: 22
				Layout.fillWidth: true
			}
			Button {
				text: qsTr("Обновить")
				onClicked: window.gameListModel.Reload()
			}
			Button {
				text: qsTr("Добавить игру")
				onClicked: window.editGame("", "", "", "")
			}
		}
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: 20
		Label {
			visible: window.errorNotifier.active && !editor.opened
			text: window.errorNotifier.message
			textFormat: Text.PlainText
			wrapMode: Text.Wrap
			Layout.fillWidth: true
		}
		Label {
			visible: window.gameListModel.count === 0
			text: qsTr("Библиотека пуста. Добавьте первую игру кнопкой сверху.")
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			wrapMode: Text.Wrap
			Layout.fillWidth: true
			Layout.fillHeight: true
		}
		ListView {
			id: games
			visible: window.gameListModel.count > 0
			Layout.fillWidth: true
			Layout.fillHeight: true
			clip: true
			spacing: 8
			model: window.gameListModel
			ScrollBar.vertical: ScrollBar {}
			delegate: Pane {
				id: row
				required property var model
				width: games.width
				contentItem: RowLayout {
					ColumnLayout {
						Layout.fillWidth: true
						Label {
							text: row.model.name
							textFormat: Text.PlainText
							font.bold: true
							elide: Text.ElideRight
							Layout.fillWidth: true
						}
						Label {
							text: row.model.platform || qsTr("Платформа не указана")
							textFormat: Text.PlainText
							elide: Text.ElideRight
							Layout.fillWidth: true
						}
						Label {
							text: row.model.path
							textFormat: Text.PlainText
							elide: Text.ElideMiddle
							Layout.fillWidth: true
						}
					}
					Button {
						text: qsTr("Изменить")
						onClicked: window.editGame(row.model.id, row.model.name, row.model.path, row.model.platform)
					}
					Button {
						text: qsTr("Удалить")
						onClicked: {
							deletion.gameId = row.model.id
							deletion.gameName = row.model.name
							deletion.open()
						}
					}
				}
			}
		}
	}

	Dialog {
		id: editor
		property string gameId: ""
		title: gameId === "" ? qsTr("Добавить игру") : qsTr("Изменить игру")
		anchors.centerIn: parent
		width: Math.min(window.width - 40, 580)
		modal: true
		contentItem: ColumnLayout {
			Label { text: qsTr("Название") }
			TextField { id: nameField; Layout.fillWidth: true; selectByMouse: true }
			Label { text: qsTr("Файл игры") }
			RowLayout {
				TextField { id: pathField; Layout.fillWidth: true; selectByMouse: true }
				Button { text: qsTr("Выбрать…"); onClicked: filePicker.open() }
			}
			Label { text: qsTr("Платформа") }
			TextField { id: platformField; Layout.fillWidth: true; selectByMouse: true }
			Label {
				visible: window.errorNotifier.active
				text: window.errorNotifier.message
				textFormat: Text.PlainText
				wrapMode: Text.Wrap
				Layout.fillWidth: true
			}
			RowLayout {
				Layout.alignment: Qt.AlignRight
				Button { text: qsTr("Отмена"); onClicked: editor.close() }
				Button {
					text: qsTr("Сохранить")
					enabled: nameField.text.trim().length > 0 && pathField.text.trim().length > 0
					onClicked: {
						if (window.gameListModel.Save(editor.gameId, nameField.text.trim(), pathField.text, platformField.text.trim()))
							editor.close()
					}
				}
			}
		}
		onOpened: nameField.forceActiveFocus()
	}
	FileDialog {
		id: filePicker
		title: qsTr("Выберите файл игры")
		fileMode: FileDialog.OpenFile
		onAccepted: pathField.text = window.gameListModel.LocalPath(selectedFile)
	}
	Dialog {
		id: deletion
		property string gameId: ""
		property string gameName: ""
		title: qsTr("Удалить игру?")
		anchors.centerIn: parent
		width: Math.min(window.width - 40, 480)
		modal: true
		standardButtons: Dialog.Yes | Dialog.No
		Label {
			width: parent.width
			text: qsTr("Удалить «%1» и её профили запуска? Файл игры останется на диске.").arg(deletion.gameName)
			textFormat: Text.PlainText
			wrapMode: Text.Wrap
		}
		onAccepted: window.gameListModel.Remove(gameId)
	}
}
