#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include "splashkit.h"
#include "splashkit-arrays.h"
#include <string>
#include <cmath>

using std::string;

const int GAME_W = 800;
const int GAME_H = 600;
const int HUD_H = 40;

const double GRAVITY = 0.5;
const double MAX_FALL = 12.0;
const double MOVE_SPEED = 4.0;
const double JUMP_SPEED = -10.0;
const double CLIMB_SPEED = 3.0;

const double PLAYER_W = 24;
const double PLAYER_H = 32;
const double MONSTER_W = 26;
const double MONSTER_H = 20;
const double STAR_SIZE = 16;

const int STARS_PER_LEVEL = 3;
const int LIVES_PER_LEVEL = 3;
const unsigned int HURT_FLASH_MS = 450;
const int LEVEL_COUNT = 3;
const int MODE_COUNT = 2;

const unsigned int SPEEDRUN_LIMIT_MS = 120000;
const unsigned int LIGHTNING_LIMIT_MS = 30000;
const unsigned int NOTIFY_DURATION_MS = 2500;
const unsigned int BANNER_DURATION_MS = 2500;
const int DEATH_GOAL = 10;
const double CLIMB_GOAL = 3000;
const unsigned int NO_TIME = 0xFFFFFFFF;

color col_text();
color col_text_soft();
color col_pink();
color col_mint();
color col_peach();
color col_lavender();
color col_gold();
color col_star_off();
color col_red_dot();
color col_p1();
color col_p2();

/** @brief An axis-aligned rectangle. */
struct Box
{
    double position_x, position_y;
    double width, height;
};

/** @brief The keyboard keys used by one player. */
struct Controls
{
    key_code left;
    key_code right;
    key_code up;
    key_code down;
    key_code jump;
};

/** @brief A player-controlled bean character. */
struct Player
{
    double position_x, position_y;
    double velocity_x, velocity_y;
    double width, height;
    double start_x, start_y;
    bool on_ground;
    bool climbing;
    int facing;
    color body_color;
    string label;
    Controls controls;
};

/** @brief A slime monster that walks back and forth between min_x and max_x. */
struct Monster
{
    double position_x, position_y;
    double start_x;
    double min_x, max_x;
    double speed;
    int direction;
};

/** @brief A collectable star. */
struct Star
{
    Box box;
    bool collected;
};

struct Glyph
{
    string rows[7];
};

/** @brief All the data that describes one level's map. */
struct Level
{
    string name;
    string hint;
    dynamic_array<Box> platforms;
    dynamic_array<Box> walls;
    dynamic_array<Box> ladders;
    dynamic_array<Box> spikes;
    dynamic_array<Monster> monsters;
    dynamic_array<Star> stars;
    Box key_box;
    Box door;
    double player1_start_x, player1_start_y;
    double player2_start_x, player2_start_y;
};

/** @brief Index of each achievement in Game::achievements. */
enum AchievementId
{
    ACH_FIRST_DEATH,
    ACH_SPEEDRUNNER,
    ACH_LEVEL_MASTER,
    ACH_FIRST_STEPS,
    ACH_KEY_FINDER,
    ACH_LIGHTNING,
    ACH_FLAWLESS,
    ACH_TEAMWORK,
    ACH_NEVER_GIVE_UP,
    ACH_CHAMPION,
    ACH_CLIMBER,
    ACH_STAR_CATCHER,
    ACH_STAR_HOARDER,
    ACH_COLLECTOR,
    ACH_COUNT
};

/** @brief At most one queued message per achievement can be added. */
struct Achievement
{
    string name;
    string description;
    bool unlocked;
    bool is_new;
};

/** @brief The screen the game is currently showing (a simple state machine). */
enum GameState
{
    STATE_MAIN_MENU,
    STATE_LEVEL_SELECT,
    STATE_PLAYING,
    STATE_LEVEL_COMPLETE,
    STATE_MODE_COMPLETE,
    STATE_ACHIEVEMENTS,
    STATE_GAME_OVER
};

/** @brief The complete state of the game. */
struct Game
{

    GameState state;
    bool quit;
    int menu_index;
    int level_select_index;
    int mode;
    int level_index;

    Level level;
    dynamic_array<Player> players;
    bool key_collected;
    int key_holder;

    timer level_timer;
    bool has_paused_level;
    int paused_mode;
    int paused_level_index;
    unsigned int final_time;
    int final_stars;
    int level_deaths;
    int lives;
    unsigned int last_hurt_ticks;

    int total_deaths;
    int total_clears;
    int total_keys;
    double climb_distance;
    bool flawless_clear;
    bool perfect_star_clear;
    unsigned int fastest_clear;
    bool completed[MODE_COUNT][LEVEL_COUNT];
    unsigned int best_time[MODE_COUNT][LEVEL_COUNT];
    int best_stars[MODE_COUNT][LEVEL_COUNT];

    Achievement achievements[ACH_COUNT];
    dynamic_array<string> notifications;
    unsigned int notify_start;
};

string pad_two(unsigned int value);
double wrapped_value(double value, double period);
string format_time(unsigned int milliseconds);
unsigned int elapsed_time(const Game &game);
double seconds_now();
int new_achievement_count(const Game &game);
int collected_star_count(const Level &level);
string mode_name(int mode);

bool boxes_overlap(const Box &box_a, const Box &box_b);
Box player_box(const Player &player);
Box spike_hitbox(const Box &spike);
Box monster_hitbox(const Monster &monster);
int find_touching_ladder(const Player &player, const Level &level);
void resolve_platform_collisions(Player &player, const Level &level, double previous_bottom);
void resolve_wall_collisions_x(Player &player, const Level &level);
void resolve_wall_collisions_y(Player &player, const Level &level);
void apply_bounds(Player &player);
bool player_hit_hazard(const Player &player, const Level &level);

Monster create_monster(double min_x, double max_x, double ground_y, double speed);
Star create_star(double position_x, double position_y);
Level load_level(int index);

Player create_player(double position_x, double position_y, color body_color, const string &label,
                     const Controls &controls);
Controls player1_controls();
Controls player2_controls();
double update_player(Player &player, const Level &level);
void update_monsters(dynamic_array<Monster> &monsters);

Achievement make_achievement(const string &name, const string &description);
void init_achievements(Game &game);
void unlock_achievement(Game &game, int index);
bool mode_all_cleared(const Game &game, int mode);
bool mode_all_stars(const Game &game, int mode);
bool any_level_cleared_in_mode(const Game &game, int mode);
void check_achievements(Game &game);
void clear_new_marks(Game &game);
void update_notifications(Game &game);

void reset_level_state(Game &game);
void start_level(Game &game, int index);
void pause_level(Game &game);
void resume_paused_level(Game &game);
void handle_death(Game &game);
void complete_level(Game &game);
void collect_items(Game &game);
bool all_players_at_door(const Game &game);
void update_level(Game &game);

bool menu_up_pressed();
bool menu_down_pressed();
int move_selection(int index, int option_count);
void handle_main_menu(Game &game);
void handle_level_select(Game &game);
void handle_level_complete(Game &game);
void handle_game_over(Game &game);
void handle_mode_complete(Game &game);
void handle_achievements_page(Game &game);

void init_game(Game &game);
void update_game(Game &game);

void canvas_rect(bitmap canvas, color draw_color, double position_x, double position_y, double width,
                 double height);
void canvas_outline(bitmap canvas, color draw_color, double position_x, double position_y, double width,
                    double height);
void canvas_circle(bitmap canvas, color draw_color, double position_x, double position_y, double radius);
void canvas_ellipse(bitmap canvas, color draw_color, double position_x, double position_y, double width,
                    double height);
void canvas_triangle(bitmap canvas, color draw_color, double first_x, double first_y, double second_x,
                     double second_y, double third_x, double third_y);
void canvas_line(bitmap canvas, color draw_color, double first_x, double first_y, double second_x,
                 double second_y);
void canvas_text(bitmap canvas, const string &text, color draw_color, double position_x, double position_y);
void canvas_text_centered(bitmap canvas, const string &text, color draw_color, double position_y);
void canvas_text_in_box(bitmap canvas, const string &text, color draw_color, double position_x,
                        double position_y, double width, double height);
void present_canvas(bitmap canvas);

void draw_round_rect(bitmap canvas, color draw_color, double position_x, double position_y, double width,
                     double height, double radius);
void draw_heart(bitmap canvas, double center_x, double center_y, double size, color draw_color);
void draw_star_shape(bitmap canvas, double center_x, double center_y, double radius, color draw_color);
void draw_star_row(bitmap canvas, double position_x, double center_y, double radius, int earned);
void draw_heart_row(bitmap canvas, double position_x, double center_y, double size, int remaining);
void draw_red_dot(bitmap canvas, double center_x, double center_y, double radius);
void draw_cloud(bitmap canvas, double position_x, double position_y, double cloud_scale);
void draw_sparkle(bitmap canvas, double position_x, double position_y, double size, int alpha);
void draw_pastel_background(bitmap canvas, double animation_time);
void draw_grass_ground(bitmap canvas, double ground_y);
void draw_cute_blob(bitmap canvas, double position_x, double position_y, double width, double height,
                    color body, int facing, bool blink);
void draw_key_shape(bitmap canvas, double position_x, double position_y, double size);
void draw_cute_button(bitmap canvas, const string &text, double position_x, double position_y, double width,
                      double height, color base, bool selected, double animation_time);
void draw_panel(bitmap canvas, double position_x, double position_y, double width, double height);
Glyph glyph_for(char character);
void draw_pixel_title(bitmap canvas, const string &title, double position_y, double pixel_size,
                      int color_offset, double animation_time);

void draw_main_menu(bitmap canvas, const Game &game);
void draw_level_select(bitmap canvas, const Game &game);

void draw_ladder(bitmap canvas, const Box &ladder);
void draw_platform(bitmap canvas, const Box &platform);
void draw_wall(bitmap canvas, const Box &wall);
void draw_spikes(bitmap canvas, const Box &spike);
void draw_monster(bitmap canvas, const Monster &monster, double animation_time);
void draw_level_star(bitmap canvas, const Star &cloud_scale, double animation_time);
void draw_door(bitmap canvas, const Box &door, bool unlocked, bool two_players);
void draw_player(bitmap canvas, const Player &player, bool holding_key, double animation_time);
void draw_hud(bitmap canvas, const Game &game);
void draw_level_banner(bitmap canvas, const Game &game);
void draw_hurt_flash(bitmap canvas, const Game &game);
void draw_door_messages(bitmap canvas, const Game &game);
void draw_level(bitmap canvas, const Game &game);

void draw_level_complete(bitmap canvas, const Game &game);
void draw_game_over(bitmap canvas, const Game &game);
void draw_mode_complete(bitmap canvas, const Game &game);
void draw_achievements_page(bitmap canvas, const Game &game);
void draw_notification(bitmap canvas, const Game &game);
void draw_game(bitmap canvas, const Game &game);

#endif