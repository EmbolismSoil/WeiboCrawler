#ifndef SCRAWLERCONTROLLER_H
#define SCRAWLERCONTROLLER_H
#include "userinfo.h"
#include "userinfoextractor.h"
#include "userinfostroage.h"
#include "htmlloader.h"
#include "maineventloop.h"
#include <atomic>

class ScrawlerController
{
public:
    ScrawlerController(std::string const& config_path, QSqlDatabase const& db);
    void run(std::string const& root_url);

private:
    UserInfoExtractor _extractor;
    UserInfoStroage _stroage;
    std::shared_ptr<HtmlLoader> _loader;
    std::shared_ptr<MainEventLoop> _main_loop;

    std::atomic<bool> _exit;
};

#endif // SCRAWLERCONTROLLER_H
