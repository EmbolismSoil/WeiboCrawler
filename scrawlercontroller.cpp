#include "scrawlercontroller.h"

ScrawlerController::ScrawlerController(const std::string &config_path, QSqlDatabase const& db):
    _extractor(config_path),
    _stroage(db)
{
    _exit.store(false);
    auto argv = new char*[1];
    argv[0] = "ScrawlerController";
    _main_loop = MainEventLoop::instance(1, argv);
    _loader = HtmlLoader::instance(_main_loop);
}

void ScrawlerController::run(std::string const& root_url)
{
    while(!_exit)
    {

    }
}
