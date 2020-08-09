/**
 * Row.cpp
 */

#include <iostream>
#include <sstream>

#include "Row.h"

namespace afm {
    namespace database {

        Row::~Row()
        {
            m_columns.clear();
        }

        bool Row::initialize()
        {
            bool success = true;

            return success;
        }

        bool Row::isDirty() const
        {
            bool is_dirty = false;

            for (auto column : m_columns) {
                if (column->isDirty() == true) {
                    is_dirty = true;
                    break;
                }
            }

            return is_dirty;
        }

        void Row::clearDirtyFlag()
        {
            for (auto column : m_columns) {
                column->clearDirtyFlag();
            }
        }

        IColumnSPtr Row::getColumn(const std::string &columnName) const
        {
            IColumnSPtr pColumn = nullptr;

            for (auto column : m_columns) {
                if (column->getName() == columnName) {
                    pColumn = column;
                    break;
                }
            }
            return pColumn;
        }

        bool Row::setValue(const std::string &columnName, const char *pValue, uint32_t length)
        {
            bool success = false;

            for (auto column : m_columns) {
                if (column->getName() == columnName) {
                    success = column->setValue(pValue, length);
                    break;
                }
            }

            return success;
        }

        bool Row::setValue(int index, const char *pValue, uint32_t length)
        {
            bool success = false;

            if (index < (int)m_columns.size()) {
                m_columns[index]->setValue(pValue, length);
                success = true;
            }

            return success;
        }

        bool Row::setValues(const RowData &rowData)
        {
            bool success = false;

            if (rowData.size() <= m_columns.size()) {
                int index = 0;
                for (auto row : rowData) {
                    m_columns[index]->setValue(row);
                    m_columns[index++]->clearDirtyFlag();
                }
                success = true;
            }

            return success;
        }

        std::string Row::toString() const
        {
            std::stringstream row;

            for (uint32_t index = 0; index < m_columns.size(); index++) {
                IVariableDataSPtr pValue = m_columns[index]->getValue();
                row << "[" << pValue->getValue() << "]";
                if (index < m_columns.size() - 1) {
                    row << " ";
                }
            }
            row << "\n";
            return row.str();
        }
    }
}