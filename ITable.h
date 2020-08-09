/**
 * ITable.h
 * 
 * @brief - Table representation
 */

#ifndef _H_ITABLE
#define _H_ITABLE

#include <memory>
#include <set>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "IColumn.h"
#include "IRow.h"

namespace afm {
    namespace database {
        using TableOptions = nlohmann::json;

        using QueryOptions = nlohmann::json;

        static const QueryOptions sm_emptyOptions = nlohmann::json{};

        class ITable
        {
            public:
                virtual ~ITable() {}

                virtual bool initialize(const TableOptions &options) = 0;
                virtual bool initialize(const std::string &table_name) = 0;
                virtual std::string getName() const = 0;
                virtual Columns getColumns() const = 0;
                virtual bool get(IRowSPtr &pRow, const QueryOptions &options = sm_emptyOptions) = 0;
                virtual bool set(IRowSPtr &pRow, const QueryOptions &options = sm_emptyOptions) = 0;
                virtual bool get(Rows &rows, const QueryOptions &options = sm_emptyOptions) = 0;
                virtual bool create(IRowSPtr &pRow) = 0;

                virtual IRowSPtr createEmptyRow() const = 0;
                virtual IColumnSPtr createEmptyColumn() const = 0;
                virtual std::string getColumnNames() const = 0;
        };

        using ITableSPtr = std::shared_ptr<ITable>;
        using Tables = std::set<ITableSPtr>;
    }
}
#endif