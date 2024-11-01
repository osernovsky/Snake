#include <ncurses.h> // Библиотека упавления терминалом
#include <stdlib.h> // Вызов функций рандомайзера
#include <time.h>
#include <unistd.h>

#define WIDTH 60 // Задаем размеры игрового поля, без учета рамки
#define HEIGHT 15
#define NUM_FOODS 100 // Количество яблок на поле

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

// Объявляю функции
bool food_collision(struct Snake *snake, struct Food *food, int num_food);


void print_table(void){

    system("clear");
    printf("\033[94m");

    for(int i = 0; i < WIDTH + 2; i++) printf("#");
    printf("\n");
    for (int j = 0; j < HEIGHT; j++){
        printf("#");
        for(int i = 0; i < WIDTH; i++){
            printf(" ");
        }
        printf("#\n");
    }
    for(int i = 0; i < WIDTH + 2; i++) printf("#");
    printf("\033[0m\n");
}

void placeCharAt(int x, int y, char c) {
    // Escape-последовательность для перемещения курсора
    // Формат: "\033[y;xH"
    printf("\033[%d;%dH%c", y, x, c);
    // Обновляем буфер и печатаем немедленно
    fflush(stdout);
}

void print_food(struct Food *food){
    for (int i = 0; i < NUM_FOODS; i++){
        printf("\033[92m"); // зеленый цвет яблок
        placeCharAt(food->foods[i].x + 1, food->foods[i].y + 1, '@');
        printf("\033[0m"); // возвращаю цвет
    }
}

void print_snake(struct Snake *snake){
    for (int i = 0; i < snake->length; i++){
        printf("\033[93m"); // жёлтый цвет змеи
        placeCharAt(snake->body[i].x + 1, snake->body[i].y + 1, '0');
        printf("\033[0m"); // возвращаю цвет
    }    
}

void clear_tail(struct Snake *snake){
    placeCharAt(snake->body[snake->length - 1].x + 1, snake->body[snake->length - 1].y + 1, ' ');
}

// Функция инициализации игры

void start_game (struct Snake *snake, struct Food *food){

    snake->length = 3; // начальная длина змеи
//    snake->body = malloc(snake->length * sizeof (struct Point)); // выделяем память змее
    snake->body = malloc(50 * sizeof (struct Point)); // выделяем память змее
    snake->body[0] = (struct Point) {WIDTH / 2, HEIGHT / 2 + 1}; // задаем начальную позицию
    snake->body[1] = (struct Point) {WIDTH / 2 - 1, HEIGHT / 2 + 1};
    snake->body[2] = (struct Point) {WIDTH / 2 - 2, HEIGHT / 2 + 1};

    snake->dx = 1; // змея стартует вправо
    snake->dy = 0;

    srand(time(NULL)); // рассыпаем начальные яблоки
    food->foods = malloc(NUM_FOODS * sizeof (struct Point)); // выделяем память яблокам
    
    for(int i = 0; i < NUM_FOODS; i++){
        do{
            food->foods[i].x = (rand() % WIDTH) + 1;
            food->foods[i].y = (rand() % HEIGHT) + 1;
        } while (food_collision(snake, food, i));
    }

    print_table();
//    print_snake(snake);
    print_food(food);
}

// Функция поиска коллизий при генерации яблок

bool food_collision (struct Snake *snake, struct Food *food, int num_food){
    for(int j = 0; j < snake->length; j++) {
        if (food->foods[num_food].x == snake->body[j].x &&
            food->foods[num_food].y == snake->body[j].y) return TRUE;
    }
    for (int i = 0; i < NUM_FOODS; i++) {
        if (food->foods[num_food].x == food->foods[i].x &&
            food->foods[num_food].y == food->foods[i].y &&
            i != num_food) return TRUE;
    }
return FALSE;
}

// Функция поиска коллизий движения змеи

int snake_collision (struct Snake *snake, struct Food *food, int x, int y){
    for(int i = 1; i < snake->length; i++){
        if (x == snake->body[i].x &&
            y == snake->body[i].y) return -1;
    }
    for(int j = 0; j < NUM_FOODS; j++){
        if (x == food->foods[j].x &&
            y == food->foods[j].y) return (j+1);
    }
    return 0;
}

int move_snake (struct Snake *snake, struct Food *food){

    int x, y;
    x = snake->body[0].x + snake->dx;
    y = snake->body[0].y + snake->dy;
    if(x > WIDTH) x = 1;
    if(x <1) x = WIDTH;
    if(y > HEIGHT) y = 1;
    if(y <1) y = HEIGHT;

    int i = snake_collision(snake, food, x, y);

    if (i < 0) return 1; // игра окончена

    if (i == 0){ // простое движение, стираем хвост, рисуем новую голову
        clear_tail(snake);
        for( int j = snake->length; j > 0; j --){
            snake->body[j] = snake->body[j-1];
        }
        snake->body[0].x = x;
        snake->body[0].y = y;
        print_snake(snake);
        return 0;
    } 
    
    if (i > 0) { // съели яблоко, увеличиваем змею
        printf("\033[%d;1H Apple number:%d ", HEIGHT + 3, i);

        snake->length++;
//        snake->body = realloc(snake->body, snake->length * sizeof(struct Point));

        for( int j = snake->length; j > 0; j --){
            snake->body[j] = snake->body[j-1];
        }
        snake->body[0].x = x;
        snake->body[0].y = y;
        print_snake(snake);
        
        return 0;
    }
}


int main (void){

    struct Snake snake;
    struct Food food;

    // Инициализация ncurses
//    initscr();
//    noecho();         // Не отображать вводимые символы
//    curs_set(0);      // Отключить курсор
//    keypad(stdscr, TRUE);  // Включить поддержку клавиш-стрелок

    start_game(&snake, &food);

    while (move_snake(&snake, &food) == 0){
        usleep(200000);
    }
    printf("\033[%d;1H Game over", HEIGHT + 4);
    
    free(snake.body);
    return 0;
}
