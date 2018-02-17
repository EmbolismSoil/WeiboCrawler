#include "mysqlclientfactory.h"
#include <fstream>

#if 1
MysqlClientFactory::MysqlClientFactory(const std::string &config_path)
{
    Json::Reader reader;
    std::ifstream config_file(config_path);
    if (!reader.parse(config_file, _config) || !_config.isMember("stroage")){
        throw std::invalid_argument("failed in parse config file");
    }

    _config = _config["stroage"];
}

QSqlDatabase MysqlClientFactory::get_mysql_client(const std::string &client_name)
{
    if (!_config.isMember(client_name)){
        throw std::invalid_argument(std::string("has no client named ") + client_name);
    }

    Json::Value client_cfg = _config[client_name];
    if (!(client_cfg.isMember("user_name") &&
          client_cfg.isMember("host") &&
          client_cfg.isMember("password") && client_cfg.isMember("db")))
    {
        throw std::invalid_argument("config paramters error");
    }

    int port = 3306;
    if (client_cfg.isMember("port")){
        port = client_cfg["port"].asInt();
    }
    std::string host = client_cfg["host"].asString();
    std::string db_name = client_cfg["db"].asString();
    std::string user_name = client_cfg["user_name"].asString();
    std::string password = client_cfg["password"].asString();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(QString::fromStdString(host));
    db.setPort(port);
    db.setUserName(QString::fromStdString(user_name));
    db.setPassword(QString::fromStdString(password));
    db.setDatabaseName(QString::fromStdString(db_name));

    return db;
}

#endif
