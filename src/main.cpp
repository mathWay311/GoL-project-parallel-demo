#include <iostream>
#include <vector>
#include "../header/utility.h"
#include "../header/game_of_life.h"
#include <SFML/Graphics.hpp>

using namespace utility_methods;
using namespace game_of_life;

unsigned int num_cells_x = 10;
unsigned int num_cells_y = 10;
unsigned int SCREEN_WIDTH = 640;
unsigned int SCREEN_HEIGHT = 480;

int main(int argc, char const *argv[])
{
	process_arguments(
	 argc,
	 argv, 
	 num_cells_x, 
	 num_cells_y, 
	 SCREEN_WIDTH, 
	 SCREEN_HEIGHT);
    
	config_printout(num_cells_x, num_cells_y);
	GameOfLifeFacade gameOfLife(SCREEN_WIDTH, SCREEN_HEIGHT, num_cells_x, num_cells_y);

	return 0;
}