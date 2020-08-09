/**
 * DatabaseFactory.h
 * 
 * @brief - creates a database based on incoming json cfg
 */


#ifndef _H_DATABASE_FACTORY
#define _H_DATABASE_FACTORY

#include <memory>
#include <string>
#include "IDatabase.h"

namespace afm {
    namespace database
    {
        class DatabaseFactory;

        using DatabaseFactorySPtr = std::shared_ptr<DatabaseFactory>;

        class DatabaseFactory
        {
            public:
                static DatabaseFactorySPtr getInstance();

                IDatabaseSPtr createDatabase(const DatabaseOptions &options);
                IDatabaseSPtr createDatabase(const std::string &connection, const std::string &name, DatabaseType type);
        };
    } // namespace database
}
#endif