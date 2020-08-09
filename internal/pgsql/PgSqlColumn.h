/**
 * PgSqlColumn.h
 * 
 * @brief - Table Column representation
 */

#ifndef _H_PGSQL_COLUMN
#define _H_PGSQL_COLUMN

#include <vector>
#include "Column.h"

namespace afm {
    namespace database {
        enum PgSqlTableColumns : uint8_t {
            sc_column_name = 0,
            sc_column_default,
            sc_is_nullable,
            sc_data_type,
            sc_character_maximum_length,
            sc_numeric_precision,
            sc_udt_name,
            sc_is_identity,
            sc_max_pgsql_columns
        };

        class PgSqlColumn : public Column
        {
            public:
                PgSqlColumn();
                PgSqlColumn(const Column &source);

                virtual ~PgSqlColumn();

                virtual bool initialize(const std::string &details) override;
                virtual std::string getTypeName() const override;


            protected:
                virtual DataType is_integer(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_float(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_decimal(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_character(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_binary(const std::string &type, bool is_unsigned = false) override;

            private:
                std::string m_defaultValue;
        };
    }
}
#endif