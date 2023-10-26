#include "../header/utility.h"


/* 
    input:    	Аргументы командной строки, адреса переменных

    output:   	нет.

    remarks:	Обрабатывает аргументы командной строки для заполнения глобальных переменных
*/
void utility_methods::process_arguments(int argc, char const *argv[], unsigned int &num_cells_x, unsigned int &num_cells_y)
{
	for (int i {}; i < argc; i++)
	{
		if (strcmp(argv[i], "-x") == 0)
		{
			num_cells_x = std::stoi(argv[i+1]);
		}
		if (strcmp(argv[i], "-y") == 0)
		{
			num_cells_y = std::stoi(argv[i+1]);
		}
	}
}

void utility_methods::single_var_printout(std::string name, double var)
{
	std::cout << name << ": " << var << std::endl;
}

void utility_methods::config_printout(unsigned int num_cells_x, unsigned int num_cells_y)
{
	std::cout << "Программа симуляции будет запущена со следующими параметрами:" << std::endl;
	single_var_printout("Размер сетки по оси Х", num_cells_x);
	single_var_printout("Размер сетки по оси Х", num_cells_y);	
}