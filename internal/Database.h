/**
 * Database.h
 * 
 * @brief - database base class regardless of implementation
 */

#ifndef _H_DATABASE
#define _H_DATABASE

#include "IDatabase.h"

namespace afm {
    namespace database {

        extern const std::string sc_create_database;
        extern const std::string sc_database_table_drop;

        struct ConnectionDetails {
            std::string server_name;
            std::string user_id;
            std::string user_password;
            uint32_t    port;
            std::string named_pipe;
        };

        enum CreateDatabaseField {
            AUTOINCREMENT_FIELD,
            PRIMARY_KEY_FIELD,
            KEY_FIELD,
            END_CREATE_DATABASE_FIELDS
        };

        using DatabaseFieldMap = std::map<CreateDatabaseField, std::string>;

        class Database : public IDatabase
        {
            public:
                Database();
                virtual ~Database();

                virtual bool initialize(const std::string &connection, const std::string &database_name) override;

                virtual DatabaseType getType() const final;

                virtual Tables getTables() final;

                virtual ITableSPtr getTable(const std::string &name) override;

                virtual bool dropTable(const std::string &name) final;

                virtual ITableSPtr createTable(const TableOptions &details) override = 0;

                virtual bool test_database() override = 0;

            protected:
                virtual void load_tables() = 0;
                virtual uint32_t get_default_port() const { return 0; }
                void addTable(ITableSPtr &pTable) { m_tables.insert(pTable); }
                void removeTable(ITableSPtr &pTable);
                void setType(DatabaseType type) { m_type = type; }
                std::string getName() const { return m_database_name; }
                virtual std::string build_table_create(ITableSPtr &pTable) const;
                const ConnectionDetails &getConnectionDetails() const { return m_connection_details; }
                void parse_connection_details(const std::string &connection_details);
                void set_database_field(CreateDatabaseField field, const std::string &field_value);
                std::string get_database_field(CreateDatabaseField field) const;

                virtual std::string get_column_creation(IColumnSPtr &pColumn) const;
                virtual bool on_drop_table(const std::string &query) = 0;

            private:
                DatabaseFieldMap    m_field_map;
                ConnectionDetails   m_connection_details;
                std::string         m_database_name;
                DatabaseType        m_type = DatabaseType::END_DATABASE_TYPES;
                Tables              m_tables;

        };
    }
}
#endif