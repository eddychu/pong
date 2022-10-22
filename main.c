
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

const SDL_Color BLACK = {0, 0, 0, 255};
const SDL_Color WHITE = {255, 255, 255, 255};
const int32_t PADDLE_HEIGHT = 60;
const int32_t PADDLE_WIDTH = 20;
const int32_t BALL_SIZE = 20;

typedef struct Player
{
    float x;
    float y;
    int score;
} Player;

typedef struct Ball
{
    float x;
    float y;
    float vx;
    float vy;
} Ball;

typedef struct State
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    TTF_Font *font;
    Player player1;
    Player player2;
    Ball ball;
} State;

void game_draw(State *state)
{
    SDL_Rect left = {state->player1.x, state->player1.y, PADDLE_WIDTH, PADDLE_HEIGHT};
    SDL_Rect right = {state->player2.x, state->player2.y, PADDLE_WIDTH, PADDLE_HEIGHT};
    SDL_Rect ball = {state->ball.x, state->ball.y, BALL_SIZE, BALL_SIZE};
    SDL_SetRenderDrawColor(state->renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);

    SDL_RenderFillRect(state->renderer, &left);
    SDL_RenderFillRect(state->renderer, &right);
    SDL_RenderFillRect(state->renderer, &ball);

    char scores[100];
    sprintf(scores, "%d   %d", state->player1.score, state->player2.score);

    SDL_Surface *message = TTF_RenderText_Solid(state->font, scores, WHITE);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(state->renderer, message);
    int width, height = 0;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SDL_Rect center = {
        SCREEN_WIDTH / 2 - width / 2,
        50,
        width,
        height};
    SDL_RenderCopy(state->renderer, texture, NULL, &center);
    SDL_FreeSurface(message);
    SDL_DestroyTexture(texture);
}

bool game_init(State *state)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        return false;
    }
    if (IMG_Init(IMG_INIT_PNG) == -1)
    {
        printf("failed image\n");
        return false;
    }
    if (TTF_Init() == -1)
    {
        printf("failed ttf\n");
        return false;
    }

    state->font = TTF_OpenFont("assets/font.ttf", 28);

    state->window = SDL_CreateWindow(
        "Pong", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);
    state->renderer = SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);

    Player player1 = {0, (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2};
    state->player1 = player1;
    Player player2 = {SCREEN_WIDTH - PADDLE_WIDTH, (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2};
    state->player2 = player2;

    Ball ball = {(SCREEN_WIDTH - BALL_SIZE) / 2, (SCREEN_HEIGHT - BALL_SIZE) / 2, -0.05, 0.05};
    state->ball = ball;

    return true;
}

void move_player(Player *player, float offset)
{
    player->y += offset;
    if (player->y < 0)
    {
        player->y = 0;
    }
    if (player->y > SCREEN_HEIGHT - PADDLE_HEIGHT)
    {
        player->y = SCREEN_HEIGHT - PADDLE_HEIGHT;
    }
}

void move_ball(Ball *ball)
{
    ball->x += ball->vx;
    ball->y += ball->vy;
}

bool is_collide(Player *player, Ball *ball)
{
    if (ball->x < player->x + PADDLE_WIDTH && ball->x + BALL_SIZE > player->x && ball->y < player->y + PADDLE_HEIGHT && ball->y + BALL_SIZE > player->y)
    {
        return true;
    }
    return false;
}

void respawn(State *state)
{
    state->ball.x = (SCREEN_WIDTH - BALL_SIZE) / 2;
    state->ball.y = (SCREEN_HEIGHT - BALL_SIZE) / 2;
}

void game_update(State *state)
{
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_W])
    {
        move_player(&state->player1, -0.1);
    }
    if (key_state[SDL_SCANCODE_S])
    {
        move_player(&state->player1, 0.1);
    }
    if (key_state[SDL_SCANCODE_UP])
    {
        move_player(&state->player2, -0.1);
    }
    if (key_state[SDL_SCANCODE_DOWN])
    {
        move_player(&state->player2, 0.1);
    }
    move_ball(&state->ball);
    if (is_collide(&state->player1, &state->ball))
    {
        if (state->ball.x < state->player1.x + PADDLE_WIDTH)
        {
            state->ball.x = state->player1.x + PADDLE_WIDTH;
        }
        state->ball.vx = -state->ball.vx;
    }
    if (is_collide(&state->player2, &state->ball))
    {

        if (state->ball.x > state->player2.x - PADDLE_WIDTH)
        {
            state->ball.x = state->player2.x - PADDLE_WIDTH;
        }
        state->ball.vx = -state->ball.vx;
    }
    if (state->ball.y >= SCREEN_HEIGHT - BALL_SIZE)
    {
        state->ball.vy = -state->ball.vy;
    }
    if (state->ball.y <= 0)
    {
        state->ball.vy = -state->ball.vy;
    }

    if (state->ball.x <= -BALL_SIZE)
    {
        state->player2.score += 1;
        respawn(state);
    }
    if (state->ball.x > SCREEN_WIDTH)
    {
        state->player1.score += 1;
        respawn(state);
    }
}

void game_shutdown(State *state)
{
    TTF_CloseFont(state->font);
    SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);
    SDL_Quit();
    state->font = NULL;
    state->renderer = NULL;
    state->window = NULL;
}

void game_run(State *state)
{
    bool running = true;
    Uint32 start = SDL_GetTicks();
    while (running)
    {
        while (SDL_PollEvent(&state->event) != 0)
        {
            if (state->event.type == SDL_QUIT)
            {
                running = false;
                break;
            }
        }
        Uint32 delta = SDL_GetTicks() - start;
        if (delta > 300)
        {
            game_update(state);
            SDL_Renderer *renderer = state->renderer;
            SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
            SDL_RenderClear(renderer);
            game_draw(state);
            SDL_RenderPresent(renderer);
        }
    }
    game_shutdown(state);
}



int main(int argc, char **argv)
{
    State state = {NULL, NULL};

    if (game_init(&state))
    {
        game_run(&state);
    }
    return EXIT_SUCCESS;
}
