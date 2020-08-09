/**
 * MariaUtility.h
 * 
 * @brief - Maria universal utilities
 */

#ifndef _H_MARIA_UTILITY
#define _H_MARIA_UTILITY

#include <string>
#include <mariadb/mysql.h>

#include "IRow.h"

namespace afm {
    namespace database {
        bool issueCommand(MYSQL *p_db, const std::string &command, MYSQL_RES **);
        std::size_t getRows(MYSQL_RES *pResults, RowData &data, char delimiter = 0, bool include_empty = false);
    }
}
#endif