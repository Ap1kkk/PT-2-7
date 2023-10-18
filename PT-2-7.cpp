#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool isNumber(string value) {
	for (size_t i = 0; i < value.size(); i++) {
		if (!isdigit(value[i])) {
			return false;
		}
	}
	return true;
}

bool alertIfNotNumber(string value)
{
	if (!isNumber(value))
	{
		cout << "Неправильно введено число\n";
		return false;
	}
	return true;
}

enum class InputType
{
	None,
	Console,
	File
};

class CrsMatrix
{
public:
	CrsMatrix(int* a, int* lj, int* li, int notNullElements, int size)
	{
		_notNullElements = notNullElements;
		_size = size;
		A = new int[notNullElements];
		LJ = new int[notNullElements];
		LI = new int[size + 1];

		for (size_t i = 0; i < notNullElements; i++)
		{
			A[i] = a[i];
			LJ[i] = lj[i];
		}
		for (size_t i = 0; i < size + 1; i++)
		{
			LI[i] = li[i];
		}
	}

	void Print()
	{
		cout << "A:\t";
		for (size_t i = 0; i < _notNullElements; i++)
		{
			cout << to_string(A[i]) << " ";
		}
		cout << "\nLJ:\t";
		for (size_t i = 0; i < _notNullElements; i++)
		{
			cout << to_string(LJ[i] + 1) << " ";
		}
		cout << "\nLI:\t";
		for (size_t i = 0; i < _size + 1; i++)
		{
			cout << to_string(LI[i]) << " ";
		}
	}

private:
	int* A = nullptr;
	int* LJ = nullptr;
	int* LI = nullptr;

	int _notNullElements = 0;
	int _size = 0;
};


class CsMatrix
{
public:
	CsMatrix(InputType type) : unpackedMatrix(nullptr), A(nullptr), LI(nullptr), LJ(nullptr)
	{
		switch (type)
		{
		case InputType::Console:
			ReadFromConsole();
			break;
		case InputType::File:
			ReadFromFile();
			break;
		default:
			break;
		}
	}
	CsMatrix(int* a, int* lj, int* li, int notNullElements)
	{
		_notNullElements = notNullElements;
		AllocatePackMemory();

		for (size_t i = 0; i < notNullElements; i++)
		{
			A[i] = a[i];
			LJ[i] = lj[i];
			LI[i] = li[i];
		}
	}

	~CsMatrix()
	{
		ClearMemory();
		ClearPackMemory();
	}

	void ReadFromConsole()
	{
		string input;

		while (true)
		{
			cout << "Введите размер матрицы: ";
			cin >> input;

			if (alertIfNotNumber(input))
			{
				_size = atoi(input.c_str());
				break;
			}
		}

		AllocateMemory();

		bool incorrectInput = true;

		while (incorrectInput)
		{
			for (size_t i = 0; i < _size; i++)
			{
				for (size_t j = 0; j < _size; j++)
				{
					incorrectInput = false;
					int number;
					cin >> input;
					if (!alertIfNotNumber(input))
					{
						_size = atoi(input.c_str());
						incorrectInput = true;
						break;
					}

					number = atoi(input.c_str());

					if (number != 0)
						_notNullElements++;

					unpackedMatrix[i][j] = number;
				}
				if (incorrectInput)
				{
					cout << "Введите матрицу заново\n";
					ClearMemory();
					AllocateMemory();
					break;
				}
			}
		}
		
		PackMatrix();
	}
	void ReadFromFile()
	{
		string input;
		ifstream fin;

		while (true)
		{
			cout << "Введите имя файла: ";
			cin >> input;

			fin.open(input);

			if (fin.is_open())
			{
				break;
			}
			else
			{
				cout << "Неправильное имя файла, попробуйте еще раз\n";
			}
		}

		fin >> input;

		if (alertIfNotNumber(input))
		{
			_size = atoi(input.c_str());
		}
		else
		{
			cout << "Неверная матрица\n";
		}


		AllocateMemory();

		bool incorrectInput = true;

		for (size_t i = 0; i < _size; i++)
		{
			for (size_t j = 0; j < _size; j++)
			{
				incorrectInput = false;
				int number;
				fin >> input;
				if (!alertIfNotNumber(input))
				{
					_size = atoi(input.c_str());
					incorrectInput = true;
					break;
				}

				number = atoi(input.c_str());

				if (number != 0)
					_notNullElements++;

				unpackedMatrix[i][j] = number;
			}
			if (incorrectInput)
			{
				cout << "Введите матрицу заново\n";
				ClearMemory();
				AllocateMemory();
				break;
			}
		}

		PackMatrix();
	}
	
	void Print()
	{
		cout << "A:\t";
		for (size_t i = 0; i < _notNullElements; i++)
		{
			cout << to_string(A[i]) << " ";
		}
		cout << "\nLJ:\t";
		for (size_t i = 0; i < _notNullElements; i++)
		{
			cout << to_string(LJ[i] + 1) << " ";
		}
		cout << "\nLI:\t";
		for (size_t i = 0; i < _notNullElements; i++)
		{
			cout << to_string(LI[i] + 1) << " ";
		}
	}

	CrsMatrix* operator + (const CsMatrix& right)
	{
		if (_size != right._size)
		{
			cout << "нельзя сложить матрицы неравного размера\n";
			return nullptr;
		}

		int* Aout = new int[_size * _size] {0};
		int* LJout = new int[_size * _size] {0};
		int* LIout = new int[_size + 1] {0};
		
		int notNullElements = 0;
		
		int leftPtr = 0;
		int rightPrt = 0;
		int liPtr = -1;

		for (size_t i = 0; i < _notNullElements; i++)
		{
			for (size_t j = 0; j < right._notNullElements; j++)
			{
				bool found = false;
				if (LI[leftPtr] == i && LJ[leftPtr] == j)
				{
					Aout[notNullElements] = A[leftPtr];
					LJout[notNullElements] = j;

					if (LI[leftPtr] != liPtr)
					{
						liPtr = LI[leftPtr];
						LIout[liPtr] = notNullElements + 1;
					}

					notNullElements++;
					leftPtr++;
					found = true;
				}
				if (right.LI[rightPrt] == i && right.LJ[rightPrt] == j)
				{
					if (!found)
					{
						Aout[notNullElements] = right.A[rightPrt];
						LJout[notNullElements] = j;

						if (right.LI[rightPrt] != liPtr)
						{
							liPtr = right.LI[rightPrt];
							LIout[liPtr] = notNullElements + 1;
						}

						notNullElements++;
					}
					else
					{
						Aout[notNullElements - 1] += right.A[rightPrt];
					}
					rightPrt++;
					found = true;
				}
			}
		}
		LIout[_size] = notNullElements;

		CrsMatrix* out = new CrsMatrix(Aout, LJout, LIout, notNullElements, _size);
		return out;
	}

	int* GetA() { return A; }
	int* GetLJ() { return LJ; }
	int* GetLI() { return LI; }

private:
	void AllocateMemory()
	{
		unpackedMatrix = new int* [_size];
		for (size_t i = 0; i < _size; i++)
		{
			unpackedMatrix[i] = new int[_size];
		}
	}
	void ClearMemory()
	{
		for (size_t i = 0; i < _size; i++)
		{
			delete[] unpackedMatrix[i];
		}
		delete[] unpackedMatrix;
		unpackedMatrix = nullptr;
	}

	void AllocatePackMemory()
	{
		A = new int[_notNullElements];
		LI = new int[_notNullElements];
		LJ = new int[_notNullElements];
	}
	void ClearPackMemory()
	{
		delete A;
		delete LI;
		delete LJ;

		A = nullptr;
		LI = nullptr;
		LJ = nullptr;
	}

	void PackMatrix()
	{
		AllocatePackMemory();

		int currentElementIndex = 0;
		for (size_t i = 0; i < _size; i++)
		{
			for (size_t j = 0; j < _size; j++)
			{
				if (unpackedMatrix[i][j] != 0)
				{
					A[currentElementIndex] = unpackedMatrix[i][j];
					LI[currentElementIndex] = i;
					LJ[currentElementIndex] = j;
					currentElementIndex++;
				}
			}
		}
	}

	int** unpackedMatrix;
	int* A;
	int* LI;
	int* LJ;
	int _notNullElements = 0;
	int _size = 0;
};


int main()
{
	setlocale(LC_ALL, "Ru");

	const char* filename = "input.txt";

	int choise;
	cout << "1 - КОНСОЛЬ, 2 - ФАЙЛ, 3 - ВЫХОД\n";
	string inputNumber = "";
	cin >> choise;

	if (choise == 1)                    // Ввод с консоли
	{
		CsMatrix matrix1(InputType::Console);
		CsMatrix matrix2(InputType::Console);
		auto sum = matrix1 + matrix2;
		sum->Print();
	}
	else if (choise == 2)                // Ввод с файла
	{
		CsMatrix matrix1(InputType::File);
		CsMatrix matrix2(InputType::File);
		auto sum = matrix1 + matrix2;
		sum->Print();
	}
	else if (choise == 3)                // Выход
	{
		return 0;
	}
	else
	{
		cout << "Неправильный выбор\n";
	}


	return 0;
}
