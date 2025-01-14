#include "game.h"
#include <iostream>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
      snake = std::make_shared<Snake>(grid_width, grid_height);
      PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;
  bool restart = false;
  bool pause = false;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, pause, restart, snake);
    if(!pause){
      Update();
    }
    if(restart){
      RestartGame();
      restart = false;
    }
    
    renderer.Render(snake, food, hungry, GetHungry());

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake->SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}


void Game::PlaceHungry() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake->SnakeCell(x, y) && x!= food.x && y!=food.y) {
      hungry.x = x;
      hungry.y = y;
      return;
    }
  }
}

void Game::Update() {
  if (!snake->alive) return;

  snake->Update();

  if(RandomBool(10000,9990) && !_isHungry){
    PlaceHungry();
    SetStartOfHungryCycle();
    SetHungry(true);
  }
  bool stillHungry = hungryPhase();
  if(!stillHungry){
    SetHungry(stillHungry);
  }
  


  int new_x = static_cast<int>(snake->head_x);
  int new_y = static_cast<int>(snake->head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake->GrowBody();
    snake->speed += 0.02;
  }

  // Check if there's hungry over here
  if (hungry.x == new_x && hungry.y == new_y && GetHungry()) {
    // Shrink snake and let hungry disappear
    SetHungry(false);
    snake->ShrinkBody();
  }
}

void Game::RestartGame(){
  score = 0;
  snake = std::make_shared<Snake>(snake->GetWidth(), snake->GetHeight());
  PlaceFood();

}

bool Game::RandomBool(int range, int threshold){
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<int> uni(0, range);
  auto randomNumber = uni(rng);
  if(randomNumber>=threshold && GetSize()>=2){
    return true;
  }
  return false;
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake->size; }

void Game::SetHungry(bool val){
  _isHungry = val; 
  }
bool Game::GetHungry() const {return _isHungry;}

void Game::SetStartOfHungryCycle(){
  _startOfHungryCycle = std::chrono::system_clock::now();
  }
std::chrono::time_point<std::chrono::system_clock> Game::GetStartOfHungryCycle(){return _startOfHungryCycle;}

// virtual function which is executed in a thread
bool Game::hungryPhase()
{
  double timeInCycle = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - GetStartOfHungryCycle()).count();
  if(timeInCycle >= 6000){
      return false;
  }
  return true;
}