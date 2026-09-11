#include "error_notifier.h"

namespace emu {

ErrorNotifier::ErrorNotifier(QObject* parent) : QObject(parent) {}

void ErrorNotifier::Present(const ApplicationError& error) {
	const int code = static_cast<int>(error.code);
	const QString message = QString::fromUtf8(
		error.message.data(), static_cast<qsizetype>(error.message.size()));
	const bool changed = !active_ || code_ != code || message_ != message;
	active_ = true;
	code_ = code;
	message_ = message;
	if (changed) emit errorChanged();
	emit errorOccurred(code_, message_);
}

void ErrorNotifier::Clear() {
	if (!active_) return;
	active_ = false;
	code_ = -1;
	message_.clear();
	emit errorChanged();
}

}
