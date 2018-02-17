#ifndef USERINFOSTROAGE_H
#define USERINFOSTROAGE_H

#include <boost/noncopyable.hpp>
#include <QSqlDatabase>
#include <mysqlclientfactory.h>
#include <userinfo.h>
#include <sstream>
#include <boost/format.hpp>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

class UserInfoStroage
{
public:
    UserInfoStroage(QSqlDatabase const& db):
        _db(db)
    {
        if (!_db.open()){
            auto last_error = _db.lastError();
            throw std::logic_error("db error : "
                                   + last_error.databaseText().toStdString()
                                   + ", driver error : "
                                   + last_error.driverText().toStdString());
        }
    }

    void save(UserInfo const& user_info)
    {
        if (!_db.transaction()){
            return;
        }

        save_user_info(user_info);
        _db.commit(); // TODO: rollback
    }

    bool has_uid(std::string uid)
    {
        boost::format sql_fmt("SELECT count(1) FROM user_info WHERE uid = %s");
        sql_fmt % to_mysql_value(uid);
        auto sql = sql_fmt.str();
        auto result = _db.exec(QString::fromStdString(sql));
        auto cnt = result.value(0).toInt();
        if (cnt > 0){
            return true;
        }else{
            return false;
        }
    }

    template<class T>
    static std::string to_str(T const& value);

    template<class T>
    static std::string to_mysql_value(T const& value)
    {
        return to_str(value);
    }

    std::string to_mysql_value(std::string const& value)
    {
        boost::format fmt(R"('%s')");
        fmt % value;
        return fmt.str();
        //return to_str(value);
    }

private:
    QSqlDatabase _db;
    void save_user_info(UserInfo const& user_info)
    {
        boost::format sql_fmt(R"(INSERT INTO user_info
                              (uid, page_id, name, fans_url, followers_url, fans_cnt, followers_cnt, weibo_cnt)
                              VALUES (%s, %s, %s, %s, %s, %s, %s, %s))");

        sql_fmt % to_mysql_value(user_info.uid)
                % to_mysql_value(user_info.page_id)
                % to_mysql_value(user_info.name)
                % to_mysql_value(user_info.fans_url)
                % to_mysql_value(user_info.followers_url)
                % to_mysql_value(user_info.fans_cnt)
                % to_mysql_value(user_info.followers_cnt)
                % to_mysql_value(user_info.weibo_cnt);

        std::string sql = sql_fmt.str();
        _db.exec(QString::fromStdString(sql));
    }
};

template<class T>
std::string UserInfoStroage::to_str(T const& value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

#endif // USERINFOSTROAGE_H
