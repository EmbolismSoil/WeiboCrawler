#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <boost/scoped_ptr.hpp>
#include <QAction>
#include <QString>

class LoginWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    void load(QString const& url);

signals:
    void login_finished(void);

public slots:
    void when_login_finished(void)
    {
        emit login_finished();
    }

private:
    QWebEngineView*  _web_view;
    QAction*    _login_finished_act;
};

#endif // LOGINWINDOW_H
