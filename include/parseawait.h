#pragma once

#include <QFuture>
#include <QEventLoop>
#include <QTimer>

namespace cg
{
    template <typename T>
    inline void await(QFuture<T> future, int timeout = -1) {
        if (future.isFinished()) {
            return;
        }

        QEventLoop loop;
        QFutureWatcher<T> watcher;
        QObject::connect(&watcher, SIGNAL(finished()), &loop, SLOT(quit()));

        watcher.setFuture(future);

        if (timeout > 0) {
            QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
        }

        loop.exec();
    }
}