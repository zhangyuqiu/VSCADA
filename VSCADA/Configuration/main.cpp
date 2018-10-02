#include <iostream>
#include "conf.h"

using namespace std;

int main()
{
    conf * config = new conf();
    config->read_config_file_data("../config_test.txt");
    return 0;
}
