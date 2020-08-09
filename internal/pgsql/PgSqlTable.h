/**
 * PgSqlTable.h
 * 
 * @brief - Postgres Table representation
 */

#ifndef _H_PGSQL_TABLE
#define _H_PGSQL_TABLE

#include <pqxx/pqxx>

#include "Table.h"
#include "Column.h"

namespace afm {
    namespace database {

        class PgSqlTable : public Table
        {
            public:
                PgSqlTable(pqxx::connection *pConnection);
                virtual ~PgSqlTable();

                virtual bool initialize(const std::string &table_name) override;

                virtual IColumnSPtr createEmptyColumn() const override;

            protected:
                virtual bool on_create_row(const std::string &query) override;
                virtual bool on_update_row(const std::string &query) override;
                virtual bool on_get_row(IRowSPtr &pRow, const std::string &query) override;
                virtual bool on_get_rows(Rows &rows, const std::string &query) override;
                virtual IColumnSPtr on_clone_column(const Column &column) const override;

            private:
                pqxx::connection    *m_pConnection = nullptr;
        };
    }
}
#endif