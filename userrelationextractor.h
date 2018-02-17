#ifndef USERRELATIONEXTRACTOR_H
#define USERRELATIONEXTRACTOR_H

#include <userrelation.h>
#include <htmlloader.h>
#include <string>
#include <pageproxy.h>
#include <json/json.h>
#include <boost/format.hpp>

class UserRelationExtractor
{
public:
    UserRelationExtractor(std::shared_ptr<HtmlLoader> const& loader, std::string const& page_id, std::string const& config_path);
    void extract(UserRelation &relation);
private:
    std::shared_ptr<HtmlLoader> _loader;
    std::string const _page_id;
    std::string _page_num_selector;
    std::string _follow_list_selector;
    std::string _fans_list_selector;
    std::string _jq_code;
    Json::Value _config;

    void _extract_follow(UserRelation &relation);
    void _do_extract_uids(std::vector<std::string> &relation, boost::format url_format);
    void _extract_a_page_follow(std::vector<std::string> &relation, PageProxy & page);
    void _extract_a_page_fans(UserRelation &relation, PageProxy &page);
    void _extract_fans(UserRelation& relation);
    void _page_import_jquery(PageProxy &page);
};

#endif // USERRELATIONEXTRACTOR_H
