#include <iostream>

using namespace std;
#include "config.h"

int main(){
    // are we running the car or the dyno?
    // preferrably same routines with different configuration files
    // DataControl * ctrl = new DataControl();
    Config * conf = new Config();
    conf->read_config_file_data("./config_test.txt");
    conf->dataCtrl->collectData();
    return 0;
}
