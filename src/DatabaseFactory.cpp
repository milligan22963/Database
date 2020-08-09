/**
 * DatabaseFactory.cpp
 */

#include <string>
#include "DatabaseFactory.h"
#include "maria/MariaDB.h"
#include "sqlite/SQLiteDB.h"
#include "pgsql/PgSqlDB.h"

namespace afm {
    namespace database
    {
        using DatabaseMap = std::map<std::string, DatabaseType>;

        // database types
        static const std::string sc_database_name = "name";
        static const std::string sc_database_connection = "connection";
        static const std::string sc_database_type = "type";

        static const std::string sc_sqlite_db = "sqlite";
        static const std::string sc_mysql_db = "mysql";
        static const std::string sc_postgres_db = "pgsql";

        // database mappings
        static const DatabaseMap DatabaseMapping {
            {sc_sqlite_db, DatabaseType::SQLITE_DB},
            {sc_mysql_db, DatabaseType::MYSQL_DB},
            {sc_postgres_db, DatabaseType::PGSQL}
        };

        DatabaseFactorySPtr DatabaseFactory::getInstance()
        {
            static DatabaseFactorySPtr pFactory = nullptr;

            if (pFactory == nullptr) {
                pFactory = std::make_shared<DatabaseFactory>();

                
            }
            return pFactory;
        }

        IDatabaseSPtr DatabaseFactory::createDatabase(const DatabaseOptions &options)
        {
            IDatabaseSPtr pDatabase = nullptr;

            // What type and name?
            if ((options.find(sc_database_type) != options.end()) && (options.find(sc_database_name) != options.end())) {
                std::string type = options[sc_database_type];
                std::string connection;
                DatabaseType database_type = DatabaseType::END_DATABASE_TYPES;

                if (options.find(sc_database_connection) != options.end()) {
                    connection = options[sc_database_connection];
                }

                auto database_iter = DatabaseMapping.find(type);
                if (database_iter != DatabaseMapping.end()) {
                    database_type = database_iter->second;
                }

                pDatabase = createDatabase(connection, options[sc_database_name], database_type);
            }

            return pDatabase;
        }

        IDatabaseSPtr DatabaseFactory::createDatabase(const std::string &connection, const std::string &name, DatabaseType type)
        {
            IDatabaseSPtr pDatabase = nullptr;

            switch (type) {
                case DatabaseType::SQLITE_DB:
                {
                    pDatabase = std::make_shared<SQLiteDatabase>();
                }
                break;
                case DatabaseType::MYSQL_DB:
                {
                    pDatabase = std::make_shared<MariaDatabase>();
                }
                break;
                case DatabaseType::PGSQL:
                {
                    pDatabase = std::make_shared<PgSqlDatabase>();
                }
                break;
                default:
                {

                }
                break;
            }

            if (pDatabase != nullptr) {
                // Initialize it and if it fails then get rid of it
                if (pDatabase->initialize(connection, name) == false) {
                    pDatabase = nullptr;
                }
            }
            return pDatabase;
        }
    } // namespace database
}