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
#include "scrawlercontroller.h"
#include "mysqlclientfactory.h"

int main(int argc,  char* argv[])
{
    std::string config_path("/home/lee/WeiboCrawler/config.json");

    std::thread t([&config_path, argc, argv] () mutable{
        MysqlClientFactory factory(config_path);
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
        ScrawlerController controller(config_path, db, argc, argv);
        controller.run("2365538803");
#endif
    });

    t.join();

    return 0;
}
