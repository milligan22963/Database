/**
 * SQLiteDB.h
 * 
 * @brief - database base class regardless of implementation
 */

#ifndef _H_SQLITE_DATABASE
#define _H_SQLITE_DATABASE

#include <sqlite3.h>

#include "Database.h"
#include "sqlite/SQLiteTable.h"

namespace afm {
    namespace database {
        
        class SQLiteDatabase : public Database
        {
            public:
                SQLiteDatabase();
                virtual ~SQLiteDatabase();

                virtual bool initialize(const std::string &connection, const std::string &database_name) override;

                virtual ITableSPtr getTable(const std::string &name) override;

                virtual ITableSPtr createTable(const TableOptions &details) override;

                virtual bool test_database() override;

            protected:
                virtual void load_tables() override;
                virtual bool on_drop_table(const std::string &query) override;

            private:
                sqlite3     *m_p_db = nullptr;
        };
    }
}
#endif