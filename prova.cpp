#include <string>
#include <thread>
#include <iostream>

#include <boost/process.hpp>
#include "include/epever.h"

namespace bp = boost::process;


using namespace std;
string cfgfilename = "/home/nicolas/Codice/TesiProject/cfg/fmxcku115r1_3.cfg";
string comandoUp ="profpga_run "+cfgfilename+" --up";


void run(){
    std::cout << "Thread con priorità elevata in esecuzione" << std::endl;
    // Esegui le operazioni necessarie...
    std::cout << "Inizio della parte critica" << std::endl;
    //pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);
    try {
        bp::system(comandoUp);  // Esegue il comando
    } catch (const std::exception &e) {
        std::cerr << "Errore durante l'esecuzione del comando: " << e.what() << "\n";
    }
    //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    std::cout << "Fine della parte critica" << std::endl;
        sleep(1);

    return;
}


void run2(){
    int c=0;
    while(c<40){
        std::cout << "Print a caso"<< "\n";
        // this_thread::sleep_for(chrono::seconds(1));
        c++;
    }

}

int main(){
    
    // run();
    // return 0;
    string cfgfilename = "/home/nicolas/Codice/TesiProject/cfg/fmxcku115r1.cfg";
    string comandoUp ="profpga_run "+cfgfilename+" --up";
    string comandoDown ="profpga_run "+cfgfilename+" --down";


    int exitcode = system(comandoUp.c_str());
    epever *device =new epever("/dev/ttyXRUSB0");

    run2();
    char *args[] = {"profpga_run", "/home/nicolas/Codice/TesiProject/cfg/fmxcku115r1.cfg", "--up", NULL};

    // char *args[] = {"dir", NULL};  // Comando equivalente su Windows

    // Esegue il comando, sostituendo il processo corrente
    // if (execvp(args[0], args) == -1) {
    //     std::cerr << "Errore durante l'esecuzione del comando.\n";
    // }

    // std::cout << "finito.\n";


    // this_thread::sleep_for(chrono::seconds(5));


    // cout<<"[MAIN] secondo tentativo"<<endl;

    exitcode = system(comandoUp.c_str());

    // this_thread::sleep_for(chrono::seconds(5));


    // exitcode = system(comandoDown.c_str());


    // return exitcode;
}