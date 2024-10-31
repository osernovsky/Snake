#include <ncurses.h> // Библиотека упавления терминалом
// #include <stdlib.h> // Вызов функций рандомайзера
// #include <unistd.h>
// #include <time.h>

#define WIDTH 40 // Задаем размеры игрового поля
#define HEIGHT 40
#define NUM_FOODS 5 // Количество яблок на поле

struct Point { // Структура координат точки (тело змеи, яблоки, препятствия)
    int x, y;
};

struct Snake {
    struct Point *body; // Массив коордитат тела
    int length; // Длина змеи
    int dx, dy; // Направление движения
};

struct Food {
    struct Point *foods; // Массив коордитат яблок
    int eat; // Какое яблоко съедено, 0 если нет 
};

// Функция инициализации игры

void start_game (struct Snake *snake, struct Food *food){

    snake->length = 3; // начальная длина змеи
    snake->body = malloc(snake->length * sizeof (struct Point)); // выделяем память
    snake->body[0] = (struct Point) {WIDTH / 2, HEIGHT / 2}; // задаем начальную позицию
    snake->body[1] = (struct Point) {WIDTH / 2 - 1, HEIGHT / 2};
    snake->body[2] = (struct Point) {WIDTH / 2 - 2, HEIGHT / 2};
    snake->dx = 1; // змея стартует вправо
    snake->dy = 0;

    srand(time(NULL)); // рассыпаем начальные яблоки
//    int foodx, foody;
    bool food_collision = FALSE;

    for(int i = 0; i < NUM_FOODS; i++){
        do{
            food->foods[i].x = rand() % WIDTH;
            food->foods[i].y = rand() % HEIGHT;

            // проверяем коллизии со змеёй
            for(int j = 0; j < snake->length; j++){
                
            }
            // проверяем коллизии с остальными яблоками

        } while (food_collision);
        food_collision = FALSE;
    }

}

int main (void){
    return 0;
}