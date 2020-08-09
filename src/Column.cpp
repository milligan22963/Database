/**
 * Column.cpp
 */

#include <cstring>
#include <cwchar>
#include <iostream>

#include "tools/tools.h"
#include "Column.h"
#include "VariableData.h"

namespace afm {
    namespace database {

        static const std::string sc_column_name = "name";
        static const std::string sc_column_type = "type";
        static const std::string sc_column_size = "size";
        static const std::string sc_column_unsigned = "unsigned";
        static const std::string sc_column_primary = "primary";
        static const std::string sc_column_key = "key";
        static const std::string sc_column_precision = "precision";
        static const std::string sc_column_autoincrement = "autoincrement";
        static const std::string sc_column_allow_null = "allow_null";
        static const std::string sc_column_unique = "unique";
        static const std::string sc_column_foreign_key = "foreign key"; // will need to handle this

        Column::Column()
        {

        }

        Column::Column(const Column &source)
        {
            m_name = source.m_name;
            m_precision = source.m_precision;
            m_isPrimary = source.m_isPrimary;
            m_isKey = source.m_isKey;
            m_canBeNull = source.m_canBeNull;
            m_isAutoIncrement = source.m_isAutoIncrement;

            if (source.m_pValue != nullptr) {
                IVariableDataSPtr pVariableData = std::make_shared<VariableData>();

                pVariableData->initialize(source.m_pValue->getType());

                m_pValue = pVariableData;
            }
        }

        Column::~Column()
        {
            m_pValue = nullptr;
        }

        bool Column::initialize(const ColumnOptions &options)
        {
            bool success = false;

            if (options.find(sc_column_name) != options.end()) {
                if (options.find(sc_column_type) != options.end()) {
                    m_name = options[sc_column_name];

                    std::string type_string = options[sc_column_type];

                    if (options.find(sc_column_size) != options.end()) {
                        std::string size_string = options[sc_column_size];
                        type_string += "(" + size_string;

                        if (options.find(sc_column_precision) != options.end()) {
                            type_string += options[sc_column_precision];
                        }
                        type_string += ")";
                    }
                    
                    bool is_unsigned = false;

                    if (options.find(sc_column_unsigned) != options.end()) {
                        is_unsigned = options[sc_column_unsigned];
                    }

                    IVariableDataSPtr pVariableData = std::make_shared<VariableData>();

                    pVariableData->initialize(determine_type(tools::to_upper(type_string), is_unsigned));

                    setValue(pVariableData);

                    if (options.find(sc_column_primary) != options.end()) {
                        setPrimary(options[sc_column_primary]);
                    }

                    if (options.find(sc_column_autoincrement) != options.end()) {
                        setAutoIncrement(options[sc_column_autoincrement]);
                    }

                    if (options.find(sc_column_unique) != options.end()) {
                        setUnique(options[sc_column_unique]);
                    }

                    if (options.find(sc_column_allow_null) != options.end()) {
                        setCanBeNull(options[sc_column_allow_null]);
                    }

                    success = true;
                }
            }

            return success;
        }

        DataType Column::getType() const
        {
            if (m_pValue != nullptr) {
                return m_pValue->getType();
            } else {
                return DataType::EndDataTypes;
            }
        }

        uint64_t Column::getMaxLength() const
        {
            uint64_t max_length = 0;

            if (m_pValue != nullptr) {
                max_length = m_pValue->getMaxLength();
            }

            return max_length;
        }

        bool Column::isDirty() const
        {
            bool is_dirty = false;

            if (m_pValue != nullptr) {
                is_dirty = m_pValue->isDirty();
            }

            return is_dirty;
        }

        void Column::clearDirtyFlag()
        {
            if (m_pValue != nullptr) {
                m_pValue->clearDirtyFlag();
            }
        }

        IVariableDataSPtr Column::getValue() const
        {
            return m_pValue;
        }

        void Column::setValue(IVariableDataSPtr &pValue)
        {
            m_pValue = pValue;
        }

        bool Column::setValue(const char *pValue, uint32_t length)
        {
            bool success = false;

            if (m_pValue != nullptr) {
                success = m_pValue->setValue(pValue, length);
            }

            return success;
        }

        bool Column::setValue(const std::string &value)
        {
            bool success = false;

            if (m_pValue != nullptr) {
                // ensure we call the correct set method
                success = m_pValue->setValue(value.c_str(), value.size());
            }

            return success;
        }

        DataType Column::determine_type(const std::string &type, bool is_unsigned)
        {
            DataType DataType = is_integer(type, is_unsigned);

            if (DataType != DataType::EndDataTypes) {
                return DataType;
            }

            DataType = is_decimal(type, is_unsigned);
            if (DataType != DataType::EndDataTypes) {
                return DataType;
            }

            DataType = is_float(type, is_unsigned);
            if (DataType != DataType::EndDataTypes) {
                return DataType;
            }

            DataType = is_character(type, is_unsigned);
            if (DataType != DataType::EndDataTypes) {
                return DataType;
            }

            DataType = is_binary(type, is_unsigned);
            if (DataType != DataType::EndDataTypes) {
                return DataType;
            }

            std::cout << "Unknown type: " << type << "\n";

            return DataType;
        }
    }
}