#include <QCoreApplication>
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QString>
#include <QTimer>
#include <QVariant>
#include <exception>

#include "gui_app_context.h"

int main(int argc, char* argv[]) {
	QGuiApplication application(argc, argv);
	try {
		emu::GuiAppContext context("config/config.json");
		QQmlApplicationEngine engine;
		QQmlEngine::setObjectOwnership(&context.game_list_model(),
									   QQmlEngine::CppOwnership);
		QQmlEngine::setObjectOwnership(&context.errors(),
									   QQmlEngine::CppOwnership);
		QQmlEngine::setObjectOwnership(&context.emulator_list_model(),
									   QQmlEngine::CppOwnership);
		QQmlEngine::setObjectOwnership(&context.profile_list_model(),
									   QQmlEngine::CppOwnership);
		engine.setInitialProperties(
			{{QStringLiteral("profileListModel"),
			  QVariant::fromValue(&context.profile_list_model())},
			 {QStringLiteral("emulatorListModel"),
			  QVariant::fromValue(&context.emulator_list_model())},
			 {QStringLiteral("gameListModel"),
			  QVariant::fromValue(&context.game_list_model())},
			 {QStringLiteral("errorNotifier"),
			  QVariant::fromValue(&context.errors())}});
		engine.loadFromModule("EmuManager", "Main");
		if (engine.rootObjects().isEmpty()) return 1;
		if (QCoreApplication::arguments().contains(
				QStringLiteral("--smoke-test"))) {
			QTimer::singleShot(0, &application, &QCoreApplication::quit);
		}
		return application.exec();
	} catch (const std::exception& error) {
		qCritical().noquote() << error.what();
		return 1;
	}
}
