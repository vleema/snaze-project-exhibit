#include "game_manager.hpp"
#include "maze.hpp"
#include "snake.hpp"
#include "terminal_utils.h"
#include "utils.hpp"

#include <cstddef>
#include <cstdlib>
#include <experimental/random>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace snaze {

void SnazeManager::process() {
  if (m_snaze_state == SnazeState::Init) {
  } else if (m_snaze_state == SnazeState::Welcome) {
    read_enter_to_proceed();
  } else if (m_snaze_state == SnazeState::PickingRandomLevel) {
  } else if (m_snaze_state == SnazeState::GameStart) {
    m_snake.reset();
    m_maze.random_food_position();
    m_snake.body.push_front(m_maze.start());
    snake_bot_think(m_snake);
  } else if (m_snaze_state == SnazeState::On) {
    if (m_snake_bot.solution.has_value() and
        m_snake_bot.solution.value().empty()) {
      snake_bot_think(m_snake);
    }
    m_snake.head_direction = m_snake_bot.solution.value().front();
    m_snake_bot.solution.value().pop_front();
  } else if (m_snaze_state == SnazeState::Damage) {
  } else if (m_snaze_state == SnazeState::Won) {
  } else {
    std::cout << snaze_mode() << "Now in process\n";
    throw std::runtime_error("Invalid SnazeState");
  }
}

void SnazeManager::update() {
  if (not m_system_msg.empty()) {
    return;
  }
  if (m_snaze_state == SnazeState::Init) {
    m_snaze_state = SnazeState::Welcome;
  } else if (m_snaze_state == SnazeState::Welcome) {
    m_snaze_state = SnazeState::PickingRandomLevel;
  } else if (m_snaze_state == SnazeState::PickingRandomLevel) {
    if (still_levels_available()) {
      size_t random_idx =
          std::experimental::randint(0, (int)m_game_levels_files.size() - 1);
      m_maze = Maze(m_game_levels_files[random_idx]);
    } else if (m_remaining_snake_lives > 0) {
      m_snaze_state = SnazeState::Won;
    }
    m_score = 0;
    m_eaten_food_amount_snake = 0;
    m_remaining_snake_lives = m_settings.lives;
    m_snaze_state = SnazeState::GameStart;
  } else if (m_snaze_state == SnazeState::GameStart) {
    m_snaze_state = SnazeState::On;
  } else if (m_snaze_state == SnazeState::On) {
    bool ate = false;
    auto updated_snake_head_position = update_snake_position();
    if (m_maze.blocked(updated_snake_head_position, Direction::None) or
        m_snake.is_snake_body(updated_snake_head_position)) {
      m_snaze_state = SnazeState::Damage;
    } else if (m_maze.found_food(updated_snake_head_position)) {
      if (++m_eaten_food_amount_snake == m_settings.food_amount) {
        m_snaze_state = SnazeState::Won;
      }
      m_maze.random_food_position();
      ate = true;
      m_score += m_settings.food_amount * 2;
    }
    if (not ate) {
      m_snake.body.pop_back();
    }
  } else if (m_snaze_state == SnazeState::Won) {
    m_snaze_state = SnazeState::PickingRandomLevel;
  } else if (m_snaze_state == SnazeState::Damage) {
    m_snaze_state = SnazeState::GameStart;
    m_snake.reset();
  } else {
    std::cout << snaze_mode() << "Now in update\n";
    throw std::runtime_error("Invalid SnazeState");
  }
}

void SnazeManager::render() {
  clear_screen();
  if (m_snaze_state == SnazeState::Welcome) {
    screen_title("Snaze Game 🐍");
    main_content("Welcome to Snaze Game!\n\n");
    interaction_msg("Press <Enter> to continue");
  } else if (m_snaze_state == SnazeState::PickingRandomLevel) {
    screen_title("Snaze Game 🐍");
    main_content("Picking a random level...\n\n");
  } else if (m_snaze_state == SnazeState::Won) {
    screen_title("Snaze Game 🐍");
    main_content("Congratulations! You won the game!\n\n");
    interaction_msg("Press <Enter> to play again");
  } else if (m_snaze_state == SnazeState::Damage) {
    screen_title("Snaze Game 🐍");
    main_content("You lost a life!\n\n");
    interaction_msg("Press <Enter> to play again");
  } else if (m_snaze_state == SnazeState::GameStart) {
    main_content(game_loop_info() + m_maze.str_symbols() + m_maze.str_spawn());
    interaction_msg(controls_im());
  } else if (m_snaze_state == SnazeState::On) {
    main_content(game_loop_mc());
  } else {
    std::cout << snaze_mode() << "Now in update\n";
    throw std::runtime_error("Invalid SnazeState");
  }

  if (not m_screen_title.empty()) {
    std::cout << screen_title();
    m_screen_title.clear();
  }
  if (not m_main_content.empty()) {
    std::cout << main_content();
    m_main_content.clear();
  }
  if (not m_system_msg.empty()) {
    std::cout << system_msg();
    m_system_msg.clear();
  }
  if (not m_interaction_msg.empty()) {
    std::cout << interaction_msg() << std::flush;
    m_interaction_msg.clear();
  }
  if (m_snaze_state == SnazeState::On) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 / m_settings.fps));
  }
}
} // namespace snaze
