/**
 * IRow.h
 * 
 * @brief - Table row representation
 */

#ifndef _H_IROW
#define _H_IROW

#include <memory>
#include <string>
#include <vector>

#include "IColumn.h"

namespace afm {
    namespace database {
        using RowData = std::vector<std::string>;

        class IRow
        {
            public:
                virtual ~IRow() {}

                virtual bool initialize() = 0;
                virtual bool isDirty() const = 0;
                virtual void clearDirtyFlag() = 0;
                virtual IColumnSPtr getColumn(const std::string &columnName) const = 0;
                virtual bool setValue(const std::string &columnName, const char *pValue, uint32_t length = 0) = 0;
                virtual bool setValue(int index, const char *pValue, uint32_t length = 0) = 0;
                virtual bool setValues(const RowData &rowData) = 0;
                virtual void addColumn(const IColumnSPtr &pColumn) = 0;
                virtual const Columns &getColumns() const = 0;
                virtual std::string toString() const = 0;
        };

        using IRowSPtr = std::shared_ptr<IRow>;
        using Rows = std::vector<IRowSPtr>;
    }
}
#endif