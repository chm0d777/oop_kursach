#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>

class KnapsackProblem {
protected:
    int capacity;
    std::vector<int> weights;
    std::vector<int> values;
    std::vector<int> selectedItems;

public:
    KnapsackProblem(int capacity) : capacity(capacity) {}

    void addItem(int weight, int value) {
        weights.push_back(weight);
        values.push_back(value);
    }

    virtual void solve() = 0;

    void printSolution() {
        std::cout << "Выбранные предметы:" << std::endl;
        for (int item : selectedItems) {
            std::cout << "Предмет " << item + 1 << ": Вес: " << weights[item] << ", Ценность: " << values[item] << std::endl;
        }
    }

    int getSolutionValue() const {
        int totalValue = 0;
        for (int item : selectedItems) {
            totalValue += values[item];
        }
        return totalValue;
    }
};

class SingleChoiceKnapsackProblem : public KnapsackProblem {
public:
    using KnapsackProblem::KnapsackProblem;

    void solve() override {
        int n = weights.size();
        std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));

        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= capacity; ++j) {
                if (weights[i - 1] <= j) {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i - 1][j - weights[i - 1]] + values[i - 1]);
                }
                else {
                    dp[i][j] = dp[i - 1][j];
                }
            }
        }

        int maxValue = dp[n][capacity];

        // Восстановление выбранных предметов
        int j = capacity;
        for (int i = n; i > 0 && maxValue > 0; --i) {
            if (maxValue != dp[i - 1][j]) {
                selectedItems.push_back(i - 1);
                maxValue -= values[i - 1];
                j -= weights[i - 1];
            }
        }

        std::reverse(selectedItems.begin(), selectedItems.end());
    }
};

class MultiChoiceKnapsackProblem : public KnapsackProblem {
private:
    int numGroups;
    std::vector<std::vector<int>> groupItems;

public:
    MultiChoiceKnapsackProblem(int capacity, int numGroups)
        : KnapsackProblem(capacity), numGroups(numGroups) {
        groupItems.resize(numGroups);
    }

    void addItem(int group, int weight, int value) {
        if (group < 0 || group >= numGroups) {
            throw std::invalid_argument("Недопустимый номер группы.");
        }

        groupItems[group].push_back(weights.size());
        KnapsackProblem::addItem(weight, value);
    }

    void solve() override {
        int n = weights.size();
        std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));

        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= capacity; ++j) {
                if (weights[i - 1] <= j) {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i - 1][j - weights[i - 1]] + values[i - 1]);
                }
                else {
                    dp[i][j] = dp[i - 1][j];
                }
            }
        }

        int maxValue = dp[n][capacity];

        // Восстановление выбранных предметов
        for (int group = 0; group < numGroups; ++group) {
            int groupMaxValue = 0;
            int selectedItem = -1;

            for (int item : groupItems[group]) {
                if (values[item] > groupMaxValue && weights[item] <= capacity) {
                    groupMaxValue = values[item];
                    selectedItem = item;
                }
            }

            if (selectedItem != -1) {
                selectedItems.push_back(selectedItem);
                maxValue += groupMaxValue - values[selectedItem];
                capacity -= weights[selectedItem];
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    try {
        std::cout << "Выберите тип задачи о рюкзаке:" << std::endl;
        std::cout << "1. Задача о рюкзаке" << std::endl;
        std::cout << "2. Задача о рюкзаке с мультивыбором" << std::endl;
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "Введите вместимость рюкзака: ";
            int capacity;
            std::cin >> capacity;

            std::cout << "Введите количество предметов: ";
            int numItems;
            std::cin >> numItems;

            SingleChoiceKnapsackProblem problem(capacity);

            for (int i = 0; i < numItems; ++i) {
                std::cout << "Введите вес и ценность предмета " << i + 1 << ": ";
                int weight, value;
                std::cin >> weight >> value;
                problem.addItem(weight, value);
            }

            problem.solve();

            std::cout << "Максимальная ценность предметов в рюкзаке: " << problem.getSolutionValue() << std::endl;
            problem.printSolution();
        }
        else if (choice == 2) {
            std::cout << "Введите вместимость рюкзака: ";
            int capacity;
            std::cin >> capacity;

            std::cout << "Введите количество групп: ";
            int numGroups;
            std::cin >> numGroups;

            MultiChoiceKnapsackProblem problem(capacity, numGroups);

            for (int i = 0; i < numGroups; ++i) {
                std::cout << "Введите количество предметов в группе " << i + 1 << ": ";
                int numItems;
                std::cin >> numItems;

                for (int j = 0; j < numItems; ++j) {
                    std::cout << "Введите вес и ценность предмета " << j + 1 << " в группе " << i + 1 << ": ";
                    int weight, value;
                    std::cin >> weight >> value;
                    problem.addItem(i, weight, value);
                }
            }

            problem.solve();

            std::cout << "Максимальная ценность предметов в рюкзаке: " << problem.getSolutionValue() << std::endl;
            problem.printSolution();
        }
        else {
            throw std::invalid_argument("Недопустимый выбор.");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}



