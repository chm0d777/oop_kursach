#include <iostream>
#include <vector>
#include <limits>
#include <ctime>

using namespace std;

//узел дерева для отражения логики выбора хода
struct node {
	int alpha, beta, value, i, j, figure;
	node* right; //указатель на правое поддерево
	node* left; //на левое

	//конструктор
	node() {
		alpha = beta = value = i = j = figure = 0;
		right = left = nullptr;
	}

	//вывод информации узла
	void print(int lvl) {
		for (int i = 0; i < lvl; i++) {
			cout << "   ";
		}
		//конечный узел
		if ((left == right) && !left) {
			cout << "(" << value << "), ";
			cout << "[" << i << "," << j << "] – " << ((figure == 1) ? 'x' : 'o') << endl;
		}
		else {
			//правило
			cout << "(";
			if (alpha == numeric_limits<int>::min()) {
				cout << "-inf" << "; ";
			}
			else {
				cout << alpha << "; ";
			}

			if (beta == numeric_limits<int>::max()) {
				cout << "+inf" << "; ";
			}
			else {
				cout << beta << "; ";
			}

			if (value == numeric_limits<int>::min()) {
				cout << "-inf";
			}
			else if (value == numeric_limits<int>::max()) {
				cout << "+inf";
			}
			else {
				cout << value;
			}
			if (figure != 0) {
				cout << "), ";
				cout << "[" << i << "," << j << "] -> " << ((figure == 1) ? 'x' : 'o') << endl;
			}
			else {
				cout << ")" << endl;
			}
		}
	}
};

//рекурсивный вывод дерева
void print(node* root, int lvl = 0) {
	if (root) {
		print(root->right, lvl + 1);
		root->print(lvl);
		print(root->left, lvl + 1);
	}
}

//очистка дерева
void clear(node* root) {
	if (root) {
		clear(root->left);
		clear(root->right);
		delete root;
	}
}

//класс хранящий данные о поле
class TicTacToe {
private:
	vector<vector<int>> board; //доска для игры
	int n; //размер доски
	int winCount; //кол-во крестиков или ноликов необходимых для победы
	int free; //кол-во свободных клеток
	int maxdepth; //максимальная глубина рекурсии

public:

	//конструктор
	TicTacToe(int n, int winCount) {
		reScale(n);
		winState(winCount);
		reset();
		maxdepth = 7;
	}

	//вывод поля
	void makeVisible() const {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {

				//пусто
				if (board[i][j] == 0) {
					cout << "| |";
				}
				else if (board[i][j] == 1) {

					//крестик
					cout << "|x|";
				}
				else {

					//нолик
					cout << "|o|"; //-1
				}
			}
			cout << endl;
		}
	}

	//изменить размер поля
	bool reScale(int value) {

		//проверка размера поля
		if (value > 2 && value < 7) {
			n = value;
			return true;
		}
		else {
			return false;
		}
	}

	//изменение кол-во знаков для выигрыша
	bool winState(int value) {
		if (value <= 2 || value > n) {
			return false;
		}
		else {
			winCount = value;
			return true;
		}
	}

	//суммируем сплошной ряд
	int sum(int i, int j, const pair<int, int>& dir, int sign) const {
		int s = 0; //кол-во одинаковых знаков
		bool flag = true; //флаг для выхода

		//цикл без учета i,j координаты
		for (int k = 1; k < n && flag; k++) {
			int x = j + k * dir.first;
			int y = i + k * dir.second;

			//вышли за границы поля
			if (x < 0 || x >= n || y < 0 || y >= n) {
				flag = false;
			}
			else {

				//если далее идет нужный знак
				if (board[y][x] == sign) {
					s++;
				}
				else
					flag = false;
			}
		}
		return s;
	}

	//проверяем сплошной ряд знаков
	int winner(int i, int j, int sign) const {

		//возможные направления
		const vector<pair<int, int>> LineChecker = {

			//право-лево
			{1,0},
			{-1,0},

			//вверх-низ
			{0,-1},
			{0,1},

			//главная диагональ
			{1,1},
			{-1,-1},

			//побочная
			{1,-1},
			{-1,1}
		};

		//по всем направлениям
		int s1 = 0;
		int s2 = 0;
		int maxsum = 0;
		for (int k = 0; k < LineChecker.size(); k += 2) {
			s1 = sum(i, j, LineChecker[k], sign);
			s2 = sum(i, j, LineChecker[k + 1], sign);
			if ((s1 + s2 + 1) == winCount) {
				return sign;
			}
			if (maxsum < s1 + s2 + 1) {
				maxsum = s1 + s2 + 1;
			}
		}

		//если нет свободных клеток, то ничья иначе нет победителя
		return (free == 0) ? 0 : maxsum + 1;
	}

	//алгоритм минимакса + альфа-бета отсечение
	int minimax(node*& root, int y, int x, int alpha, int beta, bool isMax, int depth = 1) {

		//новый узел дерева
		if (!root) {
			root = new node();
			root->figure = (isMax) ? 1 : -1;
			root->i = y;
			root->j = x;
		}

		//считаем кто выиграл
		int win = winner(y, x, ((isMax) ? 1 : -1));

		//если достигли максимальной глубины, то выдаем наибольший ряд
		if (depth >= maxdepth) {
			root->value = (isMax) ? -win : win;
			return root->value;
		}
		else {
			switch (win) {
			case 1: {
				//выиграли крестики
				root->value = -maxdepth * 2;
				return root->value;
			}
			case -1: {
				//выиграли нолики
				root->value = maxdepth * 2;
				return root->value;
			}
			case 0: {
				//ничья
				root->value = 0;
				return root->value;
			}
			default: {
				//ход max игрока
				if (isMax) {
					int value = numeric_limits<int>::min();

					//обход свободных клеток
					for (int i = 0; i < n; i++) {
						for (int j = 0; j < n; j++) {
							if (board[i][j] == 0) {
								board[i][j] = -1;
								free--;

								value = max(value, minimax(root->left, i, j, alpha, beta, false, depth + 1));
								alpha = max(alpha, value);

								board[i][j] = 0;
								free++;

								if (value >= beta) {
									root->alpha = alpha;
									root->beta = beta;
									root->value = value;
									return value;
								}
							}
						}
					}
					root->alpha = alpha;
					root->beta = beta;
					root->value = value;
					return value;
				}
				else {
					int value = numeric_limits<int>::max();

					//ход min игрока
					for (int i = 0; i < n; i++) {
						for (int j = 0; j < n; j++) {
							if (board[i][j] == 0) {
								board[i][j] = 1;
								free--;

								value = min(value, minimax(root->right, i, j, alpha, beta, true, depth + 1));
								beta = min(beta, value);

								board[i][j] = 0;
								free++;

								if (value <= alpha) {
									root->alpha = alpha;
									root->beta = beta;
									root->value = value;
									return value;
								}
							}
						}
					}
					root->alpha = alpha;
					root->beta = beta;
					root->value = value;
					return value;
				}
			}
			}
		}
	}


	//поиск наилучшего хода для бота (поиск максимума)
	void bestMove(int& imax, int& jmax, int verbose) {

		//поиск максимального выигрыша для бота
		vector<vector<int>> field(n, vector<int>(n, -1000));
		int maxval = numeric_limits<int>::min();
		int value = numeric_limits<int>::min();
		int alpha = numeric_limits<int>::min();
		constexpr int beta = numeric_limits<int>::max();
		vector<node*> roots;

		//цикл по каждой свободной клетке
		clock_t start = clock();
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {

				//если свободная клетка
				if (board[i][j] == 0) {
					board[i][j] = -1;
					free--;

					roots.push_back(new node());
					node* root = roots[roots.size() - 1];
					field[i][j] = minimax(root->left, i, j, alpha, beta, false);

					root->value = field[i][j];
					root->alpha = alpha;
					root->beta = beta;
					root->figure = 0;

					value = max(value, field[i][j]);
					alpha = max(alpha, value);
					if (maxval < value) {
						maxval = value;
						imax = i;
						jmax = j;
					}

					board[i][j] = 0;
					free++;
				}
			}
		}
		clock_t end = clock();
		// Вычисляем разницу между началом и концом выполнения
		double duration = (end - start) / (double)CLOCKS_PER_SEC;

		// Выводим результат
		std::cout << "Время выполнения: " << duration << " секунд." << std::endl;


		//вывод сформированного поля возможных ходов
		if (verbose) {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					if (field[i][j] == -1000) {
						cout << "| |";
					}
					else {
						cout << "|" << field[i][j] << "|";
					}
				}
				cout << endl;
			}
			cout << "\nОптимальная клетка – (" << imax << "," << jmax << ") -> " << maxval << endl;
			cout << "Время хода: " << (double)(end - start) / CLOCKS_PER_SEC << " сек." << endl;
			cout << "Показать подробности y/n?" << endl;
			char y;
			cin >> y;
			if (y == 'y') {
				cout << "(alpha, beta, value)" << endl;
				for (int i = 0; i < roots.size(); i++) {
					print(roots[i]);
					cout << endl;
				}
				cout << "Для продолжения нажмите enter" << endl;
				getchar(); getchar();
			}
		}
		for (int i = 0; i < roots.size(); i++) {
			clear(roots[i]);
		}
	}

	//цикл игры бота против человека
	void start(int verbose = 0) {
		int i = 0, j = 0; //текущие координаты
		int sign = 1; //чей первый ход
		string info = ""; //для вывода сообщений
		bool run = true; //флаг игры
		reset();

		while (run) {
			do {
				system("cls"); //очистка экрана
				makeVisible();
				cout << endl;
				if (info.size() > 0) {
					cout << info << endl << endl;
				}
				if (sign == 1) {
					cout << "Ход крестиков:" << endl;
				}
				else {
					cout << "Ход ноликов:" << endl;
				}
				cout << "Выберите клетку для хода (от 1 1): ";
				cin >> i >> j;
				i = i - 1;
				j = j - 1;

				//ошибка ввода
				if (cin.fail()) {
					cin.clear();
					cin.ignore(numeric_limits<int>::max(), '\n');
					info = "Указаны неверные координаты";
				}
				else

					//проверка координат
					if (i >= 0 && i < n && j >= 0 && j < n) {

						//проверка доски
						if (board[i][j] == 0) {
							board[i][j] = sign;
							free--;
							break; //все верно
						}
						else {
							info = "Клетка уже используется";
						}
					}
					else {
						info = "Указаны неверные координаты";
					}
			} while (1);

			//проверка выигрыша
			int win = winner(i, j, sign);
			switch (win) {
			case 1: {
				cout << "Выиграли крестики" << endl;
				break;
			}
			case -1: {
				cout << "Выиграли нолики" << endl;
				break;
			}
			case 0: {
				cout << "Ничья" << endl;
				break;
			}
			}

			//если нет победителя и есть свободные клетки
			if (win > 1) {
				sign = (sign == 1) ? -1 : 1; //меняем игрока
				info = "";
				cout << endl;

				//ход компьютера
				int imax = -1;
				int jmax = -1;
				bestMove(imax, jmax, verbose);
				if (imax != -1 && jmax != -1) {
					board[imax][jmax] = sign;
					free--;

					//проверка выигрыша
					int win = winner(imax, jmax, sign);
					switch (win) {
					case 1: {
						cout << "Выиграли крестики" << endl;
						break;
					}
					case -1: {
						cout << "Выиграли нолики" << endl;
						break;
					}
					case 0: {
						cout << "Ничья" << endl;
						break;
					}
					}

					//компьютер выиграл
					if (win == 1 || win == -1 || win == 0)
						run = false;
				}

				sign = (sign == 1) ? -1 : 1; //меняем игрока
			}
			else {
				run = false; //конец игры
			}
		}

		makeVisible();
		cout << endl;
	}

	//сброс игры
	void reset() {
		free = n * n;
		board = vector<vector<int>>(n, vector<int>(n, 0));
	}
};

//тест игры
int main() {
	setlocale(LC_ALL, "RU");
	system("chcp 1251");
	system("cls");
	int matrix = 3;  // Размер поля по умолчанию
	cout << "Введите размер поля:" << endl;
	cin >> matrix;
	TicTacToe g(matrix, matrix); // Создание объекта TicTacToe с размером поля 3x3
	int n = matrix;
	int winCount = 3; // Количество крестиков или ноликов для победы по умолчанию
	cout << "Введите количество крестиков или ноликов для победы:" << endl;
	cin >> winCount;
	// Проверяем, можно ли установить размер поля и выигрышное количество
	if (!g.reScale(n) || !g.winState(winCount)) {
		cout << "Ошибка при установке параметров игры." << endl;
		system("pause");
		return 0;
	}

	// Выбираем, нужно ли выводить поиск хода для бота
	char y = 'y'; // По умолчанию выводим поиск хода для бота
	cout << "Выводить поиск хода для бота y/n?" << endl;
	cin >> y;
	if (y == 'n') {
		g.start(0); // Начинаем игру, без вывода поиска хода для бота
	}
	else {
		g.start(1); // Начинаем игру, с выводом поиска хода для бота
	}

	cout << endl;
	system("pause");
	return 0;
}
