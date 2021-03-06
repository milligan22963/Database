/**
 * SQLiteColumn.cpp
 */

#include <iostream>
#include <regex>
#include "tools/tools.h"
#include "sqlite/SQLiteColumn.h"

#include "VariableData.h"

namespace afm {
    namespace database {

        SQLiteColumn::SQLiteColumn()
            : Column()
        {

        }

        SQLiteColumn::SQLiteColumn(const Column &source)
            : Column(source)
        {

        }

        SQLiteColumn::~SQLiteColumn()
        {

        }

        bool SQLiteColumn::initialize(const std::string &details)
        {
            bool success = false;

            tools::StringTokens tokens;

            // need at least two, name and type
            if (tools::split_string(tokens, details, ' ') >= 2) {
                // get the name (without brackets [Name])
                setName(tokens[0].substr(1, tokens[0].size() - 2));

                uint8_t type_index = 1;

                // type should be the next field unless its unsigned
                // then may span two
                std::string type_string = tokens[type_index];
                bool is_unsigned = false;
                // look for UNSIGNED
                if (type_string.find(sc_unsigned_value) != std::string::npos) {
                    is_unsigned = true;
                    type_string = tokens[++type_index];
                }

                // Even though it may or may not be unsigned we need to look for
                // BIG INT
                // VARYING CHARACTER
                // NATIVE CHARACTER
                // DOUBLE PRECISION
                if ((type_string.find(sc_varying_value) != std::string::npos) ||
                    (type_string.find(sc_native_value) != std::string::npos) ||
                    (type_string.find(sc_big_value) != std::string::npos) ||
                    (type_string.find(sc_double_type) != std::string::npos)) {
                    // and glue them together
                    type_string += " " + type_string[++type_index];
                }

                IVariableDataSPtr pVariableData = std::make_shared<VariableData>();

                pVariableData->initialize(determine_type(type_string, is_unsigned));

                setValue(pVariableData);

                std::size_t size_start = type_string.find("(");

                if (size_start != std::string::npos) {
                    std::size_t size_end = type_string.find(")");

                    if (size_end > size_start) {
                        size_start++; // move past the character
                        std::string size = type_string.substr(size_start, size_end - size_start);

                        tools::StringTokens numeric_fields;

                        if (tools::split_string(numeric_fields, size, ',') > 0) {
                            // first is the size
                            pVariableData->setMaxLength(std::stoull(numeric_fields[0]));
                            if (numeric_fields.size() > 1) {
                                setPrecision(std::stoul(numeric_fields[1]));
                            }
                        }
                    }
                }

                // Now lets see if there are any qualifiers such as KEY, PRIMARY, etc
                if (details.find(sc_primary_key) != std::string::npos) {
                    setPrimary(true);
                } else if (details.find(sc_key) != std::string::npos) {
                    setKey(true);
                }

                if (details.find(sc_auto_increment) != std::string::npos) {
                    setAutoIncrement(true);
                }

                if (details.find(sc_not_null) != std::string::npos) {
                    setCanBeNull(false);
                }

                success = true;
            }

            return success;
        }

        std::string SQLiteColumn::getTypeName() const
        {
            std::string typeName = "";

            switch (getType()) {
                case DataType::BIT_T:
                case DataType::TINY_INT_T:
                case DataType::SMALL_INT_T:
                case DataType::INT_T:
                {
                    typeName = sc_integer_type;
                }
                break;
                case DataType::BIG_INT_T:
                case DataType::TIMESTAMP_T:
                {
                    typeName = sc_big_int_type;
                }
                break;
                case DataType::DECIMAL_T:
                case DataType::NUMERIC_T:                    
                {
                    typeName = sc_decimal_type;
                }
                break;
                case DataType::FLOAT_T:
                case DataType::REAL_T:                    
                {
                    typeName = sc_real_type;
                }
                break;
                case DataType::DATE_T:
                case DataType::YEAR_T:
                {
                    typeName = sc_date_type;
                }
                break;
                case DataType::TIME_T:
                case DataType::DATE_TIME_T:
                {
                    typeName = sc_date_time_type;
                }
                break;
                case DataType::CHAR_T:
                case DataType::VARCHAR_T:
                case DataType::VARCHAR_MAX_T:
                case DataType::TEXT_T:
                case DataType::XML_T:
                case DataType::JSON_T:
                case DataType::CLOB_T:
                {
                    typeName = sc_text_type;
                }
                break;
                case DataType::NCHAR_T:
                case DataType::NVARCHAR_T:
                case DataType::NVARCHAR_MAX_T:
                case DataType::NTEXT_T:
                {
                    typeName = sc_ncharacter_type;
                }
                break;
                case DataType::BINARY_T:
                case DataType::VARBINARY_T:
                case DataType::VARBINARY_MAX_T:
                case DataType::IMAGE_T:
                case DataType::BLOB_T:
                {
                    typeName = sc_blob_type;
                }
                break;
                case DataType::EndDataTypes:
                {
                }
                break;
            }
            return typeName;
        }

        // internal parts

        /**
            Fields handled by this method

            static const std::string sc_int_type = "INT";
            static const std::string sc_integer_type = "INTEGER";
            static const std::string sc_tinyint_type = "TINYINT";
            static const std::string sc_smallint_type = "SMALLINT";
            static const std::string sc_mediumint_type = "MEDIUMINT";
            static const std::string sc_bigint_type = "BIGINT";
            static const std::string sc_big_int_type = "BIG INT";
            static const std::string sc_int2_type = "INT2";
            static const std::string sc_int8_type = "INT8";
         */
        DataType SQLiteColumn::is_integer(const std::string &type, bool is_unsigned)
        {
            DataType DataType = DataType::EndDataTypes;

            // we want the integer first so we don't always match on INT
            if (type.find(sc_integer_type) != std::string::npos) {
                DataType = DataType::INT_T;
            } else if (type.find(sc_tinyint_type) != std::string::npos) {
                DataType = DataType::TINY_INT_T;
            } else if ((type.find(sc_smallint_type) != std::string::npos) ||
                    (type.find(sc_int2_type) != std::string::npos)) {
                DataType = DataType::SMALL_INT_T;
            } else if (type.find(sc_mediumint_type) != std::string::npos) {
                DataType = DataType::INT_T;
            } else if ((type.find(sc_bigint_type) != std::string::npos) ||
                    (type.find(sc_big_int_type) != std::string::npos) ||
                    (type.find(sc_int8_type) != std::string::npos)) {
                DataType = DataType::BIG_INT_T;
            } else if (type.find(sc_int_type) != std::string::npos) {
                DataType = DataType::INT_T;
            }

            return DataType;
        }

        /**
            Fields handled by this method

            static const std::string sc_double_type = "DOUBLE";
            static const std::string sc_real_type = "REAL";
            static const std::string sc_double_precision_type = "DOUBLE PRECISION";
            static const std::string sc_float_type = "FLOAT";
         */
        DataType SQLiteColumn::is_float(const std::string &type, bool is_unsigned)
        {
            DataType DataType = DataType::EndDataTypes;

            // DOUBLE works for both single and with precision
            if ((type.find(sc_double_type) != std::string::npos) ||
                 (type.find(sc_real_type) != std::string::npos)) {
                DataType = DataType::REAL_T;
            } else if (type.find(sc_float_type) != std::string::npos) {
                DataType = DataType::FLOAT_T;
            }

            return DataType;
        }

        /**
            Fields handled by this method

            static const std::string sc_numeric_type = "NUMERIC"
            static const std::string sc_decimal_type = "DECIMAL"
            static const std::string sc_boolean_type = "BOOLEAN"
            static const std::string sc_date_type = "DATE"
            static const std::string sc_date_time_type = "DATETIME";
         */
        DataType SQLiteColumn::is_decimal(const std::string &type, bool is_unsigned)
        {
            DataType DataType = DataType::EndDataTypes;

            // DATETIME first
            if (type.find(sc_date_time_type) != std::string::npos) {
                DataType = DataType::DATE_TIME_T;
            } else if (type.find(sc_date_type) != std::string::npos) {
                DataType = DataType::DATE_T;
            } else if (type.find(sc_numeric_type) != std::string::npos) {
                DataType = DataType::NUMERIC_T;
            } else if (type.find(sc_decimal_type) != std::string::npos) {
                DataType = DataType::DECIMAL_T;
            } else if (type.find(sc_boolean_type) != std::string::npos) {
                DataType = DataType::BIT_T;
            }

            return DataType;
        }

        /**
            Fields handled by this method

            static const std::string sc_character_type = "CHARACTER";
            static const std::string sc_varchar_type = "VARCHAR";
            static const std::string sc_varyingchar_type = "VARYING CHARACTER";
            static const std::string sc_nchar_type = "NCHAR";
            static const std::string sc_ncharacter_type = "NATIVE CHARACTER";
            static const std::string sc_nvarchar_type = "NVARCHAR";
            static const std::string sc_text_type = "TEXT";
            static const std::string sc_clob_type = "CLOB";
         */
        DataType SQLiteColumn::is_character(const std::string &type, bool is_unsigned)
        {
            DataType DataType = DataType::EndDataTypes;

            // look for native first, and then NVARCHAR / NCHAR so we don't place in varchar
            if ((type.find(sc_ncharacter_type) != std::string::npos) ||
                (type.find(sc_nvarchar_type) != std::string::npos)) {
                if (type.find("(") != std::string::npos) {
                    DataType = DataType::NVARCHAR_MAX_T;
                } else {
                    DataType = DataType::NVARCHAR_T;
                }
            } else if ((type.find(sc_varchar_type) != std::string::npos) ||
                       (type.find(sc_varyingchar_type) != std::string::npos)) {
                if (type.find("(") != std::string::npos) {
                    DataType = DataType::VARCHAR_MAX_T;
                } else {
                    DataType = DataType::VARCHAR_T;
                }
            } else if (type.find(sc_nchar_type) != std::string::npos) {
                if (type.find("(") != std::string::npos) {
                    DataType = DataType::NVARCHAR_MAX_T;
                } else {
                    DataType = DataType::NVARCHAR_T;
                }
            } else if (type.find(sc_character_type) != std::string::npos) {
                if (type.find("(") != std::string::npos) {
                    DataType = DataType::VARCHAR_MAX_T;
                } else {
                    DataType = DataType::CHAR_T;
                }
            } else if (type.find(sc_text_type) != std::string::npos) {
                DataType = DataType::TEXT_T;
            } else if (type.find(sc_clob_type) != std::string::npos) {
                DataType = DataType::CLOB_T;
            }

            return DataType;
        }

        /**
            Fields handled by this method

            static const std::string sc_blob_type = "BLOB";
            static const std::string sc_binary_type  = "BINARY";
        */
        DataType SQLiteColumn::is_binary(const std::string &type, bool is_unsigned)
        {
            DataType DataType = DataType::EndDataTypes;

            if (type.find(sc_blob_type) != std::string::npos) {
                DataType = DataType::BLOB_T;
            } else if (type.find(sc_binary_type) != std::string::npos) {
                DataType = DataType::BINARY_T;
            }

            return DataType;
        }
    }
}