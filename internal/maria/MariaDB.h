/**
 * MariaDB.h
 * 
 * @brief - database base class regardless of implementation
 */

#ifndef _H_MARIA_DATABASE
#define _H_MARIA_DATABASE

#include <mariadb/mysql.h>

#include "Database.h"

namespace afm {
    namespace database {
        
        class MariaDatabase : public Database
        {
            public:
                MariaDatabase();
                virtual ~MariaDatabase();

                virtual bool initialize(const std::string &connection, const std::string &database_name) override;

                virtual ITableSPtr getTable(const std::string &name) override;

                virtual ITableSPtr createTable(const TableOptions &details) override;

                virtual bool test_database() override;

            protected:
                virtual void load_tables() override;
                virtual uint32_t get_default_port() const override { return m_port; }
                bool select_database(const std::string &name);
                bool create_database(const std::string &name);
                virtual bool on_drop_table(const std::string &query) override;

            private:
                MYSQL     *m_p_db = nullptr;
                uint32_t  m_port = 3306;
        };
    }
}
#endif