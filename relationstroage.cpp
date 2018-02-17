#include "relationstroage.h"
#include <boost/format.hpp>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <boost/algorithm/string.hpp>
#include <iostream>

RelationStroage::RelationStroage(QSqlDatabase const& db):
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

void RelationStroage::save(const UserRelation &relation)
{
    boost::format fans_sql_fmt("INSERT IGNORE INTO fans_relation(uid, fans_uid) VALUES %s");
    boost::format fans_value_fmt("('%s', '%s')");
    std::vector<std::string> fans_values;

    for (auto const& fans_uid : relation.fans_uid){
        if (fans_uid.empty()){
            continue;
        }

        fans_value_fmt % relation.uid % fans_uid;
        fans_values.push_back(fans_value_fmt.str());
    }

    auto values = boost::join(fans_values, ", ");
    fans_sql_fmt % values;


    boost::format follow_sql_fmt("INSERT IGNORE INTO follow_relation(uid, follow_uid) VALUES %s");
    boost::format follow_value_fmt("('%s', '%s')");
    std::vector<std::string> follow_values;

    for (auto const& follow_uid : relation.followers_uid){
        if (follow_uid.empty()){
            continue;
        }

        follow_value_fmt % relation.uid % follow_uid;
        follow_values.push_back(follow_value_fmt.str());
    }

    auto f_values = boost::join(follow_values, ", ");
    follow_sql_fmt %  f_values;

    _db.exec(QString::fromStdString(fans_sql_fmt.str()));
    _db.exec(QString::fromStdString(follow_sql_fmt.str()));

    //_db.commit();
}
