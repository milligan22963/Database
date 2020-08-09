/**
 * MariaTable.cpp
 */

#include <iostream>
#include <sstream>
#include "Row.h"

#include "tools/tools.h"
#include "maria/MariaTable.h"
#include "maria/MariaColumn.h"
#include "maria/MariaUtility.h"

namespace afm {
    namespace database {

        static const std::string sc_table_describe = "describe %s";

        MariaTable::MariaTable(MYSQL *p_db)
            : Table()
            , m_p_db(p_db)
        {

        }

        MariaTable::~MariaTable()
        {

        }

        /*
Row: id int(11) NO PRI auto_increment 
Row: component varchar(128) NO 
Row: product_id varchar(128) NO 
Row: version varchar(128) NO 
Row: build_date varchar(256) NO 
Row: time_stamp timestamp NO current_timestamp() on update current_timestamp() 
Row: active smallint(6) YES 1 
         */

        bool MariaTable::initialize(const std::string &table_name)
        {
            bool success = Table::initialize(table_name);

            if (success == true) {
                MYSQL_RES *pResults = nullptr;
                std::string query = sc_table_describe;

                query.replace(query.find("%s"), 2, table_name);

                if (issueCommand(m_p_db, query, &pResults) == true) {
                    if (pResults != nullptr) {
                        RowData columns;

                        if (getRows(pResults, columns, ' ') > 0) {
                            for (auto column : columns) {
                                IColumnSPtr pColumn = createEmptyColumn();

                                if (pColumn->initialize(column) == true) {
                                    add_column(pColumn);
                                }
                            }
                        }
                        mysql_free_result(pResults);
                    } else {
                        success = false;
                    }
                }
            }

            return success;
        }

        bool MariaTable::get(IRowSPtr &pRow, const QueryOptions &options)
        {
            bool success = false;
            std::string query = build_select(options);

            if (query.size() > 0) {
                MYSQL_RES *pResults = nullptr;
                if (issueCommand(m_p_db, query, &pResults) == true) {
                    if (pResults != nullptr) {
                        RowData row_data;

                        if (getRows(pResults, row_data, ',', true) > 0) {
                            tools::StringTokens column_data;
                            if (tools::split_string(column_data, row_data[0], ',', true) > 0) {
                                pRow = createEmptyRow();
                                pRow->setValues(column_data);
                            }
                        }
                        mysql_free_result(pResults);
                        success = true;
                    }
                }
            }

            return success;
        }

        bool MariaTable::get(Rows &rows, const QueryOptions &options)
        {
            bool success = false;
            std::string query = build_select(options);

            rows.clear();

            if (query.size() > 0) {
                MYSQL_RES *pResults = nullptr;
                if (issueCommand(m_p_db, query, &pResults) == true) {
                    if (pResults != nullptr) {
                        RowData row_data;

                        if (getRows(pResults, row_data, ',', true) > 0) {
                            for (auto row : row_data) {
                                tools::StringTokens column_data;
                                if (tools::split_string(column_data, row, ',', true) > 0) {
                                    IRowSPtr pRow = createEmptyRow();
                                    pRow->setValues(column_data);
                                    rows.push_back(pRow);
                                }
                            }
                        }
                        mysql_free_result(pResults);
                        success = true;
                    }
                }
            }

            return success;
        }

        IColumnSPtr MariaTable::createEmptyColumn() const
        {
            return std::make_shared<MariaColumn>();
        }

        bool MariaTable::on_create_row(const std::string &query)
        {
            MYSQL_RES *pResults = nullptr;

            bool success = issueCommand(m_p_db, query, &pResults);
            if (pResults != nullptr) {
                mysql_free_result(pResults);                
            }
            return success;
        }

        bool MariaTable::on_update_row(const std::string &query)
        {
            MYSQL_RES *pResults = nullptr;

            bool success = issueCommand(m_p_db, query, &pResults);
            if (pResults != nullptr) {
                mysql_free_result(pResults);
            }
            return success;
        }

        bool MariaTable::on_get_row(IRowSPtr &pRow, const std::string &query)
        {
            bool success = false;
            MYSQL_RES *pResults = nullptr;

            if (issueCommand(m_p_db, query, &pResults) == true) {
                if (pResults != nullptr) {
                    RowData row_data;

                    if (getRows(pResults, row_data, ',', true) > 0) {
                        tools::StringTokens column_data;
                        if (tools::split_string(column_data, row_data[0], ',', true) > 0) {
                            pRow = createEmptyRow();
                            pRow->setValues(column_data);
                        }
                    }
                    mysql_free_result(pResults);
                    success = true;
                }
            }

            return success;
        }

        bool MariaTable::on_get_rows(Rows &rows, const std::string &query)
        {
            bool success = false;

            MYSQL_RES *pResults = nullptr;
            if (issueCommand(m_p_db, query, &pResults) == true) {
                if (pResults != nullptr) {
                    RowData row_data;

                    if (getRows(pResults, row_data, ',', true) > 0) {
                        for (auto row : row_data) {
                            tools::StringTokens column_data;
                            if (tools::split_string(column_data, row, ',', true) > 0) {
                                IRowSPtr pRow = createEmptyRow();
                                pRow->setValues(column_data);
                                rows.push_back(pRow);
                            }
                        }
                    }
                    mysql_free_result(pResults);
                    success = true;
                }
            }

            return success;
        }

        IColumnSPtr MariaTable::on_clone_column(const Column &column) const
        {
            return std::make_shared<MariaColumn>(column);
        }
    }
}