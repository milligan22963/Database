/**
 * SQLiteColumn.h
 * 
 * @brief - Table Column representation
 */

#ifndef _H_SQLITE_COLUMN
#define _H_SQLITE_COLUMN

#include "Column.h"

namespace afm {
    namespace database {
        class SQLiteColumn : public Column
        {
            public:
                SQLiteColumn();
                SQLiteColumn(const Column &source);

                virtual ~SQLiteColumn();

                virtual bool initialize(const std::string &details) override;
                virtual std::string getTypeName() const override;


            protected:
                virtual DataType is_integer(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_float(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_decimal(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_character(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_binary(const std::string &type, bool is_unsigned = false) override;
        };
    }
}
#endif