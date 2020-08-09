/**
 * PgSqlUtility.h
 * 
 * @brief - PgSQL universal utilities
 */

#ifndef _H_PGSQL_UTILITY
#define _H_PGSQL_UTILITY

#include <string>

#include <pqxx/pqxx>
#include <pqxx/result.hxx>

namespace afm {
    namespace database {
        bool issueCommand(pqxx::connection *pConnection, const std::string &command, pqxx::result &result);
    }
}
#endif