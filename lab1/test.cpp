#include "daemon.h"
#include <iostream>
#include <fstream>

int main() {
    Daemon& my_daemon = Daemon::get_instance();
    my_daemon.daemon_launch();    
}