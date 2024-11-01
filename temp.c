#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

#define WIDTH 60
#define HEIGHT 15
#define NUM_FOODS 20
#define INITIAL_SNAKE_LENGTH 3
#define SNAKE_BODY_ALLOC_STEP 10 // Увеличение длины тела змеи

int score = 0;

struct Point {
    int x, y;
};

struct Snake {
    struct Point *body;
    int length;
    int capacity; // Полный размер выделенной памяти для оптимизации
    int dx, dy;
};

struct Food {
    struct Point *foods;
};

bool food_collision(struct Snake *snake, struct Food *food, int num_food) {
    for (int j = 0; j < snake->length; j++) {
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

void enable_nonblocking() {
    struct termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void disable_nonblocking() {
    struct termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

int get_key_press(struct Snake *snake) {
    char ch = getchar();
    if (ch == 27) {
        char next = getchar();
        if (next == '[') {
            next = getchar();
            switch (next) {
                case 'A': if (snake->dy == 0){snake->dx = 0; snake->dy = -1; return 1;}
                case 'B': if (snake->dy == 0){snake->dx = 0; snake->dy = 1; return 1;}
                case 'C': if (snake->dx == 0){snake->dy = 0; snake->dx = 1; return 1;}
                case 'D': if (snake->dx == 0){snake->dy = 0; snake->dx = -1; return 1;}
            }
        }
        return -1;
    }
    return 0;
}

void print_table() {
    system("clear");
    printf("\033[94m");
    for (int i = 0; i < WIDTH + 2; i++) printf("#");
    printf("\n");
    for (int j = 0; j < HEIGHT; j++) {
        printf("#");
        for (int i = 0; i < WIDTH; i++) {
            printf(" ");
        }
        printf("#\n");
    }
    for (int i = 0; i < WIDTH + 2; i++) printf("#");
    printf("\033[0m\n");
}

void placeCharAt(int x, int y, char c) {
    printf("\033[%d;%dH%c", y, x, c);
    fflush(stdout);
}

void print_food(struct Food *food) {
    for (int i = 0; i < NUM_FOODS; i++) {
        printf("\033[92m");
        placeCharAt(food->foods[i].x + 1, food->foods[i].y + 1, '@');
        printf("\033[0m");
    }
}

void print_snake(struct Snake *snake) {
    for (int i = 0; i < snake->length; i++) {
        printf("\033[93m");
        placeCharAt(snake->body[i].x + 1, snake->body[i].y + 1, '0');
        printf("\033[0m");
    }
}

void clear_tail(struct Snake *snake) {
    placeCharAt(snake->body[snake->length - 1].x + 1, snake->body[snake->length - 1].y + 1, ' ');
}

void allocate_snake_body(struct Snake *snake) {
    if (snake->length >= snake->capacity) {
        snake->capacity += SNAKE_BODY_ALLOC_STEP;
        snake->body = realloc(snake->body, snake->capacity * sizeof(struct Point));
    }
}

void start_game(struct Snake *snake, struct Food *food) {
    snake->length = INITIAL_SNAKE_LENGTH;
    snake->capacity = INITIAL_SNAKE_LENGTH + SNAKE_BODY_ALLOC_STEP;
    snake->body = malloc(snake->capacity * sizeof(struct Point));

    snake->body[0] = (struct Point) {WIDTH / 2, HEIGHT / 2 + 1};
    snake->body[1] = (struct Point) {WIDTH / 2 - 1, HEIGHT / 2 + 1};
    snake->body[2] = (struct Point) {WIDTH / 2 - 2, HEIGHT / 2 + 1};

    snake->dx = 1;
    snake->dy = 0;

    srand(time(NULL));
    food->foods = malloc(NUM_FOODS * sizeof(struct Point));
    for (int i = 0; i < NUM_FOODS; i++) {
        do {
            food->foods[i].x = (rand() % WIDTH) + 1;
            food->foods[i].y = (rand() % HEIGHT) + 1;
        } while (food_collision(snake, food, i));
    }

    print_table();
    print_food(food);
}

int move_snake(struct Snake *snake, struct Food *food) {
    int key = get_key_press(snake);
    if (key == -1) return 1;

    int x = snake->body[0].x + snake->dx;
    int y = snake->body[0].y + snake->dy;
    if (x > WIDTH) x = 1;
    if (x < 1) x = WIDTH;
    if (y > HEIGHT) y = 1;
    if (y < 1) y = HEIGHT;

    int collision = snake_collision(snake, food, x, y);
    if (collision < 0) return 1;

    if (collision == 0) {
        clear_tail(snake);
        for (int j = snake->length; j > 0; j--) {
            snake->body[j] = snake->body[j - 1];
        }
        snake->body[0].x = x;
        snake->body[0].y = y;
        print_snake(snake);
    } else if (collision > 0) {
        allocate_snake_body(snake);
        snake->length++;
        score += 10;
        printf("\033[1;3H Score: %d ", score);
        printf("\a");

        for (int j = snake->length - 1; j > 0; j--) {
            snake->body[j] = snake->body[j - 1];
        }
        snake->body[0].x = x;
        snake->body[0].y = y;
        print_snake(snake);

        do {
            food->foods[collision - 1].x = (rand() % WIDTH) + 1;
            food->foods[collision - 1].y = (rand() % HEIGHT) + 1;
        } while (food_collision(snake, food, collision));

        printf("\033[92m");
        placeCharAt(food->foods[collision - 1].x + 1, food->foods[collision - 1].y + 1, '@');
        printf("\033[0m");
    }
    return 0;
}

int main(void) {
    struct Snake snake;
    struct Food food;

    enable_nonblocking();
    start_game(&snake, &food);

    while (move_snake(&snake, &food) == 0) {
        usleep(100000);
    }

    printf("\033[%d;1H Game over", HEIGHT + 4);
    free(snake.body);
    disable_nonblocking();
    return 0;
}
