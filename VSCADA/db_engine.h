#ifndef DB_ENGINE_H
#define DB_ENGINE_H
#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <QString>
#include <QtSql>
#include <mutex>
#include "typedefs.h"

using namespace std;

class DB_Engine
{
public:

    // member function declarations
    DB_Engine(string dbName);
    ~DB_Engine();

    int empty_buffer();
    string quote(const string &s);
    int number_of_rows(string table);
    int max_rowid(string table);
    int runScript(string filename);
    int clear_table(string table);
    int clear_tables_except(string table);
    string sql_file_to_string(string fileName);
    int delete_item(string table, string condition, string condition_val);
    static int sql_get_rownum(void* data, int count, char** rows, char**);
    int insert_row(string table, string column, string row);
    static int sql_get_rows(void *param, int argc, char **argv, char **azColName);
    vector<string> get_conditional_values(string table, vector<string> column, string condit_col, string condition);
    vector<string> get_row_values(string table, vector<string> column, int row_num);
    vector<vector<string> > retrieve_all_items(string table, vector<string> cols);
    vector<string> search_part_word(string table, vector<string> cols, string column, string thread, int row_num);
    int update_value(string table, string column, string col_id, string id, string new_value);
    void setFile(string name);
    vector<QString> getTargetColumn(QString table, QString column, QString condit_col, QString condition);

    // global items
    string db_file;
    sqlite3 * db;
    std::mutex dbMutex;
    vector<string> dbCmds;
    int numCmds = 0;
    bool empty_DB = false;

    QSqlDatabase mydb;
    bool connected = false;
};
#endif // DB_ENGINE_H
