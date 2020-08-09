/**
 * SQLiteDB.cpp
 */

#include <iostream>
#include <string>
#include <vector>

#include "sqlite/SQLiteDB.h"
#include "sqlite/SQLiteTable.h"
#include "sqlite/SQLiteUtility.h"

namespace afm {
    namespace database {

        class SQLiteSentry;

        using SQLiteSentrySPtr = std::shared_ptr<SQLiteSentry>;

        class SQLiteSentry
        {
            public:
                static SQLiteSentrySPtr getInstance()
                {
                    static SQLiteSentrySPtr pSentry = nullptr;

                    if (pSentry == nullptr) {
                        pSentry = std::make_shared<SQLiteSentry>();
                    }

                    return pSentry;
                }

                SQLiteSentry()
                {
                    sqlite3_initialize();
                }
                ~SQLiteSentry()
                {
                    sqlite3_shutdown();
                }
        };

        using TableNames = std::vector<std::string>;
        static const std::string sc_table_query = "SELECT name FROM sqlite_master WHERE type='table';";
        static const std::string sc_single_table_query = "SELECT name FROM sqlite_master WHERE type='table' and name='%s'";
        static const std::string sc_table_ignore = "sqlite_";

        int sqlite_table_query_callback(void *p_tablenames, int col_count, char **pp_data, char **pp_columns);

        SQLiteDatabase::SQLiteDatabase()
            : Database()
        {
            SQLiteSentry::getInstance(); // we will create one to manage sqlite initialize/shutdown
            setType(DatabaseType::SQLITE_DB);
        }

        SQLiteDatabase::~SQLiteDatabase()
        {
            if (m_p_db != nullptr) {
                sqlite3_close(m_p_db);
                m_p_db = nullptr;
            }
        }

        bool SQLiteDatabase::initialize(const std::string &connection, const std::string &database_name)
        {
            bool success = false;

            if (Database::initialize(connection, database_name) == true) {
                if (sqlite3_open(database_name.c_str(), &m_p_db) != SQLITE_OK) {
                    m_p_db = nullptr;
                } else {
                    load_tables();
                    success = true;
                }
            }
            return success;
        }

        ITableSPtr SQLiteDatabase::getTable(const std::string &name)
        {
            ITableSPtr pTable = Database::getTable(name);

            // if not found in the base class then go see if we can find it
            if (pTable == nullptr) {
                TableNames tables;

                std::string query = sc_single_table_query;
                
                query.replace(query.find("%s"), 2, name);

                if (issueCommand(m_p_db, query, sqlite_table_query_callback, &tables) == true) {
                    if (tables.size() > 0) {
                        pTable = std::make_shared<SQLiteTable>(m_p_db);

                        pTable->initialize(tables[0]);
                        addTable(pTable);
                    }
                }
            }
            return pTable;
        }

        ITableSPtr SQLiteDatabase::createTable(const DatabaseOptions &details)
        {
            ITableSPtr pTable = std::make_shared<SQLiteTable>(m_p_db);

            if (pTable->initialize(details) == true) {
                if (issueCommand(m_p_db, build_table_create(pTable)) == false) {
                    pTable = nullptr;
                } else {
                    addTable(pTable);
                }
            } else {
                pTable = nullptr;
            }

            return pTable;
        }

        static const std::string sc_test_columns [] = {
            "[ArtistId] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL",
            "[My_Column] VARCHAR(22),",
            "[My_Column] NVARCHAR(410),",
            "[My_Column] VARCHAR",
            "[My_Column] VARCHAR,",
            "[My_Column] REAL,",
            "[My_Column] TEXT",
            "[My_Column] BLOB,",
            "[My_Column] FLOAT",
            "[DecimalCol] DECIMAL(10,5)",
            "[DBP] DOUBLE PRECISION",
            "[BigIntTest] BIG INT",
            "[UnInt] UNSIGNED BIG INT",
            "[Name] VARYING CHARACTER"
//            "[ValidateTest] WHATEVER THIS IS"
        };

        bool SQLiteDatabase::test_database()
        {
            bool success = true;

            IColumnSPtr pColumn = std::make_shared<SQLiteColumn>();

            for (auto column : sc_test_columns) {
                if (pColumn->test_type(column) == DataType::EndDataTypes) {
                    std::cout << "\tFailed to process column: " << column << "\n";
                    success = false;
                }
            }

            return success;
        }

        // internal methods
        void SQLiteDatabase::load_tables()
        {
            TableNames tables;

            if (issueCommand(m_p_db, sc_table_query, sqlite_table_query_callback, &tables) == true) {
                for (auto table : tables) {
                    ITableSPtr pTable = std::make_shared<SQLiteTable>(m_p_db);

                    pTable->initialize(table);

                    addTable(pTable);
                }
            }
        }

        bool SQLiteDatabase::on_drop_table(const std::string &query)
        {
            return issueCommand(m_p_db, query);
        }

        // callbacks
        int sqlite_table_query_callback(void *p_tablenames, int col_count, char **pp_data, char **pp_columns)
        {
            TableNames *pTables = (TableNames *)p_tablenames;

            if (pp_data != nullptr) {
                if (strstr(pp_data[0], sc_table_ignore.c_str()) == nullptr) {

                    // return just the name as we don't want to issue another call from within
                    // the callback
                    pTables->push_back(pp_data[0]);
                }
            }

            return SQLITE_OK;
        }
    }
}