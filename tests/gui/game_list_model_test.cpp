#include "models/game_list_model.h"

#include "emu/persistence/sqlite_repositories.h"
#include <QSignalSpy>
#include <QTemporaryDir>
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
    QCOMPARE(model.data(model.index(0), GameListModel::kNameRole).toString(),
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
  QCOMPARE(model.data(model.index(0), GameListModel::kPlatformRole).toString(),
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

} // namespace
} // namespace emu

QTEST_GUILESS_MAIN(emu::GameListModelTest)

#include "game_list_model_test.moc"
