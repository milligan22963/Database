/**
 * MariaTable.h
 * 
 * @brief - Maria Table representation
 */

#ifndef _H_MARIA_TABLE
#define _H_MARIA_TABLE

#include <mariadb/mysql.h>

#include "Table.h"
#include "Column.h"

namespace afm {
    namespace database {

        class MariaTable : public Table
        {
            public:
                MariaTable(MYSQL *p_db);
                virtual ~MariaTable();

                virtual bool initialize(const std::string &table_name) override;
                virtual bool get(IRowSPtr &pRow, const QueryOptions &options = sm_emptyOptions) override;
                virtual bool get(Rows &rows, const QueryOptions &options = sm_emptyOptions) override;

                virtual IColumnSPtr createEmptyColumn() const override;

            protected:
                virtual bool on_create_row(const std::string &query) override;
                virtual bool on_update_row(const std::string &query) override;
                virtual bool on_get_row(IRowSPtr &pRow, const std::string &query) override;
                virtual bool on_get_rows(Rows &rows, const std::string &query) override;
                virtual IColumnSPtr on_clone_column(const Column &column) const override;

            private:
                MYSQL     *m_p_db;
        };
    }
}
#endif