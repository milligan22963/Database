/**
 * Table.cpp
 */

#include <sstream>

#include "Row.h"
#include "Table.h"

namespace afm {
    namespace database {
        // should be common across different databases though the internal methods can be
        // overridden as desired.
        static const std::string sc_table_load = "select * from ";
        static const std::string sc_table_where_clause = " where ";
        static const std::string sc_table_and_clause = " and ";

        static const std::string sc_insert_row_start = "insert into %s (";
        static const std::string sc_insert_row_middle = ") values (";
        static const std::string sc_insert_row_end = ")";

        static const std::string sc_update_row_start = "update %s set ";

        // table schema
        static const std::string sc_table_name = "name";
        static const std::string sc_columns = "columns";

        Table::~Table()
        {
            m_columns.clear();
        }

        bool Table::initialize(const TableOptions &options)
        {
            bool success = false;

            TableOptions::const_iterator iter = options.find(sc_table_name);

            if (iter != options.end()) {
                m_table_name = options[sc_table_name];
                if (options.find(sc_columns) != options.end()) {
                    
                    success = true;

                    for (auto column : options[sc_columns]) {
                        IColumnSPtr pColumn = createEmptyColumn();

                        if (pColumn->initialize(column) == true) {
                            add_column(pColumn);
                        } else {
                            success = false;
                            break;
                        }
                    }
                }
            }

            return success;
        }

        bool Table::initialize(const std::string &table_name)
        {
            bool success = true;

            m_table_name = table_name;

            return success;
        }

        bool Table::get(IRowSPtr &pRow, const QueryOptions &options)
        {
            bool success = false;
            std::string query = build_select(options);

            if (query.size() > 0) {
                success = on_get_row(pRow, query);
            }

            return success;
        }

        bool Table::set(IRowSPtr &pRow, const QueryOptions &options)
        {
            bool success = false;

            // we will need to have the row tells us what needs to be done
            if (pRow->isDirty() == true) {
                // we shouldn't just blindly write
                success = on_update_row(build_update(pRow, options));
            }

            return success;
        }

        bool Table::get(Rows &rows, const QueryOptions &options)
        {
            bool success = false;
            std::string query = build_select(options);

            rows.clear();

            if (query.size() > 0) {
                success = on_get_rows(rows, query);
            }

            return success;
        }

        bool Table::create(IRowSPtr &pRow)
        {
            return on_create_row(build_insert(pRow));
        }

        std::string Table::getColumnNames() const
        {
            std::stringstream header;

            uint32_t columnCount = 0;

            for (auto column : m_columns) {
                header << "[" << column->getName() << "]";

                if (++columnCount < m_columns.size() - 1) {
                    header << " ";
                }
            }
            header << "\n";

            return header.str();
        }

        IRowSPtr Table::createEmptyRow() const
        {
            IRowSPtr pRow = std::make_shared<Row>();

            // add a copy of each column to the row and then return it
            for (auto column : get_columns()) {
                std::shared_ptr<Column> pColumn = std::dynamic_pointer_cast<Column>(column);
                if (pColumn != nullptr) {
                    pRow->addColumn(on_clone_column(*pColumn));
                } else {
                    // error
                    break;
                }
            }
            return pRow;
        }

        // internal
        void Table::process_table_options(std::stringstream &output, const QueryOptions &options) const
        {
            if (options.size() > 0) {
                std::size_t option_index = 1;

                output << sc_table_where_clause;
                for (nlohmann::json::const_iterator iter = options.begin(); iter != options.end(); iter++) {
                    output << iter.key() << "=";
                    if (iter->is_string() == true) {
                        output << "'" << iter.value() << "'";
                    } else {
                        output << iter.value();
                    }
                    // we need to use an and after each additional option past the first one
                    if (option_index < options.size()) {
                        output << sc_table_and_clause;
                    }
                    option_index++;
                }
            }
        }

        std::string Table::build_select(const QueryOptions &options)
        {
            std::stringstream query_string;

            query_string << sc_table_load << m_table_name;

            process_table_options(query_string, options);

            return query_string.str();
        }

        std::string Table::build_insert(const IRowSPtr &pRow) const
        {
            std::stringstream insert_string;
            std::stringstream values;
            bool is_first = true;

            std::string query = sc_insert_row_start;

            // replace table with my table name
            query.replace(query.find("%s"), 2, m_table_name);

            insert_string << query;

            for (auto column : pRow->getColumns()) {
                bool add_column = true;

                if (column->isPrimary() == true) {
                    if (column->isAutoIncrement() == true) {
                        // skip it
                        add_column = false; // auto increment, let db take care of it
                    }
                }

                if (add_column == true) {
                    // don't tack a comma on for the first time in, and don't add
                    // one until we know for sure there is more than one entry
                    // this will ensure the last value doesn't have a trailing comma
                    if (is_first == false) {
                        insert_string << ",";
                        values << ",";
                    } else {
                        is_first = false;
                    }
                    insert_string << column->getName();
                    IVariableDataSPtr pValue = column->getValue();
                    if (pValue != nullptr) {
                        if (pValue->isCharacterData() == true) {
                            values << "'" << pValue->getValue() << "'";
                        } else {
                            values << pValue->getValue();
                        }
                    }
                }
            }

            insert_string << sc_insert_row_middle;
            insert_string << values.str();
            insert_string << sc_insert_row_end;

            return insert_string.str();
        }

        std::string Table::build_update(const IRowSPtr &pRow, const QueryOptions &options) const
        {
            std::stringstream update_string;
            bool is_first = true;
            IColumnSPtr pKeyColumn = nullptr;

            std::string query = sc_update_row_start;

            // replace table with my table name
            query.replace(query.find("%s"), 2, m_table_name);

            update_string << query;

            for (auto column : pRow->getColumns()) {
                bool add_column = true;

                if (column->isAutoIncrement() == true) {
                    // skip it
                    add_column = false; // auto increment, let db take care of it

                    if (column->isPrimary() == true) {
                        pKeyColumn = column;
                    }
                }

                if (add_column == true) {
                    // don't tack a comma on for the first time in, and don't add
                    // one until we know for sure there is more than one entry
                    // this will ensure the last value doesn't have a trailing comma
                    if (is_first == false) {
                        update_string << ",";
                    } else {
                        is_first = false;
                    }
                    update_string << column->getName() << "=";

                    IVariableDataSPtr pValue = column->getValue();
                    if (pValue != nullptr) {
                        if (pValue->isCharacterData() == true) {
                            update_string << "'" << pValue->getValue() << "'";
                        } else {
                            update_string << pValue->getValue();
                        }
                    }
                }
            }

            if (options.size() > 0) {
                process_table_options(update_string, options);
            } else if (pKeyColumn != nullptr) {
                update_string << sc_table_where_clause << pKeyColumn->getName() << "=";
                IVariableDataSPtr pValue = pKeyColumn->getValue();
                if (pValue != nullptr) {
                    if (pValue->isCharacterData() == true) {
                        update_string << "'" << pValue->getValue() << "'";
                    } else {
                        update_string << pValue->getValue();
                    }
                }
            }
            return update_string.str();
        }
    }
}