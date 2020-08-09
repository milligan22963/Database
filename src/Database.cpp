/**
 * Database.cpp
 */

#include <sstream>

#include "tools/tools.h"

#include "Database.h"
#include "Column.h"

namespace afm {
    namespace database {
        // database schema
        static const std::string sc_database_connection = "connection";
        static const std::string sc_database_name = "name";
        static const std::string sc_database_type = "type";
        static const std::string sc_server = "server";
        static const std::string sc_user_id = "uid";
        static const std::string sc_password = "pwd";
        static const std::string sc_port = "port";
        static const std::string sc_pipe = "pipe";

        const std::string sc_create_database = "create database %s";
        const std::string sc_database_table_drop = "drop table ";
        static const std::string sc_table_create_command_start = "create table ";
        static const std::string sc_table_create_command_middle = " (";
        static const std::string sc_table_create_command_end = ")";
        static const std::string sc_table_create_if_not_exists_clause = "if not exists ";

        Database::Database()
        {
            m_field_map[AUTOINCREMENT_FIELD] = sc_auto_increment;
            m_field_map[PRIMARY_KEY_FIELD] = sc_primary_key;
            m_field_map[KEY_FIELD] = sc_key;
        }

        Database::~Database()
        {
            m_tables.clear();
        }

        bool Database::initialize(const std::string &connection, const std::string &database_name)
        {
            bool success = false;

            if (database_name.size() > 0) {
                parse_connection_details(connection);

                m_database_name = database_name;

                success = true;
            }
            return success;
        }

        DatabaseType Database::getType() const
        {
            return m_type;
        }

        Tables Database::getTables()
        {
            return m_tables;
        }

        ITableSPtr Database::getTable(const std::string &name)
        {
            ITableSPtr pTable = nullptr;

            for (auto table : m_tables) {
                if (table->getName() == name) {
                    pTable = table;
                    break;
                }
            }

            return pTable;
        }

        void Database::removeTable(ITableSPtr &pTable)
        {
            Tables::const_iterator iter = m_tables.find(pTable);

            if (iter != m_tables.end()) {
                m_tables.erase(iter);
            }
        }

        bool Database::dropTable(const std::string &name)
        {
            bool success = false;
            ITableSPtr pTable = getTable(name);

            if (pTable != nullptr) {
                success = on_drop_table(sc_database_table_drop + name);
                removeTable(pTable);
            }

            return success;
        }

        std::string Database::build_table_create(ITableSPtr &pTable) const
        {
            std::stringstream query;

            std::size_t column_count = 0;
            Columns columns = pTable->getColumns();

            query << sc_table_create_command_start << pTable->getName() << sc_table_create_command_middle;

            for (auto column : columns) {
                query << get_column_creation(column);
                if (++column_count < columns.size()) {
                    query << ", ";
                }
            }

            query << sc_table_create_command_end;

            return query.str();
        }
 
        //                "server=127.0.0.1;uid=mysql;pwd=12345"
        void Database::parse_connection_details(const std::string &connection_details)
        {
            tools::StringTokens tokens;

            m_connection_details.port = get_default_port();

            if (tools::split_string(tokens, connection_details, ';') > 0) {
                tools::StringTokens params;
                for (auto token : tokens) {
                    if (tools::split_string(params, token, '=') > 0) {
                        // we should now have a name/value pair of tokens
                        if (params.size() == 2) {
                            if (params[0] == sc_server) {
                                m_connection_details.server_name = params[1];
                            } else if (params[0] == sc_user_id) {
                                m_connection_details.user_id = params[1];
                            } else if (params[0] == sc_password) {
                                m_connection_details.user_password = params[1];
                            } else if (params[0] == sc_port) {
                                m_connection_details.port = atoi(params[1].c_str());
                            } else if (params[0] == sc_pipe) {
                                m_connection_details.named_pipe = params[1];
                            }
                        }
                    }
                }
            }
        }

        void Database::set_database_field(CreateDatabaseField field, const std::string &field_value)
        {
            m_field_map[field] = field_value;
        }

        std::string Database::get_database_field(CreateDatabaseField field) const
        {
            DatabaseFieldMap::const_iterator iter = m_field_map.find(field);

            if (iter != m_field_map.end()) {
                return iter->second;
            }

            return "";
        }

        std::string Database::get_column_creation(IColumnSPtr &pColumn) const
        {
            std::stringstream query;

            query << pColumn->getName() << " ";
            query << pColumn->getTypeName();
            if (pColumn->isAutoIncrement() == true) {
                query << " " << m_field_map.find(AUTOINCREMENT_FIELD)->second;
            }
            if (pColumn->isPrimary() == true) {
                query << " " << m_field_map.find(PRIMARY_KEY_FIELD)->second;
            } else if (pColumn->isKey() == true) {
                query << " " << m_field_map.find(KEY_FIELD)->second;
            }
            if (pColumn->canBeNull() == false) {
                query << " " << sc_not_null;
            }
            if (pColumn->isUnique() == true) {
                query << " " << sc_unique;
            }

            return query.str();
        }
    }
}