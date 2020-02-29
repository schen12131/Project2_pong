/*use W and S as up and down for the left side controls and the arrow keys for the right side
use the space bar the start the game
when someone loses the ball returns to the center and you can press the space bar to play again*/
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, shineMatrix, roseMatrix, rracketmat, lracketmat, modelMatrix, ballmatrix;

int width = 640;
int height = 480;

float racket_w =0.2f;
float racket_h = 0.9f;
float racket_speed = 0.03f;

float lracket_x = -4.5f;
float lracket_y = -0.5f;

float rracket_x = 4.0f;
float rracket_y = -0.5f;

float player_speed = 0.1f;

float ball_x = 0.0f;
float ball_y = 0.0f;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
float size = 0.15;
float ball_speed = 0.05f;

// Start at 0, 0, 0
glm::vec3 rracket_pos = glm::vec3(0, 0, 0);
// Don’t go anywhere (yet).
glm::vec3 rracket_move = glm::vec3(0, 0, 0);
glm::vec3 lracket_pos = glm::vec3(0, 0, 0);
// Don’t go anywhere (yet).
glm::vec3 lracket_move = glm::vec3(0, 0, 0);
bool move_ball = false;

float lastTicks = 0.0f;


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project2: pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    rracketmat = glm::mat4(1.0f);
    lracketmat = glm::mat4(1.0f);
    ballmatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);

    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void ProcessInput() {
    
    rracket_move = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                        break;
                    case SDLK_SPACE:
                        move_ball = true;
                        // start the game
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_UP]) {
        if (rracket_y <= 3.4){
            rracket_move.y = 0.1f;
            rracket_y += 0.1f;
        }
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        if (rracket_y >= -3.8){
            rracket_move.y = -0.1f;
            rracket_y += -0.1f;
        }
    }
    
    if (glm::length(rracket_move) > 0.5f) {
        rracket_move = glm::normalize(rracket_move);
    }
    // using W and S as up/down for the left side
    if (keys[SDL_SCANCODE_W]) {
        if (lracket_y <= 3.4){
            lracket_move.y = 0.1f;
            lracket_y += 0.1f;
        }
    }
    else if (keys[SDL_SCANCODE_S]) {
        if (lracket_y >= -3.8){
            lracket_move.y = -0.1f;
            lracket_y += -0.1f;
        }
    }
    
    if (glm::length(rracket_move) > 0.5f) {
        lracket_move = glm::normalize(rracket_move);
    }
}

void norm(float& x, float& y){
    float len = sqrt((x * x) + (y*y));
    if (len != 0.0f){
        len = 1.0f/len;
        x *= len;
        y *= len;
    }
}


void ball_update(){
    if (move_ball == true){
        ball_x += ball_dir_x * ball_speed;
        ball_y += ball_dir_y * ball_speed;
        if(ball_x < lracket_x + racket_w && ball_x > lracket_x && ball_y < lracket_y + racket_h && ball_y > lracket_y){
            float p = ((ball_y - lracket_y)/racket_h) - 0.5f;
            ball_dir_x = fabs(ball_dir_x);
            ball_dir_y = p;
        }
        if(ball_x > rracket_x && ball_x < rracket_x + racket_w && ball_y < rracket_y + racket_h && ball_y > rracket_y){
            float p = ((ball_y - rracket_y)/racket_h) - 0.5f;
            ball_dir_x = -fabs(ball_dir_x);
            ball_dir_y = p;
        }
        if (ball_y > 3.4){
            ball_dir_y = -fabs(ball_dir_y);
            
        }
        if (ball_y < -3.8){
            ball_dir_y = fabs(ball_dir_y);
 
        }
        if (ball_x < -5.0 || ball_x > 5.0){
            ball_x = 0.0f;
            ball_y = 0.0f;
            ball_dir_x = -1.0f;
            ball_dir_y = 0.0f;
            move_ball = false;
        }
    }
    //norm(ball_x, ball_y);
    
}

void Update() {
    ball_update();
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    // Add (direction * units per second * elapsed time)
    rracket_move += rracket_move * player_speed * deltaTime;
    rracketmat = glm::mat4(1.0f);
    rracketmat = glm::translate(rracketmat, rracket_move);
    
    lracket_move += rracket_move * player_speed * deltaTime;
    lracketmat = glm::mat4(1.0f);
    lracketmat = glm::translate(rracketmat, rracket_move);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetModelMatrix(modelMatrix);
    
    float right[] = {
        rracket_x, rracket_y + racket_h, rracket_x + racket_w, rracket_y + racket_h, rracket_x + racket_w, rracket_y,
        rracket_x, rracket_y + racket_h, rracket_x + racket_w, rracket_y, rracket_x, rracket_y };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, right);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    
    float left[] = {
        lracket_x, lracket_y + racket_h, lracket_x + racket_w, lracket_y + racket_h, lracket_x + racket_w, lracket_y,
        lracket_x, lracket_y + racket_h, lracket_x + racket_w, lracket_y, lracket_x, lracket_y };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, left);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    
    float ball[] = {ball_x - size, ball_y - size, ball_x, ball_y - size, ball_x, ball_y, ball_x - size,ball_y - size, ball_x, ball_y, ball_x - size, ball_y};
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ball);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}

