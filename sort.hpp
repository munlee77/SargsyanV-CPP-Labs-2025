#include <iomanip>
#include <iostream>
#include "random"
namespace InterfaceFunctions {
enum class ArrayType {
    staticArr = 1,
    dynamicArr,
    exit,
};
void SelectionSort(int*, size_t, bool, int&, int&);
void BubbleSort(int*, size_t, bool, int&, int&);
void StaticArrayOutput();
void DynamicArrayOutput();
void Menu();
void LaunchApp();
}  // namespace InterfaceFunctions