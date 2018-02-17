#ifndef HTMLLOADER_H
#define HTMLLOADER_H

#include <QObject>
#include <QThread>
#include <mutex>
#include <deque>
#include <future>
#include <functional>
#include <type_traits>
#include <memory>
#include <QtWebEngineWidgets/QWebEngineView>
#include <boost/scoped_ptr.hpp>
#include <QMultiHash>
#include <QQueue>
#include <QSignalMapper>
#include <iostream>
#include <boost/noncopyable.hpp>
#include <atomic>
#include <QApplication>
#include <QWebEngineScriptCollection>
#include <functional>
#include "pageproxy.h"
#include "maineventloop.h"
#include "loginwindow.h"

class PageId: public QObject
{
public:
    PageId(std::shared_ptr<QWebEnginePage> page,
           std::shared_ptr<std::promise<PageProxy>> pms):
        _page(page),
        _pms(pms)
    {

    }

    std::shared_ptr<QWebEnginePage> get_page(){ return _page;}
    std::shared_ptr<std::promise<PageProxy>> get_promise(){return _pms;}

private:
    std::shared_ptr<QWebEnginePage> _page;
    std::shared_ptr<std::promise<PageProxy>> _pms;
};

class HtmlLoader: public QObject, private boost::noncopyable
{
    Q_OBJECT

public:
    using page_ptr = std::shared_ptr<QWebEnginePage>;
    using promise_ptr = std::shared_ptr<std::promise<QString>>;

    static std::shared_ptr<HtmlLoader> instance(std::shared_ptr<MainEventLoop> loop)
    {
        std::shared_ptr<HtmlLoader> loader;
        std::atomic<bool> flag;
        flag.store(false);

        loop->post_task([loop, &loader, &flag](){
            std::shared_ptr<HtmlLoader> loader_guard(new HtmlLoader(loop));
            loader = loader_guard;
            flag.store(true);
        });

        while(!flag.load());
        return loader;
    }

    std::future<PageProxy> load(std::string const& url)
    {
        std::shared_ptr<std::promise<PageProxy>> pms(new std::promise<PageProxy>());

        _loop->post_task([this, pms, url]() mutable{
            //new an page then load the url
            auto page = _loop->get_page();//std::make_shared<QWebEnginePage>();

            connect(page.get(), SIGNAL(loadFinished(bool)), &_signal_mapper, SLOT(map()));
            _signal_mapper.setMapping(page.get(), (new PageId(page, pms)));
            connect(&_signal_mapper, SIGNAL(mapped(QObject*)), this, SLOT(on_load_finished(QObject*)));

            page->load(QUrl(QString::fromStdString(url)));

            //clean ref
        });

        return pms->get_future();
    }

    std::future<bool> login()
    {
        std::shared_ptr<std::promise<bool>> pms(new std::promise<bool>());
        _login_promise = pms;

        _loop->post_task([this, pms]() mutable{
            _login_window->load("https://www.weibo.com/login");
            _login_window->show();
        });

        return pms->get_future();
    }

public slots:
    void on_login_finished(void)
    {
        _login_window->setVisible(false);
        _login_promise->set_value(true);
    }

    void on_render_terminated(void)
    {
        std::cout << "render terminated" << std::endl;
    }

    void on_load_finished(QObject* obj)
    {        
        PageId *page_id = dynamic_cast<PageId*>(obj);
        if (!page_id){
            return;
        }

        auto page = page_id->get_page();
        auto pms = page_id->get_promise();

        delete page_id;

        pms->set_value(PageProxy(page, _loop));

        disconnect(page.get(), SIGNAL(loadFinished(bool)), &_signal_mapper, SLOT(map()));
        _signal_mapper.removeMappings(page.get());
        disconnect(&_signal_mapper, SIGNAL(mapped(QObject*)), this, SLOT(on_load_finished(QObject*)));
    }

private:
    std::shared_ptr<MainEventLoop> _loop;
    std::shared_ptr<LoginWindow> _login_window;
    std::shared_ptr<std::promise<bool>> _login_promise;
    QSignalMapper _signal_mapper;

    HtmlLoader(std::shared_ptr<MainEventLoop> loop):
        _loop(loop)
    {
        _login_window = std::make_shared<LoginWindow>(nullptr);
        connect(_login_window.get(), SIGNAL(login_finished()), this, SLOT(on_login_finished()));
    }
};

#endif // HTMLLOADER_H
