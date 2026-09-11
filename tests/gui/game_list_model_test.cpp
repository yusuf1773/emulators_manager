#include "models/game_list_model.h"

#include <QSignalSpy>
#include <QtTest>

#include "emu/application/game_service.h"
#include "error_notifier.h"
#include "fakes.h"

namespace emu {
namespace {

class GameListModelTest : public QObject {
	Q_OBJECT

private slots:
	void ExposesGameRoles();
	void EmitsResetAndCountSignals();
	void PreservesRowsAndPresentsLoadErrors();
};

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
