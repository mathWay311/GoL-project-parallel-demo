#pragma once
#include <iostream>
#include <cstring>

namespace utility_methods {

	void process_arguments(int argc, char const *argv[], unsigned int &num_cells_x, unsigned int &num_cells_y);
	void config_printout(unsigned int num_cells_x, unsigned int num_cells_y);
	void single_var_printout(std::string name, double var);

};