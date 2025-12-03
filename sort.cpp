#include "sort.hpp"

using namespace std;

namespace {

const size_t kStaticArraySize = 5;
const bool kSwitchMin = false;
const bool kSwitchMax = true;
const size_t kLoopStart = 0;

//генератор рандом чисел для массива
void CreateRandomArray(int* sourceArray, size_t arraySize) {
    const int kminDistribution = 0; // мин значение
    const int kmaxDistribution = 99; // макс значение

    random_device r{};
    default_random_engine randomEngine(r());
    uniform_int_distribution distribution(kminDistribution, kmaxDistribution);
    // цикл по всем элементам массива
    for (size_t i = kLoopStart; i < arraySize; ++i) {
        sourceArray[i] = distribution(randomEngine); //заполнение элемента случ числом
    }
}

// ф-ция копирования массива в новый массив
void CreateCloneArray(int* arrNew, int* arrOld, size_t arraySize) {
    for (size_t i = kLoopStart; i < arraySize; ++i) {
        arrNew[i] = arrOld[i];
    }
}

//
void CheckDynamicArrayElementsNumber(size_t elementsNumber) {
    const size_t arrayElementsMinNumber = 2; //мин кол-во элементов
    const size_t arrayElementsMaxNumber = 1e5; //макс кол-во элементов
    //проверка диапозона
    if (elementsNumber < arrayElementsMinNumber || elementsNumber > arrayElementsMaxNumber) {
        cout << "Введены неверные данные" << endl;
        exit(0);
    }
}

// ввод номера задания пользователем
[[nodiscard]] int EnterTaskNumber() {
    int task{};
    cout << "Выберите (1/2/3)" << endl;
    cin >> task;
    return task;
}

// ввод кол-ва элементов динамического массива
[[nodiscard]] size_t EnterDynamicArrayElementsNumber() {
    size_t dynamicArrayElementsNumber{};
    cout << "Введите количество элементов динамического массива: (2 <= n <= 10000)" << endl;
    cin >> dynamicArrayElementsNumber;
    CheckDynamicArrayElementsNumber(dynamicArrayElementsNumber);
    return dynamicArrayElementsNumber;
}

// выбор задания (меню)
void SelectTask() {
    cout << "Выберите задание:" << endl;
    cout << "1. Сортировка статичного массива" << endl;
    cout << "2. Сортировка динамического массива" << endl;
    cout << "3. Выход" << endl;
}

// обмен значений двух int переменных
void FuncSwap(int& left, int& right) {
    int replacement = left;
    left = right;
    right = replacement;
}

// вывод элементов массива
void ArrayOutput(int* array, size_t arraySize) {
    for (size_t i = kLoopStart; i < arraySize; ++i) {
        cout << array[i] << " ";
    }
}

// форматированный вывод результатов сортировки в виде таблицы
void TableOutput(int* array, size_t arraySize, int permutationsNumber, int comparisonsNumber) {
    const int kWidthPermutationsNumber = 32; // ширина колонки для количества перестановок
    const int kWidthComparisonsNumber = 32; // ширина колонки для количества сравнений

    cout << '\t' << "Количество перестановок" << '\t' << '\t' << "Количество сравнений\n";
    ArrayOutput(array, arraySize);
    cout << setw(kWidthPermutationsNumber) << permutationsNumber << setw(kWidthComparisonsNumber) << comparisonsNumber << '\n';
}
}  // namespace

namespace InterfaceFunctions { // для ф-ций интерфейса

void LaunchApp() { // запуск программы
    char continueExecution = 'y';
    while (continueExecution == 'y') {
        Menu();

        cout << "Продолжить (y/n)?" << endl;
        cin >> continueExecution;

        if (continueExecution != 'y' && continueExecution != 'n') {
            cout << "Введены неверные данные" << endl;
            exit(0);
        }
    }
}

void Menu() { // главное меню
    SelectTask();
    int task = EnterTaskNumber();

    switch (static_cast<ArrayType>(task)) {
        case ArrayType::staticArr: {
            StaticArrayOutput();
            break;
        }
        case ArrayType::dynamicArr: {
            DynamicArrayOutput();
            break;
        }
        case ArrayType::exit: {
            cout << "Работа программы завершена" << endl;
            exit(0);
            break;
        }
        default: {
            cout << "error" << endl;
            break;
        }
    }
}

void StaticArrayOutput() { // для статичного массива
    int permutationsNumber = 0;
    int comparisonsNumber = 0;
    int sourceArray[kStaticArraySize]{};
    int cloneArray[kStaticArraySize]{};

    CreateRandomArray(sourceArray, kStaticArraySize);
    CreateCloneArray(cloneArray, sourceArray, kStaticArraySize);

    cout << "Исходный массив:\n";
    ArrayOutput(sourceArray, kStaticArraySize);

    SelectionSort(sourceArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    cout << "\nСортировка выбором по возрастанию 1";
    TableOutput(sourceArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    SelectionSort(sourceArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    cout << "Сортировка выбором по возрастанию 2";
    TableOutput(sourceArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    SelectionSort(sourceArray, kStaticArraySize, kSwitchMax, permutationsNumber, comparisonsNumber);
    cout << "Сортировка выбором по убыванию" << '\t';
    TableOutput(sourceArray, kStaticArraySize, permutationsNumber, comparisonsNumber);

    BubbleSort(cloneArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    cout << "Сортировка пузырьком по возрастанию 1";
    TableOutput(cloneArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    BubbleSort(cloneArray, kStaticArraySize, kSwitchMin, permutationsNumber, comparisonsNumber);
    cout << "Сортировка пузырьком по возрастанию 2";
    TableOutput(cloneArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
    BubbleSort(cloneArray, kStaticArraySize, kSwitchMax, permutationsNumber, comparisonsNumber);
    cout << "Сортировка пузырьком по убыванию";
    TableOutput(cloneArray, kStaticArraySize, permutationsNumber, comparisonsNumber);
}

void DynamicArrayOutput() { // для динамического массива
    int permutationsNumber{}; // счет перестановок
    int comparisonsNumber{}; // счет сравнений
    size_t dynamicArrayElementsNumber = EnterDynamicArrayElementsNumber();

    int* sourceArray = new int[dynamicArrayElementsNumber]; // выделение памяти для исходного массива
    int* cloneArray = new int[dynamicArrayElementsNumber]; // выделение памяти для массива-копии

    CreateRandomArray(sourceArray, dynamicArrayElementsNumber);
    CreateCloneArray(cloneArray, sourceArray, dynamicArrayElementsNumber);

    SelectionSort(sourceArray, dynamicArrayElementsNumber, kSwitchMin, permutationsNumber, comparisonsNumber); // сортировка выбором
    cout << "Количество перестановок и сравнений:" << endl;
    cout << permutationsNumber << ' ' << comparisonsNumber << endl;

    BubbleSort(cloneArray, dynamicArrayElementsNumber, kSwitchMin, permutationsNumber, comparisonsNumber); // сортировка пузырьком
    cout << "Количество перестановок и сравнений: " << endl;
    cout << permutationsNumber << ' ' << comparisonsNumber << endl;

    //освобождение памяти перестановок и сравнений
    delete[] sourceArray;
    delete[] cloneArray;
}

// алгоритм сортировки выбором
void SelectionSort(int* arrSelection, size_t arraySize, bool switchMinMax, int& permutationsNumber, int& comparisionsNumber) {
    permutationsNumber = 0;
    comparisionsNumber = 0;
    size_t rememberedIndex{};

    for (size_t i = kLoopStart; i < arraySize; i++) {
        rememberedIndex = i;

        for (size_t t = i + 1; t < arraySize; t++) {
            if (switchMinMax ? arrSelection[t] > arrSelection[rememberedIndex] : arrSelection[t] < arrSelection[rememberedIndex]) {
                rememberedIndex = t;
            }
            ++comparisionsNumber;
        }

        if (arrSelection[i] != arrSelection[rememberedIndex]) {
            FuncSwap(arrSelection[i], arrSelection[rememberedIndex]);
            ++permutationsNumber;
        }
    }
}

//алгоритм сортировки пузырьком
void BubbleSort(int* arrBubble, size_t arraySize, bool switchMinMax, int& permutationsNumber, int& comparisionsNumber) {
    const int kcounterInitialValue = 0;
    int counter = 1;
    permutationsNumber = 0;
    comparisionsNumber = 0;
    int occupiedCellsNumber = 0;

    while (counter != kcounterInitialValue) {
        counter = kcounterInitialValue;
        for (size_t i = kLoopStart; i < arraySize - 1 - occupiedCellsNumber; ++i) {
            if (switchMinMax ? arrBubble[i] < arrBubble[i + 1] : arrBubble[i] > arrBubble[i + 1]) {
                FuncSwap(arrBubble[i], arrBubble[i + 1]);
                ++counter;
                ++permutationsNumber;
            }
            ++comparisionsNumber;
        }
        ++occupiedCellsNumber;
    }
}

}  // namespace InterfaceFunctions
