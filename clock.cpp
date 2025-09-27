#include <iostream>

namespace {
const int kDecimalNum = 10;        // десятичное число
const int kTwelveHoursFormat = 12;  // 12-числовой формат

const int kMidnight = 0;  // полночь
const int kMidday = 12;   // полдень

const int kMinMinutes = 0;   // мин. значение минут
const int kMaxMinutes = 59;  // макс. значение минут
const int kMinHours = 0;     // мин. значение часов
const int kMaxHours = 23;    // макс. значение часов

const int kNightBeg = 0;     // ночь - начало
const int kNightEnd = 4;     // ночь - конец
const int kMorningBeg = 5;   // утро - начало
const int kMorningEnd = 11;  // утро - конец
const int kDayBeg = 12;      // день - начало
const int kDayEnd = 17;      // день - конец
const int kEveningBeg = 18;  // вечер - начало
const int kEveningEnd = 23;  // вечер - конец

const int kNominSinMinutes = 1;    // минуты в им. падеже, ед. ч.
const int kGenSinMinutesLow = 2;   // минуты в род. падеже, ед. ч. (нижняя граница)
const int kGenSinMinutesUpp = 4;   // минуты в род. падеже, ед. ч. (верхняя граница)
const int kGenPluMinutesLow = 11;  // минуты в род. падеже, мн. ч. (нижняя граница)
const int kGenPluMinutesUpp = 14;  // минуты в род. падеже, мн. ч. (верхняя граница)
const int kNominSinHours = 1;      // часы в им. падеже, ед. ч.
const int kGenSinHoursLow = 2;     // часы в род. падеже, ед. ч. (нижняя граница)
const int kGenSinHoursUpp = 4;     // часы в род. падеже, ед. ч. (верхняя граница)
}  // namespace

int main(int, char**) {
    int hours = 0;    // час
    int minutes = 0;  // минуты

    std::cout << "Введите время в формате *Часы Минуты* при помощи цифр. (0 <= Часы < 24, 0 <= Минуты < 60)\n";
    std::cin >> hours >> minutes;

    if (hours < kMinHours || hours > kMaxHours || minutes < kMinMinutes || minutes > kMaxMinutes ||
        std::cin.fail()) {  // выдает ошибку при неверном вводе
        std::cout << "Введены недопустимые данные\n";
        return 1;
    }

    if ((hours == kMidday) && (minutes == kMinMinutes)) {  // Если точно 12:00, пишет "Полдень"
        std::cout << "Полдень\n";
        return 0;
    } else if ((hours == kMidnight) && (minutes == kMinMinutes)) {  // Если точно 0:00 — "Полночь".
        std::cout << "Полночь\n";
        return 0;
    }

    int k12HoursFormat =
        (hours > kTwelveHoursFormat) ? (hours - kTwelveHoursFormat) : hours;  // С помощью тернарного оператора устанавливаем значение часов

    std::cout << k12HoursFormat;
    if (k12HoursFormat == kNominSinHours) {  // Вывод часов с правильным окончанием
        std::cout << " час";
    } else if ((k12HoursFormat >= kGenSinHoursLow) && (k12HoursFormat <= kGenSinHoursUpp)) {
        std::cout << " часа";
    } else {
        std::cout << " часов";
    }

    if (minutes != kMinMinutes) {                    // Если не ноль, вывод минут с правильным окончанием
        int lastDigMinutes = minutes % kDecimalNum;  // последняя цифра минут
        std::cout << ' ' << minutes;
        if (lastDigMinutes == kNominSinMinutes && minutes != kGenPluMinutesLow) {
            std::cout << " минута";
        } else if ((lastDigMinutes >= kGenSinMinutesLow) && (lastDigMinutes <= kGenSinMinutesUpp) &&
                   (minutes < kGenPluMinutesLow || minutes > kGenPluMinutesUpp)) {
            std::cout << " минуты";
        } else {
            std::cout << " минут";
        }
    }

    if ((hours >= kNightBeg) && (hours <= kNightEnd)) {  // Определение времени суток
        std::cout << " ночи";
    } else if ((hours >= kMorningBeg) && (hours <= kMorningEnd)) {
        std::cout << " утра";
    } else if ((hours >= kDayBeg) && (hours <= kDayEnd)) {
        std::cout << " дня";
    } else if ((hours >= kEveningBeg) && (hours <= kEveningEnd)) {
        std::cout << " вечера";
    }

    if (minutes == kMinMinutes) {  // Если минут 0, выводится "ровно"
        std::cout << " ровно";
    }
    std::cout << '\n';
    return 0;
}
