#ifndef GAME_H
#define GAME_H

#include <random>
#include <memory>

#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

  void SetStartOfHungryCycle();
  std::chrono::time_point<std::chrono::system_clock> GetStartOfHungryCycle();

  void SetHungry(bool val);
  bool GetHungry() const;

 private:
  std::shared_ptr<Snake> snake;
  SDL_Point food;
  SDL_Point hungry;
  std::chrono::time_point<std::chrono::system_clock> _startOfHungryCycle;

  //bool _pauseGame;
  bool _isHungry{false};

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void PlaceFood();
  void PlaceHungry();

  bool RandomBool(int range, int threshold);

  void Update();

  void RestartGame();

  bool hungryPhase();
};

#endif