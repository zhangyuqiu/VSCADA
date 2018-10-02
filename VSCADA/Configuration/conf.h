#ifndef CONF_H
#define CONF_H
#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iterator>
#include "typedefs.h"
#define CONFIG_PRINTOUTS

using namespace std;

class conf
{
public:
    conf();
    ~conf();
    bool read_config_file_data(string configFile);
    vector<string> csv_split(string str);
    vector<string> split(string s, string delim);
    vector<meta> s;
};

#endif // CONF_H
