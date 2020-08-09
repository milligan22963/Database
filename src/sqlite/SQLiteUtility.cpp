/**
 * SQLiteUtility.cpp
 */

#include "sqlite/SQLiteUtility.h"


namespace afm {
    namespace database {
        bool issueCommand(sqlite3 *p_db, const std::string &command)
        {
            return issueCommand(p_db, command, nullptr, nullptr);
        }

        bool issueCommand(sqlite3 *p_db, const std::string &command, SQLiteCallbackFunction callback, void *p_data)
        {
            bool success = true;
            char *p_err_message = nullptr;

            if (sqlite3_exec(p_db, command.c_str(), callback, p_data, &p_err_message) != SQLITE_OK) {
                success = false;
                if (p_err_message != nullptr) {
                    // TODO: add logging of error messages
                    sqlite3_free(p_err_message);
                }
            }
            return success;
        }
    }
}
