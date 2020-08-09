/**
 * SQLiteTable.cpp
 */

#include <iostream>
#include <string>
#include <vector>

#include "Row.h"
#include "sqlite/SQLiteColumn.h"
#include "sqlite/SQLiteTable.h"
#include "sqlite/SQLiteUtility.h"

namespace afm {
    namespace database {
        using ColumnNames = std::vector<std::string>;

        static const std::string sc_table_describe = "select sql from sqlite_master where name='%s'";
        static const std::string sc_foriegn_key_restraint = "FOREIGN KEY";
        static const std::string sc_constraint = "CONSTRAINT";

        struct RowCallbackData {
            RowCallbackData(SQLiteTable *pTable, Rows &rows)
                : m_pTable(pTable)
                , m_rows(rows)
            {

            }
            SQLiteTable *m_pTable = nullptr;
            Rows &m_rows;
        };

        int sqlite_table_rows_callback(void *p_row_data, int col_count, char **pp_data, char **pp_columns);
        int sqlite_table_columns_callback(void *p_column_details, int col_count, char **pp_data, char **pp_columns);
        int sqlite_table_add_update_callback(void *p_flag, int col_count, char **pp_data, char **pp_columns);

        SQLiteTable::SQLiteTable(sqlite3 *p_db)
            : Table()
            , m_p_db(p_db)
        {

        }

        SQLiteTable::~SQLiteTable()
        {
        }

        bool SQLiteTable::initialize(const std::string &table_name)
        {
            bool success = Table::initialize(table_name);

            if (success == true) {
                ColumnNames columns;

                // get column details
                std::string query = sc_table_describe;

                query.replace(query.find("%s"), 2, table_name);

                if (issueCommand(m_p_db, query, sqlite_table_columns_callback, &columns) == true) {
                    // we can now look at each column and it's details
                    for (auto column : columns) {
                        // skipping foriegn key constraints right now
                        if ((column.find(sc_foriegn_key_restraint) == std::string::npos) &&
                            (column.find(sc_constraint) == std::string::npos)) {
                            IColumnSPtr pColumn = createEmptyColumn();

                            if (pColumn->initialize(column) == true) {
                                add_column(pColumn);
                            }
                        } else {
                            // we should notify the database about foreign keys and constraints
                        }
                    }
                } else {
                    success = false;
                }
            }
            return success;
        }

        IColumnSPtr SQLiteTable::createEmptyColumn() const
        {
            return std::make_shared<SQLiteColumn>();
        }

        bool SQLiteTable::on_create_row(const std::string &query)
        {
            bool success = false;

            issueCommand(m_p_db, query, sqlite_table_add_update_callback, &success);

            return success;
        }

        bool SQLiteTable::on_update_row(const std::string &query)
        {
            bool success = false;

            if (issueCommand(m_p_db, query, sqlite_table_add_update_callback, &success) == false) {
                // log it
            }

            return success;
        }

        bool SQLiteTable::on_get_row(IRowSPtr &pRow, const std::string &query)
        {
            bool success = false;
            Rows rows;
            RowCallbackData rowData(this, rows);

            if (issueCommand(m_p_db, query, sqlite_table_rows_callback, &rowData) == true) {
                if (rows.size() > 0) {
                    // we should warn when more than one row is returned
                    pRow = rows[0];
                    success = true;
                }
            }

            return success;
        }

        bool SQLiteTable::on_get_rows(Rows &rows, const std::string &query)
        {
            bool success = false;

            RowCallbackData rowData(this, rows);

            if (issueCommand(m_p_db, query, sqlite_table_rows_callback, &rowData) == true) {
                if (rows.size() > 0) {
                    success = true;
                }
            }

            return success;
        }

        IColumnSPtr SQLiteTable::on_clone_column(const Column &column) const
        {
            return std::make_shared<SQLiteColumn>(column);
        }

        int sqlite_table_columns_callback(void *p_column_details, int col_count, char **pp_data, char **pp_columns)
        {
            ColumnNames *pColumns = (ColumnNames *)p_column_details;

            if (col_count > 0) {
                char *pStart = pp_data[0];
                char *pEOL = pStart;
 
                do {
                    pEOL = strchr(pStart, '\n');
                    if (pEOL != nullptr) {
                        *pEOL = '\0';
                    }
                    // Column declaration?
                    if (strchr(pStart, '[') != nullptr) {
                        pColumns->push_back(pStart);
                    }
                    pStart = pEOL + 1;
                } while (pEOL != nullptr);
            }

            return SQLITE_OK;
        }

        int sqlite_table_rows_callback(void *p_row_data, int col_count, char **pp_data, char **pp_columns)
        {
            RowCallbackData *pRowData = (RowCallbackData *)p_row_data;

            if (col_count > 0) {
                IRowSPtr pRow = pRowData->m_pTable->createEmptyRow();
                RowData rowData;
                for (int index = 0; index < col_count; index++) {
                    rowData.push_back(pp_data[index]);
                }

                if (pRow->setValues(rowData) == true) {
                    pRowData->m_rows.push_back(pRow);
                }
            }

            return SQLITE_OK;
        }

        int sqlite_table_add_update_callback(void *p_flag, int col_count, char **pp_data, char **pp_columns)
        {
            bool *pSuccess = (bool *)p_flag;

            *pSuccess = true;

            return SQLITE_OK;
        }
    }
}