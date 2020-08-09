/**
 * Row.h
 * 
 * @brief - Table row representation
 */

#ifndef _H_ROW
#define _H_ROW

#include "IRow.h"

namespace afm {
    namespace database {
        class Row : public IRow
        {
            public:
                virtual ~Row();

                virtual bool initialize() override;
                virtual bool isDirty() const override;
                virtual void clearDirtyFlag() final;
                virtual IColumnSPtr getColumn(const std::string &columnName) const override;
                virtual bool setValue(const std::string &columnName, const char *pValue, uint32_t length = 0) override;
                virtual bool setValue(int index, const char *pValue, uint32_t length = 0) override;
                virtual bool setValues(const RowData &rowData) override;
                virtual void addColumn(const IColumnSPtr &pColumn) { m_columns.push_back(pColumn); }
                virtual const Columns &getColumns() const final { return m_columns; };
                virtual std::string toString() const override;

            private:
                std::vector<IColumnSPtr> m_columns;
        };
    }
}
#endif