#include <QtWebEngineWidgets/QWebEngineView>
#include "htmlloader.h"
#include <iostream>
#include <QXmlQuery>
#include <gq/Document.h>
#include <gq/Node.h>
#include "userinfoextractor.h"
#include "userinfostroage.h"
#include <fstream>
#include "maineventloop.h"
#include "userrelationextractor.h"
#include "relationstroage.h"
#include "mysqlclientfactory.h"

int main(int argc,  char* argv[])
{
    auto main_loop = MainEventLoop::instance(argc, argv);
    std::string config_path("/home/lee/WeiboCrawler/config.json");

    std::thread t([main_loop, &config_path] () mutable{
        MysqlClientFactory factory(config_path);
        auto loader = HtmlLoader::instance(main_loop);
        main_loop->keep(loader);
        auto db = factory.get_mysql_client("default");
#if 0
        UserInfoStroage stroage(db);
        for (int i = 0; i < 10; ++i){
            auto fut = loader->load("https://weibo.com/u/2017937831?from=myfollow_all");
            auto page = fut.get();
            ::sleep(10);
            UserInfoExtractor extractor(config_path);
            extractor.set_page(page);
            auto user_info = extractor.extract();
            //stroage.save(user_info);
            std::cout << "uid = " << user_info.uid
                      << ", page_id = " << user_info.page_id
                      << ", name = " << user_info.name
                      << ", fans_cnt = " << user_info.fans_cnt
                      << ", followers_cnt = " << user_info.followers_cnt
                      << ", fans_url = "  << user_info.fans_url
                      << ", followers_url = " << user_info.followers_url
                      << ", weibo_cnt = " << user_info.weibo_cnt << std::endl;
        }
#else
        loader->login().get();

        UserRelation relation;
        relation.uid = "2365538803";
        UserRelationExtractor relation_extractor(loader, "1005052365538803", config_path);
        relation_extractor.extract(relation);
        auto relation_stroage = RelationStroage(db);
        relation_stroage.save(relation);
#endif
    });

    t.join();
    main_loop->exit();
    main_loop.reset();

    return 0;
}
