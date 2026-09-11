#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QString>
#include <QTimer>
#include <cstdlib>

int main(int argc, char* argv[]) {
	QGuiApplication application(argc, argv);
	QQmlApplicationEngine engine;
	engine.loadFromModule("EmuManager", "Main");
	if (engine.rootObjects().isEmpty()) return EXIT_FAILURE;
	if (QCoreApplication::arguments().contains(
			QStringLiteral("--smoke-test"))) {
		QTimer::singleShot(0, &application, &QCoreApplication::quit);
	}
	return application.exec();
}
