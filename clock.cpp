#include <iostream>

namespace {
const int tDecimalNum = 10;        // десятичное число
const int TwelveHoursFormat = 12;  // 12-числовой формат

const int tMidnight = 0;  // полночь
const int tMidday = 12;   // полдень

const int tMinMinutes = 0;   // мин. значение минут
const int tMaxMinutes = 59;  // макс. значение минут
const int tMinHours = 0;     // мин. значение часов
const int tMaxHours = 23;    // макс. значение часов

const int tNightBeg = 0;     // ночь - начало
const int tNightEnd = 4;     // ночь - конец
const int tMorningBeg = 5;   // утро - начало
const int tMorningEnd = 11;  // утро - конец
const int tDayBeg = 12;      // день - начало
const int tDayEnd = 17;      // день - конец
const int tEveningBeg = 18;  // вечер - начало
const int tEveningEnd = 23;  // вечер - конец

const int tNominSinMinutes = 1;    // минуты в им. падеже, ед. ч.
const int tGenSinMinutesLow = 2;   // минуты в род. падеже, ед. ч. (нижняя граница)
const int tGenSinMinutesUpp = 4;   // минуты в род. падеже, ед. ч. (верхняя граница)
const int tGenPluMinutesLow = 11;  // минуты в род. падеже, мн. ч. (нижняя граница)
const int tGenPluMinutesUpp = 14;  // минуты в род. падеже, мн. ч. (верхняя граница)
const int tNominSinHours = 1;      // часы в им. падеже, ед. ч.
const int tGenSinHoursLow = 2;     // часы в род. падеже, ед. ч. (нижняя граница)
const int tGenSinHoursUpp = 4;     // часы в род. падеже, ед. ч. (верхняя граница)
}  // namespace

int main(int, char**) {
    int hours = 0;    // час
    int minutes = 0;  // минуты

    std::cout << "Введите время в формате *Часы Минуты* при помощи цифр. (0 <= Часы < 24, 0 <= Минуты < 60)\n";
    std::cin >> hours >> minutes;

    if (hours < tMinHours || hours > tMaxHours || minutes < tMinMinutes || minutes > tMaxMinutes ||
        std::cin.fail()) {  // выдает ошибку при неверном вводе
        std::cout << "Введены недопустимые данные\n";
        return 1;
    }

    if ((hours == tMidday) && (minutes == tMinMinutes)) {  // Если точно 12:00, пишет "Полдень"
        std::cout << "Полдень\n";
        return 0;
    } else if ((hours == tMidnight) && (minutes == tMinMinutes)) {  // Если точно 0:00 — "Полночь".
        std::cout << "Полночь\n";
        return 0;
    }

    int t12HoursFormat =
        (hours > TwelveHoursFormat) ? (hours - TwelveHoursFormat) : hours;  // С помощью тернарного оператора устанавливаем значение часов

    std::cout << t12HoursFormat;
    if (t12HoursFormat == tNominSinHours) {  // Вывод часов с правильным окончанием
        std::cout << " час";
    } else if ((t12HoursFormat >= tGenSinHoursLow) && (t12HoursFormat <= tGenSinHoursUpp)) {
        std::cout << " часа";
    } else {
        std::cout << " часов";
    }

    if (minutes != tMinMinutes) {                    // Если не ноль, вывод минут с правильным окончанием
        int lastDigMinutes = minutes % tDecimalNum;  // последняя цифра минут
        std::cout << ' ' << minutes;
        if (lastDigMinutes == tNominSinMinutes && minutes != tGenPluMinutesLow) {
            std::cout << " минута";
        } else if ((lastDigMinutes >= tGenSinMinutesLow) && (lastDigMinutes <= tGenSinMinutesUpp) &&
                   (minutes < tGenPluMinutesLow || minutes > tGenPluMinutesUpp)) {
            std::cout << " минуты";
        } else {
            std::cout << " минут";
        }
    }

    if ((hours >= tNightBeg) && (hours <= tNightEnd)) {  // Определение времени суток
        std::cout << " ночи";
    } else if ((hours >= tMorningBeg) && (hours <= tMorningEnd)) {
        std::cout << " утра";
    } else if ((hours >= tDayBeg) && (hours <= tDayEnd)) {
        std::cout << " дня";
    } else if ((hours >= tEveningBeg) && (hours <= tEveningEnd)) {
        std::cout << " вечера";
    }

    if (minutes == tMinMinutes) {  // Если минут 0, выводится "ровно"
        std::cout << " ровно";
    }
    std::cout << '\n';
    return 0;
}
