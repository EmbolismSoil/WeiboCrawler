#ifndef RELATIONSTROAGE_H
#define RELATIONSTROAGE_H

#include "mysqlclientfactory.h"
#include <string>
#include "userrelation.h"

class RelationStroage
{
public:
    RelationStroage(QSqlDatabase const& db);
    void save(UserRelation const& relation);

private:
    QSqlDatabase _db;
};

#endif // RELATIONSTROAGE_H
