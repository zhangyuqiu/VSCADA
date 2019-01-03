#include "db_engine.h"

DB_Engine::DB_Engine()
{
//    rc = sqlite3_open(db_file.c_str(), &db);
}

DB_Engine::~DB_Engine()
{
//    sqlite3_close(db);
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
    //char * zErrMsg;
    //incatenate vector elements into a string
    stringstream col_buf;
    for (uint i = 0; i < column.size(); i++){
        col_buf << column.at(i);
        if(i != column.size()-1) col_buf << ", ";
    }
    QCoreApplication::processEvents();
    //incatenate vector elements into a string
    stringstream row_buf;
    for (uint i = 0; i < row.size(); i++){
        row_buf << quote(row.at(i));
        if(i != row.size()-1) row_buf << ", ";
    }
    //run SQLite command
    string sql = "INSERT INTO "+table+"(" + col_buf.str() + ")" +
            " VALUES " + "(" + row_buf.str() + ");";
#ifdef DEBUG
    cout << endl;
    cout << sql << endl;
    cout << endl;
#endif
    QCoreApplication::processEvents();
    dbMutex.lock();
    int rc = sqlite3_open(db_file.c_str(), &db);
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    sqlite3_close(db);
    dbMutex.unlock();
    QCoreApplication::processEvents();
    if (rc != SQLITE_OK){
//        cout << "Row wasn't inserted" << endl;
        return 0;
    } else {
//        cout << "Row inserted successfully" << endl;
        return 1;
    }
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
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

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
}

int DB_Engine::number_of_rows(string table){
    sqlite3_open(db_file.c_str(), &db);
    //char * zErrMsg;
    int count = 0;

    fflush(stdout);
    string sql = "SELECT Count FROM "+table;
#ifdef DEBUG
    cout << sql << endl;
#endif
    sqlite3_exec(db, sql.c_str(), sql_get_rownum, &count, nullptr);
    //sqlite3_free(zErrMsg);
    //sqlite3_db_release_memory(db);
    sqlite3_close(db);
    return count;
}

int DB_Engine::max_rowid(string table){
    sqlite3_open(db_file.c_str(), &db);
    //char * zErrMsg;
    int count = 0;

    string sql = "SELECT marowid) FROM "+table;
#ifdef DEBUG
    cout << sql << endl;
#endif
    sqlite3_exec(db, sql.c_str(), sql_get_rownum, &count, nullptr);
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
    inFile.close();
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
    for (uint i = 0; i < column.size(); i++){
        col_buf << column.at(i);
        if(i != column.size()-1) col_buf << ", ";
    }

    //run SQLite command
    sql = "SELECT "+col_buf.str()+" FROM "+table+" WHERE "+condit_col+" = '"+condition+"';";
#ifdef DEBUG
    cout << sql << endl;
#endif
    rc = sqlite3_exec(db, sql.c_str(), sql_get_rows, &holder, nullptr);
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
    for (uint i = 0; i < holder.size(); i++){
        buf << holder.at(i);
        if(i != holder.size()-1) buf << ", ";
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
    vector<string> *xyz = static_cast<vector<string> *>(param);
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
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    sqlite3_close(db);
    return 1;
}

vector<string> DB_Engine::get_row_values(string table, vector<string> column, int row_num){
    int rc = sqlite3_open(db_file.c_str(), &db);
    //char * zErrMsg;
    vector<string> holder;
    string sql;
    stringstream col_buf;
    for (uint i = 0; i < column.size(); i++){
        col_buf << column.at(i);
        if(i != column.size()-1) col_buf << ", ";
    }

    //run SQLite command
    sql = "SELECT "+col_buf.str()+" FROM "+table+" WHERE rowid ="+to_string(row_num);
#ifdef DEBUG
    cout << sql << endl;
#endif
    rc = sqlite3_exec(db, sql.c_str(), sql_get_rows, &holder, nullptr);
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
    for (uint i = 0; i < holder.size(); i++){
        buf << holder.at(i);
        if(i != holder.size()-1) buf << ", ";
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
        if (tmp.size() > 0){
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
    for (uint i = 0; i < cols.size(); i++){
        col_buf << cols.at(i);
        if(i != cols.size()-1) col_buf << ", ";
    }

    //run SQLite command
    sql = "SELECT " + col_buf.str() + " FROM "+table+" WHERE rowid = " + to_string(row_num) + " AND " + column + " LIKE \"%" + thread + "%\";";
//#ifdef DEBUG
    cout << sql << endl;
//#endif
    rc = sqlite3_exec(db, sql.c_str(), sql_get_rows, &holder, nullptr);
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
    for (uint i = 0; i < holder.size(); i++){
        buf << holder.at(i);
        if(i != holder.size()-1) buf << ", ";
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
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
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
}

int DB_Engine::clear_table(string table){
    sqlite3_open(db_file.c_str(), &db);
    string sql = "delete from " + table + ";";
    cout << sql << endl;
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    sqlite3_close(db);
    return 1;
}

void DB_Engine::setFile(string name){
    this->db_file=name;
}

vector<QString> DB_Engine::getTargetColumn(QString currentTable, QString read, QString target, QString name){
    QString currentBase = QString::fromStdString(db_file);
    QSqlDatabase mydb;
    mydb=QSqlDatabase::addDatabase("QSQLITE");
    if(0== currentBase.compare("./system.db")){
          mydb.setDatabaseName(currentBase);//path of data base
    }else{
        mydb.setDatabaseName("../VSCADA/savedsessions/"+currentBase);//path of data base
}
    mydb.open();

    QSqlQuery* qry = new QSqlQuery(mydb);
    vector<QString> data;
    if(0== target.compare(" ")){

        QString selectName="SELECT "+read+" FROM "+currentTable;
        qry->prepare(selectName);
        qry->exec();
        while(qry->next()){
            QString num =qry->value(0).toString();
            data.push_back(num);
        }
    } else {
        QString selectName="SELECT "+read+" FROM "+currentTable+" WHERE "+target+"='"+name+"'";
        qry->prepare(selectName);
        qry->exec();
        while(qry->next()){
            QString num =qry->value(0).toString();
            data.push_back(num);
        }
    }
    mydb.close();
    delete qry;
    return data;
}
