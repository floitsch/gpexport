#include "gplog.h"

std::ofstream _GPLogFile;

void init_gp_logs(const std::string &logfile) 
{
	_GPLogFile.open(logfile.c_str());
}

void close_gp_logs()
{
	_GPLogFile.close();
}