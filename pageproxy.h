#ifndef PAGEPROXY_H
#define PAGEPROXY_H

#include <QObject>
#include <QWebEnginePage>
#include <sstream>
#include <memory>
#include <future>
#include <functional>
#include <maineventloop.h>
#include <iostream>
#include <fstream>
#include "common.h"

class PageProxy
{
public:
    std::shared_ptr<QWebEnginePage> _page;

    explicit PageProxy(std::shared_ptr<QWebEnginePage> page,
                       std::shared_ptr<MainEventLoop>  loop):
        _page(page),
        _loop(loop)
    {

    }

    std::future<std::string> run_script(std::string const& code)
    {
        std::shared_ptr<std::promise<std::string>> pms(new std::promise<std::string>());
        _loop->post_task([pms, code, this]() mutable{
            _page->runJavaScript(QString::fromStdString(code), [pms](QVariant const& v) mutable{
                pms->set_value(v.toString().toStdString());
            });
        });

        return pms->get_future();
    }

    std::future<std::string> run_script(std::ifstream const& ifs)
    {
        std::stringstream ss;
        ss << ifs.rdbuf();
        return run_script(ss.str());
    }

    std::future<std::string> get_html()
    {
        std::shared_ptr<std::promise<std::string>> pms(new std::promise<std::string>());
        _loop->post_task([pms, this]() mutable{
            _page->toHtml([pms, this](QString const& html) mutable{
                pms->set_value(html.toStdString());
            });
        });

        return pms->get_future();
    }

    virtual ~PageProxy(){        
    }

signals:   

public slots:

private:
    std::shared_ptr<MainEventLoop> _loop;
};

#endif // PAGEPROXY_H
