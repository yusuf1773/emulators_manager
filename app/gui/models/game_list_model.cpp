#include "models/game_list_model.h"

#include <QString>
#include <algorithm>
#include <utility>

namespace emu {
namespace {

QString FromUtf8(const std::string &value) {
  return QString::fromUtf8(value.data(), static_cast<qsizetype>(value.size()));
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

} // namespace

GameListModel::GameListModel(GameService &games, ErrorNotifier &errors,
                             QObject *parent)
    : QAbstractListModel(parent), service_(games), errors_(errors) {}

int GameListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return count();
}

QVariant GameListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= count()) {
    return {};
  }
  const Game &game = games_[static_cast<std::size_t>(index.row())];
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
  if (count_changed)
    emit countChanged();
  return true;
}

QString GameListModel::LocalPath(const QUrl &url) const {
  return url.isLocalFile() ? url.toLocalFile() : QString();
}

bool GameListModel::Save(const QString &id, const QString &name,
                         const QString &path, const QString &platform) {
#ifdef _WIN32
  const std::filesystem::path content_path(path.toStdWString());
#else
  const std::filesystem::path content_path(path.toStdString());
#endif
  auto result = id.isEmpty() ? service_.Create(name.toStdString(), content_path,
                                               platform.toStdString())
                             : service_.Update(GameId(id.toStdString()),
                                               name.toStdString(), content_path,
                                               platform.toStdString());
  if (!result) {
    errors_.Present(result.error());
    return false;
  }
  auto found =
      std::find_if(games_.begin(), games_.end(),
                   [&](const Game &game) { return game.id == result->id; });
  if (found == games_.end()) {
    const int row = count();
    beginInsertRows(QModelIndex(), row, row);
    games_.push_back(std::move(*result));
    endInsertRows();
    emit countChanged();
  } else {
    const int row = static_cast<int>(found - games_.begin());
    *found = std::move(*result);
    emit dataChanged(index(row), index(row));
  }
  errors_.Clear();
  return true;
}

bool GameListModel::Remove(const QString &id) {
  auto result = service_.Remove(GameId(id.toStdString()));
  if (!result) {
    errors_.Present(result.error());
    return false;
  }
  auto found =
      std::find_if(games_.begin(), games_.end(), [&](const Game &game) {
        return game.id.value() == id.toStdString();
      });
  if (found != games_.end()) {
    const int row = static_cast<int>(found - games_.begin());
    beginRemoveRows(QModelIndex(), row, row);
    games_.erase(found);
    endRemoveRows();
    emit countChanged();
  }
  errors_.Clear();
  return true;
}

} // namespace emu
