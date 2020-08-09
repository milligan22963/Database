/**
 * Column.h
 * 
 * @brief - Table Column representation
 */

#ifndef _H_COLUMN
#define _H_COLUMN

#include <cstdint>
#include <ctime> 

#include "IColumn.h"

namespace afm {
    namespace database {
        // Descriptive
        static const std::string sc_unsigned_value = "UNSIGNED";
        static const std::string sc_native_value = "NATIVE";
        static const std::string sc_precision_value = "PRECISION";
        static const std::string sc_varying_value = "VARYING";
        static const std::string sc_big_value = "BIG";
        static const std::string sc_primary_key = "PRIMARY KEY";
        static const std::string sc_foreign_key = "FOREIGN KEY"; // will need to handle this
        static const std::string sc_unique = "UNIQUE";
        static const std::string sc_key = "KEY";
        static const std::string sc_auto_increment = "AUTOINCREMENT";
        static const std::string sc_not_null = "NOT NULL";

        // both descriptive and valid type
        static const std::string sc_double_type = "DOUBLE";

        // Valid types
        static const std::string sc_null_type = "NULL";
        static const std::string sc_bit_type = "BIT";
        static const std::string sc_blob_type = "BLOB";
        static const std::string sc_binary_type  = "BINARY";

        static const std::string sc_int_type = "INT";
        static const std::string sc_integer_type = "INTEGER";
        static const std::string sc_tinyint_type = "TINYINT";
        static const std::string sc_smallint_type = "SMALLINT";
        static const std::string sc_mediumint_type = "MEDIUMINT";
        static const std::string sc_bigint_type = "BIGINT";
        static const std::string sc_big_int_type = "BIG INT";
        static const std::string sc_int2_type = "INT2";
        static const std::string sc_int8_type = "INT8";

        static const std::string sc_real_type = "REAL";
        static const std::string sc_double_precision_type = "DOUBLE PRECISION";
        static const std::string sc_float_type = "FLOAT";

        static const std::string sc_numeric_type = "NUMERIC";
        static const std::string sc_decimal_type = "DECIMAL";
        static const std::string sc_boolean_type = "BOOLEAN";
        static const std::string sc_date_type = "DATE";
        static const std::string sc_date_time_type = "DATETIME";

        static const std::string sc_character_type = "CHARACTER";
        static const std::string sc_varchar_type = "VARCHAR";
        static const std::string sc_varyingchar_type = "VARYING CHARACTER";
        static const std::string sc_nchar_type = "NCHAR";
        static const std::string sc_ncharacter_type = "NATIVE CHARACTER";
        static const std::string sc_nvarchar_type = "NVARCHAR";
        static const std::string sc_text_type = "TEXT";
        static const std::string sc_clob_type = "CLOB";

        class Column : public IColumn
        {
            public:
                Column();
                Column(const Column &source);
                virtual ~Column();

                virtual bool initialize(const ColumnOptions &options) override;

                virtual bool initialize(const std::string &details) override = 0;

                virtual std::string getName() const final { return m_name; }
                virtual DataType getType() const final;
                virtual std::string getTypeName() const override = 0;
                virtual uint64_t getMaxLength() const final;
                virtual bool isPrimary() const final { return m_isPrimary; }
                virtual bool isKey() const { return m_isKey; }
                virtual bool isAutoIncrement() const final { return m_isAutoIncrement; }
                virtual bool isUnique() const final { return m_isUnique; }

                virtual bool isDirty() const final;
                virtual void clearDirtyFlag() final;
                virtual bool canBeNull() const final { return m_canBeNull; }
                virtual uint8_t getPrecision() const final { return m_precision; }


                virtual IVariableDataSPtr getValue() const final;
                virtual void setValue(IVariableDataSPtr &pValue) final;
                virtual bool setValue(const char *pValue, uint32_t length) final;
                virtual bool setValue(const std::string &value) final;

                virtual DataType test_type(const std::string &type) { return determine_type(type); }

            protected:
                DataType determine_type(const std::string &type, bool is_unsigned = false);
                virtual DataType is_integer(const std::string &type, bool is_unsigned = false) = 0;
                virtual DataType is_float(const std::string &type, bool is_unsigned = false) = 0;
                virtual DataType is_decimal(const std::string &type, bool is_unsigned = false)  = 0;
                virtual DataType is_character(const std::string &type, bool is_unsigned = false) = 0;
                virtual DataType is_binary(const std::string &type, bool is_unsigned = false) = 0;
                void setName(const std::string &name) { m_name = name; }

                // primary by default is a key
                void setPrimary(bool isPrimary) { m_isPrimary = isPrimary; m_isKey = isPrimary; }
                void setKey(bool isKey) { m_isKey = isKey; }
                void setAutoIncrement(bool isAutoIncrement) { m_isAutoIncrement = isAutoIncrement; }
                void setUnique(bool isUnique) { m_isUnique = isUnique; }
                void setCanBeNull(bool canBeNull) { m_canBeNull = canBeNull; }
                void setPrecision(uint8_t precision) { m_precision = precision; }

            private:
                std::string         m_name;
                IVariableDataSPtr   m_pValue = nullptr;
                uint8_t             m_precision = 0;
                bool                m_isPrimary = false;
                bool                m_isKey = false;
                bool                m_isUnique = false;
                bool                m_canBeNull = true;
                bool                m_isAutoIncrement = false;
        };
    }
}
#endif