/**
 * MariaColumn.h
 * 
 * @brief - Table Column representation
 */

#ifndef _H_MARIA_COLUMN
#define _H_MARIA_COLUMN

#include <vector>
#include "Column.h"

namespace afm {
    namespace database {
        using EnumTypes = std::vector<std::string>;

        class MariaColumn : public Column
        {
            public:
                MariaColumn();
                MariaColumn(const Column &source);

                virtual ~MariaColumn();

                virtual bool initialize(const std::string &details) override;
                virtual std::string getTypeName() const override;


            protected:
                virtual DataType is_integer(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_float(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_decimal(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_character(const std::string &type, bool is_unsigned = false) override;
                virtual DataType is_binary(const std::string &type, bool is_unsigned = false) override;

            private:
                bool        m_isEnum = false;
                EnumTypes   m_enums;
                std::string m_defaultValue;
        };
    }
}
#endif