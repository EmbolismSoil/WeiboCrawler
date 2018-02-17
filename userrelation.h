#ifndef USERRELATION_H
#define USERRELATION_H

#include <string>
#include <vector>
struct UserRelation{
  std::string uid;
  std::vector<std::string> fans_uid;
  std::vector<std::string> followers_uid;
};

#endif // USERRELATION_H
