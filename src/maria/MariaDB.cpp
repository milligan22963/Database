/**
 * MariaDB.cpp
 */

#include <iostream>
#include "maria/MariaDB.h"
#include "maria/MariaTable.h"
#include "maria/MariaUtility.h"

namespace afm {
    namespace database {
        
        class MariaDBSentry;

        using MariaDBSentrySPtr = std::shared_ptr<MariaDBSentry>;

        class MariaDBSentry
        {
            public:
                static MariaDBSentrySPtr getInstance()
                {
                    static MariaDBSentrySPtr pSentry = nullptr;

                    if (pSentry == nullptr) {
                        pSentry = std::make_shared<MariaDBSentry>();
                    }

                    return pSentry;
                }

                MariaDBSentry()
                {
                    // revisit to see if we need/desire to utilize the calling syntax
                    if (mysql_library_init(0, nullptr, nullptr) != 0) {
                        // issue error
                    }
                }
                ~MariaDBSentry()
                {
                    mysql_library_end();
                }
        };

        static const std::string sc_show_tables = "show tables";
        static const std::string sc_use_database = "use %s";

        MariaDatabase::MariaDatabase()
        {
            MariaDBSentry::getInstance(); // we will create one to manage mariadb initialize/shutdown
        }

        MariaDatabase::~MariaDatabase()
        {
            if (m_p_db != nullptr) {
                free(m_p_db);
                m_p_db = nullptr;
            }
        }

        bool MariaDatabase::initialize(const std::string &connection, const std::string &database_name)
        {
            bool success = false;

            if (Database::initialize(connection, database_name) == true) {
                m_p_db = mysql_init(nullptr);

                if (m_p_db != nullptr) {
                    // now connect
                    const ConnectionDetails &details = getConnectionDetails();
                    const char *pPipeName = nullptr;

                    if (details.named_pipe.size() > 0) {
                        pPipeName = details.named_pipe.c_str();
                    }

                    // connect w/o database name and then select database if it exists
                    // if it doesn't then create it and then select it
                    if (mysql_real_connect(m_p_db, details.server_name.c_str(), details.user_id.c_str(),
                                          details.user_password.c_str(), nullptr, m_port,
                                          pPipeName, CLIENT_MULTI_STATEMENTS) != nullptr) {

                        success = true;

                        // try and select it, if it fails, try and create it
                        if (select_database(database_name) == false) {
                            if (create_database(database_name) == true) {
                                success = select_database(database_name);
                            } else {
                                // unable to create database, fail.
                                success = false;
                            }
                        }

                        if (success == true) {
                            load_tables();
                        }
                    }
                }
            }
            return success;
        }

        ITableSPtr MariaDatabase::getTable(const std::string &name)
        {
            ITableSPtr pTable = Database::getTable(name);

            // if not found in the base class then go see if we can find it
            if (pTable == nullptr) {
                pTable = std::make_shared<MariaTable>(m_p_db);

                if (pTable->initialize(name) == true) {
                    addTable(pTable);
                }
            }
            return pTable;
        }

        ITableSPtr MariaDatabase::createTable(const TableOptions &details)
        {
            ITableSPtr pTable = std::make_shared<MariaTable>(m_p_db);

            if (pTable->initialize(details) == true) {
                std::string create_query = build_table_create(pTable);
                MYSQL_RES *pResults = nullptr;

                if (issueCommand(m_p_db, create_query, &pResults) == true) {
                    if (pResults != nullptr) {
                        mysql_free_result(pResults);
                    }
                    addTable(pTable);
                } else {
                    pTable = nullptr;
                }
            } else {
                pTable = nullptr;
            }

            return pTable;
        }

        bool MariaDatabase::test_database()
        {
            return true;
        }

        void MariaDatabase::load_tables()
        {
            MYSQL_RES *pResults = nullptr;

            if (issueCommand(m_p_db, sc_show_tables, &pResults) == true) {
                if (pResults != nullptr) {
                    RowData tables;

                    if (getRows(pResults, tables) > 0) {
                        for (auto table_name : tables) {
                            ITableSPtr pTable = std::make_shared<MariaTable>(m_p_db);
                            if (pTable->initialize(table_name) == true) {
                                addTable(pTable);
                            }
                        }
                    }
                    mysql_free_result(pResults);
                }
            }
        }

        bool MariaDatabase::select_database(const std::string &name)
        {
            bool success = false;
            MYSQL_RES *pResults = nullptr;

            std::string query = sc_use_database;

            query.replace(query.find("%s"), 2, name);

            success = issueCommand(m_p_db, query, &pResults);
            if (pResults != nullptr) {
                mysql_free_result(pResults);                
            }

            return success;
        }

        bool MariaDatabase::create_database(const std::string &name)
        {
            bool success = false;
            MYSQL_RES *pResults = nullptr;

            std::string query = sc_create_database;

            query.replace(query.find("%s"), 2, name);

            success = issueCommand(m_p_db, query, &pResults);
            if (pResults != nullptr) {
                mysql_free_result(pResults);
            }

            return success;
        }

        bool MariaDatabase::on_drop_table(const std::string &query)
        {
            bool success = false;

            MYSQL_RES *pResults = nullptr;

            if (issueCommand(m_p_db, query, &pResults) == true) {
                if (pResults != nullptr) {
                    mysql_free_result(pResults);                
                }
                success = true;
            }

            return success;
        }
    }
}
