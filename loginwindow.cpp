#include "loginwindow.h"
#include <QMenu>
#include <QMenuBar>

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    _web_view(new QWebEngineView()),
    _login_finished_act(new QAction("login finished"))
{
    this->setCentralWidget(_web_view);
    auto menu = this->menuBar()->addMenu("login");
    menu->addAction(_login_finished_act);

    connect(_login_finished_act, SIGNAL(triggered(bool)), this, SLOT(when_login_finished()));
}

void LoginWindow::load(const QString &url)
{
    _web_view->load(QUrl(url));
}
