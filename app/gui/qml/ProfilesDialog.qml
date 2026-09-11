pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Dialog {
	id: root
	required property var profiles
	required property var emulators
	required property var errors
	property string gameName: ""
	property string gameId: ""
	title: qsTr("Профили — %1").arg(gameName)
	modal: true
	width: Math.min(parent.width - 30, 740)
	height: Math.min(parent.height - 30, 560)
	anchors.centerIn: parent
	standardButtons: Dialog.Close

	function openFor(id, name) {
		gameId = id
		gameName = name
		errors.Clear()
		emulators.Reload()
		profiles.SelectGame(id)
		open()
	}
	function edit(values) {
		errors.Clear()
		form.profileId = values.profileId || ""
		profileName.text = values.name || ""
		emulator.currentIndex = values.emulatorId ? emulator.indexOfValue(values.emulatorId) : (emulator.count > 0 ? 0 : -1)
		strategy.currentIndex = values.strategy && values.strategy !== "direct" ? -1 : 0
		args.text = (values.arguments || []).join("\n")
		directory.text = values.directory || ""
		isDefault.checked = values.isDefault || false
		form.open()
	}
	contentItem: ColumnLayout {
		Label {
			Layout.fillWidth: true
			visible: root.errors.active && !form.opened
			text: root.errors.message
			textFormat: Text.PlainText
			wrapMode: Text.Wrap
		}
		Button {
			text: qsTr("Добавить профиль")
			enabled: root.emulators.count > 0
			onClicked: root.edit({})
		}
		Label {
			visible: root.emulators.count === 0
			text: qsTr("Сначала добавьте эмулятор на вкладке «Эмуляторы».")
			wrapMode: Text.Wrap
			Layout.fillWidth: true
		}
		Label { visible: root.profiles.count === 0; text: qsTr("У этой игры пока нет профилей запуска.") }
		ListView {
			id: list
			Layout.fillWidth: true
			Layout.fillHeight: true
			clip: true
			model: root.profiles
			ScrollBar.vertical: ScrollBar {}
			delegate: Pane {
				id: row
				required property int index
				required property var model
				width: list.width
				contentItem: RowLayout {
					Label {
						Layout.fillWidth: true
						text: row.model.name + (row.model.isDefault ? qsTr(" · по умолчанию") : "")
						textFormat: Text.PlainText
						elide: Text.ElideRight
					}
					Button { text: qsTr("Основной"); enabled: !row.model.isDefault; onClicked: root.profiles.SetDefault(row.model.profileId) }
					Button { text: qsTr("Изменить"); onClicked: root.edit(root.profiles.Get(row.index)) }
					Button {
						text: qsTr("Удалить")
						onClicked: { remove.profileId = row.model.profileId; remove.open() }
					}
				}
			}
		}
	}
	Dialog {
		id: form
		property string profileId: ""
		title: profileId === "" ? qsTr("Новый профиль") : qsTr("Изменить профиль")
		modal: true
		anchors.centerIn: parent
		width: Math.min(root.width - 20, 600)
		contentItem: ColumnLayout {
			TextField { id: profileName; placeholderText: qsTr("Название профиля"); Layout.fillWidth: true }
			Label { text: qsTr("Эмулятор") }
			ComboBox { id: emulator; model: root.emulators; textRole: "name"; valueRole: "id"; Layout.fillWidth: true }
			Label { text: qsTr("Стратегия запуска") }
			ComboBox { id: strategy; model: ["direct"]; Layout.fillWidth: true }
			Label { text: qsTr("Аргументы — по одному на строку") }
			ScrollView {
				Layout.fillWidth: true
				Layout.preferredHeight: 70
				TextArea { id: args; selectByMouse: true }
			}
			Label { text: qsTr("Рабочая папка (необязательно)") }
			RowLayout {
				TextField { id: directory; Layout.fillWidth: true }
				Button { text: qsTr("Выбрать…"); onClicked: folder.open() }
			}
			CheckBox { id: isDefault; text: qsTr("Профиль по умолчанию") }
			Label { visible: root.errors.active; text: root.errors.message; textFormat: Text.PlainText; wrapMode: Text.Wrap; Layout.fillWidth: true }
			RowLayout {
				Button { text: qsTr("Отмена"); onClicked: form.close() }
				Button {
					text: qsTr("Сохранить")
					enabled: profileName.text.trim().length > 0 && emulator.currentIndex >= 0 && strategy.currentIndex >= 0
					onClicked: {
						if (root.profiles.Save(form.profileId, profileName.text.trim(), emulator.currentValue, strategy.currentText, args.text === "" ? [] : args.text.split("\n"), directory.text, isDefault.checked)) form.close()
					}
				}
			}
		}
	}
	FolderDialog { id: folder; onAccepted: directory.text = root.emulators.LocalPath(selectedFolder) }
	Dialog {
		id: remove
		property string profileId: ""
		title: qsTr("Удалить профиль запуска?")
		modal: true
		anchors.centerIn: parent
		standardButtons: Dialog.Yes | Dialog.No
		onAccepted: root.profiles.Remove(profileId)
	}
}
