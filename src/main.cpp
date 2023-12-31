#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <time.h>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <thread>
#include <omp.h>

bool** cells;								// Текущий кадр симуляции
bool** cells_next;							// Следующий кадр симуляции

enum class print_modes {NONE, FIRSTLAST, ALL};				// Режимы вывода на экран
char border_char = '.', alive_char = 'o', dead_char = '.'; 	// Символы
unsigned num_cells_x = 10;									// Количество клеток по вертикали
unsigned num_cells_y = 10;									// Количество клеток по горизонтали
int randomizer_seed = 2107;									// Зерно генератора случайных чисел
unsigned iter_count = 0;									// Количество итераций
unsigned tests_count = 1;									// Количество повторов замеров (для получения более объективной цифры).
bool flag_mode_iteration = false;							// Режим заданного количества итераций (если false, то будет симулировать пока не достигнет стационарного состояния)
bool flag_glider_mode = false;								// Если истина, то не рандомизировать клетки, а поставить глайдер в угол кадра симуляции. (для тестирования правильности)
bool flag_parallel_mode = false;							// Режим распараллеливания с использованием OpenMP
print_modes print_mode = print_modes::ALL;					// Текущий режим печати


void process_arguments(int argc, char const *argv[]) {
	/**
     * Обработка аргументов
     *
     * @param argc 	Количество аргументов
     * @param agrv 	Массив аргументов
     */

	for (int i {}; i < argc; i++)
	{
		if (strcmp(argv[i], "-x") == 0)
		{
			num_cells_x = std::stoi(argv[i+1]) + 2;
		}
		if (strcmp(argv[i], "-y") == 0)
		{
			num_cells_y = std::stoi(argv[i+1]) + 2;
		}
		if (strcmp(argv[i], "-seed") == 0)
		{
			randomizer_seed = std::stoi(argv[i+1]);
		}
		if (strcmp(argv[i], "-iter") == 0)
		{
			iter_count = std::stoi(argv[i+1]);
			flag_mode_iteration = true;
		}
		if (strcmp(argv[i], "-tests") == 0)
		{
			tests_count = std::stoi(argv[i+1]);
		}
		if (strcmp(argv[i], "-parallel") == 0)
		{
			flag_parallel_mode = true;
		}
		if (strcmp(argv[i], "-mode") == 0)
		{
			std::map <const std::string, print_modes> arg_to_print_mode {
				{"all", 		print_modes::ALL},
				{"firstlast", 	print_modes::FIRSTLAST},
				{"none", 		print_modes::NONE},
			};
			print_mode = arg_to_print_mode[argv[i+1]];
		}
		if (strcmp(argv[i], "-deadchar") == 0)
		{
			if (strlen(argv[i+1]) == 1) dead_char = static_cast<char>(*argv[i+1]);
			else std::cout << "Не удастся использовать данный символ dead_char." << std::endl;
		}
		if (strcmp(argv[i], "-alivechar") == 0)
		{
			if (strlen(argv[i+1]) == 1) alive_char = static_cast<char>(*argv[i+1]);
			else std::cout << "Не удастся использовать данный символ alive_char." << std::endl;
			 
		}
		if (strcmp(argv[i], "-borderchar") == 0)
		{
			if (strlen(argv[i+1]) == 1) border_char = static_cast<char>(*argv[i+1]);
			else std::cout << "Не удастся использовать данный символ border_char." << std::endl;
		}
		if (strcmp(argv[i], "-glider") == 0)
		{
			flag_glider_mode = true;
		}
	}
}


void single_var_printout(std::string name, auto var) {
	/**
     * Вывод одной инфо-строки конфигурации
     *
     * @param name	Название переменной
     * @param var 	Переменная
     */
	std::cout << "<i>\t" << name << ": " << "\t\t\t" << var << std::endl;
}



void config_printout() {
	/**
     * Вывод всей конфигурации
     */

	std::map <print_modes, std::string> print_mode_to_info{
				{print_modes::ALL,			"каждый кадр"},
				{print_modes::FIRSTLAST,	"первый и последний кадр"},
				{print_modes::NONE,			"не выводить кадры"},
			};

	std::cout << "Программа симуляции будет запущена со следующими параметрами:" << std::endl;
	single_var_printout("Размер сетки по оси Х", 	::num_cells_x);
	single_var_printout("Размер сетки по оси Y", 	::num_cells_y);	
	single_var_printout("Зерно рандомизатора", 		::randomizer_seed);	
	single_var_printout("Символ границы", 			::border_char);
	single_var_printout("Символ живой клетки", 		::alive_char);	
	single_var_printout("Символ мёртвой клетки", 	::dead_char);	
	single_var_printout("Режим вывода кадра", 		print_mode_to_info[print_mode]);
	single_var_printout("Количество тестов", 		tests_count);
	if (iter_count != 0) single_var_printout("Количество итераций", iter_count);
	else std::cout << "<i>\tКоличество итераций:\t\t\tпока не стационарное состояние" << std::endl;
	std::cout << "Максимальное число потоков: " << omp_get_max_threads() << std::endl;
}


// game of life logic
unsigned int num_of_neighbours(unsigned int i, unsigned int j)
{
	/**
     * Подсчёт количества соседей заданной клетки
     *
     * @param i 	индекс i
     * @param j 	индекс j
     * @return количество соседей клетки.
     */
	unsigned int neighbours = 0;

	if (cells[i+1][j+1]) 	neighbours++;
	if (cells[i+1][j]) 		neighbours++;
	if (cells[i+1][j-1]) 	neighbours++;
	if (cells[i][j+1]) 		neighbours++;
	if (cells[i][j-1]) 		neighbours++;
	if (cells[i-1][j+1]) 	neighbours++;
	if (cells[i-1][j]) 		neighbours++;
	if (cells[i-1][j-1]) 	neighbours++;

	return neighbours;
}

void sim_frame_copy()
{
	/**
     * Копирование кадра симуляции
     */
	for (unsigned int i = 0; i < num_cells_x; i++)
	{	
		for (unsigned int j = 0; j < num_cells_y; j++)
		{
			cells[i][j] = cells_next[i][j];
		}
	}
}

bool step_next()
{
	/**
     * Рассчёт следующего шага симуляции
     *
     * @return true, если состояние с предыдущего кадра поменялось и false, если не поменялось
     */
	bool flag_changes_made = false;
	int neighbours;
	if (flag_parallel_mode)
	{
		unsigned int i,j = 1;
		
		#pragma omp parallel for private(i,j)
		for (unsigned int i = 1; i < num_cells_x-1; i++)
		{
			for (unsigned int j = 1; j < num_cells_y-1; j++)
			{
				bool prev_state = cells[i][j];
				neighbours = num_of_neighbours(i,j);

				if 		(cells[i][j] == true and (neighbours == 2 or neighbours == 3))		cells_next[i][j] = true;
				else if (cells[i][j] == false and neighbours == 3)							cells_next[i][j] = true;
				else																		cells_next[i][j] = false;

				if (prev_state != cells_next[i][j]) flag_changes_made = true;
			}
		}
	}
	else
	{
		for (unsigned int i = 1; i < num_cells_x-1; i++)
		{
			for (unsigned int j = 1; j < num_cells_y-1; j++)
			{
				bool prev_state = cells[i][j];
				neighbours = num_of_neighbours(i,j);

				if 		(cells[i][j] == true and (neighbours == 2 or neighbours == 3))		cells_next[i][j] = true;
				else if (cells[i][j] == false and neighbours == 3)							cells_next[i][j] = true;
				else																		cells_next[i][j] = false;

				if (prev_state != cells_next[i][j]) flag_changes_made = true;
			}
		}
	}
	sim_frame_copy();
	return flag_changes_made;
	
}

void randomize_cells()
{
	/**
     * Рандомизировать содержимое клеток
     */
	srand(randomizer_seed);
	for (unsigned int i = 1; i < num_cells_x; i++)
	{
		for (unsigned int j = 1; j < num_cells_y; j++)
		{
			if (rand()%2 == 0) cells[i][j] = true;
			else  cells[i][j] = false;
			
		}
	}
}

void debug_print(unsigned iteration_num)
{
	/**
     * Вывод кадра симуляции на экран
     *
     * @param iteration_num 	номер итерации для вывода.
     */
	std::cout << "Итерация " << iteration_num << std::endl;
	for (unsigned int i = 0; i < num_cells_x; i++)
	{
		for (unsigned int j = 0; j < num_cells_y; j++)
		{
			if (i == 0 or i == num_cells_x or j == 0 or j == num_cells_y) std::cout << border_char << ' ';
				else{
					if (cells[i][j]) std::cout << alive_char << ' ';
					else std::cout << dead_char << ' ';
				}

			 
		}
		std::cout << std::endl;
	}
}


void put_glider()
{
	cells[3][1] = true;
	cells[3][2] = true;
	cells[3][3] = true;
	cells[2][3] = true; 
	cells[1][2] = true;
}

void clear_cells()
{
	for (unsigned int i = 0; i < num_cells_x; i++)
	{
		for (unsigned int j = 0; j < num_cells_y; j++)
		{
			 cells[i][j] = false;
		}
	}
}

void allocate_cells()
{

	cells = new bool*[num_cells_x];
	cells_next = new bool*[num_cells_x];

	for(int i = 0; i < num_cells_x; i++)
	{
		cells[i] = new bool[num_cells_y];
	}

	for(int i = 0; i < num_cells_x; i++)
	{
		cells_next[i] = new bool[num_cells_y];
	}
}

int main(int argc, char const *argv[])
{
	
	process_arguments(
	 argc,
	 argv);
    
	config_printout();

	char ans;
	std::cout << "Вы уверены? (y/n)" ;
	std::cin >> ans;
	std::cout << std::endl;
	if (ans != 'y' and ans != 'Y') return 0;

	allocate_cells();
		 
	double avg = 0, maximum = -1, sum = 0;
	for (unsigned test_number {0}; test_number < tests_count; test_number++)
	{
			clear_cells();
			int iteration_num = 0;
			if (!flag_glider_mode) randomize_cells();
			else put_glider();
			
			if (print_mode == print_modes::FIRSTLAST or print_mode == print_modes::ALL) debug_print(iteration_num);
			auto start = std::chrono::high_resolution_clock::now();
				if (iter_count != 0)
				{
					for (; iteration_num < iter_count; iteration_num++)
					{
						if (print_mode == print_modes::ALL and iteration_num != iter_count - 1 and iteration_num != 0) debug_print(iteration_num);
						if (not step_next()) 
						{
							std::cout << "<!>\tСтационарное состояние. Выход из симуляции" << std::endl;
							break;
						}
					}
				}
				else
				{
					while(true)
					{
						iteration_num++;
						if (print_mode == print_modes::ALL) debug_print(iteration_num);
						if (!step_next()) 
						{
							std::cout << "<!>\tСтационарное состояние. Выход из симуляции" << std::endl;
							break;
						}
						
					}
				}
				
			auto end = std::chrono::high_resolution_clock::now();
			if (print_mode == print_modes::FIRSTLAST or print_mode == print_modes::ALL) debug_print(iteration_num);
			auto seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			sum += seconds;
			if (seconds > maximum) maximum = seconds;

			std::cout << "<!>\tТест " << test_number << " завершен.\tКоличество итераций: " << iteration_num << "\tВремя работы: "<< seconds/1000. << " секунд."<< std::endl;
	}
	
	std::cout << "<!> Симуляция завершена. \tСреднее время работы: " << sum/tests_count/1000. << " секунд\tМаксимальное время работы: " << maximum/1000. << " секунд" << std::endl;

	return 0;
}

