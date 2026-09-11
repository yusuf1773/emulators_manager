#pragma once

#include <QAbstractListModel>
#include <vector>

#include "emu/application/game_service.h"
#include "emu/domain/game.h"
#include "error_notifier.h"

namespace emu {

class GameListModel final : public QAbstractListModel {
	Q_OBJECT
	Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
	enum Role {
		kIdRole = Qt::UserRole + 1,
		kNameRole,
		kPlatformRole,
		kPathRole,
	};
	Q_ENUM(Role)

	GameListModel(GameService& games, ErrorNotifier& errors,
				  QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index,
				  int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;
	int count() const { return static_cast<int>(games_.size()); }

	Q_INVOKABLE bool Reload();

signals:
	void countChanged();

private:
	GameService& service_;
	ErrorNotifier& errors_;
	std::vector<Game> games_;
};

}
