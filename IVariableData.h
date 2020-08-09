/**
 * IVariableData.h
 * 
 * @brief - Variable data interface
 */

#ifndef _H_IVARIABLE_DATA
#define _H_IVARIABLE_DATA

#include <memory>
#include <string>
#include <vector>
#include <ctime>

namespace afm {
    namespace database {
        using BinaryBlob = std::vector<uint8_t>;

        enum class DataType
        {
            // Numerical types
            BIT_T,              // 0 -> 1
            TINY_INT_T,         // -128 -> 127 (8 bit)
            SMALL_INT_T,        // -32768 -> 32767 (16 bit)
            INT_T,              // -2,147,483,648 -> 2,147,483,647 (32 bit)
            BIG_INT_T,          // -9,223,372,036,854,775,808 -> 9,223,372,036,854,775,807 (64 bit)
            DECIMAL_T,          // -10^38 +1 -> 10^38 -1 (float)
            NUMERIC_T,          // -10^38 +1 -> 10^38 -1 (float)
            FLOAT_T,            // -1.79E + 308 -> 1.79E + 308 (double)
            REAL_T,             // -3.40E + 38 -> 3.40E + 38 (double)

            // Date / Time types
            DATE_T,             // YYYY-MM-DD
            TIME_T,             // HH:MM:SS
            DATE_TIME_T,        // YYYY-MM-DD HH:MM:SS
            TIMESTAMP_T,        // seconds since ‘1970-01-01 00:00:00’ UTC (big int 64)
            YEAR_T,             // can be 2 or 4, with 70-69 representing 1970 - 2069, 4 digit representing 1901 - 2155

            // Character and strings (non unicode)
            CHAR_T,             // Upto 8000 characters fixed length
            VARCHAR_T,          // Variable length upto 8000 characters
            VARCHAR_MAX_T,      // Variable length upto MAX characters
            TEXT_T,             // Variable length upto 2GB

            // Character and strings (unicode)
            NCHAR_T,            // Upto 4000 characters fixed length
            NVARCHAR_T,         // Variable length upto 4000 characters
            NVARCHAR_MAX_T,     // Variable length upto MAX characters
            NTEXT_T,            // Variable length upto 1GB

            // Binary data types
            BINARY_T,           // Upto 8000 characters fixed length
            VARBINARY_T,        // Variable length upto 8000 characters
            VARBINARY_MAX_T,    // Variable length upto MAX characters
            IMAGE_T,            // Variable length upto 2GB

            // Miscellanous
            CLOB_T,             // Upto 2GB of character objects
            BLOB_T,             // Binary large objects
            XML_T,              // XML data storage
            JSON_T,             // JSON data storage

            EndDataTypes
        };

        class IVariableData
        {
            public:
                virtual ~IVariableData() {}

                virtual bool initialize(const DataType &dataType) = 0;

                virtual DataType getType() const = 0;
                virtual uint64_t getMaxLength() const = 0;
                virtual void setMaxLength(const uint64_t &max_length) = 0;
                virtual bool isDirty() const = 0;
                virtual void clearDirtyFlag() = 0;
                virtual bool isCharacterData() const = 0;

                // generic which the class will attempt to convert automagically
                virtual bool setValue(const char *pValue, uint32_t length) = 0;
                virtual std::string getValue() const = 0;

                // Bit type representation
                virtual bool getValue(bool &value) const = 0;
                virtual bool setValue(const bool &value) = 0;

                // Tiny int type representation, unsigned
                virtual bool getValue(uint8_t &value) const = 0;
                virtual bool setValue(const uint8_t &value) = 0;

                // Tiny int type representation, signed
                virtual bool getValue(int8_t &value) const = 0;
                virtual bool setValue(const int8_t &value) = 0;

                // Small int type representation, unsigned
                virtual bool getValue(uint16_t &value) const = 0;
                virtual bool setValue(const uint16_t &value) = 0;

                // Small int type representation, signed
                virtual bool getValue(int16_t &value) const = 0;
                virtual bool setValue(const int16_t &value) = 0;

                // Standard int type representation, unsigned
                virtual bool getValue(uint32_t &value) const = 0;
                virtual bool setValue(const uint32_t &value) = 0;

                // Standard int type representation, signed
                virtual bool getValue(int32_t &value) const = 0;
                virtual bool setValue(const int32_t &value) = 0;

                // 64 bit int type representation, unsigned, time_t
                virtual bool getValue(uint64_t &value) const = 0;
                virtual bool setValue(const uint64_t &value) = 0;

                // 64 bit int type representation, signed
                virtual bool getValue(int64_t &value) const = 0;
                virtual bool setValue(const int64_t &value) = 0;

                // Decimal / Numeric
                virtual bool getValue(float &value) const = 0;
                virtual bool setValue(const float &value) = 0;

                // Float / Real
                virtual bool getValue(double &value) const = 0;
                virtual bool setValue(const double &value) = 0;

                // Time related
                virtual bool getValue(struct tm &value) const = 0;
                virtual bool setValue(const struct tm &value) = 0;

                // Strings
                virtual bool getValue(std::string &value) const = 0;
                virtual bool setValue(const std::string &value) = 0;

                // WStrings
                virtual bool getValue(std::wstring &value) const = 0;
                virtual bool setValue(const std::wstring &value) = 0;

                // Binary
                virtual bool getValue(BinaryBlob &value) const = 0;
                virtual bool setValue(BinaryBlob &value) = 0;
        };

        using IVariableDataSPtr = std::shared_ptr<IVariableData>;
    }
}
#endif