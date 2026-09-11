#pragma once

#include <QObject>
#include <QString>

#include "emu/application/application_error.h"

namespace emu {

class ErrorNotifier final : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool active READ active NOTIFY errorChanged)
	Q_PROPERTY(int code READ code NOTIFY errorChanged)
	Q_PROPERTY(QString message READ message NOTIFY errorChanged)

public:
	explicit ErrorNotifier(QObject* parent = nullptr);

	bool active() const { return active_; }
	int code() const { return code_; }
	QString message() const { return message_; }

	void Present(const ApplicationError& error);
	Q_INVOKABLE void Clear();

signals:
	void errorChanged();
	void errorOccurred(int code, const QString& message);

private:
	bool active_ = false;
	int code_ = -1;
	QString message_;
};

}
