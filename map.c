#include <ncurses.h>
#include <stdlib.h>
#include <time.h>


/**
 * a100536 - Pedro Rosário
 * Seleciona uma posição random livre na matriz.
 */
// returns vector with 2 elements: x and y
int* get_random_free_space(int ncols, int nrows, int map[ncols][nrows]) {
    int x, y;
    do {
        x = rand() % ncols;
        y = rand() % nrows;
    } while (map[x][y] != 0);
    static int coords[2]; // static so that the array is not destroyed after the function ends (it is destroyed after the function ends if it is not static)
    // or: int* coords = malloc(2 * sizeof(int));
    coords[0] = x;
    coords[1] = y;
    return coords;
}

/**
 * a100536 - Pedro Rosário
 * Seleciona uma posição random livre na matriz, a x espaços das paredes.
 */
// get random free space with a minimum distance from a wall (to avoid spawning player/enemies inside walls)
int min_distance = 8;
int* get_random_free_space_with_min_distance_from_wall(int ncols, int nrows, int map[ncols][nrows]) {
    // point to checkk if is x blocks near a wall
    int x, y;
    do {
        x = rand() % ncols;
        y = rand() % nrows;
    } while (map[x][y] != 0 || x <= min_distance || x >= ncols - min_distance || y <= min_distance || y >= nrows - min_distance);
    static int coords[2]; // static so that the array is not destroyed after the function ends (it is destroyed after the function ends if it is not static)
    // or: int* coords = malloc(2 * sizeof(int));
    coords[0] = x;
    coords[1] = y;
    return coords;
}

/**
 * a104168 - Jorge Rafael Fernandes
 * Gera no mapa os items especiais (armas e medkit) e a saida para o proximo nivel.
 */
#define MAX_ITEMS 3

void generate_especial_items(int ncols, int nrows, int map[ncols][nrows]) {
    // put a 4 (door) in a place in the map boundaries ( 0 < x < ncols-1, 0 < y < nrows-1)
    /*if (rand() % 2 == 0) {
        // put the door on the left or right side of the map
        int x = rand() % (ncols - 2) + 1;
        if (rand() % 2 == 0) {
            map[x][0] = 4;
        } else {
            map[x][nrows - 1] = 4;
        }
    } else {
        // put the door on the top or bottom side of the map
        int y = rand() % (nrows - 2) + 1;
        if (rand() % 2 == 0) {
            map[0][y] = 4;
        } else {
            map[ncols - 1][y] = 4;
        }
    }*/
    // make the game more fun and put it in a random place
    int x1 = get_random_free_space_with_min_distance_from_wall(ncols, nrows, map)[0];
    int y1 = get_random_free_space_with_min_distance_from_wall(ncols, nrows, map)[1];
    map[x1][y1] = 4;
    // generate between 1 and MAX_ITEMS items
    int n_items = rand() % MAX_ITEMS + 1;
    for (int i = 0; i < n_items; i++) {
        int x = get_random_free_space_with_min_distance_from_wall(ncols, nrows, map)[0];
        int y = get_random_free_space_with_min_distance_from_wall(ncols, nrows, map)[1];
        map[x][y] = rand() % 2 + 7; // 7 (knife) or 8 (sword)
    }
    // spawn a medkit by map
    int x2 = get_random_free_space_with_min_distance_from_wall(ncols, nrows, map)[0];
    int y2 = get_random_free_space_with_min_distance_from_wall(ncols, nrows, map)[1];
    map[x2][y2] = 9;
}

/**
 * a104365 - Fábio Magalhães
 * Gera a matriz do mapa.
 */
// values that can be changed to make the map more or less random

// TODO: make sure theres a path from one side of the map to the other
// add an "x" as exit of the map (go to next level) and make sure theres a path to it
void generate_map(int ncols, int nrows, int map[ncols][nrows]) {
    // Set the random seed
    srand(time(NULL));

    // Fill the map with random rocks
    for (int i = 0; i < ncols; i++) {
        for (int j = 0; j < nrows; j++) {
            if ((rand() % 100) < 40) {
                // Set a random chance for a rock to spawn
                map[i][j] = 1;
            } else {
                map[i][j] = 0;
            }
        }
    }

    // Smooth out the cave
    for (int i = 0; i < 5; i++) { // 5 é o "smoother" do mapa (o 5 é numero pequeno pq assim salvamos iteração fazendo mais tarde o inverso)
        for (int x = 1; x < ncols - 1; x++) { // exclude the borders
            for (int y = 1; y < nrows - 1; y++) { // exclude the borders
                int count = 0; // count neighbour rocks
                for (int dx = -1; dx <= 1; dx++) { // entre -1 e 1 (vizinhos) vamos ver se temos rochas
                    for (int dy = -1; dy <= 1; dy++) { // entre -1 e 1 (vizinhos) vamos ver se temos rochas
                        if (map[x + dx][y + dy] == 1) { // se tivermos uma rocha
                            count++; // contamos como vizinho
                        }
                    }
                }
                if (count > 4) { // se tivermos mais de 4 vizinhos
                    map[x][y] = 1; // colocamos uma rocha
                } else if (count < 3) {
                    map[x][y] = 0;
                }
            }
        }
    }

    // Revert all the map, where 1 put 0, and 0 put 1
    for (int i = 0; i < ncols; i++) {
        for (int j = 0; j < nrows; j++) {
            if (map[i][j] == 1) {
                map[i][j] = 0;
            } else {
                map[i][j] = 1;
            }
        }
    }

    // Make sure theres a path from one side of the map to the other (left to right)
    // LAZY FIX: draw a line from left to right using a random y postition
    int y = rand() % (nrows - 2) + 1;
    for (int i = 0; i < ncols; i++) {
        map[i][y+1] = 0;
        map[i][y] = 0;
    }

    // Draw boundaries
    for (int i = 0; i < ncols; i++) {
        for (int j = 0; j < nrows; j++) {
            if (j == 0 || j == nrows - 1 || i == 0 || i == ncols - 1) {
                map[i][j] = 1;
            }
        }
    }

    // Generate an exit "x" door (number 4) and weapons
    generate_especial_items(ncols, nrows, map);
}

// if its 0, is not being illuminated, if its 1, is being illuminated (draw items in function draw_map)
// given a map and playerposition and a radius, it will illuminate the map
// the objective its simples, if it encounters a wall, it will stop illuminating from there forward
#include <math.h>

/**
 * a104361 - Filipe Viana
 * Calcula a luz/visão do jogador.
 */
// TODO: fix the light system when it encounters a wall
int circle_light;
int being_illuminated(int ncols, int nrows, int playerX, int playerY, int posX, int posY, int radius, int map[ncols][nrows]) {
    circle_light = get_menu_option_state(0);
    if (circle_light == 1) {
        int distance = sqrt(pow(playerX - posX, 2) + pow(playerY - posY, 2));
        return (distance <= radius) ? 1 : 0;
    } else {
        int distance = sqrt(pow(playerX - posX, 2) + pow(playerY - posY, 2));

        if (distance <= radius) {
            // Check for wall intersections along the line
            int dx = posX - playerX;
            int dy = posY - playerY;
            int steps = fmax(abs(dx), abs(dy));

            float xInc = dx / (float)steps;
            float yInc = dy / (float)steps;

            float x = playerX;
            float y = playerY;

            for (int i = 0; i < steps; i++) {
                x += xInc;
                y += yInc;

                int xFloor = (int)floor(x);
                int yFloor = (int)floor(y);

                if (map[xFloor][yFloor] == 1) {
                    // Intersection with a wall, stop illuminating
                    return 0;
                }
            }

            return 1;
        }

        return 0;
    }
}

/**
 * a104365 - Fábio Magalhães
 * Desenha mapa.
 */
int view_radius = 8; // vision/light radius/limits

// Map debug
int draw_all_map;
int debug_light; // draw walls

void draw_map(int ncols, int nrows, int map[ncols][nrows], PLAYER *player, ENEMY enemies[], int enemy_count) {
    draw_all_map = get_menu_option_state(1);
    debug_light = get_menu_option_state(2);
    int playerX = player->x;
    int playerY = player->y;
    int radius = view_radius;

    for (int j = 0; j < nrows; j++) {
        for (int i = 0; i < ncols; i++) {
            if (map[i][j] == 1 && draw_all_map == 0 && debug_light == 0) {
                // Skip drawing walls if not in draw_all_map mode
                continue;
            }

            if (being_illuminated(ncols, nrows, playerX, playerY, i, j, radius, map) || draw_all_map) {
                if (map[i][j] == 1) {
                    attron(COLOR_PAIR(COLOR_WHITE));
                    mvprintw(j, i, "#");
                    attroff(COLOR_PAIR(COLOR_WHITE));
                } else if (map[i][j] == 4) {
                    attron(COLOR_PAIR(COLOR_YELLOW));
                    attron(A_BOLD);
                    mvprintw(j, i, "x");
                    attroff(A_BOLD);
                    attroff(COLOR_PAIR(COLOR_YELLOW));
                } else if (map[i][j] == 7) {
                    attron(COLOR_PAIR(COLOR_GREEN));
                    attron(A_BOLD);
                    mvprintw(j, i, "k");
                    attroff(A_BOLD);
                    attroff(COLOR_PAIR(COLOR_GREEN));
                } else if (map[i][j] == 8) {
                    attron(COLOR_PAIR(COLOR_GREEN));
                    attron(A_BOLD);
                    mvprintw(j, i, "s");
                    attroff(A_BOLD);
                    attroff(COLOR_PAIR(COLOR_GREEN));
                } else if (map[i][j] == 9) {
                    attron(COLOR_PAIR(COLOR_GREEN));
                    attron(A_BOLD);
                    mvprintw(j, i, "m");
                    attroff(A_BOLD);
                    attroff(COLOR_PAIR(COLOR_GREEN));
                } else if (map[i][j] == 0) {
                    if (draw_all_map == 1) {
                        mvprintw(j, i, " ");
                    } else {
                        mvprintw(j, i, "."); // draw light on ground/where the player can see, only if light mode is enabled
                    }
                } else {
                    mvprintw(j, i, " ");
                }
            } else {
                mvprintw(j, i, " ");
            }
        }
    }

    for (int k = 0; k < enemy_count; k++) {
        int enemyX = enemies[k].x;
        int enemyY = enemies[k].y;

        if (being_illuminated(ncols, nrows, playerX, playerY, enemyX, enemyY, radius, map) || draw_all_map) {
            attron(COLOR_PAIR(COLOR_RED));
            attron(A_BOLD);
            if (enemies[k].type == 0) {
                mvprintw(enemyY, enemyX, "z"); // if type is 0, it's a zombie "z"
            } else if (enemies[k].type == 1) {
                mvprintw(enemyY, enemyX, "s"); // if type is 1, it's a skeleton "s"
            } else if (enemies[k].type == 2) {
                mvprintw(enemyY, enemyX, "g"); // if type is 2, it's a ghost "g"
            }
            attroff(A_BOLD);
            attroff(COLOR_PAIR(COLOR_RED));
        }
    }
}