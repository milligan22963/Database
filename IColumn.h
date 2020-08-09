/**
 * IColumn.h
 * 
 * @brief - Table Column representation
 */

#ifndef _H_ICOLUMN
#define _H_ICOLUMN

#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>

#include "IVariableData.h"

namespace afm {
    namespace database {
        using ColumnOptions = nlohmann::json;

        class IColumn
        {
            public:
                virtual ~IColumn() {}

                virtual bool initialize(const ColumnOptions &options) = 0;
                virtual bool initialize(const std::string &details) = 0;
                virtual std::string getName() const = 0;
                virtual DataType getType() const = 0;
                virtual std::string getTypeName() const = 0;
                virtual uint64_t getMaxLength() const = 0;
                virtual bool isPrimary() const = 0;
                virtual bool isKey() const = 0;
                virtual bool isAutoIncrement() const = 0;
                virtual bool isDirty() const = 0;
                virtual bool isUnique() const = 0;
                virtual void clearDirtyFlag() = 0;
                virtual bool canBeNull() const = 0;
                virtual uint8_t getPrecision() const = 0;

                virtual IVariableDataSPtr getValue() const = 0;
                virtual void setValue(IVariableDataSPtr &pValue) = 0;
                virtual bool setValue(const char *pValue, uint32_t length) = 0;
                virtual bool setValue(const std::string &value) = 0;

                virtual DataType test_type(const std::string &type) = 0;
        };

        using IColumnSPtr = std::shared_ptr<IColumn>;
        using Columns = std::vector<IColumnSPtr>;
    }
}
#endif