#include <ncurses.h>   // Для управления терминалом
#include <stdlib.h>     // Для функций rand() и srand()
#include <unistd.h>     // Для функции usleep()
#include <time.h>       // Для инициализации генератора случайных чисел

#define WIDTH 40        // Ширина игрового поля
#define HEIGHT 20       // Высота игрового поля

struct Point {
    int x, y;  // Координаты точки на поле
};

struct Snake {
    struct Point *body;  // Массив точек, представляющий тело змеи
    int length;          // Текущая длина змеи
    int dx, dy;          // Направление движения змеи
};

void init_game(struct Snake *snake, struct Point *food);
void draw(struct Snake *snake, struct Point *food);
void update(struct Snake *snake, struct Point *food, int *game_over);
void handle_input(struct Snake *snake);

int main() {
    // Инициализация ncurses
    initscr();
    noecho();         // Не отображать вводимые символы
    curs_set(0);      // Отключить курсор
    keypad(stdscr, TRUE);  // Включить поддержку клавиш-стрелок
    timeout(100);     // Установить таймаут ввода (100 мс)

    struct Snake snake;
    struct Point food;
    int game_over = 0;

    init_game(&snake, &food);

    while (!game_over) {
        draw(&snake, &food);
        handle_input(&snake);
        update(&snake, &food, &game_over);
    }

    // Завершение ncurses
    endwin();
    free(snake.body);
    return 0;
}

// Функция инициализации игры
void init_game(struct Snake *snake, struct Point *food) {
    snake->length = 3;
    snake->body = malloc(snake->length * sizeof(struct Point));
    snake->body[0] = (struct Point){WIDTH / 2, HEIGHT / 2};
    snake->body[1] = (struct Point){WIDTH / 2 - 1, HEIGHT / 2};
    snake->body[2] = (struct Point){WIDTH / 2 - 2, HEIGHT / 2};
    snake->dx = 1;
    snake->dy = 0;

    srand(time(NULL));
    food->x = rand() % WIDTH;
    food->y = rand() % HEIGHT;
}

// Функция отрисовки игрового поля и объектов
void draw(struct Snake *snake, struct Point *food) {
    clear();
    // Отрисовка границ
    for (int i = 0; i < WIDTH + 2; i++) {
        mvprintw(0, i, "#");
        mvprintw(HEIGHT + 1, i, "#");
    }
    for (int i = 0; i < HEIGHT + 2; i++) {
        mvprintw(i, 0, "#");
        mvprintw(i, WIDTH + 1, "#");
    }

    // Отрисовка еды
    mvprintw(food->y + 1, food->x + 1, "*");

    // Отрисовка тела змеи
    for (int i = 0; i < snake->length; i++) {
        mvprintw(snake->body[i].y + 1, snake->body[i].x + 1, "O");
    }

    refresh();
}

// Функция обработки ввода с клавиатуры
void handle_input(struct Snake *snake) {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
            if (snake->dy == 0) { snake->dx = 0; snake->dy = -1; }
            break;
        case KEY_DOWN:
            if (snake->dy == 0) { snake->dx = 0; snake->dy = 1; }
            break;
        case KEY_LEFT:
            if (snake->dx == 0) { snake->dx = -1; snake->dy = 0; }
            break;
        case KEY_RIGHT:
            if (snake->dx == 0) { snake->dx = 1; snake->dy = 0; }
            break;
    }
}

// Функция обновления состояния игры
void update(struct Snake *snake, struct Point *food, int *game_over) {
    // Перемещение тела змеи
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0].x += snake->dx;
    snake->body[0].y += snake->dy;

    // Проверка на столкновение с границами
    if (snake->body[0].x < 0 || snake->body[0].x >= WIDTH ||
        snake->body[0].y < 0 || snake->body[0].y >= HEIGHT) {
        *game_over = 1;
    }

    // Проверка на столкновение с собственным телом
    for (int i = 1; i < snake->length; i++) {
        if (snake->body[0].x == snake->body[i].x &&
            snake->body[0].y == snake->body[i].y) {
            *game_over = 1;
        }
    }

    // Проверка на съедание еды
    if (snake->body[0].x == food->x && snake->body[0].y == food->y) {
        snake->length++;
        snake->body = realloc(snake->body, snake->length * sizeof(struct Point));
        food->x = rand() % WIDTH;
        food->y = rand() % HEIGHT;
    }
}
