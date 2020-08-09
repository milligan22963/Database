/**
 * SQLiteTable.h
 * 
 * @brief - SQLite Table representation
 */

#ifndef _H_SQLITE_TABLE
#define _H_SQLITE_TABLE

#include <sqlite3.h>

#include "Table.h"
#include "sqlite/SQLiteUtility.h"
#include "sqlite/SQLiteColumn.h"

namespace afm {
    namespace database {

        class SQLiteTable : public Table
        {
            public:
                SQLiteTable(sqlite3 *p_db);
                virtual ~SQLiteTable();

                virtual bool initialize(const std::string &table_name) override;

                virtual IColumnSPtr createEmptyColumn() const override;

            protected:
                virtual bool on_create_row(const std::string &query) override;
                virtual bool on_update_row(const std::string &query) override;
                virtual bool on_get_row(IRowSPtr &pRow, const std::string &query) override;
                virtual bool on_get_rows(Rows &rows, const std::string &query) override;
                virtual IColumnSPtr on_clone_column(const Column &column) const override;

            private:
                sqlite3     *m_p_db = nullptr;
        };
    }
}
#endif