#ifndef _GPEXPORT_LOG_
#define _GPEXPORT_LOG_


#include <fstream>
#include <string>
#include <iostream>

extern std::ofstream _GPLogFile;
//
#ifdef _DEBUG 
	#define _GPLOG(log)  _GPLogFile << log << std::endl; \
						std::cout << log << std::endl;
	#define _GPDEBUG(log)	_GPLogFile << log << std::endl; \
							std::cout << log << std::endl;
#else 
	#define _GPLOG(log)  std::cout << log << std::endl;
	#define _GPDEBUG(log) 
#endif
//
void init_gp_logs(const std::string &logfile);
void close_gp_logs();


#endif