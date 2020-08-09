/**
 * VariableData.cpp
 */

#include <cstring>
#include <cwchar>
#include <sstream>

#include "tools/tools.h"
#include "VariableData.h"

namespace afm {
    namespace database {

        VariableData::~VariableData()
        {
            if ((m_type == DataType::CHAR_T) ||
                (m_type == DataType::VARCHAR_T) ||
                (m_type == DataType::VARCHAR_MAX_T) ||
                (m_type == DataType::TEXT_T) ||
                (m_type == DataType::CLOB_T) ||
                (m_type == DataType::XML_T) ||
                (m_type == DataType::JSON_T)) {
                    if (m_values.varchar != nullptr) {
                        delete [] m_values.varchar;
                    }
                    m_values.varchar = nullptr;
            } else if ((m_type == DataType::NCHAR_T) ||
                (m_type == DataType::NVARCHAR_T) ||
                (m_type == DataType::NVARCHAR_MAX_T) ||
                (m_type == DataType::NTEXT_T)) {
                    if (m_values.wvarchar != nullptr) {
                        delete [] m_values.wvarchar;
                    }
                    m_values.wvarchar = nullptr;
            } else if ((m_type == DataType::BINARY_T) ||
                       (m_type == DataType::VARBINARY_T) ||
                       (m_type == DataType::VARBINARY_MAX_T) ||
                       (m_type == DataType::IMAGE_T) ||
                       (m_type == DataType::BLOB_T)) {

                    if (m_values.varbinary != nullptr) {
                        delete [] m_values.varbinary;
                    }
                    m_values.varbinary = nullptr;
            }
        }

        bool VariableData::initialize(const DataType &dataType)
        {
            bool success = true;

            m_type = dataType;
            m_character_data = true;

            switch (m_type) {
                case DataType::CHAR_T:
                case DataType::VARCHAR_T:
                case DataType::VARCHAR_MAX_T:
                {
                    m_max_length = sc_max_text_size;
                }
                break;
                case DataType::TEXT_T:
                case DataType::IMAGE_T:
                case DataType::CLOB_T:
                case DataType::BLOB_T:
                case DataType::XML_T:
                case DataType::JSON_T:
                {
                    m_max_length = sc_max_file_size;
                }
                break;
                case DataType::BINARY_T:
                case DataType::VARBINARY_T:
                case DataType::VARBINARY_MAX_T:
                {
                    m_max_length = sc_max_binary_size;
                }
                break;
                case DataType::NCHAR_T:
                case DataType::NVARCHAR_T:
                case DataType::NVARCHAR_MAX_T:
                {
                    m_max_length = sc_max_wtext_size;
                }
                break;
                case DataType::NTEXT_T:
                {
                    m_max_length = sc_max_file_size / 2;
                }
                break;
                default:
                {
                    m_character_data = false;
                    m_max_length = 0; // we don't care for these types
                }
                break;
            }

            return success;
        }

        DataType VariableData::getType() const
        {
            return m_type;
        }

        uint64_t VariableData::getMaxLength() const
        {
            return m_max_length;
        }

        void VariableData::setMaxLength(const uint64_t &max_length)
        {
            // we will need to validate this against the type and ensure we don't blow out something
            m_max_length = max_length;
        }

        // generic which the class will attempt to convert automagically
        bool VariableData::setValue(const char *pValue, uint32_t length)
        {
            bool success = true;

            try {
                switch (m_type)
                {
                    case DataType::BIT_T:
                    {
                        int value = std::stoul(pValue);

                        // if 0 then false otherwise true
                        setValue(value != 0);
                    }
                    break;
                    case DataType::TINY_INT_T:
                    {
                        int value = std::stoul(pValue);

                        setValue((int8_t)value);
                    }
                    break;
                    case DataType::SMALL_INT_T:
                    {
                        int value = std::stoul(pValue);

                        setValue((int16_t)value);
                    }
                    break;
                    case DataType::INT_T:
                    {
                        int32_t value = std::stoul(pValue);

                        setValue(value);
                    }
                    break;
                    case DataType::BIG_INT_T:
                    case DataType::TIMESTAMP_T:
                    {
                        int64_t value = std::stoull(pValue);

                        setValue(value);
                    }
                    break;
                    case DataType::DECIMAL_T:
                    case DataType::NUMERIC_T:                    
                    {
                        float value = std::stof(pValue);

                        setValue(value);
                    }
                    break;
                    case DataType::FLOAT_T:
                    case DataType::REAL_T:                    
                    {
                        double value = std::stod(pValue);

                        setValue(value);
                    }
                    break;
                    case DataType::DATE_T: // YYYY-MM-DD
                    {
                        struct tm value = {0};
                        tools::StringTokens tokens;

                        if (tools::split_string(tokens, pValue, '-') == 3) {
                            value.tm_year = std::stoul(tokens[0]);
                            value.tm_mon = std::stoul(tokens[1]);
                            value.tm_mday = std::stoul(tokens[2]);

                            setValue(value);
                        } else {
                            success = false;
                        }
                    }
                    break;
                    case DataType::TIME_T: // HH:MM:SS
                    {
                        struct tm value = {0};
                        tools::StringTokens tokens;

                        if (tools::split_string(tokens, pValue, ':') == 3) {
                            value.tm_hour = std::stoul(tokens[0]);
                            value.tm_min = std::stoul(tokens[1]);
                            value.tm_sec = std::stoul(tokens[2]);
                            
                            setValue(value);
                        } else {
                            success = false;
                        }
                    }
                    break;
                    case DataType::DATE_TIME_T: // YYYY-MM-DD HH:MM:SS
                    {
                        struct tm value = {0};
                        tools::StringTokens tokens;

                        if (tools::split_string(tokens, pValue, ' ') == 2) {
                            tools::StringTokens date_values;
                            tools::StringTokens time_values;

                            if ((tools::split_string(date_values, tokens[0], '-') == 3) &&
                                (tools::split_string(time_values, tokens[1], ':') == 3)) {
                                value.tm_year = std::stoul(date_values[0]);
                                value.tm_mon = std::stoul(date_values[1]);
                                value.tm_mday = std::stoul(date_values[2]);

                                value.tm_hour = std::stoul(time_values[0]);
                                value.tm_min = std::stoul(time_values[1]);
                                value.tm_sec = std::stoul(time_values[2]);
                                
                                setValue(value);
                            } else {
                                success = false;
                            }
                        } else {
                            success = false;
                        }
                    }
                    break;
                    case DataType::YEAR_T: // can be 2 or 4, with 70-69 representing 1970 - 2069, 4 digit representing 1901 - 2155
                    {
                        struct tm value = {0};

                        value.tm_year = std::stoul(pValue);

                        if (strlen(pValue) == 2) {
                            if (value.tm_year <= 69) {
                                value.tm_year += 2000;
                            } else {
                                value.tm_year += 1970;
                            }
                        }

                        setValue(value);
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
                        // force cast so we don't recurse to nowhere
                        setValue(std::string(pValue));
                    }
                    break;
                    case DataType::NCHAR_T:
                    case DataType::NVARCHAR_T:
                    case DataType::NVARCHAR_MAX_T:
                    case DataType::NTEXT_T:
                    {
                        setValue(std::wstring((wchar_t *)pValue));
                    }
                    break;
                    case DataType::BINARY_T:
                    case DataType::VARBINARY_T:
                    case DataType::VARBINARY_MAX_T:
                    case DataType::IMAGE_T:
                    case DataType::BLOB_T:
                    {
                        // binary data, we will need to use the length to store it properly
                    }
                    break;
                    case DataType::EndDataTypes:
                    {
                        success = false; // we shouldn't see this...
                    }
                    break;
                }
            }
            catch (const std::invalid_argument &invalidArgument) {
                success = false;
            }
            catch (const std::out_of_range &outOfRange) {
                success = false;
            }

            return success;
        }

        std::string VariableData::getValue() const
        {
            std::stringstream value;

            switch (m_type)
            {
                case DataType::BIT_T:
                {
                    if (m_values.byte == 1) {
                        value << "true";
                    } else {
                        value << "false";
                    }
                }
                break;
                case DataType::TINY_INT_T:
                {
                    value << (int)m_values.tinyint;
                }
                break;
                case DataType::SMALL_INT_T:
                {
                    value << m_values.s_word;
                }
                break;
                case DataType::INT_T:
                {
                    value << m_values.s_doubleword;
                }
                break;
                case DataType::BIG_INT_T:
                case DataType::TIMESTAMP_T:
                {
                    value << m_values.s_quadword;
                }
                break;
                case DataType::DECIMAL_T:
                case DataType::NUMERIC_T:                    
                {
                    value << m_values.decimal;
                }
                break;
                case DataType::FLOAT_T:
                case DataType::REAL_T:                    
                {
                    value << m_values.bigdecimal;
                }
                break;
                case DataType::DATE_T: // YYYY-MM-DD
                {
                    value << m_values.time_data.tm_year << "-" << m_values.time_data.tm_mon << "-" << m_values.time_data.tm_mday;
                }
                break;
                case DataType::TIME_T: // HH:MM:SS
                {
                    value << m_values.time_data.tm_hour << ":" << m_values.time_data.tm_min << ":" << m_values.time_data.tm_sec;
                }
                break;
                case DataType::DATE_TIME_T: // YYYY-MM-DD HH:MM:SS
                {
                    value << m_values.time_data.tm_year << "-" << m_values.time_data.tm_mon << "-" << m_values.time_data.tm_mday << " ";
                    value << m_values.time_data.tm_hour << ":" << m_values.time_data.tm_min << ":" << m_values.time_data.tm_sec;
                }
                break;
                case DataType::YEAR_T: // can be 2 or 4, with 70-69 representing 1970 - 2069, 4 digit representing 1901 - 2155
                {
                    value << m_values.time_data.tm_year;
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
                    if (m_values.varchar != nullptr) {
                        value << m_values.varchar;
                    } else {
                        value << "NULL";
                    }
                }
                break;
                case DataType::NCHAR_T:
                case DataType::NVARCHAR_T:
                case DataType::NVARCHAR_MAX_T:
                case DataType::NTEXT_T:
                {
                    // will need to fix this later
                    if (m_values.wvarchar != nullptr) {
                        value << (char *)m_values.wvarchar;
                    } else {
                        value << "NULL";
                    }
                }
                break;
                case DataType::BINARY_T:
                case DataType::VARBINARY_T:
                case DataType::VARBINARY_MAX_T:
                case DataType::IMAGE_T:
                case DataType::BLOB_T:
                {
                    value << "BINARY";
                }
                break;
                case DataType::EndDataTypes:
                {
                    value << "<<< ERROR >>>";
                }
                break;
            }
            return value.str();
        }

        // Bit type
        bool VariableData::getValue(bool &value) const
        {
            bool success = false;

            if (m_type == DataType::BIT_T) {
                value = m_values.byte == 0 ? false : true;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const bool &value)
        {
            bool success = false;

            if (m_type == DataType::BIT_T) {
                uint8_t new_value = value == false ? 0 : 1;

                if (m_values.byte != new_value) {
                    m_values.byte = new_value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        // uint8_t
        bool VariableData::getValue(uint8_t &value) const
        {
            bool success = false;

            if (m_type == DataType::TINY_INT_T) {
                value = m_values.byte;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const uint8_t &value)
        {
            bool success = false;

            if (m_type == DataType::TINY_INT_T) {
                if (m_values.byte != value) {
                    m_values.byte = value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(int8_t &value) const
        {
            bool success = false;

            if (m_type == DataType::TINY_INT_T) {
                value = m_values.tinyint;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const int8_t &value)
        {
            bool success = false;

            if (m_type == DataType::TINY_INT_T) {
                if (m_values.tinyint != value) {
                    m_values.tinyint = value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(uint16_t &value) const
        {
            bool success = false;

            if (m_type == DataType::SMALL_INT_T) {
                value = m_values.word;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const uint16_t &value)
        {
            bool success = false;

            if (m_type == DataType::SMALL_INT_T) {
                if (m_values.word != value) {
                    m_values.word = value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(int16_t &value) const
        {
            bool success = false;

            if (m_type == DataType::SMALL_INT_T) {
                value = m_values.s_word;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const int16_t &value)
        {
            bool success = false;

            if (m_type == DataType::SMALL_INT_T) {
                if (m_values.s_word != value) {
                    m_values.s_word = value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(uint32_t &value) const
        {
            bool success = false;

            if (m_type == DataType::INT_T) {
                value = m_values.doubleword;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const uint32_t &value)
        {
            bool success = false;

            if (m_type == DataType::INT_T) {
                if (m_values.doubleword != value) {
                    m_values.doubleword = value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(int32_t &value) const
        {
            bool success = false;

            if (m_type == DataType::INT_T) {
                value = m_values.s_doubleword;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const int32_t &value)
        {
            bool success = false;

            if (m_type == DataType::INT_T) {
                if (m_values.s_doubleword != value) {
                    m_values.s_doubleword = value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(uint64_t &value) const
        {
            bool success = false;

            if ((m_type == DataType::BIG_INT_T) || (m_type == DataType::TIMESTAMP_T)) {
                value = m_values.quadword;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const uint64_t &value)
        {
            bool success = false;

            if ((m_type == DataType::BIG_INT_T) || (m_type == DataType::TIMESTAMP_T)) {
                if (m_values.quadword != value) {
                    m_values.quadword = value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(int64_t &value) const
        {
            bool success = false;

            if (m_type == DataType::BIG_INT_T) {
                value = m_values.s_quadword;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const int64_t &value)
        {
            bool success = false;

            if (m_type == DataType::BIG_INT_T) {
                if (m_values.s_quadword != value) {
                    m_values.s_quadword = value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(float &value) const
        {
            bool success = false;

            if ((m_type == DataType::DECIMAL_T)  || (m_type == DataType::NUMERIC_T)) {
                value = m_values.decimal;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const float &value)
        {
            bool success = false;

            if ((m_type == DataType::DECIMAL_T)  || (m_type == DataType::NUMERIC_T)) {
                if (m_values.decimal != value) {
                    m_values.decimal = value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(double &value) const
        {
            bool success = false;

            if ((m_type == DataType::FLOAT_T)  || (m_type == DataType::REAL_T)) {
                value = m_values.bigdecimal;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const double &value)
        {
            bool success = false;

            if ((m_type == DataType::FLOAT_T)  || (m_type == DataType::REAL_T)) {
                if (m_values.bigdecimal != value) {
                    m_values.bigdecimal = value;
                    m_is_dirty = true;
                }
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(struct tm &value) const
        {
            bool success = false;

            if ((m_type == DataType::DATE_T) || (m_type == DataType::DATE_TIME_T)) {
                value.tm_year = m_values.time_data.tm_year;
                value.tm_mday = m_values.time_data.tm_mday;
                value.tm_mon = m_values.time_data.tm_mon;
                success = true;
            }
            if ((m_type == DataType::TIME_T) || (m_type == DataType::DATE_TIME_T)) {
                value.tm_hour = m_values.time_data.tm_hour;
                value.tm_min = m_values.time_data.tm_min;
                value.tm_sec = m_values.time_data.tm_sec;
                success = true;
            }

            if (m_type == DataType::YEAR_T) {
                value.tm_year = m_values.time_data.tm_year;
                success = true;
            }

            return success;
        }

        bool VariableData::setValue(const struct tm &value)
        {
            bool success = false;

            if ((m_type == DataType::DATE_T) || (m_type == DataType::DATE_TIME_T)) {
                if (m_values.time_data.tm_year != value.tm_year) {
                    m_values.time_data.tm_year = value.tm_year;
                    m_is_dirty = true;
                }
                if (m_values.time_data.tm_mday != value.tm_mday) {
                    m_values.time_data.tm_mday = value.tm_mday;
                    m_is_dirty = true;
                }
                if (m_values.time_data.tm_mon != value.tm_mon) {
                    m_values.time_data.tm_mon = value.tm_mon;
                    m_is_dirty = true;
                }
                success = true;
            }
            if ((m_type == DataType::TIME_T) || (m_type == DataType::DATE_TIME_T)) {
                if (m_values.time_data.tm_hour != value.tm_hour) {
                    m_values.time_data.tm_hour = value.tm_hour;
                    m_is_dirty = true;
                }
                if (m_values.time_data.tm_min != value.tm_min) {
                    m_values.time_data.tm_min = value.tm_min;
                    m_is_dirty = true;
                }
                if (m_values.time_data.tm_sec != value.tm_sec) {
                    m_values.time_data.tm_sec = value.tm_sec;
                    m_is_dirty = true;
                }
                success = true;
            }

            if (m_type == DataType::YEAR_T) {
                m_values.time_data.tm_year = value.tm_year;
                success = true;
            }

            return success;
        }

        bool VariableData::getValue(std::string &value) const
        {
            bool success = false;

            if ((m_type == DataType::CHAR_T) ||
                (m_type == DataType::VARCHAR_T) ||
                (m_type == DataType::VARCHAR_MAX_T) ||
                (m_type == DataType::CLOB_T) ||
                (m_type == DataType::TEXT_T) ||
                (m_type == DataType::XML_T) ||
                (m_type == DataType::JSON_T)) {
                    value.clear();
                    if (m_values.varchar != nullptr) {
                        value = m_values.varchar;
                    }
                    success = true;
            }

            return success;
        }

        bool VariableData::setValue(const std::string &value)
        {
            bool success = false;

            if (value.size() <= m_max_length) {
                if ((m_type == DataType::CHAR_T) ||
                    (m_type == DataType::VARCHAR_T) ||
                    (m_type == DataType::VARCHAR_MAX_T) ||
                    (m_type == DataType::CLOB_T) ||
                    (m_type == DataType::TEXT_T) ||
                    (m_type == DataType::XML_T) ||
                    (m_type == DataType::JSON_T)) {
                    if ((m_length != value.size()) || (strcmp(m_values.varchar, value.c_str()) != 0)) {
                        m_length = value.size();
                        if (m_values.varchar != nullptr) {
                            delete [] m_values.varchar;
                        }
                        m_values.varchar = new char[m_length + 1];
                        strncpy(m_values.varchar, value.c_str(), m_length);
                        m_values.varchar[m_length] = '\0'; // ensure terminated
                        m_is_dirty = true;
                    }
                    success = true;
                }
            }

            return success;
        }

        bool VariableData::getValue(std::wstring &value) const
        {
            bool success = false;

            if ((m_type == DataType::NCHAR_T) ||
                (m_type == DataType::NVARCHAR_T) ||
                (m_type == DataType::NVARCHAR_MAX_T) ||
                (m_type == DataType::NTEXT_T)) {
                    value.clear();
                    if (m_values.varchar != nullptr) {
                        value = m_values.wvarchar;
                    }
                    success = true;
            }

            return success;
        }

        bool VariableData::setValue(const std::wstring &value)
        {
            bool success = false;

            if (value.size() <= m_max_length) {
                if ((m_type == DataType::NCHAR_T) ||
                    (m_type == DataType::NVARCHAR_T) ||
                    (m_type == DataType::NVARCHAR_MAX_T) ||
                    (m_type == DataType::NTEXT_T)) {
                    if ((m_length != value.size()) || (wcscmp(m_values.wvarchar, value.c_str()) != 0)) {
                        m_length = value.size();
                        if (m_values.wvarchar != nullptr) {
                            delete [] m_values.wvarchar;
                        }
                        m_values.wvarchar = new wchar_t[m_length + 1];
                        wcsncpy(m_values.wvarchar, value.c_str(), m_length);
                        m_values.wvarchar[m_length] = '\0'; // ensure terminated
                        m_is_dirty = true;
                    }
                    success = true;
                }
            }

            return success;
        }

        bool VariableData::getValue(BinaryBlob &value) const
        {
            bool success = false;

            if ((m_type == DataType::BINARY_T) ||
                (m_type == DataType::VARBINARY_T) ||
                (m_type == DataType::VARBINARY_MAX_T) ||
                (m_type == DataType::IMAGE_T) ||
                (m_type == DataType::BLOB_T)) {
                    value.clear();
                    if (m_values.varbinary != nullptr) {
                        value.reserve(m_length);
                        std::copy(m_values.varbinary, m_values.varbinary + m_length, std::back_inserter(value));
                    }
                    success = true;
            }

            return success;
        }

        bool VariableData::setValue(BinaryBlob &value)
        {
            bool success = false;

            if (value.size() <= m_max_length) {
                if ((m_type == DataType::BINARY_T) ||
                    (m_type == DataType::VARBINARY_T) ||
                    (m_type == DataType::VARBINARY_MAX_T) ||
                    (m_type == DataType::IMAGE_T) ||
                    (m_type == DataType::BLOB_T)) {
                    if ((m_length != value.size()) || (memcmp(m_values.varbinary, value.data(), m_length) != 0)) {
                        m_length = value.size();
                        if (m_values.varbinary != nullptr) {
                            delete [] m_values.varbinary;
                        }
                        m_values.varbinary = new uint8_t[m_length];
                        memcpy(m_values.varbinary, value.data(), m_length);
                        m_is_dirty = true;
                    }
                    success = true;
                }
            }

            return success;
        }
    }
}
