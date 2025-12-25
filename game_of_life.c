#include <ncurses.h>
#include <stdio.h>

#define W 80
#define H 25
#define MAX_S 10
#define MIN_S 1
#define FNAME_LEN 20
#define BASE_DELAY 30

void init_ncurses(void) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
}

void draw_border(void) {
    int i = 0;
    while (i < W + 2) {
        mvaddch(0, i, '-');
        mvaddch(H + 1, i, '-');
        i = i + 1;
    }
    i = 0;
    while (i < H + 2) {
        mvaddch(i, 0, '|');
        mvaddch(i, W + 1, '|');
        i = i + 1;
    }
}

void draw_cells(char field[H][W]) {
    int y = 0;
    while (y < H) {
        int x = 0;
        while (x < W) {
            mvaddch(y + 1, x + 1, field[y][x]);
            x = x + 1;
        }
        y = y + 1;
    }
}

int count_neighbors(const char field[H][W], int y, int x) {
    int cnt = 0;
    int dy = -1;
    while (dy <= 1) {
        int dx = -1;
        while (dx <= 1) {
            int not_center = (dx != 0 || dy != 0);
            if (not_center) {
                int ny = (y + dy + H) % H;
                int nx = (x + dx + W) % W;
                if (field[ny][nx] == '#') {
                    cnt = cnt + 1;
                }
            }
            dx = dx + 1;
        }
        dy = dy + 1;
    }
    return cnt;
}

void next_generation(char field[H][W]) {
    char new[H][W];
    int y = 0;
    while (y < H) {
        int x = 0;
        while (x < W) {
            int n = count_neighbors(field, y, x);
            char cell = field[y][x];
            if (cell == '#') {
                if (n == 2 || n == 3) {
                    new[y][x] = '#';
                } else {
                    new[y][x] = '.';
                }
            } else {
                if (n == 3) {
                    new[y][x] = '#';
                } else {
                    new[y][x] = '.';
                }
            }
            x = x + 1;
        }
        y = y + 1;
    }
    y = 0;
    while (y < H) {
        int x = 0;
        while (x < W) {
            field[y][x] = new[y][x];
            x = x + 1;
        }
        y = y + 1;
    }
}

void init_field(char field[H][W], int id) {
    char name[FNAME_LEN];
    snprintf(name, sizeof(name), "%d.txt", id);

    FILE *f = fopen(name, "r");
    int y = 0;

    if (f == NULL) {
        while (y < H) {
            int x = 0;
            while (x < W) {
                field[y][x] = '.';
                x = x + 1;
            }
            y = y + 1;
        }
    } else {
        while (y < H) {
            char line[W + 2];
            int x = 0;
            if (fgets(line, sizeof(line), f) != NULL) {
                while (x < W) {
                    if (line[x] == '#') {
                        field[y][x] = '#';
                    } else {
                        field[y][x] = '.';
                    }
                    x = x + 1;
                }
            } else {
                while (x < W) {
                    field[y][x] = '.';
                    x = x + 1;
                }
            }
            y = y + 1;
        }
        fclose(f);
    }
}

void show_info(int speed, int gen, int id) {
    char s[100];
    snprintf(
        s, sizeof(s),
        "\n\t\tGameMap: %2d || Speed: %2d || Generation: %d\n\t\tA/Z: SpeedUp/SpeedDown   || Quit: SpaceBar",
        id, speed, gen);
    mvaddstr(H + 2, 1, s);
}

int get_file_id(void) {
    int id = 1;
    char inp[10];
    int valid = 0;

    printf("Select GameMap (press 1-10 or 21): ");

    if (fgets(inp, sizeof(inp), stdin) != NULL) {
        if (sscanf(inp, "%d", &id) == 1) {
            int in_range = (id >= 1 && id <= 10) || id == 21;
            if (in_range) {
                valid = 1;
            }
        }
    }

    if (valid == 0) {
        printf("Use 1\n");
        id = 1;
    }

    return id;
}

void process_input(int *speed, int *run) {
    int k = getch();
    if (k != ERR) {
        if (k == 'a' || k == 'A') {
            if (*speed < MAX_S) {
                *speed = *speed + 1;
            }
        } else {
            if (k == 'z' || k == 'Z') {
                if (*speed > MIN_S) {
                    *speed = *speed - 1;
                }
            } else {
                if (k == ' ') {
                    *run = 0;
                }
            }
        }
    }
}

int get_delay(int s) { return BASE_DELAY * (MAX_S - s + 1); }

void wait_and_check(int delay, int *speed, int *run) {
    int i = 0;
    while (i < delay / 10 && *run) {
        process_input(speed, run);
        if (*run) {
            napms(10);
            i = i + 1;
        }
    }
}

int main(void) {
    char field[H][W];
    int speed = 1;
    int gen = 0;
    int run = 1;
    int id = get_file_id();

    init_field(field, id);
    init_ncurses();

    while (run) {
        clear();
        draw_border();
        draw_cells(field);
        show_info(speed, gen, id);
        refresh();
        next_generation(field);
        gen = gen + 1;
        wait_and_check(get_delay(speed), &speed, &run);
    }

    endwin();
    return 0;
}
