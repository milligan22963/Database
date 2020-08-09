/**
 * MariaUtility.cpp
 */

#include <sstream>
#include "maria/MariaUtility.h"

namespace afm {
    namespace database {
        bool issueCommand(MYSQL *p_db, const std::string &command, MYSQL_RES **ppResults)
        {
            bool success = false;

            if (mysql_real_query(p_db, command.c_str(), command.size()) == 0) {
                // get the results
                *ppResults = mysql_store_result(p_db);
                success = true;
            }

            return success;
        }

        std::size_t getRows(MYSQL_RES *pResults, RowData &data, char delimiter, bool include_empty)
        {
            data.clear();

            if (pResults != nullptr) {
                uint32_t field_count = mysql_num_fields(pResults);
                MYSQL_ROW row;
                char *p_buffer = new char[128];
                uint32_t buffer_size = 128;
                std::stringstream row_stream;

                while ((row = mysql_fetch_row(pResults)) != nullptr) {
                    long unsigned int *pLengths = mysql_fetch_lengths(pResults);
                    for (uint32_t index = 0; index < field_count; index++) {
                        if ((row[index] != nullptr) && (pLengths[index] > 0)) {
                            if (buffer_size < pLengths[index]) {
                                delete [] p_buffer;
                                buffer_size = pLengths[index] + 1;
                                p_buffer = new char[buffer_size];
                            }
                            memcpy(p_buffer, row[index], pLengths[index]);
                            p_buffer[pLengths[index]] = '\0'; // ensure null terminated

                            row_stream << p_buffer;
                        } else if (include_empty == true) {
                            row_stream << " ";
                        }
                        if (delimiter != 0) {
                            row_stream << delimiter;
                        }
                    }
                    data.push_back(row_stream.str());
                    row_stream.str(""); // clear it
                }
                delete [] p_buffer;
            }
            return data.size();
        }
    }
}