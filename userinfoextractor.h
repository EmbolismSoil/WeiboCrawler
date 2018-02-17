#ifndef USERINFOEXTRACTOR_H
#define USERINFOEXTRACTOR_H
#include <stdint.h>
#include <gq/Document.h>
#include <gq/Node.h>
#include <userinfo.h>
#include <json/json.h>
#include <fstream>
#include <exception>
#include <boost/noncopyable.hpp>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "pageproxy.h"

class UserInfoExtractor
{
public:
    UserInfoExtractor(std::string const & config_path)
    {
        Json::Reader reader;
        std::ifstream cfg(config_path);

        if (!reader.parse(cfg, _config) || !_config.isMember("UserInfo"))
        {
            throw std::invalid_argument("failed in parser config");
        }

        _config = _config["UserInfo"];
        if (_config.isMember("uid_selector")
                && _config.isMember("name_selector")
                && _config.isMember("follower_url_selector")
                && _config.isMember("fans_url_selector")
                && _config.isMember("fans_cnt_selector")
                && _config.isMember("follower_cnt_selector")
                && _config.isMember("weibo_cnt_selector")
                && _config.isMember("jquery")
            )
        {
            _uid_selector = _config["uid_selector"].asString();
            _name_selector = _config["name_selector"].asString();
            _follower_url_selector = _config["follower_url_selector"].asString();
            _fans_url_selector = _config["fans_url_selector"].asString();
            _fans_cnt_selector = _config["fans_cnt_selector"].asString();
            _follower_cnt_selector = _config["follower_cnt_selector"].asString();
            _weibo_cnt_selector = _config["weibo_cnt_selector"].asString();
        }else{
            throw std::invalid_argument("selector config paramter error");
        }
    }

    virtual UserInfo extract()
    {
        UserInfo user_info;
        user_info.uid = _extract_uid();
        user_info.page_id = _extract_page_id();
        user_info.name = selector_to_text(_name_selector);

        user_info.followers_url = _extract_link(_follower_url_selector);
        user_info.fans_url = _extract_link(_fans_url_selector);


        user_info.fans_cnt = ::atoi(selector_to_text(_fans_cnt_selector).c_str());
        user_info.followers_cnt = ::atoi(selector_to_text(_follower_cnt_selector).c_str());
        user_info.weibo_cnt = ::atoi(selector_to_text(_weibo_cnt_selector).c_str());

        return user_info;
    }

    virtual void set_page(PageProxy const& page)
    {
        _page = std::make_shared<PageProxy>(page);
        std::ifstream ifs(_config["jquery"].asString());
        auto ununsed = _page->run_script(ifs); // import jquery
        ununsed.get();
    }

private:
    std::string selector_to_text(std::string const& selector)
    {
        //auto cselector = _doc.find(selector);
        //return cselector.nodeAt(0).text();
        boost::format jq("$('%s').text()");
        jq % selector;
        auto text = _page->run_script(jq.str());
        return text.get();
    }

    std::string selector_to_attr(std::string const& selector, std::string const& attr)
    {
        boost::format jq("$('%s').attr('%s')");
        jq % selector % attr;
        auto js = jq.str();
        return _page->run_script(js).get();
    }

    std::string const _extract_page_id()
    {
        auto page_id = _page->run_script("$CONFIG['page_id']").get();
        return page_id;
    }

    std::string const _extract_uid()
    {
#if 0
        std::string uid_attr = selector_to_attr(_uid_selector, "suda-data");
        std::vector<std::string> uid_kv;        

        boost::split(uid_kv, uid_attr, boost::is_any_of("&"));
        if (uid_kv.size() != 2){
            throw std::logic_error("failed in extract_uid");
        }

        std::string uid_value = uid_kv[1];
        uid_kv.clear();
        boost::split(uid_kv, uid_value, boost::is_any_of("="));
        if (uid_kv.size() != 2){
            throw std::logic_error("failed in extract_uid");
        }

        boost::trim(uid_kv[1]);
        return uid_kv[1];
#else
        auto uid = _page->run_script("$CONFIG['oid']").get();
        return uid;
#endif

    }   

    std::string const _extract_link(std::string const& a_tag_selector)
    {
        auto link = selector_to_attr(a_tag_selector, "href");
        return link;
    }

    std::shared_ptr<PageProxy> _page;
    std::string _uid_selector;
    std::string _fans_url_selector;
    std::string _follower_url_selector;
    std::string _fans_cnt_selector;
    std::string _follower_cnt_selector;
    std::string _name_selector;
    std::string _weibo_cnt_selector;

    Json::Value _config;
};

#endif // USERINFOEXTRACTOR_H
