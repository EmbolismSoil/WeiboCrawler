#include "scrawlercontroller.h"
#include <boost/format.hpp>
#include <iostream>


ScrawlerController::ScrawlerController(const std::string &config_path,
                                       QSqlDatabase const& db, int argc, char* argv[]):
    _user_info_extractor(config_path),
    _user_info_stroage(db),
    _relation_stroage(db),
    _relation_extractor(config_path)
{
    _exit.store(false);
    _main_loop = MainEventLoop::instance(argc, argv);
    _loader = HtmlLoader::instance(_main_loop);
    _main_loop->keep(_loader);
    _relation_extractor.set_loader(_loader);

    _loader->login().get();
}

void ScrawlerController::run(std::string const& root_uid)
{
    boost::format home_page_fmt("http://www.weibo.com/u/%s");
    _uids.push_back(root_uid);

    while(!_exit)
    {
        if (_uids.empty()){
            break;
        }

        std::string uid(std::move(_uids.front()));
        _uids.pop_front();
        std::cout << "extract user info, uid = " << uid << std::endl;

        home_page_fmt % uid;
        auto user_info_page = _loader->load(home_page_fmt.str()).get();
        ::sleep(2); //wait for rendering

        _user_info_extractor.set_page(user_info_page);
        auto user_info = _user_info_extractor.extract();
        if (user_info.uid.empty() || user_info.page_id.empty()
                || user_info.name.empty() || user_info.fans_cnt <= 0
                || user_info.followers_cnt <= 0 || user_info.weibo_cnt <= 0
                || user_info.followers_url.empty() || user_info.fans_url.empty())
        {
            std::cout << "extract error, uid is empty" << std::endl;
            continue;
        }

        std::cout << "uid = " << user_info.uid
                  << ", page_id = " << user_info.page_id
                  << ", name = " << user_info.name
                  << ", fans_cnt = " << user_info.fans_cnt
                  << ", followers_cnt = " << user_info.followers_cnt
                  << ", fans_url = "  << user_info.fans_url
                  << ", followers_url = " << user_info.followers_url
                  << ", weibo_cnt = " << user_info.weibo_cnt << std::endl;

        auto rate = (double(user_info.followers_cnt) / user_info.fans_cnt > 4.0);
        _user_info_stroage.save(user_info);
        if (user_info.fans_cnt / user_info.followers_cnt > 100 ||
                (rate && user_info.followers_cnt > 1000) || user_info.fans_cnt > 3000)
        {
            std::cout << "uid = " << user_info.uid << ", name = "
                      << user_info.name << " is not an private account" << std::endl;
            continue;
        }

        if (user_info.page_id.empty()){
            std::cout << "page id is empty" << std::endl;
            continue;
        }

        UserRelation relation;
        relation.uid = user_info.uid;
        _relation_extractor.set_page_id(user_info.page_id);
        _relation_extractor.extract(relation);

        _relation_stroage.save(relation);
        if (relation.fans_uid.empty() || relation.followers_uid.empty()){
            std::cout << "extract relation error, fans_uid or followers_uid is empty" << std::endl;
        }

        for (auto const& fans_uid : relation.fans_uid){
            if (fans_uid.empty() || _user_info_stroage.has_uid(fans_uid)){
                continue;
            }

            _uids.push_back(fans_uid);
        }

        for (auto const& follow_uid : relation.followers_uid){
            if (follow_uid.empty() || _user_info_stroage.has_uid(follow_uid)){
                continue;
            }

            _uids.push_back(follow_uid);
        }

        std::cout << "uid queue size = " << _uids.size() << std::endl;
    }
}

ScrawlerController::~ScrawlerController()
{
    auto loop = _main_loop.get();
    _main_loop.reset();
    loop->exit();
}
