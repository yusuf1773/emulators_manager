pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
	id: window
	required property var gameListModel
	required property var emulatorListModel
	property bool emulatorMode: navigation.currentIndex === 1
	readonly property var catalogModel: emulatorMode ? emulatorListModel : gameListModel
	required property var errorNotifier
	width: 1000
	height: 680
	minimumWidth: 640
	minimumHeight: 480
	visible: true
	title: qsTr("Emulator Manager")
	footer: TabBar {
		id: navigation
		TabButton { text: qsTr("Игры") }
		TabButton { text: qsTr("Эмуляторы") }
		onCurrentIndexChanged: window.errorNotifier.Clear()
	}

	function editGame(gameId, gameName, gamePath, gamePlatform, argumentsList) {
		editor.gameId = gameId
		nameField.text = gameName
		pathField.text = gamePath
		platformField.text = gamePlatform
		argumentsField.text = (argumentsList || []).join("\n")
		window.errorNotifier.Clear()
		editor.open()
	}

	header: ToolBar {
		RowLayout {
			anchors.fill: parent
			anchors.margins: 8
			Label {
				text: (window.emulatorMode ? qsTr("Эмуляторы · %1") : qsTr("Библиотека · %1")).arg(window.catalogModel.count)
				font.pixelSize: 22
				Layout.fillWidth: true
			}
			Button {
				text: qsTr("Обновить")
				onClicked: window.catalogModel.Reload()
			}
			Button {
				text: (window.emulatorMode ? qsTr("Добавить эмулятор") : qsTr("Добавить игру"))
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
			visible: window.catalogModel.count === 0
			text: window.emulatorMode ? qsTr("Эмуляторов пока нет. Добавьте первый кнопкой сверху.") : qsTr("Библиотека пуста. Добавьте первую игру кнопкой сверху.")
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			wrapMode: Text.Wrap
			Layout.fillWidth: true
			Layout.fillHeight: true
		}
		ListView {
			id: games
			visible: window.catalogModel.count > 0
			Layout.fillWidth: true
			Layout.fillHeight: true
			clip: true
			spacing: 8
			model: window.catalogModel
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
						onClicked: window.editGame(row.model.id, row.model.name, row.model.path, row.model.platform, window.emulatorMode ? row.model.arguments : [])
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
		title: gameId === "" ? (window.emulatorMode ? qsTr("Добавить эмулятор") : qsTr("Добавить игру")) : (window.emulatorMode ? qsTr("Изменить эмулятор") : qsTr("Изменить игру"))
		anchors.centerIn: parent
		width: Math.min(window.width - 40, 580)
		modal: true
		contentItem: ColumnLayout {
			Label { text: qsTr("Название") }
			TextField { id: nameField; Layout.fillWidth: true; selectByMouse: true }
			Label { text: window.emulatorMode ? qsTr("Исполняемый файл") : qsTr("Файл игры") }
			RowLayout {
				TextField { id: pathField; Layout.fillWidth: true; selectByMouse: true }
				Button { text: qsTr("Выбрать…"); onClicked: filePicker.open() }
			}
			Label { text: qsTr("Платформа") }
			TextField { id: platformField; Layout.fillWidth: true; selectByMouse: true }
			Label { visible: window.emulatorMode; text: qsTr("Аргументы — по одному на строку, без обрамляющих кавычек") }
			ScrollView {
				visible: window.emulatorMode
				Layout.fillWidth: true
				Layout.preferredHeight: 90
				TextArea { id: argumentsField; selectByMouse: true; wrapMode: TextEdit.NoWrap }
			}
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
						const values = argumentsField.text === "" ? [] : argumentsField.text.split("\n")
						const saved = window.emulatorMode
							? window.catalogModel.Save(editor.gameId, nameField.text.trim(), pathField.text, platformField.text.trim(), values)
							: window.catalogModel.Save(editor.gameId, nameField.text.trim(), pathField.text, platformField.text.trim())
						if (saved)
							editor.close()
					}
				}
			}
		}
		onOpened: nameField.forceActiveFocus()
	}
	FileDialog {
		id: filePicker
		title: window.emulatorMode ? qsTr("Выберите исполняемый файл") : qsTr("Выберите файл игры")
		fileMode: FileDialog.OpenFile
		onAccepted: pathField.text = window.catalogModel.LocalPath(selectedFile)
	}
	Dialog {
		id: deletion
		property string gameId: ""
		property string gameName: ""
		title: window.emulatorMode ? qsTr("Удалить эмулятор?") : qsTr("Удалить игру?")
		anchors.centerIn: parent
		width: Math.min(window.width - 40, 480)
		modal: true
		standardButtons: Dialog.Yes | Dialog.No
		Label {
			width: parent.width
			text: qsTr("Удалить «%1» и связанные профили запуска? Файлы останутся на диске.").arg(deletion.gameName)
			textFormat: Text.PlainText
			wrapMode: Text.Wrap
		}
		onAccepted: window.catalogModel.Remove(gameId)
	}
}
