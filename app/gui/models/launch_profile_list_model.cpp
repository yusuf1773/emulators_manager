#include "models/launch_profile_list_model.h"

#include <algorithm>
#include <utility>

namespace emu {

LaunchProfileListModel::LaunchProfileListModel(LaunchProfileService& service,
											   ErrorNotifier& errors)
	: service_(service), errors_(errors) {}
int LaunchProfileListModel::rowCount(const QModelIndex& parent) const {
	return parent.isValid() ? 0 : count();
}
QHash<int, QByteArray> LaunchProfileListModel::roleNames() const {
	return {{kIdRole, "profileId"},		   {kNameRole, "name"},
			{kEmulatorRole, "emulatorId"}, {kStrategyRole, "strategy"},
			{kArgumentsRole, "arguments"}, {kDirectoryRole, "directory"},
			{kDefaultRole, "isDefault"}};
}
QVariant LaunchProfileListModel::data(const QModelIndex& index,
									  int role) const {
	if (!index.isValid() || index.model() != this || index.row() < 0 ||
		index.row() >= count())
		return {};
	const auto& p = profiles_[index.row()];
	switch (role) {
		case kIdRole:
			return QString::fromStdString(p.id.value());
		case kNameRole:
			return QString::fromStdString(p.name);
		case kEmulatorRole:
			return QString::fromStdString(p.emulator_id.value());
		case kStrategyRole:
			return QString::fromStdString(p.strategy_id);
		case kDefaultRole:
			return p.is_default;
		case kArgumentsRole: {
			QStringList values;
			for (const auto& value : p.arguments)
				values.push_back(QString::fromStdString(value));
			return values;
		}
		case kDirectoryRole:
#ifdef _WIN32
			return QString::fromStdWString(p.working_directory.native());
#else
			return QString::fromStdString(p.working_directory.native());
#endif
		default:
			return {};
	}
}
QVariantMap LaunchProfileListModel::Get(int row) const {
	QVariantMap values;
	if (row < 0 || row >= count()) return values;
	const auto roles = roleNames();
	for (auto i = roles.begin(); i != roles.end(); ++i)
		values.insert(QString::fromUtf8(i.value()), data(index(row), i.key()));
	return values;
}
bool LaunchProfileListModel::SelectGame(const QString& id) {
	beginResetModel();
	profiles_.clear();
	game_ = GameId(id.toStdString());
	endResetModel();
	emit countChanged();
	return Reload();
}
bool LaunchProfileListModel::Reload() {
	auto result = service_.ListByGame(game_);
	if (!result) {
		errors_.Present(result.error());
		return false;
	}
	beginResetModel();
	profiles_ = std::move(*result);
	endResetModel();
	emit countChanged();
	return true;
}
bool LaunchProfileListModel::Save(const QString& id, const QString& name,
								  const QString& emulator,
								  const QString& strategy,
								  const QStringList& arguments,
								  const QString& directory, bool is_default) {
	if (strategy != QStringLiteral("direct")) {
		errors_.Present(
			{ErrorCode::kInvalidArgument, "Unknown launch strategy"});
		return false;
	}
	std::vector<std::string> args;
	for (const auto& value : arguments) args.push_back(value.toStdString());
#ifdef _WIN32
	std::filesystem::path path(directory.toStdWString());
#else
	std::filesystem::path path(directory.toStdString());
#endif
	if (!id.isEmpty()) {
		auto existing = service_.FindById(LaunchProfileId(id.toStdString()));
		if (!existing) {
			errors_.Present(existing.error());
			return false;
		}
		if (!*existing || (*existing)->game_id != game_) {
			errors_.Present({ErrorCode::kNotFound,
							 "Profile does not belong to selected game"});
			return false;
		}
	}
	auto result =
		id.isEmpty()
			? service_.Create(name.toStdString(), game_,
							  EmulatorId(emulator.toStdString()),
							  strategy.toStdString(), args, path, is_default)
			: service_.Update(LaunchProfileId(id.toStdString()),
							  name.toStdString(), game_,
							  EmulatorId(emulator.toStdString()),
							  strategy.toStdString(), args, path, is_default);
	if (!result) {
		errors_.Present(result.error());
		return false;
	}
	errors_.Clear();
	return Reload();
}
bool LaunchProfileListModel::Remove(const QString& id) {
	auto found = std::find_if(
		profiles_.begin(), profiles_.end(),
		[&](const auto& p) { return p.id.value() == id.toStdString(); });
	if (found == profiles_.end()) {
		errors_.Present({ErrorCode::kNotFound, "Launch profile not found"});
		return false;
	}
	auto result = service_.Remove(found->id);
	if (!result) {
		errors_.Present(result.error());
		return false;
	}
	errors_.Clear();
	return Reload();
}
bool LaunchProfileListModel::SetDefault(const QString& id) {
	for (int row = 0; row < count(); ++row) {
		const auto values = Get(row);
		if (values.value("profileId").toString() == id)
			return Save(id, values.value("name").toString(),
						values.value("emulatorId").toString(),
						values.value("strategy").toString(),
						values.value("arguments").toStringList(),
						values.value("directory").toString(), true);
	}
	errors_.Present({ErrorCode::kNotFound, "Launch profile not found"});
	return false;
}

}
