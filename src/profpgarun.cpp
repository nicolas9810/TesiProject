#include <string>

using namespace std;

/*
Potrei crearmi una lista contenente il path di tutti i config così è più semplice e immediato eseguirli
*/
int profpga_run_up(string cfgfile){
    string comandoUp ="profpga_run "+cfgfile+" --up";

    int exitcode = system(comandoUp.c_str());
    
    return exitcode;
}


int profpga_run_down(string cfgfile){
    string comandoUp ="profpga_run "+cfgfile+" --down";

    int exitcode = system(comandoUp.c_str());
    
    return exitcode;
}