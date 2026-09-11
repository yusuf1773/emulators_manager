#include "models/game_list_model.h"

#include <QString>
#include <utility>

namespace emu {
namespace {

QString FromUtf8(const std::string& value) {
	return QString::fromUtf8(value.data(),
							 static_cast<qsizetype>(value.size()));
}

QString FromPath(const std::filesystem::path& path) {
#ifdef _WIN32
	const auto value = path.native();
	return QString::fromWCharArray(value.data(),
								   static_cast<qsizetype>(value.size()));
#else
	return FromUtf8(path.native());
#endif
}

}

GameListModel::GameListModel(GameService& games, ErrorNotifier& errors,
							 QObject* parent)
	: QAbstractListModel(parent), service_(games), errors_(errors) {}

int GameListModel::rowCount(const QModelIndex& parent) const {
	if (parent.isValid()) return 0;
	return count();
}

QVariant GameListModel::data(const QModelIndex& index, int role) const {
	if (!index.isValid() || index.row() < 0 || index.row() >= count()) {
		return {};
	}
	const Game& game = games_[static_cast<std::size_t>(index.row())];
	switch (role) {
		case kIdRole:
			return FromUtf8(game.id.value());
		case kNameRole:
			return FromUtf8(game.name);
		case kPlatformRole:
			return FromUtf8(game.platform);
		case kPathRole:
			return FromPath(game.content_path);
		default:
			return {};
	}
}

QHash<int, QByteArray> GameListModel::roleNames() const {
	return {
		{kIdRole, "id"},
		{kNameRole, "name"},
		{kPlatformRole, "platform"},
		{kPathRole, "path"},
	};
}

bool GameListModel::Reload() {
	auto games = service_.List();
	if (!games) {
		errors_.Present(games.error());
		return false;
	}
	const bool count_changed = games_.size() != games->size();
	beginResetModel();
	games_ = std::move(*games);
	endResetModel();
	if (count_changed) emit countChanged();
	return true;
}

}
