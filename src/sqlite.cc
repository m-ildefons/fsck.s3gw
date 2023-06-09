/*
 * Copyright 2023 SUSE, LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sqlite.h"

#include <sqlite3.h>

#include <filesystem>
#include <iostream>

Database::Database(std::filesystem::path dbpath) {
  db = dbpath;

  int rc = sqlite3_open(db.string().c_str(), &handle);
  if (rc != SQLITE_OK) {
    std::cout << "failed to open db" << std::endl;
    sqlite3_close(handle);
  }
}

Database::~Database() {
  sqlite3_close(handle);
}

int Database::prepare(std::string query, sqlite3_stmt** stm) {
  int rc = 0;
  const char* unused = 0;
  rc = sqlite3_prepare(handle, query.c_str(), query.length(), stm, &unused);
  if (rc != SQLITE_OK) {
    std::cout << "error while prepare: " << rc << std::endl;
    std::cout << query << std::endl;
  }
  return rc;
}

/* Count in Table - Count number of rows in named table where the condition
 * is true. Translates directly into:
 *
 *   SELECT COUNT(*) FROM table WHERE condition ;
 */
int Database::count_in_table(std::string table, std::string condition) {
  std::string query =
      "SELECT COUNT(*) FROM " + table + " WHERE " + condition + ";";
  int count = 0;
  int rc = 0;
  sqlite3_stmt* stm;
  const char* unused = 0;
  rc = sqlite3_prepare(handle, query.c_str(), query.length(), &stm, &unused);
  if (rc != SQLITE_OK) {
    std::cout << "error while prepare: " << rc << std::endl;
    std::cout << query << std::endl;
    return 0;
  }
  rc = sqlite3_step(stm);
  if (rc == SQLITE_ROW && sqlite3_column_count(stm) > 0) {
    count = sqlite3_column_int(stm, 0);
  }
  sqlite3_finalize(stm);
  return count;
}

/* Select from Table - Get all non-null entries of one column from a table.
 * Translates into:
 *
 *   SELECT column FROM table WHERE column IS NOT NULL ;
 */
std::vector<std::string> Database::select_from_table(
    std::string table, std::string column
) {
  std::string query = "SELECT " + column + " FROM " + table + " WHERE " +
                      column + " IS NOT NULL;";
  int rc = 0;
  sqlite3_stmt* stm;
  const char* unused = 0;
  std::vector<std::string> result;
  rc = sqlite3_prepare(handle, query.c_str(), query.length(), &stm, &unused);
  if (rc != SQLITE_OK) {
    std::cout << "error while prepare: " << rc << std::endl;
    std::cout << query << std::endl;
    return result;
  }
  do {
    rc = sqlite3_step(stm);

  } while (rc == SQLITE_ROW);
  sqlite3_finalize(stm);
  return result;
}
