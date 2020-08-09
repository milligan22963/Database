/**
 * PgSqlUtility.cpp
 */

#include "pgsql/PgSqlUtility.h"

namespace afm {
    namespace database {
        bool issueCommand(pqxx::connection *pConnection, const std::string &command, pqxx::result &result)
        {
            bool success = true;

            try {
                pqxx::work work_unit(*pConnection);

                result = work_unit.exec(command.c_str());
                work_unit.commit();
            }
            catch (const std::exception &db_error) {
                success = false;
            }
            return success;
        }
    }
}