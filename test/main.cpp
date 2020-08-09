#include <iostream>

#include <nlohmann/json.hpp>
#include <DatabaseFactory.h>

afm::database::ITableSPtr create_table(afm::database::IDatabaseSPtr &pDatabase, const std::string &table_name);

void test_sqlite()
{
    afm::database::DatabaseOptions options;

    options["name"] = "chinook.db";
    options["type"] = "sqlite";

    afm::database::IDatabaseSPtr pDatabase = afm::database::DatabaseFactory::getInstance()->createDatabase(options);

    if (pDatabase != nullptr) {
        afm::database::Tables table_set = pDatabase->getTables();

        for (auto table : table_set) {
            std::cout << "Table: " << table->getName() << "\n";
        }

        afm::database::ITableSPtr pTable = pDatabase->getTable("artists");
        if (pTable != nullptr) {
            std::cout << "We found the table: " << pTable->getName() << "\n";
            for (auto column : pTable->getColumns()) {
                std::cout << "\tColumn: " << column->getName() << "\n";
            }

            afm::database::Rows rows;

            pTable->get(rows);
            std::cout << pTable->getColumnNames();
            for (auto row : rows) {
                std::cout << row->toString();
            }

            afm::database::IRowSPtr pRow = pTable->createEmptyRow();
            if (pRow != nullptr) {
                afm::database::QueryOptions options;

                options["ArtistId"] = 51;
                if (pTable->get(pRow, options) == true) {
                    std::cout << "Specific row: " << pRow->toString();
                    if (pRow->setValue("Name", "Test") == false) {
                        std::cout << "Failed to set column value.\n";
                    }
                    pTable->set(pRow);
                    std::cout << "Specific row: " << pRow->toString();

                    pRow->setValue("Name", "Dan");
                    pTable->create(pRow);
                } else {
                    std::cout << "Unable to get single row.\n";
                }
            }

            afm::database::ITableSPtr pTable = create_table(pDatabase, "bodacius");
            if (pTable != nullptr) {
                //
            }

        } else {
            std::cout << "FAILED to find table: artists\n";
        }

        if (pDatabase->test_database() != true) {
            std::cout << "FAILED database test.\n";
        }
    }
}

void test_mysql()
{
   afm::database::DatabaseOptions options;

    options["name"] = "orville";
    options["type"] = "mysql";
    options["connection"] = "server=localhost;uid=abc;pwd=password";

    afm::database::IDatabaseSPtr pDatabase = afm::database::DatabaseFactory::getInstance()->createDatabase(options);

    if (pDatabase != nullptr) {
        afm::database::Tables table_set = pDatabase->getTables();

        if (table_set.size() > 0) {
            for (auto table : table_set) {
                std::cout << "Table: " << table->getName() << "\n";
            }

            afm::database::ITableSPtr pTable = create_table(pDatabase, "bodacius");
            if (pTable != nullptr) {
                //
            }

            pTable = pDatabase->getTable("everactive_ext_control_nodes");
            if (pTable != nullptr) {
                afm::database::IRowSPtr pRow = nullptr;
                afm::database::QueryOptions options;

                afm::database::Rows rows;

                pTable->get(rows);
                std::cout << pTable->getColumnNames();
                for (auto row : rows) {
                    std::cout << row->toString();
                }

                std::cout << "Looking for a row\n";

                options["id"] = 1;

                if (pTable->get(pRow, options) == true) {
                    std::cout << "Specific row: " << pRow->toString();
                    if (pRow->setValue("friendly_name", "Test") == false) {
                        std::cout << "Failed to set column value.\n";
                    }
                    if (pTable->set(pRow) == true) {
                        std::cout << "Specific row: " << pRow->toString();
                    }

                    pRow->setValue("friendly_name", "Dan");
                    if (pTable->create(pRow) == true) {
                        std::cout << "New row added: " << pRow->toString();
                    }
                } else {
                    std::cout << "Unable to get single row.\n";
                }
            } else {
                std::cout << "Couldn't find table in database.\n";
            }
        }

        if (pDatabase->test_database() != true) {
            std::cout << "FAILED database test.\n";
        }
    } else {
        std::cout << "FAILED to create database\n";
    }
}

void test_postgres()
{
   afm::database::DatabaseOptions options;

    options["name"] = "orville";
    options["type"] = "pgsql";
    options["connection"] = "server=127.0.0.1;uid=abc;pwd=password";

    afm::database::IDatabaseSPtr pDatabase = afm::database::DatabaseFactory::getInstance()->createDatabase(options);

    if (pDatabase != nullptr) {
        std::cout << "Looking at tables\n";
        afm::database::Tables table_set = pDatabase->getTables();
        afm::database::ITableSPtr pTable = create_table(pDatabase, "bonkers");
        if (pTable != nullptr) {

        } else {
            std::cout << "Failed to create a bonkers table\n";
        }

        pTable = pDatabase->getTable("company");
        if (pTable != nullptr) {
            std::cout << "Found the company table\n";
        }
    }
}

afm::database::ITableSPtr create_table(afm::database::IDatabaseSPtr &pDatabase, const std::string &table_name)
{
    afm::database::DatabaseOptions options;

    // create some tables
    options["name"] = table_name;
    options["columns"] = nlohmann::json::array();
    options["columns"].push_back({ {"name", "test1"}, {"type", "integer"}, {"primary", true}, {"autoincrement", true} });
    options["columns"].push_back({ {"name", "test2"}, {"type", "text"} });

    afm::database::ITableSPtr pNewTable = pDatabase->createTable(options);

    if (pNewTable != nullptr) {
        std::cout << "Table was created.\n";
    } else {
        std::cout << "Unable to create table - does it already exist?\n";
        pNewTable = nullptr;
    }

    return pNewTable;
}

int main(int argc, char *argv[])
{
    std::cout << "Starting up\n";

    test_sqlite();
    //test_mysql();
    //test_postgres();
 
    std::cout << "Shutting down\n";
    
    return 0;    
}
