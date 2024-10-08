#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "maze.hpp"
#include "snake.hpp"
#include <sstream>
#include <stack>
#include <string>
#include <vector>

namespace snaze {

/// Snaze runnings opts that are cone be read from a ini file
struct Settings {
  size_t fps;
  size_t lives;
  size_t food_amount;
  std::string player_type;
};

/// Class keeps track of the Snaze as whole, and follows GameLoop design
/// pattern: https://gameprogrammingpatterns.com/game-loop.html
class SnazeManager {
  /// Enum that represents the possibles game states
  enum class SnazeState {
    Init,
    Welcome,
    PickingRandomLevel,
    GameStart,
    On,
    Damage,
    Won,
    Quit,
  };

  // Header info
  size_t m_score;

  // Back-end related
  SnazeState m_snaze_state{
      SnazeState::Init}; //!< The current state of the SnazeManager
  Settings m_settings{}; //!< Snaze running opts
  size_t m_remaining_snake_lives{
      0}; //!< Current game session amount of remaining lives
  size_t m_eaten_food_amount_snake{
      0};                  //!< How much food the snake have already eaten
  bool m_game_over{false}; //!< Boolean to tell if the game as ended or not
  bool m_new_game{true};
  SnakeBot m_snake_bot; //!< A bot that autoplays the game
  Snake m_snake;        //!< The actual snake that are being moved
  Maze m_maze;          //!< Representation of the maze
  std::vector<std::string>
      m_game_levels_files; //!<- A list containing all the game levels

  // Render related variables and methods
  std::string m_screen_title;
  std::string m_main_content;
  std::string m_system_msg;
  std::string m_interaction_msg;

  /* All screens may have up to 4 components:
   *  (1) title (st),
   *  (2) main content (mc),
   *  (3) a system message (sm), used for errors in process
   *  (4) an interaction message (im).
   */
  /// Gets the m_screen_title variable value
  [[nodiscard]] std::string screen_title() const;
  /// Gets the m_main_content variable value
  [[nodiscard]] std::string main_content() const;
  /// Gets the m_system_msg variable value
  [[nodiscard]] std::string system_msg() const;
  /// Gets the m_interaction_msg variable value
  [[nodiscard]] std::string interaction_msg() const;
  /// Sets the m_screen_title variable value
  void screen_title(std::string new_screen_title);
  /// Sets the m_main_content variable value
  void main_content(std::string new_main_content);
  /// Sets the m_system_msg variable value
  void system_msg(std::string new_system_msg);
  /// Sets the m_interaction_msg variable value
  void interaction_msg(std::string new_interaction_msg);
  /// The main menu screen of the snaze
  [[nodiscard]] static std::string main_menu_mc();
  /// Quit screen
  [[nodiscard]] static std::string quit_mc();
  /// SnazeMode screen
  [[nodiscard]] static std::string snaze_mode_mc();
  /// BotMode screen
  [[nodiscard]] static std::string bot_mode_mc();
  // Controls interaction message
  [[nodiscard]] std::string controls_im() const;
  /// It renders the remaining lives and pontuation of the player
  [[nodiscard]] std::string game_loop_info() const;
  /// It renders the snake with the maze
  [[nodiscard]] std::string game_loop_mc() const;

  // Process related variables and methods
  bool m_asked_to_quit{false};
  /// Update snake position based on her head direction, returns the new
  /// position of the snake
  Position update_snake_position();
  // Update related variables and methods
  void change_state_by_selected_menu_option();
  /// Verify if all the levels were played
  [[nodiscard]] bool still_levels_available();
  /// Calls the bot for thinking, can throw a exception when there's no solution
  void snake_bot_think(const Snake &snake);

  std::string snaze_mode() {
    std::ostringstream oss;
    switch (m_snaze_state) {
    case SnazeState::Welcome:
      oss << "Welcome to Snaze Game!\n\n";
      break;
    case SnazeState::PickingRandomLevel:
      oss << "Picking a random level...\n";
      break;
    case SnazeState::Won:
      oss << "Won the game\n\n";
      break;
    case SnazeState::Damage:
      oss << "Suffered Damage\n\n";
      break;
    case SnazeState::GameStart:
      oss << "Game Start\n\n";
      break;
    case SnazeState::On:
      oss << "On\n\n";
      break;
    case SnazeState::Quit:
      oss << "Quit\n\n";
      break;
    case SnazeState::Init:
      oss << "Init\n\n";
      break;
    }
    return oss.str();
  }

public:
  /// Constructor
  SnazeManager(const std::string &game_levels_directory,
               const std::string &ini_config_path);
  /// Function to process user o bot inputs
  void process();
  /// Function to update the snaze state
  void update();
  /// Function to display graphical things in the screen
  void render();
  /// Function that tells if the game will quit or not
  [[nodiscard]] bool quit() const { return m_asked_to_quit; }
};
} // namespace snaze

#endif // !GAME_MANAGER_HPP
