/**
 * SQLiteUtility.h
 * 
 * @brief - SQLite universal utilities
 */

#ifndef _H_SQLITE_UTILITY
#define _H_SQLITE_UTILITY

#include <string>

#include <sqlite3.h>

namespace afm {
    namespace database {
        using SQLiteCallbackFunction = int (*)(void*,int,char**,char**);

        bool issueCommand(sqlite3 *p_db, const std::string &command);
        bool issueCommand(sqlite3 *p_db, const std::string &command, SQLiteCallbackFunction callback, void *p_data);
    }
}
#endif