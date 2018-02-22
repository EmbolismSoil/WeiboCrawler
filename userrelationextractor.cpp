#include "userrelationextractor.h"
#include <boost/format.hpp>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

UserRelationExtractor::UserRelationExtractor(
         std::string const& config_path)
{
    Json::Reader reader;
    std::ifstream config_json(config_path);

    if (!reader.parse(config_json, _config)){
        throw std::logic_error("failed in UserRelationExtractor: parse config error");
    }

    if (!_config.isMember("UserRelation")){
        throw std::logic_error("failed in UserRelationExtractor: parse config error");
    }

    _config = _config["UserRelation"];

    if (!(_config.isMember("jquery")
          && _config.isMember("page_num_selector")
          && _config.isMember("follow_list_selector")
          && _config.isMember("fans_list_selector")
          ))
    {
        throw std::logic_error("failed in UserRelationExtractor: parse config error");
    }

    auto jq_path = _config["jquery"].asString();
    std::ifstream jq_file(jq_path);
    std::stringstream buffer;

    buffer << jq_file.rdbuf();
    _jq_code = buffer.str();

    _page_num_selector = _config["page_num_selector"].asString();
    _follow_list_selector = _config["follow_list_selector"].asString();
    _fans_list_selector = _config["fans_list_selector"].asString();
}

void UserRelationExtractor::extract(UserRelation &relation)
{
    _extract_follow(relation);
    _extract_fans(relation);
}

void UserRelationExtractor::set_page_id(const std::string &id)
{
    _page_id = id;
}

void UserRelationExtractor::set_loader(const std::shared_ptr<HtmlLoader> &loader)
{
    _loader = loader;
}

void UserRelationExtractor::_extract_follow(UserRelation &relation)
{
    boost::format fmt("http://www.weibo.com/p/%s/follow?page=%d");
    _do_extract_uids(relation.followers_uid, fmt);
}

void UserRelationExtractor::_do_extract_uids(std::vector<std::string> &relation, boost::format url_format)
{
    boost::format follow_url_fmt = url_format;
    follow_url_fmt % _page_id % 1;
    auto page_1_url = follow_url_fmt.str();
    auto page_1 = _loader->load(page_1_url).get();

    _page_import_jquery(page_1);
    _extract_a_page_follow(relation, page_1);

    boost::format jq_page_num_fmt("$('%s').prev().text()");
    jq_page_num_fmt % _page_num_selector;
    auto page_num = page_1.run_script(jq_page_num_fmt.str()).get();
    int i_page_num = ::atoi(page_num.c_str());

    if (i_page_num <= 1){
        return;
    }

    if (i_page_num > 5){
        i_page_num = 5;
    }

    for (int cnt = 2; cnt <= i_page_num; ++cnt){
        follow_url_fmt % _page_id % cnt;
        auto page = _loader->load(follow_url_fmt.str()).get();
        _extract_a_page_follow(relation, page);
    }
}


void UserRelationExtractor::_extract_a_page_follow(std::vector<std::string> &relation, PageProxy & page)
{
    boost::format jq_get_all_uid(
                R"((function() {
                    var uids = [];
                    $('%s').each(function(){
                        var uid_kv = $(this).attr('action-data').split('&');
                        var uid = uid_kv[0].split('=')[1];
                        uids.push(uid);
                    });
                    return uids.join('|');
                  })())"
                  );

    _page_import_jquery(page);
    jq_get_all_uid % _follow_list_selector;
    //std::cout << "exec js : " << jq_get_all_uid.str() << std::endl;
    //std::cout << "html : " << page.get_html().get() << std::endl;
    auto uids_str = page.run_script(jq_get_all_uid.str()).get();

    std::vector<std::string> uids;
    boost::split(uids, uids_str, boost::is_any_of("|"));
    std::copy(uids.begin(), uids.end(), std::back_inserter(relation));
}

void UserRelationExtractor::_extract_a_page_fans(UserRelation &relation, PageProxy &page)
{
    //_extract_a_page_follow(relation, page);
}

void UserRelationExtractor::_extract_fans(UserRelation &relation)
{
    boost::format fans_url_fmt("http://www.weibo.com/p/%s/follow?relate=fans&page=%d");
    _do_extract_uids(relation.fans_uid, fans_url_fmt);
}

void UserRelationExtractor::_page_import_jquery(PageProxy &page)
{
    std::ifstream ifs(_config["jquery"].asString());
    auto ununsed = page.run_script(ifs); // import jquery
    ununsed.get();
}
