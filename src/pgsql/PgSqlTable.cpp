/**
 * PgSqlTable.cpp
 */

#include <iostream>
#include <sstream>

#include "Row.h"

#include "pgsql/PgSqlColumn.h"
#include "pgsql/PgSqlTable.h"
#include "pgsql/PgSqlUtility.h"

namespace afm {
    namespace database {
        static const std::string sc_table_describe = "select column_name, column_default, is_nullable, data_type, character_maximum_length, numeric_precision_radix, udt_name, is_identity  from information_schema.columns where table_name='%s'";

        PgSqlTable::PgSqlTable(pqxx::connection *pConnection)
            : Table()
            , m_pConnection(pConnection)
        {

        }

        PgSqlTable::~PgSqlTable()
        {
            m_pConnection = nullptr;
        }

        bool PgSqlTable::initialize(const std::string &table_name)
        {
            bool success = Table::initialize(table_name);

            if (success == true) {
                std::string query = sc_table_describe;
                pqxx::result results;
                std::stringstream column_details;

                query.replace(query.find("%s"), 2, table_name);

                if (issueCommand(m_pConnection, query, results) == true) {
                    for (auto column : results) {
                        for (uint8_t index = 0; index < sc_max_pgsql_columns; index++) {
                            column_details << column[index].c_str();
                            if (index < (sc_max_pgsql_columns - 1)) {
                                column_details << ",";
                            }
                        }
                        IColumnSPtr pColumn = createEmptyColumn();

                        if (pColumn->initialize(column_details.str()) == true) {
                            add_column(pColumn);
                        }
                        column_details.str(""); // clear it
                    }
                } else {
                    success = false;
                }
            }

            return success;
        }

        IColumnSPtr PgSqlTable::createEmptyColumn() const
        {
            return std::make_shared<PgSqlColumn>();;
        }

        bool PgSqlTable::on_create_row(const std::string &query)
        {
            bool success = false;
            pqxx::result results;

            success = issueCommand(m_pConnection, query, results);

            return success;
        }

        bool PgSqlTable::on_update_row(const std::string &query)
        {
            bool success = false;
            pqxx::result results;

            success = issueCommand(m_pConnection, query, results);

            return success;
        }

        bool PgSqlTable::on_get_row(IRowSPtr &pRow, const std::string &query)
        {
            bool success = false;

            pqxx::result results;
            if (issueCommand(m_pConnection, query, results) == true) {
                RowData row_data;
                if (results.size() > 0) {
                    pqxx::row row = results[0];
                    for (uint8_t index = 0; index < results.columns(); index++) {
                        row_data.push_back(row[index].c_str());
                    }
                }

                pRow = createEmptyRow();
                pRow->setValues(row_data);
                success = true;
            }

            return success;
        }

        bool PgSqlTable::on_get_rows(Rows &rows, const std::string &query)
        {
             bool success = false;
            pqxx::result results;

            if (issueCommand(m_pConnection, query, results) == true) {
                RowData row_data;
                for (auto row : results) {
                    row_data.clear();

                    for (uint8_t index = 0; index < results.columns(); index++) {
                        row_data.push_back(row[index].c_str());
                    }
                    IRowSPtr pRow = createEmptyRow();
                    pRow->setValues(row_data);
                    rows.push_back(pRow);
                }
                success = true;
            }

            return success;           
        }

        IColumnSPtr PgSqlTable::on_clone_column(const Column &column) const
        {
            return std::make_shared<PgSqlColumn>(column);
        }
    }
}