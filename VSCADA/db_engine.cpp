#include "db_engine.h"

DB_Engine::DB_Engine()
{

}

/**
 * @brief InitializeDatabase::insert_row This method inserts a new row in the specified
 *  table, and with the specified values. The row values must be in the order of the
 *  columns intended
 * @param table Name of the target table
 * @param column Vector of names of target columns
 * @param row Vector of names of target rows
 * @return
 */
int DB_Engine::insert_row(string table, vector<string> column, vector<string> row){
    int rc = sqlite3_open(db_file.c_str(), &db);
    //char * zErrMsg;
    //incatenate vector elements into a string
    stringstream col_buf;
    for (int i = 0; i < (int)column.size(); i++){
        col_buf << column.at(i);
        if(i != (int)column.size()-1) col_buf << ", ";
    }

    //incatenate vector elements into a string
    stringstream row_buf;
    for (int i = 0; i < (int)row.size(); i++){
        row_buf << quote(row.at(i));
        if(i != (int)row.size()-1) row_buf << ", ";
    }
    //run SQLite command
    string sql = "INSERT INTO "+table+"(" + col_buf.str() + ")" +
            " VALUES " + "(" + row_buf.str() + ");";
#ifdef DEBUG
    cout << endl;
    cout << sql << endl;
    cout << endl;
#endif
    rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL);
    if (rc != SQLITE_OK){
        sqlite3_close(db);
#ifdef DEBUG
        cout << "Row wasn't inserted" << endl;
#endif
        return 0;
    } else {
#ifdef DEBUG
        cout << "Row inserted successfully" << endl;
#endif
        sqlite3_close(db);
        return 1;
    }
    sqlite3_close(db);
    return 0;
}

string DB_Engine::quote(const string &s){
    return string("'") + s + string("'");
}

int DB_Engine::runScript(string filename){
    int rc = sqlite3_open(db_file.c_str(), &db);
    string sql = sql_file_to_string(filename);
    //char * zErrMsg;

    //incatenate vector elements into a string
#ifdef DEBUG
    cout << "----Start of Script----" << endl;
    cout << sql << endl;
    cout << "----End of Script----" << endl;
#endif
    rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL);

    if (rc != SQLITE_OK){
        //sqlite3_db_release_memory(db);
        sqlite3_close(db);
#ifdef DEBUG
        //        cout << "script wasn't run" << endl;
        //cout << zErrMsg << endl;
#endif
        return 0;
    } else {
        //sqlite3_db_release_memory(db);
        sqlite3_close(db);
#ifdef DEBUG
        cout << "script ran successfully" << endl;
#endif
        return 1;
    }
    //sqlite3_free(zErrMsg);
    //sqlite3_db_release_memory(db);
    sqlite3_close(db);
    return 0;
}

int DB_Engine::number_of_rows(string table){
    sqlite3_open(db_file.c_str(), &db);
    //char * zErrMsg;
    int count = 0;

    fflush(stdout);
    string sql = "SELECT Count(*) FROM "+table;
#ifdef DEBUG
    cout << sql << endl;
#endif
    sqlite3_exec(db, sql.c_str(), sql_get_rownum, &count, NULL);
    //sqlite3_free(zErrMsg);
    //sqlite3_db_release_memory(db);
    sqlite3_close(db);
    return count;
}

int DB_Engine::max_rowid(string table){
    sqlite3_open(db_file.c_str(), &db);
    //char * zErrMsg;
    int count = 0;

    string sql = "SELECT max(rowid) FROM "+table;
#ifdef DEBUG
    cout << sql << endl;
#endif
    sqlite3_exec(db, sql.c_str(), sql_get_rownum, &count, NULL);
    //sqlite3_free(zErrMsg);
    //sqlite3_db_release_memory(db);
    sqlite3_close(db);
    return count;
}

string DB_Engine::sql_file_to_string(string fileName){
    ifstream inFile;
    string temp;
    stringstream buf;
    inFile.open(fileName);
    while(std::getline(inFile, temp)) {
        //Do with temp
        if (temp.size() > 0) {
            if (temp.at(0) == '-') continue;
            buf << temp;
        }
        //cout << temp << endl;
    }
    //cout << buf.str() << endl;
    return buf.str();
}

int DB_Engine::sql_get_rownum(void* data, int count, char** rows, char**){
    if (count == 1 && rows) {
        *static_cast<int*>(data) = atoi(rows[0]);
        return count;
    }
    return count;
}

vector<string> DB_Engine::get_conditional_values(string table, vector<string> column, string condit_col, string condition){
    int rc = sqlite3_open(db_file.c_str(), &db);
    //char * zErrMsg;
    vector<string> holder;
    string sql;
    stringstream col_buf;
    for (int i = 0; i < (int)column.size(); i++){
        col_buf << column.at(i);
        if(i != (int)column.size()-1) col_buf << ", ";
    }

    //run SQLite command
    sql = "SELECT "+col_buf.str()+" FROM "+table+" WHERE "+condit_col+" = '"+condition+"';";
#ifdef DEBUG
    cout << sql << endl;
#endif
    rc = sqlite3_exec(db, sql.c_str(), sql_get_rows, &holder, NULL);
    if (rc != SQLITE_OK){
        //sqlite3_db_release_memory(db);
        sqlite3_close(db);
#ifdef DEBUG
        //cout << zErrMsg << endl;

    } else {
        cout << "Row(s) retrieved successfully" << endl;

#endif
    }
    //incatenate vector elements into a string
    stringstream buf;
    for (int i = 0; i < (int)holder.size(); i++){
        buf << holder.at(i);
        if(i != (int)holder.size()-1) buf << ", ";
    }
    string myString = "Retrieved Rows : " +buf.str();
//#ifdef DEBUG
    cout << endl;
    cout << myString.c_str() << endl;
//#endif
    //sqlite3_db_release_memory(db);
    sqlite3_close(db);
    return holder;
}

int DB_Engine::sql_get_rows(void *param, int argc, char **argv, char **azColName){
    vector<string> *xyz = (vector<string> *)param ;
    //store returned data
    for(int i = 0; i < argc; i++){
        xyz->push_back(argv[i]);
    }
    azColName[0][0] = '0';
    return 1;
}

int DB_Engine::delete_item(string table, string condition, string condition_val){
    sqlite3_open(db_file.c_str(), &db);
    string sql = "delete from " + table + " where " + condition + " = '" + condition_val + "';";
    cout << sql << endl;
    sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL);
    sqlite3_close(db);
    return 1;
}

vector<string> DB_Engine::get_row_values(string table, vector<string> column, int row_num){
    int rc = sqlite3_open(db_file.c_str(), &db);
    //char * zErrMsg;
    vector<string> holder;
    string sql;
    stringstream col_buf;
    for (int i = 0; i < (int)column.size(); i++){
        col_buf << column.at(i);
        if(i != (int)column.size()-1) col_buf << ", ";
    }

    //run SQLite command
    sql = "SELECT "+col_buf.str()+" FROM "+table+" WHERE rowid ="+to_string(row_num);
#ifdef DEBUG
    cout << sql << endl;
#endif
    rc = sqlite3_exec(db, sql.c_str(), sql_get_rows, &holder, NULL);
//#ifdef DEBUG
    if (rc != SQLITE_OK){
        //cout << zErrMsg << endl;
        //sqlite3_db_release_memory(db);
        sqlite3_close(db);
    } else {
        //cout << "Row(s) retrieved successfully" << endl;
    }
//#endif
    //incatenate vector elements into a string
    stringstream buf;
    for (int i = 0; i < (int)holder.size(); i++){
        buf << holder.at(i);
        if(i != (int)holder.size()-1) buf << ", ";
    }

    string myString = "Retrieved Rows : " +buf.str();
#ifdef DEBUG
    cout << endl;
    cout << myString.c_str() << endl;
#endif
    sqlite3_close(db);
    return holder;
}

vector<vector<string>> DB_Engine::retrieve_all_items(string table, vector<string> cols){
    vector<string> tmp;
    vector<vector<string>> items;
    sqlite3_open(db_file.c_str(), &db);
    int count = max_rowid("stock");
    for (int i = 1; i <= count; i++){
        tmp = get_row_values(table,cols,i);
        if ((int)tmp.size() > 0){
            items.push_back(tmp);
        }
    }
    sqlite3_close(db);
    return items;
}

vector<string> DB_Engine::search_part_word(string table, vector<string> cols, string column, string thread, int row_num){
    int rc = sqlite3_open(db_file.c_str(), &db);
    vector<string> holder;
    string sql;
    stringstream col_buf;
    for (int i = 0; i < (int)cols.size(); i++){
        col_buf << cols.at(i);
        if(i != (int)cols.size()-1) col_buf << ", ";
    }

    //run SQLite command
    sql = "SELECT " + col_buf.str() + " FROM "+table+" WHERE rowid = " + to_string(row_num) + " AND " + column + " LIKE \"%" + thread + "%\";";
//#ifdef DEBUG
    cout << sql << endl;
//#endif
    rc = sqlite3_exec(db, sql.c_str(), sql_get_rows, &holder, NULL);
//#ifdef DEBUG
    if (rc != SQLITE_OK){
        //cout << zErrMsg << endl;
        //sqlite3_db_release_memory(db);
        sqlite3_close(db);
    } else {
        //cout << "Row(s) retrieved successfully" << endl;
    }
//#endif
    //incatenate vector elements into a string
    stringstream buf;
    for (int i = 0; i < (int)holder.size(); i++){
        buf << holder.at(i);
        if(i != (int)holder.size()-1) buf << ", ";
    }

    string myString = "Retrieved Rows : " +buf.str();
#ifdef DEBUG
    cout << endl;
    cout << myString.c_str() << endl;
#endif
    sqlite3_close(db);
    return holder;
}

int DB_Engine::update_value(string table, string column, string col_id, string id, string new_value){
    sqlite3_open(db_file.c_str(), &db);
    //char * zErrMsg;
    //run SQLite command
    string sql = "UPDATE " + table + " SET " + column + " = '" + new_value +
            "' WHERE " + col_id + " = '" + id + "';";
#ifdef DEBUG
    cout << endl;
    cout << sql << endl;
    cout << endl;
#endif
    int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL);
    if (rc != SQLITE_OK){
        //sqlite3_db_release_memory(db);
        sqlite3_close(db);
#ifdef DEBUG
        cout << "Row wasn't inserted" << endl;
        //cout << zErrMsg << endl;
#endif
        return 0;
    } else {
        //sqlite3_db_release_memory(db);
        sqlite3_close(db);
#ifdef DEBUG
        cout << "Row inserted successfully" << endl;
#endif
        return 1;
    }
    sqlite3_close(db);
    return 0;
}

int DB_Engine::clear_table(string table){
    sqlite3_open(db_file.c_str(), &db);
    string sql = "delete from " + table + ";";
    cout << sql << endl;
    sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL);
    sqlite3_close(db);
    return 1;
}
