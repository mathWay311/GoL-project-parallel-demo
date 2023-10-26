#include <iostream>
#include <vector>
#include "../header/utility.h"
#include "../header/game_of_life.h"
#include "../glfw-3.3.8/include/GLFW/glfw3.h"

using namespace utility_methods;
using namespace game_of_life;

unsigned int num_cells_x = 10;
unsigned int num_cells_y = 10;

int main(int argc, char const *argv[])
{
	process_arguments(argc, argv, num_cells_x, num_cells_y);
	config_printout(num_cells_x, num_cells_y);
	GameOfLifeDomain GoLD = GameOfLifeDomain(num_cells_x, num_cells_y);
	GoLD.set_random();
	GoLD.debug_print();
	return 0;
}