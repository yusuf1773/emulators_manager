#pragma once

#include <QAbstractListModel>
#include <QStringList>
#include <QVariantMap>

#include "emu/application/launch_profile_service.h"
#include "error_notifier.h"

namespace emu {

class LaunchProfileListModel final : public QAbstractListModel {
	Q_OBJECT
	Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
	enum Role {
		kIdRole = Qt::UserRole + 1,
		kNameRole,
		kEmulatorRole,
		kStrategyRole,
		kArgumentsRole,
		kDirectoryRole,
		kDefaultRole
	};
	LaunchProfileListModel(LaunchProfileService& service,
						   ErrorNotifier& errors);
	int rowCount(const QModelIndex& parent = {}) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QHash<int, QByteArray> roleNames() const override;
	int count() const { return static_cast<int>(profiles_.size()); }
	Q_INVOKABLE bool SelectGame(const QString& id);
	Q_INVOKABLE bool Save(const QString& id, const QString& name,
						  const QString& emulator, const QString& strategy,
						  const QStringList& arguments,
						  const QString& directory, bool is_default);
	Q_INVOKABLE bool Remove(const QString& id);
	Q_INVOKABLE bool SetDefault(const QString& id);
	Q_INVOKABLE QVariantMap Get(int row) const;

signals:
	void countChanged();

private:
	bool Reload();
	LaunchProfileService& service_;
	ErrorNotifier& errors_;
	GameId game_;
	std::vector<LaunchProfile> profiles_;
};

}
