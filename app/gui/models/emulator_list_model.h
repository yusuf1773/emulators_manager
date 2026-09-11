#pragma once

#include <QAbstractListModel>
#include <QStringList>
#include <QUrl>
#include <vector>

#include "emu/application/emulator_service.h"
#include "emu/domain/emulator.h"
#include "error_notifier.h"

namespace emu {

class EmulatorListModel final : public QAbstractListModel {
	Q_OBJECT
	Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
	enum Role {
		kIdRole = Qt::UserRole + 1,
		kNameRole,
		kPlatformRole,
		kPathRole,
		kArgumentsRole,
	};
	Q_ENUM(Role)

	EmulatorListModel(EmulatorService &emulators, ErrorNotifier &errors,
					  QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index,
				  int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;
	int count() const { return static_cast<int>(emulators_.size()); }

	Q_INVOKABLE bool Reload();
	Q_INVOKABLE bool Save(const QString &id, const QString &name,
						  const QString &path, const QString &platform,
						  const QStringList &arguments);
	Q_INVOKABLE bool Remove(const QString &id);
	Q_INVOKABLE QString LocalPath(const QUrl &url) const;

signals:
	void countChanged();

private:
	EmulatorService &service_;
	ErrorNotifier &errors_;
	std::vector<Emulator> emulators_;
};

}
