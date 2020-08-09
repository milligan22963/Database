/**
 * PgSqlDB.cpp
 */

#include <iostream>
#include <sstream>
#include <cstring>
#include "pgsql/PgSqlDB.h"
#include "pgsql/PgSqlTable.h"
#include "pgsql/PgSqlUtility.h"

namespace afm {
    namespace database {
        static const std::string sc_connection_dbname = "dbname";
        static const std::string sc_connection_username = "user";
        static const std::string sc_connection_password = "password";
        static const std::string sc_conneciton_host_address = "hostaddr";
        static const std::string sc_connection_port = "port";
        static const std::string sc_default_db = "postgres";
        static const std::string sc_list_tables = "select * from pg_catalog.pg_tables";
        static const std::string sc_ignore_schema1 = "pg_catalog";
        static const std::string sc_ignore_schema2 = "information_schema";
        static const uint8_t sc_schema_name_position = 0;
        static const uint8_t sc_table_name_position = 1;
        static const std::string sc_serial_field = "serial";

        PgSqlDatabase::PgSqlDatabase()
            : Database()
        {
            // register our fields
            set_database_field(CreateDatabaseField::AUTOINCREMENT_FIELD, sc_serial_field);
        }

        PgSqlDatabase::~PgSqlDatabase()
        {
            if (m_pConnection != nullptr) {
                delete m_pConnection;
            }
        }

        bool PgSqlDatabase::initialize(const std::string &connection, const std::string &database_name)
        {
            bool success = false;

            if (Database::initialize(connection, database_name) == true) {

                if (select_database(database_name) == false) {
                    success = select_database(sc_default_db);
                } else {
                    success = true;
                }

                if (success == true) {
                    load_tables();
                }
            }

            return success;
        }

        ITableSPtr PgSqlDatabase::getTable(const std::string &name)
        {
            ITableSPtr pTable = Database::getTable(name);

            // if not found in the base class then go see if we can find it
            if (pTable == nullptr) {
                pTable = std::make_shared<PgSqlTable>(m_pConnection);

                if (pTable->initialize(name) == true) {
                    addTable(pTable);
                }
            }
            return pTable;
        }

        ITableSPtr PgSqlDatabase::createTable(const TableOptions &details)
        {
            ITableSPtr pTable = std::make_shared<PgSqlTable>(m_pConnection);

            if (pTable->initialize(details) == true) {

                std::string create_query = build_table_create(pTable);
                pqxx::result results;

                std::cout << "Query: " << create_query << "\n";

                if (issueCommand(m_pConnection, create_query, results) == true) {
                    addTable(pTable);
                } else {
                    pTable = nullptr;
                }
            } else {
                pTable = nullptr;
            }

            return pTable;
        }

        bool PgSqlDatabase::test_database()
        {
            return true;
        }

        // internal
        /*
orville=> select * from pg_catalog.pg_tables where tableowner='daniel';
 schemaname | tablename | tableowner | tablespace | hasindexes | hasrules | hastriggers | rowsecurity 
------------+-----------+------------+------------+------------+----------+-------------+-------------
 public     | company   | daniel     |            | t          | f        | f           | f
(1 row)
         */
        void PgSqlDatabase::load_tables()
        {
            pqxx::result results;

            if (issueCommand(m_pConnection, sc_list_tables, results) == true) {
                for (auto iter : results) {
                    if ((strcmp(iter[sc_schema_name_position].c_str(), sc_ignore_schema1.c_str()) != 0) &&
                        (strcmp(iter[sc_schema_name_position].c_str(), sc_ignore_schema2.c_str()) != 0)) {
                        // process this one
                        std::string table_name = iter[sc_table_name_position].c_str();

                        ITableSPtr pTable = std::make_shared<PgSqlTable>(m_pConnection);
                        if (pTable->initialize(table_name) == true) {
                            addTable(pTable);
                        }
                    }
                }
            }
        }

        bool PgSqlDatabase::select_database(const std::string &name)
        {
            bool success = false;
            const ConnectionDetails &details = getConnectionDetails();

            // create pgsql connection string
            std::stringstream connection;

            connection << sc_connection_dbname << "=" << name << " ";
            connection << sc_connection_username << "=" << details.user_id << " ";
            connection << sc_connection_password << "=" << details.user_password << " ";
            connection << sc_conneciton_host_address << "=" << details.server_name << " ";
            connection << sc_connection_port << "=" << details.port << " ";

            try {
                if (m_pConnection != nullptr) {
                    delete m_pConnection;
                    m_pConnection = nullptr;
                }

                m_pConnection = new pqxx::connection(connection.str().c_str());
                if (m_pConnection != nullptr) {
                    if (m_pConnection->is_open() == true) {
                        success = true;
                    } else {
                        delete m_pConnection;
                        m_pConnection = nullptr;
                    }
                }
            }
            catch (const std::exception &db_error) {
                m_pConnection = nullptr;
                success = false;
            }
            return success;
        }

        bool PgSqlDatabase::create_database(const std::string &name)
        {
            bool success = false;
            pqxx::result results;

            std::string query = sc_create_database;

            query.replace(query.find("%s"), 2, name);

            if (issueCommand(m_pConnection, query, results) == true) {
                success = true;
            }

            return success;
        }

        std::string PgSqlDatabase::get_column_creation(IColumnSPtr &pColumn) const
        {
            // handle auto increment specifically otherwise let the base take a stab at it
            if (pColumn->isAutoIncrement() == true) {
                std::stringstream query;

                query << pColumn->getName() << " " << sc_serial_field;
                if (pColumn->isPrimary() == true) {
                    query << " " << get_database_field(PRIMARY_KEY_FIELD);
                } else if (pColumn->isKey() == true) {
                    query << " " << get_database_field(KEY_FIELD);
                }
                if (pColumn->canBeNull() == false) {
                    query << " " << sc_not_null;
                }
                if (pColumn->isUnique() == true) {
    //                query << " " << sc_unique;
                }
                return query.str();
            }

            return Database::get_column_creation(pColumn);
        }

        bool PgSqlDatabase::on_drop_table(const std::string &query)
        {
            pqxx::result results;

            return issueCommand(m_pConnection, query, results);
        }
  }
}