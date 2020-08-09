/**
 * Table.h
 * 
 * @brief - Table representation
 */

#ifndef _H_TABLE
#define _H_TABLE


#include "ITable.h"
#include "Column.h"

namespace afm {
    namespace database {
        class Table : public ITable
        {
            public:
                virtual ~Table();

                virtual bool initialize(const TableOptions &options) override;
                virtual bool initialize(const std::string &table_name) override;
                virtual std::string getName() const final { return m_table_name; }
                virtual Columns getColumns() const final { return m_columns; }
                virtual bool get(IRowSPtr &pRow, const QueryOptions &options = sm_emptyOptions) override;
                virtual bool set(IRowSPtr &pRow, const QueryOptions &options = sm_emptyOptions) final;
                virtual bool get(Rows &rows, const QueryOptions &options = sm_emptyOptions) override;
                virtual bool create(IRowSPtr &pRow) final;

                virtual std::string getColumnNames() const override;
                virtual IRowSPtr createEmptyRow() const;
                virtual IColumnSPtr createEmptyColumn() const = 0;

            protected:
                void add_column(IColumnSPtr pColumn) { m_columns.push_back(pColumn); }
                const Columns &get_columns() const { return m_columns; }
                virtual void process_table_options(std::stringstream &output, const QueryOptions &options) const;
                virtual bool on_create_row(const std::string &query) = 0;
                virtual bool on_update_row(const std::string &query) = 0;
                virtual bool on_get_row(IRowSPtr &pRow, const std::string &query) = 0;
                virtual bool on_get_rows(Rows &rows, const std::string &query) = 0;
                virtual IColumnSPtr on_clone_column(const Column &column) const = 0;
                virtual std::string build_select(const QueryOptions &options);
                virtual std::string build_insert(const IRowSPtr &pRow) const;
                virtual std::string build_update(const IRowSPtr &pRow, const QueryOptions &options) const;

            private:
                std::string m_table_name;
                Columns     m_columns;
        };
    }
}
#endif