#pragma once
#include <iostream>
#include <cstring>
#include <vector>
#include <time.h>
#include <SFML/Graphics.hpp>

namespace game_of_life {

	class GameOfLifeDomain
	{
		public:

			unsigned int numX, numY;
			std::vector<bool> cells {0};

			GameOfLifeDomain(unsigned int num_cells_x, unsigned int num_cells_y);


			// Симулирует следующий шаг
			void step_next();

			// Вывод на экран для отладки
			void debug_print();

			// Вывод на экран для отладки
			void set_random();

		private:

			// Даёт на выход количество соседей клетки по координатам i,j
			unsigned int num_of_neighbours(unsigned int i, unsigned int j);
	};

	class RenderField : public sf::Drawable , public sf::Transformable
	{
			RenderField(unsigned int num_cells_x, unsigned int num_cells_y);
			RenderField();

			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};

	class WindowHandler
	{
		public:
			unsigned int DEFAULT_SCREEN_WIDTH = 500;
			unsigned int DEFAULT_SCREEN_HEIGHT = 500;
			WindowHandler(unsigned int resx, unsigned int resy);
			WindowHandler();

			sf::RenderWindow* window;
			RenderField* renderField;

			void draw(); 
	};

	
};