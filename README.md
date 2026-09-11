# Emulator Manager

Менеджер игр, эмуляторов и профилей запуска на C++23. Основной интерфейс сейчас
предоставляет CLI. Qt GUI подключается отдельно, загружает игры через application
layer и готов к добавлению экранов редактирования каталога.

## Архитектура

```text
app/
  bootstrap/             сборка зависимостей приложения
  cli/                   CLI и преобразование ошибок для пользователя
  gui/                   Qt Quick, GuiAppContext, модели и показ ошибок
include/emu/
  domain/                Game, Emulator, LaunchProfile и типизированные ID
  application/           сервисы и интерфейсы внешних зависимостей
  persistence/           SQLite и перенос данных через JSON
  launch/                нативный запуск процессов и стратегии
src/
  application/           реализация сценариев использования
  persistence/           конфигурация, миграции и репозитории SQLite
  launch/                DirectLaunchStrategy и NativeProcessRunner
tests/
  unit/                  тесты core с in-memory зависимостями
  integration/           SQLite, JSON, процессы, конфигурация и CLI
cmake/                   версии и подключение зависимостей
```

Направление зависимостей:

```text
domain <- application <- launch
                      <- persistence
                      <- app
```

`domain` содержит только модели и стандартную библиотеку. `application` содержит
правила создания и изменения сущностей, типизированные ошибки, интерфейсы
репозиториев и сценарий запуска. SQLite, JSON, CLI и системные API находятся во
внешних слоях. `GameListModel` преобразует данные `GameService` в роли Qt, не
добавляя Qt-зависимости в domain или application.

`GuiAppContext` является composition root графического приложения. Он создаёт
SQLite-репозитории и application-сервисы, выполняет первоначальный импорт и
передаёт `GameListModel` в QML. `ErrorNotifier` принимает типизированные ошибки
application layer и предоставляет единый канал для их отображения в интерфейсе.

У каждой игры, эмулятора и профиля есть стабильный ID. `LaunchProfile` связывает
игру с эмулятором и хранит стратегию, дополнительные аргументы, рабочую папку и
признак профиля по умолчанию. `LaunchGameService` разрешает все ссылки и передаёт
готовый `LaunchRequest` в `ProcessRunner`.

## Хранение данных

Основное хранилище — SQLite. При первом открытии создаётся схема версии 1 с
foreign keys, уникальными именами и ограничением на один профиль по умолчанию для
игры. База отклоняется, если её версия новее поддерживаемой.

Старые `games.json` и `emulators.json` автоматически импортируются при пустой
базе. Для игр и эмуляторов с одинаковой платформой создаются профили запуска.
После успешной команды актуальный каталог экспортируется в `catalog.json`.

Пути задаются в `config/config.json` и разрешаются относительно этого файла:

```json
{
    "database_path": "emulators.db",
    "games_path": "games.json",
    "emulators_path": "emulators.json",
    "export_path": "catalog.json",
    "history_path": "history.json",
    "history_max_size": 15
}
```

`games_path` и `emulators_path` нужны для однократного импорта старого формата.
`history_path` читается для совместимости со старой конфигурацией; новая история
команд хранится в SQLite.

## Сборка

Нужны CMake 3.25+, Ninja и компилятор с поддержкой используемых возможностей
C++23. Обычная сборка:

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Сборка только независимого core без CLI и persistence:

```sh
cmake --preset core
cmake --build --preset core
ctest --preset core
```

Минимальное Qt Quick приложение собирается отдельным preset:

```sh
cmake --preset gui
cmake --build --preset gui
ctest --preset gui
./build/gui/emu_manager_gui
```

Для GUI требуется Qt 6.5+ с модулями Core, Gui, Qml, Quick и QuickControls2.
При сборке тестов также используется Qt Test.
При обычной сборке `EMU_BUILD_GUI=OFF`, поэтому Qt не ищется и не влияет на CLI
или библиотеки core. `gui_game_list_model` проверяет роли модели, сигналы
обновления и обработку ошибок через `QSignalSpy`. `gui_smoke` загружает
QML-модуль в offscreen-режиме и завершает приложение без открытия окна.

CMake сначала ищет установленные SQLite, GoogleTest 1.17.0, CLI11 2.5.0 и
nlohmann/json 3.12.0, затем загружает закреплённые версии. Первая полная сборка
может требовать доступ к интернету. GitHub Actions выполняет сборку и тесты на
Linux и Windows.

## CLI

```sh
./build/debug/emu_manager add-game --name Game --path game.rom --platform console
./build/debug/emu_manager add-emu --name Emulator --path emulator.exe --platform console --arg=--fullscreen
./build/debug/emu_manager list-game
./build/debug/emu_manager list-emu
./build/debug/emu_manager add-profile --name Default --game-id GAME_ID --emulator-id EMULATOR_ID --strategy-id direct --default
./build/debug/emu_manager list-profiles
./build/debug/emu_manager launch --profile-id PROFILE_ID
./build/debug/emu_manager list-history
```

ID для профиля берутся из `list-game` и `list-emu`; ID запуска — из
`list-profiles`. Повторяемый `--arg` задаёт базовые аргументы эмулятора или
аргументы конкретного профиля. `--working-directory` задаёт рабочую папку процесса.

`NativeProcessRunner` запускает исполняемый файл напрямую: через `CreateProcessW`
на Windows и `fork`/`execv` на POSIX. Командная строка не передаётся shell.

## Тесты и стиль

Тесты на GoogleTest проверяют сервисы, стабильные ID, валидацию и ошибки,
формирование аргументов, разрешение профиля, реальный нативный runner, SQLite,
foreign keys, каскадное удаление, миграции, историю, старый JSON-импорт, экспорт,
конфигурацию и полный CLI-сценарий. Заголовки core отдельно проверяются на
самодостаточность.

Стиль основан на Google C++ Style Guide. Исходники используют `.cpp`,
`#pragma once`, табы шириной четыре символа и вынесенные влево модификаторы
доступа. Комментарии в коде не используются.
