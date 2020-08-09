/**
 * PgSqlDB.h
 * 
 * @brief - database base class regardless of implementation
 */

#ifndef _H_PGSQL_DATABASE
#define _H_PGSQL_DATABASE

#include <pqxx/pqxx>

#include "Database.h"

namespace afm {
    namespace database {
        
        class PgSqlDatabase : public Database
        {
            public:
                PgSqlDatabase();
                virtual ~PgSqlDatabase();

                virtual bool initialize(const std::string &connection, const std::string &database_name) override;

                virtual ITableSPtr getTable(const std::string &name) override;

                virtual ITableSPtr createTable(const TableOptions &details) override;

                virtual bool test_database() override;

            protected:
                virtual void load_tables() override;
                virtual uint32_t get_default_port() const override { return m_port; }
                bool select_database(const std::string &name);
                bool create_database(const std::string &name);
                virtual std::string get_column_creation(IColumnSPtr &pColumn) const override;
                virtual bool on_drop_table(const std::string &query) override;

            private:
                pqxx::connection    *m_pConnection = nullptr;
                uint32_t            m_port = 5432;
        };
    }
}
#endif