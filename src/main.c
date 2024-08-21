#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT 600
#define WIDTH 800
#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

int running = 0;
int capped_fps = 1;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int last_frame_time = 0;

typedef struct entity {
    float x;
    float y;
    float width;
    float height;
    int dx;
    int dy;
    SDL_Color color;
} entity;

struct array {
    int _alloc_size;
    int size;
    struct entity* data;
} entity_array;

entity generate_random_entity(){
    int dx = rand() % (130 - 25 + 1) + 25;
    int dy = rand() % (130 - 25 + 1) + 25;
    entity e = (entity){
        .x = rand() % (WIDTH - 25 - 25 + 1) + 25,
        .y = rand() % (HEIGHT - 25 - 25 + 1) + 25,
        .width = 15,
        .height = 15,
        .dx = (rand() % 2)? dx : -dx,
        .dy = (rand() % 2)? dy : -dy,
        .color = {.r = rand() % 256, .g = rand() % 256, .b = rand() % 256},
    };
    //printf("Created entity: (x:%f, y:%f, dx:%d, dy:%d)\n", e.x, e.y, e.dx, e.dy);
    return e;
}

void add_entity() {
    if(entity_array.size + 1 >= entity_array._alloc_size) {
        entity_array._alloc_size += 100;
        entity_array.data = realloc(entity_array.data, entity_array._alloc_size * sizeof(entity));
    }
    //printf("size: %d alloc_size: %d\n", entity_array.size, entity_array._alloc_size);
    entity_array.data[entity_array.size] = generate_random_entity();
    entity_array.size += 1;
}

void update_entity_position(entity* entity, float delta_time){
    if (entity->x + entity->width >= WIDTH || entity->x <= 0){
        entity->dx = -entity->dx;
    }
    if (entity->y + entity->height >= HEIGHT || entity->y <= 0){
        entity->dy = -entity->dy;
    }

    entity->x += entity->dx * delta_time;
    entity->y += entity->dy * delta_time;
}

void render_entity(entity entity) {
    SDL_Rect entity_rect = {
        (int)entity.x ,
        (int)entity.y ,
        (int)entity.width ,
        (int)entity.height ,
    };

    SDL_SetRenderDrawColor(renderer, entity.color.r, entity.color.g, entity.color.b, entity.color.a);
    SDL_RenderFillRect(renderer, &entity_rect);
}

int init() {
    srand(time(NULL));
    if(SDL_Init(SDL_INIT_EVERYTHING)) {
        perror("Error initializing SDL\n");
        return 0;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if(! window) {
        perror("Error creating window");
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if(! renderer) {
        perror("Error creating renderer");
        return 0;
    }

    return 1;
}


void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            running = 0;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
                break;
            } else if(event.key.keysym.sym == SDLK_n) {
                add_entity();
            }
        default:
            break;
    }

}

void setup() {
    entity_array.data = malloc(100 * sizeof(entity));
    entity_array._alloc_size = 100;
    entity_array.size = 1;
    entity_array.data[0] = (entity){
        .x = WIDTH / 2.0,
        .y = HEIGHT / 2.0,
        .width = 15,
        .height = 15,
        .dx = 90,
        .dy = 90,
        .color = {.r = 255, .g = 0, .b = 255, .a = 0}
    };
}


void update() {

    if (capped_fps) {
        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
        if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
            SDL_Delay(time_to_wait);
        }
    }
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();

    for(int i = 0; i < entity_array.size; i++)
        update_entity_position(&entity_array.data[i], delta_time);

}


void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for(int i = 0; i < entity_array.size; i++)
        render_entity(entity_array.data[i]);

    SDL_RenderPresent(renderer);
}

void cleanup() {
    free(entity_array.data);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    running = init();
    setup();

    while (running) {
        process_input();
        update();
        render();
    }
    cleanup();
    return 0;
}
