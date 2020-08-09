/**
 * VariableData.h
 * 
 * @brief - Variable data interface
 */

#ifndef _H_VARIABLE_DATA
#define _H_VARIABLE_DATA

#include <cstdint>
#include <ctime> 

#include "IVariableData.h"

namespace afm {
    namespace database {
        class VariableData : public IVariableData
        {
            public:
                virtual ~VariableData();

                virtual bool initialize(const DataType &dataType) override;

                virtual DataType getType() const final;
                virtual uint64_t getMaxLength() const final;
                virtual void setMaxLength(const uint64_t &max_length) final;

                virtual bool isDirty() const final { return m_is_dirty; }
                virtual void clearDirtyFlag() final { m_is_dirty = false; };
                virtual bool isCharacterData() const final { return m_character_data; }

                // generic which the class will attempt to convert automagically
                virtual bool setValue(const char *pValue, uint32_t length) override;
                virtual std::string getValue() const override;

                // Bit type representation
                virtual bool getValue(bool &value) const final;
                virtual bool setValue(const bool &value) final;

                // Tiny int type representation, unsigned
                virtual bool getValue(uint8_t &value) const final;
                virtual bool setValue(const uint8_t &value) final;

                // Tiny int type representation, signed
                virtual bool getValue(int8_t &value) const final;
                virtual bool setValue(const int8_t &value) final;

                // Small int type representation, unsigned
                virtual bool getValue(uint16_t &value) const final;
                virtual bool setValue(const uint16_t &value) final;

                // Small int type representation, signed
                virtual bool getValue(int16_t &value) const final;
                virtual bool setValue(const int16_t &value) final;

                // Standard int type representation, unsigned
                virtual bool getValue(uint32_t &value) const final;
                virtual bool setValue(const uint32_t &value) final;

                // Standard int type representation, signed
                virtual bool getValue(int32_t &value) const final;
                virtual bool setValue(const int32_t &value) final;

                // 64 bit int type representation, unsigned, time_t
                virtual bool getValue(uint64_t &value) const final;
                virtual bool setValue(const uint64_t &value) final;

                // 64 bit int type representation, signed
                virtual bool getValue(int64_t &value) const final;
                virtual bool setValue(const int64_t &value) final;

                // Decimal / Numeric
                virtual bool getValue(float &value) const final;
                virtual bool setValue(const float &value) final;

                // Float / Real
                virtual bool getValue(double &value) const final;
                virtual bool setValue(const double &value) final;

                // Time related
                virtual bool getValue(struct tm &value) const final;
                virtual bool setValue(const struct tm &value) final;

                // Strings
                virtual bool getValue(std::string &value) const final;
                virtual bool setValue(const std::string &value) final;

                // WStrings
                virtual bool getValue(std::wstring &value) const final;
                virtual bool setValue(const std::wstring &value) final;

                // Binary
                virtual bool getValue(BinaryBlob &value) const final;
                virtual bool setValue(BinaryBlob &value) final;

                static const uint32_t sc_max_text_size = 8000;
                static const uint32_t sc_max_binary_size = 8000;
                static const uint32_t sc_max_wtext_size = 4000;
                static const uint32_t sc_max_file_size = std::numeric_limits<int32_t>::max();

            private:
                DataType    m_type = DataType::EndDataTypes;
                uint64_t    m_length = 0;
                uint64_t    m_max_length = sc_max_text_size;
                bool        m_is_dirty = false;
                bool        m_character_data = false;

                union       variable_values {
                    uint8_t     byte;
                    int8_t      tinyint;
                    uint16_t    word;
                    int16_t     s_word;
                    uint32_t    doubleword;
                    int32_t     s_doubleword;
                    uint64_t    quadword;
                    int64_t     s_quadword;
                    float       decimal;
                    double      bigdecimal;
                    char        *varchar;
                    wchar_t     *wvarchar;
                    uint8_t     *varbinary;
                    struct tm   time_data = {0};
                };
                variable_values m_values;        
        };
    }
}
#endif