#include "maineventloop.h"

MainEventLoop::MainEventLoop(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(post_task_sginal(Task *)),
                    this, SLOT(on_post_task(Task *)));
    connect(&_gc_timer, SIGNAL(timeout()), this, SLOT(on_gc()));
    _gc_timer.start(500);
}
