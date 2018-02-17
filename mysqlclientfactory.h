#ifndef MYSQLCLIENTFACTORY_H
#define MYSQLCLIENTFACTORY_H

#include <boost/noncopyable.hpp>
#include <QSqlDatabase>
#include <memory>
#include <string>
#include <json/json.h>

class MysqlClientFactory
{
public:
    MysqlClientFactory(std::string const& config_path);
    QSqlDatabase get_mysql_client(std::string const& client_name);

private:
    Json::Value _config;
};

#endif // MYSQLCLIENTFACTORY_H
