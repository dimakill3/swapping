#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>

int PROCESS = 0;

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define GRAY 7
#define YELLOW 14
#define WHITE 15
#define MARGENTA 5

#define UP 72
#define DOWN 80
#define ENTER 13

#define MAX_PRIOR 5
#define FALSE_PROC -1
#define PAUSE_PROC -2

#define TABLE_LENGHT 40
#define SPEC_TABUL 20
#define Y_O 1
#define Y_V 5

#define BLOCK 50
#define OMEMORY 500
#define VMEMORY 1500

#define info1 pair<int, int>
#define info2 pair<string, int>

using namespace std;

void ClearRow(int X, int Y, int rowWidth)
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	DWORD                      count;
	COORD                      homeCoords = { X, Y };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;

	// Fill the row with spaces
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		rowWidth,
		homeCoords,
		&count
		)) return;

	//Fill the row with the current colors and attributes
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		rowWidth,
		homeCoords,
		&count
		)) return;

	//Move the cursor home
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

void ClearRectangle(int X1, int Y1, int X2, int Y2)			//�������� �� ���� �����, � ������ �����(�������������)
{
	while (Y1<Y2)
		ClearRow(X1, Y1++, X2 - X1);
}

void gotoxy(int x, int y)									//���������� ������� � ����������� ����������
{
	HANDLE hConsole;
	COORD cursorLoc;
	fflush(stdin);
	cursorLoc.X = x;
	cursorLoc.Y = y;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsole, cursorLoc);
}

void GetCoords(int &X, int &Y)
{
	CONSOLE_SCREEN_BUFFER_INFO info_c;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info_c);


	X = info_c.dwCursorPosition.X;
	Y = info_c.dwCursorPosition.Y;
}

void enter_spec_tabul()										//��������� ��� ��������-�����(���-�� ��������, ������ ���-�� �������� � �������������� "���� ������: ")
{
	for (int i = 0; i < SPEC_TABUL; i++)
		cout << " ";
}

void Priority_dec(vector<pair<info1, info2>> &launched_processes, pair<info1, info2> &proc)
{
	if (proc.first.first > 0)
		proc.first.first--;

	for (int t = 0; t < launched_processes.size(); t++)
	{
		if (launched_processes[t].second.first == proc.second.first)
		{
			if (launched_processes[t].first.first > 0)
				launched_processes[t].first.first--;

			break;
		}
	}
}

void Priority_inc(vector<pair<info1, info2>> &launched_processes, pair<info1, info2> &proc)
{
	if (proc.first.first >= 0)
		proc.first.first++;

	for (int t = 0; t < launched_processes.size(); t++)
	{
		if (launched_processes[t].second.first == proc.second.first)
		{
			if (launched_processes[t].first.first >= 0)
				launched_processes[t].first.first++;

			break;
		}
	}
}

void swapp(vector<pair<info1, info2>> &process_in_o_memory, vector<pair<info1, info2>> &process_in_v_memory, vector<pair<info1, info2>> &launched_processes,
	int &sum_o_memory, int &sum_v_memory, int where_empty_place, int empty_mem)
{
	int max_prior;
	int min_prior;
	char set = 0;
	char is_swapp = 0;
	int piece;

	where_empty_place == -1 ? piece = process_in_o_memory.size() - 1 : piece = where_empty_place;
	
	vector <pair<info1, info2>> low_priority;

	for (int i = 0; i < process_in_v_memory.size(); i++)
	{
		if (process_in_v_memory[i].first.first != FALSE_PROC && process_in_v_memory[i].first.first != PAUSE_PROC)
		{
			is_swapp = 1;
			max_prior = process_in_v_memory[i].first.first;
			min_prior = process_in_v_memory[i].first.first;
			break;
		}
	}

	if (!is_swapp)
		return;

	//���� ������������ ��������� �������� � ����������� ������
	for (int i = 0; i < process_in_v_memory.size(); i++)
	{
		if (process_in_v_memory[i].first.first != FALSE_PROC && process_in_v_memory[i].first.first != PAUSE_PROC)
		{
			if (max_prior < process_in_v_memory[i].first.first)
				max_prior = process_in_v_memory[i].first.first;
			if (min_prior > process_in_v_memory[i].first.first)
				min_prior = process_in_v_memory[i].first.first;
		}
	}

	//������� ��������� ��� ��������� � ����������� �����������
	if (min_prior != max_prior)
	{
		for (int i = 0; i < process_in_v_memory.size(); i++)
		{
			if ((process_in_v_memory[i].first.first == min_prior) && (sum_o_memory + process_in_v_memory[i].first.second <= OMEMORY))
			{
				int mem_back = OMEMORY;
				for (int t = 0; t < process_in_o_memory.size(); t++)
					mem_back -= process_in_o_memory[t].first.second;

				if ((process_in_v_memory[i].first.second <= mem_back) || (process_in_o_memory[piece].first.second >= process_in_v_memory[i].first.second))
					low_priority.push_back(process_in_v_memory[i]);
			}
		}
	}

	//������� � ������ ������ ����������, �������� ������� � ������ ����������� � �������� �������� ��� � ����������(������� � �����, ����� � ����������)
	for (int j = max_prior; j >= 0; j--)
	{
		for (int i = 0; i < process_in_v_memory.size(); i++)
		{
			if (process_in_v_memory[i].first.first == j)
			{
				//���� ��� ����� �������� �� ������� ������ ���������, �� ���� ��������� ������� �� ��������������
				if (sum_o_memory + process_in_v_memory[i].first.second > OMEMORY)
				{
					continue;
				}
				else
				{
					int mem_back = OMEMORY;
					for (int t = 0; t < process_in_o_memory.size(); t++)
						mem_back -= process_in_o_memory[t].first.second;

					//������� �������� � �����
					if (where_empty_place == -1)
					{
						//���� ������ �������, �� ���������
						if (process_in_v_memory[i].first.second <= mem_back)
						{
							//�������� ���������, ����� ������������������ �������� �� �������� ��������� ����������
							if (process_in_v_memory[i].first.first == max_prior && (max_prior != min_prior))
								Priority_dec(launched_processes, process_in_v_memory[i]);

							//���� ����������� ��� ���������� �f����, �� ����� ����� �� ���������
							if (empty_mem <= process_in_v_memory[i].first.second)
								set = 1;

							sum_v_memory -= process_in_v_memory[i].first.second;
							sum_o_memory += process_in_v_memory[i].first.second;
							process_in_o_memory.push_back(process_in_v_memory[i]);
							auto iter = process_in_v_memory.cbegin();
							process_in_v_memory.erase(iter + i);
							i--;
						}
					}
					else
					{
						//���� ���������� ������� ��� �������� ������� ��������, �� ������� ������� � ����������
						if (process_in_o_memory[where_empty_place].first.second >= process_in_v_memory[i].first.second)
						{
							if (process_in_v_memory[i].first.first == max_prior && (max_prior != min_prior))
								Priority_dec(launched_processes, process_in_v_memory[i]);

							if (empty_mem <= process_in_v_memory[i].first.second)
								set = 1;

							sum_o_memory += process_in_v_memory[i].first.second;
							sum_v_memory -= process_in_v_memory[i].first.second;
							auto iter = process_in_o_memory.cbegin();
							process_in_o_memory.insert(iter + where_empty_place, process_in_v_memory[i]);

							if (process_in_o_memory[where_empty_place + 1].first.second > process_in_v_memory[i].first.second)
							{
								process_in_o_memory[where_empty_place + 1].first.second -= process_in_v_memory[i].first.second;
								where_empty_place++;
							}
							else
							{
								iter = process_in_o_memory.cbegin();
								process_in_o_memory.erase(iter + where_empty_place + 1);
								set = 1;
							}

							iter = process_in_v_memory.cbegin();
							process_in_v_memory.erase(iter + i);
							i--;
						}
					}
				}
			}

			if (set)
				break;
		}

		if (set)
			break;
	}

	//���������, ���� �������� � ������ ����������� �� ������ � ����������� ������ ������ ������, ��� ����� ������������ ������ �
	if (min_prior != max_prior)
	{
		for (int i = 0; i < low_priority.size(); i++)
		{
			for (int j = 0; j < process_in_v_memory.size(); j++)
			{
				if (process_in_v_memory[j].second.first == low_priority[i].second.first)
					Priority_inc(launched_processes, process_in_v_memory[j]);
			}
		}
	}
}

void Add_process(vector<pair<info1, info2>> &process_in_o_memory, vector<pair<info1, info2>> &process_in_v_memory, vector<pair<info1, info2>> &launched_processes,
	pair<info1, info2> new_proc, int &sum_o_memory, int &sum_v_memory, int key)
{
	if (!key)
	launched_processes.push_back(new_proc);

	//���� ������ �� ����� ������� �� �������, �� ����� �������� ��� � ���������� ������
	if (sum_o_memory + new_proc.first.second > OMEMORY)
	{
		if (key)
			return;
		process_in_v_memory.push_back(new_proc);
		sum_v_memory += new_proc.first.second;
	}
	else										//����� ��������� ����������, � ���� � ��� ����� ���-�� �������� - ���������(���� ������� ������ ����������, �� ��������� ���� �����������!!!)
	{
		char set = 0;							//����, ����������� �� ��, �������� �� ������� ��� ���(���� ���, �� �������� �������� � ����� ����������)
		int mem = 0;							//������� �����, ���������� ������������
		for (int i = 0; i < process_in_o_memory.size(); i++)
		{
			if (process_in_o_memory[i].first.first == FALSE_PROC)
			{
				mem += process_in_o_memory[i].first.second;

				if (process_in_o_memory[i].first.second >= new_proc.first.second)
				{
					auto iter = process_in_o_memory.cbegin();
					process_in_o_memory.insert(iter + i, new_proc);
					sum_o_memory += new_proc.first.second;

					if (key)
					{
						sum_v_memory -= new_proc.first.second;
						for (int i = 0; i < process_in_v_memory.size(); i++)
						{
							if (process_in_v_memory[i].second.first == new_proc.second.first)
							{
								auto iter = process_in_v_memory.cbegin();
								process_in_v_memory.erase(iter + i);
								break;
							}
						}
					}

					if (process_in_o_memory[i + 1].first.second > new_proc.first.second)
						process_in_o_memory[i + 1].first.second -= new_proc.first.second;
					else
					{
						iter = process_in_o_memory.cbegin();
						process_in_o_memory.erase(iter + i + 1);
					}

					set = 1;
					break;
				}
			}
		}

		//���������� � ����� ����������
		if (!set)
		{
			if (((OMEMORY - (sum_o_memory + mem)) >= new_proc.first.second))
			{
				process_in_o_memory.push_back(new_proc);
				sum_o_memory += new_proc.first.second;

				if (key)
				{
					sum_v_memory -= new_proc.first.second;
					for (int i = 0; i < process_in_v_memory.size(); i++)
					{
						if (process_in_v_memory[i].second.first == new_proc.second.first)
						{
							auto iter = process_in_v_memory.cbegin();
							process_in_v_memory.erase(iter + i);
							break;
						}
					}
				}
			}
			else
			{
				if (!key)
				{
					process_in_v_memory.push_back(new_proc);
					sum_v_memory += new_proc.first.second;
				}
			}
		}
	}
}

void Erase_process(vector<pair<info1, info2>> &process_in_o_memory, vector<pair<info1, info2>> &process_in_v_memory, vector<pair<info1, info2>> &launched_processes,
	pair<info1, info2> del_proc, int &sum_o_memory, int &sum_v_memory)
{
	for (int i = 0; i < launched_processes.size(); i++)
	{
		if ((launched_processes[i].second.first == del_proc.second.first))
		{
			auto iter = launched_processes.cbegin();
			launched_processes.erase(iter + i);
			break;
		}
	}

	for (int i = 0; i < process_in_v_memory.size(); i++)
	{
		if (process_in_v_memory[i].second.first == del_proc.second.first)
		{
			auto iter = process_in_v_memory.cbegin();
			int mem = process_in_v_memory[i].first.second;
			process_in_v_memory.erase(iter + i);
			sum_v_memory -= mem;
			break;
		}
	}

	char empty_place = 0;
	int where_empty_place;
	int empty_mem = del_proc.first.second;					//������� ��������� ������ ����������

	for (int i = 0; i < process_in_o_memory.size(); i++)
	{
		if (process_in_o_memory[i].second.first == del_proc.second.first)
		{
			empty_place = 1;
			auto iter = process_in_o_memory.cbegin();
			int mem = process_in_o_memory[i].first.second;

			if (i != (process_in_o_memory.size() - 1))
			{
				if (i != 0 && process_in_o_memory[i - 1].first.first == FALSE_PROC && process_in_o_memory[i + 1].first.first == FALSE_PROC)
				{
					process_in_o_memory[i - 1].first.second += mem + process_in_o_memory[i + 1].first.second;

					iter = process_in_o_memory.cbegin();
					process_in_o_memory.erase(iter + i + 1);
					where_empty_place = i - 1;
				}
				else
				{
					if (i != 0 && process_in_o_memory[i - 1].first.first == FALSE_PROC)
					{
						process_in_o_memory[i - 1].first.second += mem;
						where_empty_place = i - 1;
					}
					else
					{
						if (process_in_o_memory[i + 1].first.first == FALSE_PROC)
						{
							process_in_o_memory[i + 1].first.second += mem;
							where_empty_place = i;
						}
						else
						{
							iter = process_in_o_memory.cbegin();
							process_in_o_memory.insert(iter + i + 1, make_pair(info1(FALSE_PROC, process_in_o_memory[i].first.second), info2("piece", BLACK)));
							where_empty_place = i;
						}
					}
				}
			}
			else
			{
				if (i != 0 && process_in_o_memory[i - 1].first.first == FALSE_PROC)
				{
					auto iter = process_in_o_memory.cbegin();
					process_in_o_memory.erase(iter + i - 1);
					i--;
				}

				where_empty_place = -1;
			}

			iter = process_in_o_memory.cbegin();
			process_in_o_memory.erase(iter + i);
			sum_o_memory -= mem;

			if (sum_o_memory == 0)
			{
				process_in_o_memory.clear();
				where_empty_place = -1;
			}

			break;
		}
	}

	if (empty_place && process_in_v_memory.size() != 0)
	{
		swapp(process_in_o_memory, process_in_v_memory, launched_processes, sum_o_memory, sum_v_memory, where_empty_place, empty_mem);
	}
}

void Pause_process(vector<pair<info1, info2>> &process_in_o_memory, vector<pair<info1, info2>> &process_in_v_memory, vector<pair<info1, info2>> &launched_processes, 
	pair<info1, info2> pause_proc, int &sum_o_memory, int &sum_v_memory, vector<pair<int, string>> &paused)
{
	//���� ������� � ���������� � ������ ��� ��������� �� ��������������
	for (int i = 0; i < launched_processes.size(); i++)
	{
		if (launched_processes[i].second.first == pause_proc.second.first)
		{
			paused.push_back(make_pair(launched_processes[i].first.first, launched_processes[i].second.first));
			launched_processes[i].first.first = PAUSE_PROC;
			break;
		}
	}

	//���� ������� � ����������� ������, �� ������ �� ������
	for (int i = 0; i < process_in_v_memory.size(); i++)
	{
		if (process_in_v_memory[i].second.first == pause_proc.second.first)
		{
			process_in_v_memory[i].first.first = PAUSE_PROC;
			break;
		}
	}

	char empty_place = 0;
	int where_empty_place;
	int empty_mem = pause_proc.first.second;					//������� ��������� ������ ����������

	//���� ������� � ����������� ������, �� ��������� ��� � �����������
	for (int i = 0; i < process_in_o_memory.size(); i++)
	{
		if (process_in_o_memory[i].second.first == pause_proc.second.first)
		{
			process_in_o_memory[i].first.first = PAUSE_PROC;

			empty_place = 1;
			auto iter = process_in_o_memory.cbegin();
			int mem = process_in_o_memory[i].first.second;

			if (i != (process_in_o_memory.size() - 1))
			{
				if (i != 0 && process_in_o_memory[i - 1].first.first == FALSE_PROC && process_in_o_memory[i + 1].first.first == FALSE_PROC)
				{
					process_in_o_memory[i - 1].first.second += mem + process_in_o_memory[i + 1].first.second;

					iter = process_in_o_memory.cbegin();
					process_in_o_memory.erase(iter + i + 1);
					where_empty_place = i - 1;
				}
				else
				{
					if (i != 0 && process_in_o_memory[i - 1].first.first == FALSE_PROC)
					{
						process_in_o_memory[i - 1].first.second += mem;
						where_empty_place = i - 1;
					}
					else
					{
						if (process_in_o_memory[i + 1].first.first == FALSE_PROC)
						{
							process_in_o_memory[i + 1].first.second += mem;
							where_empty_place = i;
						}
						else
						{
							iter = process_in_o_memory.cbegin();
							process_in_o_memory.insert(iter + i + 1, make_pair(info1(FALSE_PROC, process_in_o_memory[i].first.second), info2("piece", BLACK)));
							where_empty_place = i;
						}
					}
				}
			}
			else
			{
				if (i != 0 && process_in_o_memory[i - 1].first.first == FALSE_PROC)
				{
					auto iter = process_in_o_memory.cbegin();
					process_in_o_memory.erase(iter + i - 1);
					i--;
				}

				where_empty_place = -1;
			}


			process_in_v_memory.push_back(process_in_o_memory[i]);
			sum_v_memory += mem;

			iter = process_in_o_memory.cbegin();
			process_in_o_memory.erase(iter + i);
			sum_o_memory -= mem;

			if (sum_o_memory == 0)
			{
				process_in_o_memory.clear();
				where_empty_place = -1;
			}
				break;
		}
	}

	if (empty_place && process_in_v_memory.size() != 0)
	{
		swapp(process_in_o_memory, process_in_v_memory, launched_processes, sum_o_memory, sum_v_memory, where_empty_place, empty_mem);
	}
}

void Resume_process(vector<pair<info1, info2>> &process_in_o_memory, vector<pair<info1, info2>> &process_in_v_memory, vector<pair<info1, info2>> &launched_processes,
	pair<info1, info2> resume_proc, int &sum_o_memory, int &sum_v_memory, vector<pair<int, string>> &paused)
{
	int need_proc = 0;
	for (int i = 0; i < launched_processes.size(); i++)
	if (launched_processes[i].second.first == resume_proc.second.first)
	{
		for (int t = 0; t < process_in_v_memory.size(); t++)
		if (launched_processes[i].second.first == process_in_v_memory[t].second.first)
		{
			need_proc = t;
			for (int j = 0; j < paused.size(); j++)
			if (launched_processes[i].second.first == paused[j].second)
			{
				launched_processes[i].first.first = paused[j].first;
				process_in_v_memory[t].first.first = paused[j].first;
				auto iter = paused.cbegin();
				paused.erase(iter + j);
				break;
			}
		}
	}

	Add_process(process_in_o_memory, process_in_v_memory, launched_processes, process_in_v_memory[need_proc], sum_o_memory, sum_v_memory, 1);
}

char Set_Color(int proc_num_in_memory, vector<pair<info1, info2>> &process_in_memory)
{
	if (proc_num_in_memory < process_in_memory.size())
		return process_in_memory[proc_num_in_memory].second.second;
	else
		return BLACK;
}

int Set_Blocks(int proc_num_in_memory, vector<pair<info1, info2>> &process_in_memory)
{
	if (proc_num_in_memory < process_in_memory.size())
		return process_in_memory[proc_num_in_memory].first.second / BLOCK;
	else
		return OMEMORY / BLOCK;
}

void Enter_Table_Strings(vector<pair<info1, info2>> &process_in_memory)						//��������� ���������� � ���������� ��������� ��������
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int proc_num_in_memory = 0;
	char color;
	int lenght;

	while (proc_num_in_memory < process_in_memory.size())
	{
		color = Set_Color(proc_num_in_memory, process_in_memory);

		//������� �������� ��������
		std::cout << "| " << process_in_memory[proc_num_in_memory].second.first;
		lenght = process_in_memory[proc_num_in_memory].second.first.length();
		for (int i = 0; i < 8 - lenght; i++)
			std::cout << " ";

		//������� ���������� ��������� ������
		std::cout << " | " << process_in_memory[proc_num_in_memory].first.second;

		string s = to_string(process_in_memory[proc_num_in_memory].first.second);
		lenght = s.length();

		for (int i = 0; i < 6 - lenght; i++)
			std::cout << " ";
		std::cout << " |";

		//������� ���� ��������
		SetConsoleTextAttribute(hConsole, (WORD)((color << 4) | BLACK));
		for (int i = 0; i < 7; i++)
			std::cout << " ";
		SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));
		std::cout << "|";

		//������� ��������� ��������
		if (process_in_memory[proc_num_in_memory].first.first == PAUSE_PROC)
			std::cout << " Pause    |"  << endl;
		else
			std::cout << " " << process_in_memory[proc_num_in_memory].first.first << "        |" << endl;

		for (int i = 0; i < TABLE_LENGHT; i++)
			std::cout << "-";

		std::cout << endl;

		proc_num_in_memory++;
	}

}

void Print_Menu(int &x_begin, int &y_begin, vector<int> &menu_items)
{
	//��������� ������ ���� ��� �������������� � ����������	
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	menu_items.clear();

	gotoxy(x_begin, y_begin);

	ClearRectangle(x_begin, y_begin, 100, y_begin + PROCESS + 3);

	gotoxy(x_begin, y_begin);


	SetConsoleTextAttribute(hConsole, (WORD)((YELLOW << 4) | BLACK));
	std::cout << "1";
	SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));

	std::cout << ". ��������� �������..." << endl;
	std::cout << "2. ������������� �������..." << endl;
	std::cout << "3. ����������� �������..." << endl;
	std::cout << "4. ��������� �������..." << endl;
	std::cout << "5. ����� �� ���������" << endl;

	menu_items = { -1, -1, -1, -1, -1};
		gotoxy(x_begin, y_begin);
}

void Print_subMenu(char type, vector<pair<info1, info2>> &launched_processes, vector<pair<info1, info2>> &process, 
	vector<int> &menu_items, int &x_begin, int &y_begin, vector<pair<int, string>> &paused)
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	gotoxy(x_begin, y_begin);

	ClearRectangle(x_begin, y_begin, 100, y_begin + PROCESS + 3);

	gotoxy(x_begin, y_begin);

	menu_items.clear();

#pragma region add process
	if (type == 1)
	{
		int t = 1;
		std::cout << "����� ������� ���������?" << endl;

		SetConsoleTextAttribute(hConsole, (WORD)((YELLOW << 4) | BLACK));
		std::cout << t;
		SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));

		for (int i = 0; i < PROCESS; i++)
		{
			int already_launched = 0;

			for (int j = 0; j < launched_processes.size(); j++)
			{
				if (process[i].second.first == launched_processes[j].second.first)
				{
					already_launched = 1;
					break;
				}
			}

			if (!already_launched)
			{
				menu_items.push_back(i);

				if (t == 1)
					std::cout << ". " << process[i].second.first << " (�������� " << process[i].first.second << " �. �. ������, ����� ��������� " << process[i].first.first << ")" << endl;
				else
					std::cout << t << ". " << process[i].second.first << " (�������� " << process[i].first.second << " �. �. ������, ����� ��������� " << process[i].first.first << ")" << endl;

				t++;
			}
		}

		menu_items.push_back(-1);
		if (t == 1)
			std::cout << ". �����" << endl;
		else
			std::cout << t << ". �����" << endl;
	}
#pragma endregion

#pragma region pause process
	if (type == 2)
	{
		std::cout << "����� ������� �������������?" << endl;

		int t = 1;
		SetConsoleTextAttribute(hConsole, (WORD)((YELLOW << 4) | BLACK));
		std::cout << t;
		SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));

		for (int i = 0; i < launched_processes.size(); i++)
		{
			if (launched_processes[i].first.first != PAUSE_PROC)
			{
				for (int j = 0; j < PROCESS; j++)
				{
					if (launched_processes[i].second.first == process[j].second.first)
					{
						menu_items.push_back(j);
						break;
					}
				}

				if (t == 1)
					std::cout << ". " << launched_processes[i].second.first << endl;
				else
					std::cout << t << ". " << launched_processes[i].second.first << endl;

				t++;
			}

		}

		menu_items.push_back(-1);

		if (t == 1)
			std::cout << ". �����" << endl;
		else
			std::cout << t << ". �����" << endl;
	}
#pragma endregion

#pragma region resume process
	if (type == 3)
	{
		std::cout << "����� ������� �����������?" << endl;

		int t = 1;

		SetConsoleTextAttribute(hConsole, (WORD)((YELLOW << 4) | BLACK));
		std::cout << t;
		SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));

		for (int i = 0; i < launched_processes.size(); i++)\
		{
			if (launched_processes[i].first.first == PAUSE_PROC)
			{
				for (int j = 0; j < PROCESS; j++)
				{
					if (launched_processes[i].second.first == process[j].second.first)
					{
						menu_items.push_back(j);
						break;
					}
				}

				if (t == 1)
					std::cout << ". " << launched_processes[i].second.first << endl;
				else
					std::cout << t << ". " << launched_processes[i].second.first << endl;

				t++;
			}
		}

		menu_items.push_back(-1);

		if (t == 1)
			std::cout << ". �����" << endl;
		else
			std::cout << t << ". �����" << endl;
	}
#pragma endregion

#pragma region erase process
	if (type == 4)
	{
		std::cout << "����� ������� ���������?" << endl;

		int t = 1;
		SetConsoleTextAttribute(hConsole, (WORD)((YELLOW << 4) | BLACK));
		std::cout << t;
		SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));

		for (int i = 0; i < launched_processes.size(); i++)
		{
			for (int j = 0; j < PROCESS; j++)
			{
				if (launched_processes[i].second.first == process[j].second.first)
				{
					menu_items.push_back(j);
					break;
				}
			}

			if (i == 0)
				std::cout << ". " << launched_processes[i].second.first << endl;
			else
				std::cout << i + 1 << ". " << launched_processes[i].second.first << endl;

			t++;
		}

		int size = launched_processes.size() + 1;

		if (t > 2)
		{
			menu_items.push_back(-2);

			std::cout << size << ". ��������� ��� ��������" << endl;

			size++;
		}

		menu_items.push_back(-1);

		if (t == 1)
			std::cout << ". �����" << endl;
		else
			std::cout << size << ". �����" << endl;
	}
#pragma endregion

	gotoxy(x_begin, y_begin + 1);
}

void Print_Info(vector<pair<info1, info2>> &process_in_o_memory, vector<pair<info1, info2>> &process_in_v_memory, vector<pair<info1, info2>> &launched_processes, 
	int &x_begin, int &y_begin, vector<int> &menu_items)				//������ ����
{
	system("cls");
	
	HANDLE hConsole;										//��� ��������� ����� ������
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);				//

	int blocks;											//����� ���-�� ������ � ����������������� ��������-����
	char color;											//���� ��� ������� ��������
	int proc_num_in_memory;									//����� �������� �� ����� � ������

#pragma region Operativ_Progress_Bar
	//������ ��������-��� ��� ����������� ������
	blocks = 0;

	enter_spec_tabul();
	for (int i = 0; i < 21; i++)
		std::cout << "-";

	std::cout << endl;

	proc_num_in_memory = -1;
	std::cout << "����������� ������: ";
	for (int i = 0; i < 10; i++)
	{
		std::cout << "|";

		if (blocks == 0)
		{
			proc_num_in_memory++;

			color = Set_Color(proc_num_in_memory, process_in_o_memory);
			blocks = Set_Blocks(proc_num_in_memory, process_in_o_memory);
		}

			SetConsoleTextAttribute(hConsole, (WORD)((color << 4) | GRAY));

			std::cout << " ";

		if (color != BLACK)
		SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));

		blocks--;
	}

	SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));
	std::cout << "|" << endl;

	enter_spec_tabul();

	for (int i = 0; i < 21; i++)
		std::cout << "-";

	std::cout << endl << endl;

#pragma endregion

#pragma region Virtual_Progress_Bar
	//������ ��������-��� ��� ����������� ������

	blocks = 0;
	enter_spec_tabul();
	for (int i = 0; i < 61; i++)
		std::cout << "-";

	std::cout << endl;

	proc_num_in_memory = -1;
	std::cout << "����������� ������: ";
	for (int i = 0; i < 30; i++)
	{
		std::cout << "|";

		if (blocks == 0)
		{
			proc_num_in_memory++;

			color = Set_Color(proc_num_in_memory, process_in_v_memory);
			blocks = Set_Blocks(proc_num_in_memory, process_in_v_memory);

			if (color == BLACK)
				blocks *= 3;
		}

			SetConsoleTextAttribute(hConsole, (WORD)((color << 4) | GRAY));

			std::cout << " ";
		blocks--;

		if (color != BLACK)
		SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));
	}

	SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));
	std::cout << "|" << endl;

	enter_spec_tabul();

	for (int i = 0; i < 61; i++)
		std::cout << "-";

	std::cout << endl << endl;
#pragma endregion

#pragma region Table

	//��������� �������
	for (int i = 0; i < TABLE_LENGHT; i++)
		std::cout << "-";

	std::cout << endl;

	std::cout << "|   " << "Name" << "   |" << " Memory " << "|" << " Color " << "|" << " Priority " << "|" << endl;

	for (int i = 0; i < TABLE_LENGHT; i++)
		std::cout << "-";
	std::cout << endl;

	//������ �������
	if (launched_processes.size() == 0)
	{
		std::cout << "|";

		for (int i = 0; i < 10; i++)
			std::cout << " ";
		std::cout << "|";

		for (int i = 0; i < 8; i++)
			std::cout << " ";
		std::cout << "|";

		for (int i = 0; i < 7; i++)
			std::cout << " ";
		std::cout << "|";

		for (int i = 0; i < 10; i++)
			std::cout << " ";
		std::cout << "|";

		std::cout << endl;

		for (int i = 0; i < TABLE_LENGHT; i++)
			std::cout << "-";

		std::cout << endl;
	}
	else
	{
		Enter_Table_Strings(launched_processes);
	}

#pragma endregion

	//��������� ������ ���� ��� �������������� � ����������
	GetCoords(x_begin, y_begin);

	Print_Menu(x_begin, y_begin, menu_items);
}

void up(int &x_coord, int &y_coord, int &item)
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	std::cout << item + 1;
	y_coord--;
	item--;
	gotoxy(x_coord, y_coord);
	SetConsoleTextAttribute(hConsole, (WORD)((YELLOW << 4) | BLACK));
	std::cout << item + 1;
	SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));
	gotoxy(x_coord, y_coord);
}

void down(int &x_coord, int &y_coord, int &item)
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	std::cout << item + 1;
	y_coord++;
	item++;
	gotoxy(x_coord, y_coord);
	SetConsoleTextAttribute(hConsole, (WORD)((YELLOW << 4) | BLACK));
	std::cout << item + 1;
	SetConsoleTextAttribute(hConsole, (WORD)((BLACK << 4) | GRAY));
	gotoxy(x_coord, y_coord);
}

int main()
{
	setlocale(LC_ALL, "rus");
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	system("MODE CON: COLS=100 LINES=40");

	vector<pair<info1, info2>> process{ 
		make_pair(info1(1, 100), info2("Word", BLUE)), make_pair(info1(3, 150), info2("Ccleaner", GREEN)), 
		make_pair(info1(2, 50), info2("MSDos", WHITE)), make_pair(info1(5, 250), info2("Game", RED)), 
		make_pair(info1(5, 200), info2("Browser", YELLOW)), make_pair(info1(4, 150), info2("Discord", CYAN)),
		make_pair(info1(1, 100), info2("GIMP", MARGENTA))};

	PROCESS = process.size();

	vector<pair<info1, info2>> process_in_o_memory;

	vector<pair<info1, info2>> process_in_v_memory;

	vector<pair<info1, info2>> launched_processes;

	vector<int> menu_items;

	vector<pair<int, string>> paused;

	int x_begin = 0, y_begin = 0;
	int item = 0;
	int x_coord, y_coord;
	int sum_o_memory = 0, sum_v_memory = 0;
	char is_subMenu = 0;
	char do_print = 1;

	while (true)
	{
		char move;

		if (do_print)
		{
			Print_Info(process_in_o_memory, process_in_v_memory, launched_processes, x_begin, y_begin, menu_items);
			x_coord = x_begin; y_coord = y_begin;
			do_print = 0;
		}

		do
		{
			move = _getch();
		} while (move != UP && move != DOWN && move != ENTER);

		if (move == UP)
		{
			if ((item - 1) < 0)
				continue;
			else
			{
				up(x_coord, y_coord, item);
			}
		}

		if (move == DOWN)
		{
			if ((item + 1) >= menu_items.size())
				continue;
			else
			{
				down(x_coord, y_coord, item);
			}
		}

		if (move == ENTER)
		{
			if (is_subMenu)
			{
				if (menu_items[item] == -1)
				{
					Print_Menu(x_begin, y_begin, menu_items);
					y_coord = y_begin;
				}
				else
				{

					if (is_subMenu == 1)
					{
						Add_process(process_in_o_memory, process_in_v_memory, launched_processes, process[menu_items[item]], sum_o_memory, sum_v_memory, 0);
					}

					if (is_subMenu == 2)
					{
						Pause_process(process_in_o_memory, process_in_v_memory, launched_processes, process[menu_items[item]], sum_o_memory, sum_v_memory, paused);
					}

					if (is_subMenu == 3)
					{
						Resume_process(process_in_o_memory, process_in_v_memory, launched_processes, process[menu_items[item]], sum_o_memory, sum_v_memory, paused);
					}

					if (is_subMenu == 4)
					{
						if (menu_items[item] == -2)
						{
							process_in_o_memory.clear();
							process_in_v_memory.clear();
							launched_processes.clear();
							paused.clear();
							sum_o_memory = 0;
							sum_v_memory = 0;
						}
						else
						Erase_process(process_in_o_memory, process_in_v_memory, launched_processes, process[menu_items[item]], sum_o_memory, sum_v_memory);
					}

					do_print = 1;
				}

				item = 0;
				is_subMenu = 0;
			}
			else
			{
				if (item + 1 == 5)
				{
					gotoxy(x_begin + 30, y_begin + 4);
					std::cout << endl;
					return 0;
				}

				Print_subMenu(item + 1, launched_processes, process, menu_items, x_begin, y_begin, paused);
				y_coord = y_begin + 1;
				is_subMenu = item + 1;
				item = 0;
			}
		}
	}

	return 0;
}

