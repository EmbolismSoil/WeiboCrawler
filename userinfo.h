#ifndef USERINFO_H
#define USERINFO_H
#include <stdint.h>
#include <vector>
#include <string>

struct UserInfo{
    std::string uid;
    std::string page_id;
    std::string name;
    std::string fans_url;
    std::string followers_url;
    int fans_cnt;
    int followers_cnt;
    int weibo_cnt;
};
#endif // USERINFO_H
