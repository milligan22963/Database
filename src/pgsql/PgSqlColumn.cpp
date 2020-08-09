/**
 * PgSqlColumn.cpp
 */

#include <iostream>

#include "tools/tools.h"
#include "pgsql/PgSqlColumn.h"
#include "VariableData.h"

namespace afm {
    namespace database {
        static const std::string sc_canBeNull = "YES";
        static const std::string sc_cannotBeNull = "NO";
        static const std::string sc_isIdentity = "YES";
        static const std::string sc_isNotIdentity = "NO";

        PgSqlColumn::PgSqlColumn()
            : Column()
        {

        }

        PgSqlColumn::PgSqlColumn(const Column &source)
            : Column(source)
        {

        }

        PgSqlColumn::~PgSqlColumn()
        {

        }

        bool PgSqlColumn::initialize(const std::string &details)
        {
            bool success = false;

            std::cout << "Details: " << details << "\n";

            tools::StringTokens tokens;
        //static const uint8_t sc_udt_name = 6;

            // this will be the columns I ask for but some might be empty
            if (tools::split_string(tokens, details, ',', true) >= 2) {
                setName(tokens[sc_column_name]);

                IVariableDataSPtr pVariableData = std::make_shared<VariableData>();

                pVariableData->initialize(determine_type(tools::to_upper(tokens[sc_data_type]), false));

                if (tokens[sc_character_maximum_length].size() > 0) {
                    try {
                        uint64_t maxLength = std::stoull(tokens[sc_character_maximum_length]);
                        if (maxLength > 0) {
                            pVariableData->setMaxLength(maxLength);
                        }
                    }
                    catch (std::invalid_argument &invalid_arg) {
                        // no worries - it may be empty
                    }
                    catch (std::out_of_range &range_error) {
                        std::cout << "ERROR (max length): " << range_error.what() << "\n";
                    }
                }

                if (tokens[sc_numeric_precision].size() > 0) {
                    try {
                        uint64_t precision = std::stoull(tokens[sc_numeric_precision]);
                        if (precision > 0) {
                            setPrecision(precision);
                        }
                    }
                    catch (std::invalid_argument &invalid_arg) {
                        // no worries - it may be empty
                    }
                    catch (std::out_of_range &range_error) {
                        std::cout << "ERROR (precision): " << range_error.what() << "\n";
                    }
                }
                setValue(pVariableData);

                m_defaultValue = tokens[sc_column_default];
                if (tokens[sc_is_nullable] == sc_canBeNull) {
                    setCanBeNull(true);
                } else {
                    setCanBeNull(false);
                }

                if (tokens[sc_is_identity] == sc_isIdentity) {
                    setAutoIncrement(true);
                } else {
                    setAutoIncrement(false);
                }

                /*if (tokens[index].find(sc_maria_primary_key) != std::string::npos) {
                    setPrimary(true);
                } else if (tokens[index].find(sc_maria_unique_key) != std::string::npos) {
                    setKey(true);
                } else if (tokens[index].find(sc_maria_auto_increment) != std::string::npos) {
                    setAutoIncrement(true);
                }*/
                success = true;
            }

            return success;
        }

        std::string PgSqlColumn::getTypeName() const
        {
            std::string typeName = "";

            switch (getType()) {
                case DataType::BIT_T:
                {
                    typeName = sc_bit_type;
                }
                break;
                case DataType::TINY_INT_T:
                {
                    typeName = sc_tinyint_type;
                }
                break;
                case DataType::SMALL_INT_T:
                {
                    typeName = sc_smallint_type;
                }
                break;
                case DataType::INT_T:
                {
                    typeName = sc_integer_type;
                }
                break;
                case DataType::BIG_INT_T:
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
                case DataType::TIMESTAMP_T:
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

        // internal

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
        DataType PgSqlColumn::is_integer(const std::string &type, bool is_unsigned)
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
                    (type.find(sc_big_int_type) != std::string::npos)) {
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
        DataType PgSqlColumn::is_float(const std::string &type, bool is_unsigned)
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
         */
        DataType PgSqlColumn::is_decimal(const std::string &type, bool is_unsigned)
        {
            DataType DataType = DataType::EndDataTypes;

            if (type.find(sc_numeric_type) != std::string::npos) {
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
            static const std::string sc_date_type = "DATE"
            static const std::string sc_date_time_type = "DATETIME";
         */
        DataType PgSqlColumn::is_character(const std::string &type, bool is_unsigned)
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
            } else if (type.find(sc_date_time_type) != std::string::npos) {
                DataType = DataType::TEXT_T;
            } else if (type.find(sc_date_type) != std::string::npos) {
                DataType = DataType::TEXT_T;
            }

            return DataType;
        }

        /**
            Fields handled by this method

            static const std::string sc_blob_type = "BLOB";
            static const std::string sc_binary_type  = "BINARY";
        */
        DataType PgSqlColumn::is_binary(const std::string &type, bool is_unsigned)
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