#ifndef SCRAWLERCONTROLLER_H
#define SCRAWLERCONTROLLER_H
#include "userinfo.h"
#include "userinfoextractor.h"
#include "userinfostroage.h"
#include "htmlloader.h"
#include "maineventloop.h"
#include "relationstroage.h"
#include "userrelationextractor.h"
#include <atomic>
#include <deque>

class ScrawlerController
{
public:
    ScrawlerController(std::string const& config_path, QSqlDatabase const& db, int argc, char* argv[]);
    void run(std::string const& root_uid);
    virtual ~ScrawlerController();

private:
    UserInfoExtractor _user_info_extractor;
    UserRelationExtractor _relation_extractor;
    UserInfoStroage _user_info_stroage;
    RelationStroage _relation_stroage;
    std::shared_ptr<HtmlLoader> _loader;
    std::shared_ptr<MainEventLoop> _main_loop;

    std::atomic<bool> _exit;

    std::deque<std::string> _uids;
};

#endif // SCRAWLERCONTROLLER_H
