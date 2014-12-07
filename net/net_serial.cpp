#include "net_serial.h"

void net_serial_t::blank(){}
void net_serial_t::init(int argc, char** argv){printf("%d%s",argc,argv[0]);}
void net_serial_t::loop(){}
void net_serial_t::close(){}
std::string  net_serial_t::read(){return (char*)"";}
void net_serial_t::write(std::string a){/*printf("%s",a.c_str());*/}
