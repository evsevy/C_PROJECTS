/******************************************************************************
Этот код принимает координаты трех городов (Москва, Санкт-Петербург, Екатеринбург), 
запрашивает у пользователя точку отправления и цель, 
вычисляет расстояние между ними и выводит результат в километрах.
Mergulov E. V. 2024
*******************************************************************************/

#include <iostream>
#include <cmath>
#include <vector>
#include <map>

// Структура для хранения географических координат
struct Coordinate {
    double latitude;
    double longitude;
};

// Функция для вычисления расстояния между двумя точками на поверхности Земли
double calculateDistance(Coordinate start, Coordinate end) {
    // Радиус Земли в км
    const double radius = 6371.0;
    
    // Преобразуем географические координаты в радианы
    double lat1 = start.latitude * M_PI / 180;
    double lon1 = start.longitude * M_PI / 180;
    double lat2 = end.latitude * M_PI / 180;
    double lon2 = end.longitude * M_PI / 180;
    
    // Вычисляем расстояние по формуле гаверсинусов
    double dlon = lon2 - lon1;
    double dlat = lat2 - lat1;
    double a = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return radius * c;
}

int main() {
    // База данных с координатами точек
    std::map<std::string, Coordinate> coordinates = {
        {"A", {55.7558, 37.6176}}, // Москва
        {"B", {59.9343, 30.3351}}, // Санкт-Петербург
        {"C", {56.8389, 60.6057}}  // Екатеринбург
    };
    
    // Запрашиваем у пользователя точку и цель
    std::string start, end;
    std::cout << "Введите точку отправления (A, B, C): ";
    std::cin >> start;
    std::cout << "Введите цель (A, B, C): ";
    std::cin >> end;
    
    // Проверяем наличие точек в базе данных
    if (coordinates.find(start) == coordinates.end() || coordinates.find(end) == coordinates.end()) {
        std::cout << "Точка не найдена в базе данных";
        return 1;
    }
    
    // Вычисляем расстояние между точками
    double distance = calculateDistance(coordinates[start], coordinates[end]);
    
    // Выводим результат
    std::cout << "Расстояние от точки " << start << " до цели " << end << " составляет " << distance << " км";
    
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Для этой программы мы будем использовать библиотеку geopy для работы с географическими данными и вычисления пути между координатами. 
Прежде всего, необходимо установить библиотеку geopy, если она еще не установлена.
Для этого можно воспользоваться командой в командной строке: pip install geopy 
Код реализует расчет пути от Минска до цели с использованием данных из базы данных geopy. 
Предполагается, что база данных geopy уже содержит информацию о географических координатах столиц государств мира.
*/
#include <iostream>
#include <geopy/geocoders/nominatim.hpp>
#include <geopy/distance.hpp>

int main() {
    geopy::geocoders::Nominatim geocoder;

    // Координаты Минска
    geopy::Location start = geocoder.geocode("Минск");
    
    // Координаты цели (например, Рима)
    geopy::Location end = geocoder.geocode("Рим");

    // Вычисляем расстояние между Минском и целью
    geopy::Distance distance;
    auto result = distance(start, end);

    std::cout << "Расстояние от Минска до цели: " << result.km << " км" << std::endl;

    // Получаем направление движения до цели
    auto directions = distance.directions(start, end);

    std::cout << "Направления движения до цели:" << std::endl;
    for (const auto& direction : directions) {
        std::cout << direction << std::endl;
    }

    return 0;
}
/*
Этот код использует функционал библиотеки geopy для получения координат Минска и цели, 
вычисления расстояния между ними и получения последовательности направлений движения до цели. 
Результаты выводятся на экран.
Lля работы программы необходимо подключение к интернету, 
так как библиотека geopy использует API для получения географических данных.
*/