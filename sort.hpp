// Заголовочный файл, содержащий объявления функций для сортировки

#include <iomanip>
#include <iostream>
#include "random"
namespace InterfaceFunctions {
enum class ArrayType {
    staticArr = 1,
    dynamicArr,
    exit,
};

// Объявления функций:

//Алгоритм сортировки выбором
//Параметры:
//int* - указатель на массив
//size_t - размер массива
//bool - флаг направления сортировки (true - по убыванию, false - по возрастанию)
//int& - счетчик перестановок (передается по ссылке)
//int& - счетчик сравнений (передается по ссылке)
void SelectionSort(int*, size_t, bool, int&, int&);
void BubbleSort(int*, size_t, bool, int&, int&);
void StaticArrayOutput();
void DynamicArrayOutput();
void Menu();
void LaunchApp();
}  // namespace InterfaceFunctions
