#include "models/game_list_model.h"

#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

#include "emu/application/game_service.h"
#include "emu/persistence/sqlite_repositories.h"
#include "error_notifier.h"
#include "fakes.h"
#include "models/emulator_list_model.h"
#include "models/launch_profile_list_model.h"

namespace emu {
namespace {

class GameListModelTest : public QObject {
	Q_OBJECT

private slots:
	void ExposesGameRoles();
	void ProfileLifecycle();
	void EmulatorPersistenceAndArguments();
	void SavesAndRemovesPersistentGames();
	void RejectsFailedChanges();
	void EmitsResetAndCountSignals();
	void PreservesRowsAndPresentsLoadErrors();
};

void GameListModelTest::SavesAndRemovesPersistentGames() {
	QTemporaryDir directory;
	QVERIFY(directory.isValid());
	const auto path =
		std::filesystem::path(directory.path().toStdString()) / "games.db";
	{
		SqliteDatabase database(path);
		SqliteGameRepository repository(database);
		testing::FixedIdGenerator ids({"persisted"});
		GameService service(repository, ids);
		ErrorNotifier errors;
		GameListModel model(service, errors);
		QSignalSpy inserted(&model, &QAbstractItemModel::rowsInserted);
		QSignalSpy changed(&model, &QAbstractItemModel::dataChanged);
		QVERIFY(model.Save("", "Game", "game.rom", "NES"));
		QCOMPARE(inserted.count(), 1);
		QVERIFY(model.Save("persisted", "Renamed", "new.rom", "SNES"));
		QCOMPARE(changed.count(), 1);
	}
	{
		SqliteDatabase database(path);
		SqliteGameRepository repository(database);
		testing::FixedIdGenerator ids({});
		GameService service(repository, ids);
		ErrorNotifier errors;
		GameListModel model(service, errors);
		QVERIFY(model.Reload());
		QCOMPARE(model.count(), 1);
		QCOMPARE(
			model.data(model.index(0), GameListModel::kNameRole).toString(),
			QString("Renamed"));
		QSignalSpy removed(&model, &QAbstractItemModel::rowsRemoved);
		QVERIFY(model.Remove("persisted"));
		QCOMPARE(removed.count(), 1);
		QCOMPARE(model.count(), 0);
	}
	SqliteDatabase database(path);
	SqliteGameRepository repository(database);
	QVERIFY(repository.FindAll()->empty());
}

void GameListModelTest::RejectsFailedChanges() {
	testing::MemoryGameRepository repository;
	testing::FixedIdGenerator ids({"first", "second"});
	GameService service(repository, ids);
	ErrorNotifier errors;
	GameListModel model(service, errors);
	QVERIFY(model.Save("", "Game", "game.rom", "NES"));
	QVERIFY(!model.Save("", "Game", "other.rom", "NES"));
	QCOMPARE(model.count(), 1);
	QVERIFY(errors.active());
	repository.save_error = true;
	QVERIFY(!model.Save("first", "Changed", "new.rom", "NES"));
	QCOMPARE(model.data(model.index(0), GameListModel::kNameRole).toString(),
			 QString("Game"));
	QVERIFY(!model.Remove("missing"));
	QCOMPARE(model.count(), 1);
	QCOMPARE(model.LocalPath(QUrl::fromLocalFile("/games/a b.rom")),
			 QString("/games/a b.rom"));
	QVERIFY(model.LocalPath(QUrl("https://example.com/game")).isEmpty());
}

void GameListModelTest::EmulatorPersistenceAndArguments() {
	QTemporaryDir directory;
	QVERIFY(directory.isValid());
	const auto path =
		std::filesystem::path(directory.path().toStdString()) / "emulators.db";
	const QString executable =
		QString::fromUtf8("C:/Эмуляторы/My Emulator/emu.exe");
	const QStringList arguments = {"--fullscreen", "a b", "\"quoted\"", ""};
	{
		SqliteDatabase database(path);
		SqliteEmulatorRepository repository(database);
		testing::FixedIdGenerator ids({"emu-1"});
		EmulatorService service(repository, ids);
		ErrorNotifier errors;
		EmulatorListModel model(service, errors);
		QSignalSpy inserted(&model, &QAbstractItemModel::rowsInserted);
		QVERIFY(model.Save("", "Emulator", executable, "NES", arguments));
		QCOMPARE(inserted.count(), 1);
		QCOMPARE(
			model.data(model.index(0), EmulatorListModel::kPathRole).toString(),
			executable);
		QCOMPARE(model.data(model.index(0), EmulatorListModel::kArgumentsRole)
					 .toStringList(),
				 arguments);
		QVERIFY(!model.Save("", "Emulator", executable, "NES", {}));
		QCOMPARE(model.count(), 1);
		QVERIFY(!model.Save("emu-1", "", executable, "NES", {}));
		QSignalSpy changed(&model, &QAbstractItemModel::dataChanged);
		QVERIFY(model.Save("emu-1", "Renamed", executable, "SNES", arguments));
		QCOMPARE(changed.count(), 1);
	}
	{
		SqliteDatabase database(path);
		SqliteEmulatorRepository repository(database);
		testing::FixedIdGenerator ids({});
		EmulatorService service(repository, ids);
		ErrorNotifier errors;
		EmulatorListModel model(service, errors);
		QVERIFY(model.Reload());
		QCOMPARE(model.count(), 1);
		QCOMPARE(
			model.data(model.index(0), EmulatorListModel::kNameRole).toString(),
			QString("Renamed"));
		QCOMPARE(model.data(model.index(0), EmulatorListModel::kArgumentsRole)
					 .toStringList(),
				 arguments);
		QSignalSpy removed(&model, &QAbstractItemModel::rowsRemoved);
		QVERIFY(model.Remove("emu-1"));
		QCOMPARE(removed.count(), 1);
		QVERIFY(!model.Remove("missing"));
	}
	SqliteDatabase database(path);
	SqliteEmulatorRepository repository(database);
	QVERIFY(repository.FindAll()->empty());
}

void GameListModelTest::ProfileLifecycle() {
	QTemporaryDir directory;
	QVERIFY(directory.isValid());
	const auto path =
		std::filesystem::path(directory.path().toStdString()) / "profiles.db";
	{
		SqliteDatabase database(path);
		SqliteGameRepository games(database);
		SqliteEmulatorRepository emulators(database);
		SqliteLaunchProfileRepository profiles(database);
		QVERIFY(games.Save({GameId("game"), "Game", "game.rom", "NES"}));
		QVERIFY(games.Save({GameId("other"), "Other", "other.rom", "NES"}));
		QVERIFY(emulators.Save(
			{EmulatorId("emu"), "Emulator", "emu.exe", "NES", {}}));
		testing::FixedIdGenerator ids({"one", "two"});
		LaunchProfileService service(profiles, games, emulators, ids);
		ErrorNotifier errors;
		LaunchProfileListModel model(service, errors);
		QVERIFY(model.SelectGame("game"));
		QSignalSpy reset(&model, &QAbstractItemModel::modelReset);
		QVERIFY(model.Save("", "First", "emu", "direct",
						   {"a b", "--fullscreen"}, "work dir", true));
		QVERIFY(model.Save("", "Second", "emu", "direct", {}, "", true));
		QCOMPARE(model.count(), 2);
		QVERIFY(reset.count() >= 2);
		QVERIFY(model.SetDefault("one"));
		int defaults = 0;
		for (int row = 0; row < model.count(); ++row) {
			const auto value = model.Get(row);
			if (value.value("isDefault").toBool()) {
				++defaults;
				QCOMPARE(value.value("profileId").toString(), QString("one"));
			}
		}
		QCOMPARE(defaults, 1);
		QVERIFY(!model.Save("", "Invalid", "missing", "direct", {}, "", false));
		QVERIFY(!model.Save("", "Invalid", "emu", "unknown", {}, "", false));
		QCOMPARE(model.count(), 2);
		QVERIFY(model.SelectGame("other"));
		QCOMPARE(model.count(), 0);
		QVERIFY(!model.Save("one", "Moved", "emu", "direct", {}, "", true));
		QVERIFY(!model.Remove("one"));
	}
	{
		SqliteDatabase database(path);
		SqliteGameRepository games(database);
		SqliteEmulatorRepository emulators(database);
		SqliteLaunchProfileRepository profiles(database);
		testing::FixedIdGenerator ids({});
		LaunchProfileService service(profiles, games, emulators, ids);
		ErrorNotifier errors;
		LaunchProfileListModel model(service, errors);
		QVERIFY(model.SelectGame("game"));
		QCOMPARE(model.count(), 2);
		auto first = service.FindById(LaunchProfileId("one"));
		QVERIFY(first && *first);
		QVERIFY((*first)->is_default);
		QCOMPARE((*first)->arguments.size(), std::size_t(2));
		QCOMPARE((*first)->arguments[0], std::string("a b"));
		QVERIFY(model.Remove("two"));
		QCOMPARE(model.count(), 1);
		QVERIFY(emulators.Remove(EmulatorId("emu")));
		QVERIFY(model.SelectGame("game"));
		QCOMPARE(model.count(), 0);
	}
}

void GameListModelTest::ExposesGameRoles() {
	testing::MemoryGameRepository repository;
	repository.values = {
		{GameId("game-1"), "Zelda", "games/zelda.rom", "Switch"},
		{GameId("game-2"), "Mario", "games/mario.rom", "NES"},
	};
	testing::FixedIdGenerator ids({});
	GameService service(repository, ids);
	ErrorNotifier errors;
	GameListModel model(service, errors);

	QVERIFY(model.Reload());
	QCOMPARE(model.rowCount(), 2);
	QCOMPARE(model.count(), 2);
	QCOMPARE(model.rowCount(model.index(0)), 0);
	QCOMPARE(model.roleNames().value(GameListModel::kIdRole), QByteArray("id"));
	QCOMPARE(model.roleNames().value(GameListModel::kNameRole),
			 QByteArray("name"));
	QCOMPARE(model.roleNames().value(GameListModel::kPlatformRole),
			 QByteArray("platform"));
	QCOMPARE(model.roleNames().value(GameListModel::kPathRole),
			 QByteArray("path"));
	QCOMPARE(model.data(model.index(0), GameListModel::kIdRole).toString(),
			 QString("game-1"));
	QCOMPARE(model.data(model.index(0), GameListModel::kNameRole).toString(),
			 QString("Zelda"));
	QCOMPARE(
		model.data(model.index(0), GameListModel::kPlatformRole).toString(),
		QString("Switch"));
	QCOMPARE(model.data(model.index(0), GameListModel::kPathRole).toString(),
			 QString("games/zelda.rom"));
	QVERIFY(!model.data(QModelIndex(), GameListModel::kNameRole).isValid());
	QVERIFY(!model.data(model.index(0), Qt::DisplayRole).isValid());
}

void GameListModelTest::EmitsResetAndCountSignals() {
	testing::MemoryGameRepository repository;
	testing::FixedIdGenerator ids({});
	GameService service(repository, ids);
	ErrorNotifier errors;
	GameListModel model(service, errors);
	QVERIFY(model.Reload());
	QSignalSpy reset_spy(&model, &QAbstractItemModel::modelReset);
	QSignalSpy count_spy(&model, &GameListModel::countChanged);
	repository.values.push_back(
		{GameId("game-1"), "Game", "game.rom", "Console"});

	QVERIFY(model.Reload());
	QCOMPARE(reset_spy.count(), 1);
	QCOMPARE(count_spy.count(), 1);
	QCOMPARE(model.count(), 1);
}

void GameListModelTest::PreservesRowsAndPresentsLoadErrors() {
	testing::MemoryGameRepository repository;
	repository.values.push_back(
		{GameId("game-1"), "Game", "game.rom", "Console"});
	testing::FixedIdGenerator ids({});
	GameService service(repository, ids);
	ErrorNotifier errors;
	GameListModel model(service, errors);
	QVERIFY(model.Reload());
	QSignalSpy error_spy(&errors, &ErrorNotifier::errorOccurred);
	QSignalSpy changed_spy(&errors, &ErrorNotifier::errorChanged);
	repository.find_all_error = true;

	QVERIFY(!model.Reload());
	QCOMPARE(model.count(), 1);
	QCOMPARE(error_spy.count(), 1);
	QCOMPARE(changed_spy.count(), 1);
	QVERIFY(errors.active());
	QCOMPARE(errors.code(), static_cast<int>(ErrorCode::kPersistence));
	QCOMPARE(errors.message(), QString("find all failed"));

	errors.Clear();
	QVERIFY(!errors.active());
	QCOMPARE(errors.code(), -1);
	QVERIFY(errors.message().isEmpty());
	QCOMPARE(changed_spy.count(), 2);
}

}
}

QTEST_GUILESS_MAIN(emu::GameListModelTest)

#include "game_list_model_test.moc"
