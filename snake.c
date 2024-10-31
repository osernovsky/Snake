#include <ncurses.h> // Библиотека упавления терминалом
// #include <stdlib.h> // Вызов функций рандомайзера
// #include <unistd.h>
// #include <time.h>

#define WIDTH 40 // Задаем размеры игрового поля, без учета рамки
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
    snake->body = malloc(snake->length * sizeof (struct Point)); // выделяем память змее
    snake->body[0] = (struct Point) {WIDTH / 2, HEIGHT / 2}; // задаем начальную позицию
    snake->body[1] = (struct Point) {WIDTH / 2 - 1, HEIGHT / 2};
    snake->body[2] = (struct Point) {WIDTH / 2 - 2, HEIGHT / 2};
    snake->dx = 1; // змея стартует вправо
    snake->dy = 0;

    srand(time(NULL)); // рассыпаем начальные яблоки
    food->foods = calloc(NUM_FOODS * sizeof (struct Point)); // выделяем память яблокам
    
    for(int i = 0; i < NUM_FOODS; i++){
        do{
            food->foods[i].x = rand() % WIDTH;
            food->foods[i].y = rand() % HEIGHT;
        } while (food_collision(snake, food, i));
    }

}

// Функция поиска коллизий при генерации яблок

bool food_collision (struct Snake *snake, struct Food *food, int num_food){
    for(int j = 0; j < snake->length; j++) {
        if (food->foods.x[num_food] == snake->body.x[j] && food->foods.y[num_food] == snake->body.y[j]) return TRUE;
    }
    for (int i = 0; i < NUM_FOODS; i++) {
        if (food->foods.x[num_food] == food->foods.x[i] && food->foods.y[num_food] == food->foods.y[i] && i != num_food) return TRUE;
    }
return FALSE;
}

int main (void){
    return 0;
}
