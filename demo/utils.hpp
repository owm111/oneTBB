#pragma once

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define die(str) {std::cerr << progname << ": " << str << std::endl;\
	std::exit(EXIT_FAILURE);}

extern const char *progname;
