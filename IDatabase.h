/**
 * IDatabase.h
 * 
 * @brief - database interface regardless of implementation
 */

#ifndef _H_IDATABASE
#define _H_IDATABASE

#include <memory>
#include <set>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "ITable.h"

namespace afm {
    namespace database {

        using DatabaseOptions = nlohmann::json;

        enum DatabaseType {
            SQLITE_DB,
            MYSQL_DB,
            PGSQL,
            END_DATABASE_TYPES
        };

        class IDatabase
        {
            public:
                virtual ~IDatabase() {}

                virtual bool initialize(const std::string &connection, const std::string &database_name) = 0;

                virtual DatabaseType getType() const = 0;

                virtual Tables getTables() = 0;

                virtual ITableSPtr getTable(const std::string &name) = 0;

                virtual bool dropTable(const std::string &name) = 0;

                virtual ITableSPtr createTable(const TableOptions &details) = 0;

                virtual bool test_database() = 0;
        };

        using IDatabaseSPtr = std::shared_ptr<IDatabase>;
    }
}
#endif