#include "../header/game_of_life.h"



/*
=====	ЛОГИКА	=====	
*/

game_of_life::GameOfLifeDomain::GameOfLifeDomain(unsigned int num_cells_x, unsigned int num_cells_y)
{
	numX = num_cells_x;
	numY = num_cells_y;
	std::cout << numY << " " << numX << std::endl;
	cells.resize(numX * numY);

	for (unsigned int i = 0; i < numX; i++)
	{
		for (unsigned int j = 0; j < numY; j++)
		{
			cells[i*numY + j] = 0;
		}
	}

	
}

void game_of_life::GameOfLifeDomain::step_next()
{
	int neighbours;
	for (unsigned int i = 1; i < numX - 1; i++)
	{
		for (unsigned int j = 1; j < numY- 1; j++)
		{
			neighbours = num_of_neighbours(i,j);
			if (neighbours < 2) cells[i*numY + j] = false;
			if (neighbours > 3) cells[i*numY + j] = false;
			if (neighbours == 3) cells[i*numY + j] = true;
		}
	}
}


unsigned int game_of_life::GameOfLifeDomain::num_of_neighbours(unsigned int i, unsigned int j)
{
	unsigned int neighbours = 0;

	if (cells[(i+1)*numY + j+1]) neighbours++;
	if (cells[(i+1)*numY + j]) neighbours++;
	if (cells[(i+1)*numY + j-1]) neighbours++;
	if (cells[i*numY + j+1]) neighbours++;
	if (cells[i*numY + j-1]) neighbours++;
	if (cells[(i-1)*numY + j+1]) neighbours++;
	if (cells[(i-1)*numY + j]) neighbours++;
	if (cells[(i-1)*numY + j-1]) neighbours++;

	return neighbours;
}

void game_of_life::GameOfLifeDomain::set_random()
{
	srand(time(NULL));
	for (unsigned int i = 0; i < numX; i++)
	{
		for (unsigned int j = 0; j < numY; j++)
		{
			if (rand()%2 == 0) cells[i*numY + j] = false;
			else  cells[i*numY + j] = true;
			
		}
	}
}

void game_of_life::GameOfLifeDomain::debug_print()
{
	for (unsigned int i = 0; i < numX; i++)
	{
		for (unsigned int j = 0; j < numY; j++)
		{
			if (cells[i*numY + j]) std::cout << "X ";
			else std::cout << ". ";
		}
		std::cout << std::endl;
	}
}

/*
=====	РЕНДЕР	=====	
*/

game_of_life::WindowHandler::WindowHandler(unsigned int resx, unsigned int resy)
{
	window = new sf::RenderWindow(sf::VideoMode(resx, resy), "Игра 'Жизнь'");
}

game_of_life::WindowHandler::WindowHandler()
{
	window = new sf::RenderWindow(sf::VideoMode(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT), "Игра 'Жизнь'");
}

void game_of_life::WindowHandler::draw()
{
	while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
        }

        window->clear();
        window->draw(*renderField);
        window->display();
    }
}

game_of_life::RenderField::RenderField(unsigned int num_cells_x, unsigned int num_cells_y)
{

}

game_of_life::RenderField::RenderField()
{
	
}