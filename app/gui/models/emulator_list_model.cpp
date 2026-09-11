#include "models/emulator_list_model.h"

#include <QString>
#include <algorithm>
#include <utility>

namespace emu {
namespace {

QString FromUtf8(const std::string &value) {
	return QString::fromUtf8(value.data(),
							 static_cast<qsizetype>(value.size()));
}

QString FromPath(const std::filesystem::path &path) {
#ifdef _WIN32
	const auto value = path.native();
	return QString::fromWCharArray(value.data(),
								   static_cast<qsizetype>(value.size()));
#else
	return FromUtf8(path.native());
#endif
}

}

EmulatorListModel::EmulatorListModel(EmulatorService &emulators,
									 ErrorNotifier &errors, QObject *parent)
	: QAbstractListModel(parent), service_(emulators), errors_(errors) {}

int EmulatorListModel::rowCount(const QModelIndex &parent) const {
	if (parent.isValid()) return 0;
	return count();
}

QVariant EmulatorListModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid() || index.row() < 0 || index.row() >= count()) {
		return {};
	}
	const Emulator &emulator =
		emulators_[static_cast<std::size_t>(index.row())];
	switch (role) {
		case kIdRole:
			return FromUtf8(emulator.id.value());
		case kNameRole:
			return FromUtf8(emulator.name);
		case kPlatformRole:
			return FromUtf8(emulator.platform);
		case kArgumentsRole: {
			QStringList arguments;
			for (const auto &argument : emulator.base_arguments)
				arguments.push_back(FromUtf8(argument));
			return arguments;
		}
		case kPathRole:
			return FromPath(emulator.executable_path);
		default:
			return {};
	}
}

QHash<int, QByteArray> EmulatorListModel::roleNames() const {
	return {
		{kIdRole, "id"},
		{kNameRole, "name"},
		{kPlatformRole, "platform"},
		{kPathRole, "path"},
		{kArgumentsRole, "arguments"},
	};
}

bool EmulatorListModel::Reload() {
	auto emulators = service_.List();
	if (!emulators) {
		errors_.Present(emulators.error());
		return false;
	}
	const bool count_changed = emulators_.size() != emulators->size();
	beginResetModel();
	emulators_ = std::move(*emulators);
	endResetModel();
	if (count_changed) emit countChanged();
	return true;
}

QString EmulatorListModel::LocalPath(const QUrl &url) const {
	return url.isLocalFile() ? url.toLocalFile() : QString();
}

bool EmulatorListModel::Save(const QString &id, const QString &name,
							 const QString &path, const QString &platform,
							 const QStringList &arguments) {
#ifdef _WIN32
	const std::filesystem::path executable_path(path.toStdWString());
#else
	const std::filesystem::path executable_path(path.toStdString());
#endif
	std::vector<std::string> base_arguments;
	for (const auto &argument : arguments)
		base_arguments.push_back(argument.toStdString());
	auto result = id.isEmpty()
					  ? service_.Create(name.toStdString(), executable_path,
										platform.toStdString(), base_arguments)
					  : service_.Update(EmulatorId(id.toStdString()),
										name.toStdString(), executable_path,
										platform.toStdString(), base_arguments);
	if (!result) {
		errors_.Present(result.error());
		return false;
	}
	auto found = std::find_if(
		emulators_.begin(), emulators_.end(),
		[&](const Emulator &emulator) { return emulator.id == result->id; });
	if (found == emulators_.end()) {
		const int row = count();
		beginInsertRows(QModelIndex(), row, row);
		emulators_.push_back(std::move(*result));
		endInsertRows();
		emit countChanged();
	} else {
		const int row = static_cast<int>(found - emulators_.begin());
		*found = std::move(*result);
		emit dataChanged(index(row), index(row));
	}
	errors_.Clear();
	return true;
}

bool EmulatorListModel::Remove(const QString &id) {
	auto result = service_.Remove(EmulatorId(id.toStdString()));
	if (!result) {
		errors_.Present(result.error());
		return false;
	}
	auto found = std::find_if(
		emulators_.begin(), emulators_.end(), [&](const Emulator &emulator) {
			return emulator.id.value() == id.toStdString();
		});
	if (found != emulators_.end()) {
		const int row = static_cast<int>(found - emulators_.begin());
		beginRemoveRows(QModelIndex(), row, row);
		emulators_.erase(found);
		endRemoveRows();
		emit countChanged();
	}
	errors_.Clear();
	return true;
}

}
