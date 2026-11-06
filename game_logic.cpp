#include "splashkit.h"
#include "game_types.h"

/** @brief Checks whether two rectangles overlap. */
bool boxes_overlap(const Box &box_a, const Box &box_b)
{
    return box_a.position_x < box_b.position_x + box_b.width &&
           box_a.position_x + box_a.width > box_b.position_x &&
           box_a.position_y < box_b.position_y + box_b.height &&
           box_a.position_y + box_a.height > box_b.position_y;
}

/** @brief Gets the collision box of a player. */
Box player_box(const Player &player)
{
    return Box{player.position_x, player.position_y, player.width, player.height};
}

/** @brief Gets the damage area of a spike strip. */
Box spike_hitbox(const Box &spike)
{
    return Box{spike.position_x + 3, spike.position_y + 4, spike.width - 6, spike.height - 4};
}

/** @brief Gets the damage area of a monster. */
Box monster_hitbox(const Monster &monster)
{
    return Box{monster.position_x + 3, monster.position_y + 3, MONSTER_W - 6, MONSTER_H - 3};
}

/** @brief Finds the ladder_index the player is touching. */
int find_touching_ladder(const Player &player, const Level &level)
{
    Box probe{player.position_x + player.width / 2 - 4, player.position_y, 8, player.height + 2};
    for (int item_index = 0; item_index < length(level.ladders); item_index++)
    {
        if (boxes_overlap(probe, level.ladders[item_index]))
        {
            return (int)item_index;
        }
    }
    return -1;
}

/** @brief Lands the player on one-way platforms. */
void resolve_platform_collisions(Player &player, const Level &level, double previous_bottom)
{
    if (player.velocity_y < 0)
    {
        return;
    }
    for (int item_index_1 = 0; item_index_1 < length(level.platforms); item_index_1++)
    {
        const Box &platform = level.platforms[item_index_1];
        bool horizontal_overlap = player.position_x + player.width > platform.position_x &&
                                  player.position_x < platform.position_x + platform.width;
        if (horizontal_overlap && previous_bottom <= platform.position_y + 0.01 &&
            player.position_y + player.height >= platform.position_y)
        {
            player.position_y = platform.position_y - player.height;
            player.velocity_y = 0;
            player.on_ground = true;
        }
    }
}

/** @brief Stops the player from walking into a solid wall. */
void resolve_wall_collisions_x(Player &player, const Level &level)
{
    for (int item_index_2 = 0; item_index_2 < length(level.walls); item_index_2++)
    {
        const Box &wall = level.walls[item_index_2];
        if (!boxes_overlap(player_box(player), wall))
        {
            continue;
        }
        if (player.velocity_x > 0)
        {
            player.position_x = wall.position_x - player.width;
        }
        else if (player.velocity_x < 0)
        {
            player.position_x = wall.position_x + wall.width;
        }
    }
}

/** @brief Handles vertical collisions with solid walls. */
void resolve_wall_collisions_y(Player &player, const Level &level)
{
    for (int item_index_3 = 0; item_index_3 < length(level.walls); item_index_3++)
    {
        const Box &wall = level.walls[item_index_3];
        if (!boxes_overlap(player_box(player), wall))
        {
            continue;
        }
        bool from_above = player.velocity_y > 0 ||
                          (player.velocity_y == 0 &&
                           player.position_y + player.height / 2 < wall.position_y + wall.height / 2);
        if (from_above)
        {
            player.position_y = wall.position_y - player.height;
            player.velocity_y = 0;
            player.on_ground = true;
            player.climbing = false;
        }
        else
        {
            player.position_y = wall.position_y + wall.height;
            if (player.velocity_y < 0)
            {
                player.velocity_y = 0;
            }
        }
    }
}

/** @brief Keeps the player inside the left, right and top edges of the screen. */
void apply_bounds(Player &player)
{
    if (player.position_x < 0)
    {
        player.position_x = 0;
    }
    if (player.position_x + player.width > GAME_W)
    {
        player.position_x = GAME_W - player.width;
    }
    if (player.position_y < HUD_H)
    {
        player.position_y = HUD_H;
        if (player.velocity_y < 0)
        {
            player.velocity_y = 0;
        }
    }
}

/** @brief Checks whether the player is touching a spike or a monster. */
bool player_hit_hazard(const Player &player, const Level &level)
{
    Box player_bounds = player_box(player);
    for (int item_index_4 = 0; item_index_4 < length(level.spikes); item_index_4++)
    {
        const Box &spike = level.spikes[item_index_4];
        if (boxes_overlap(player_bounds, spike_hitbox(spike)))
        {
            return true;
        }
    }
    for (int item_index_5 = 0; item_index_5 < length(level.monsters); item_index_5++)
    {
        const Monster &monster = level.monsters[item_index_5];
        if (boxes_overlap(player_bounds, monster_hitbox(monster)))
        {
            return true;
        }
    }
    return false;
}

/** @brief Creates a monster that patrols along the ground. */
Monster create_monster(double min_x, double max_x, double ground_y, double speed)
{
    Monster monster;
    monster.position_x = min_x;
    monster.start_x = min_x;
    monster.position_y = ground_y - MONSTER_H;
    monster.min_x = min_x;
    monster.max_x = max_x;
    monster.speed = speed;
    monster.direction = 1;
    return monster;
}

/** @brief Creates an uncollected star at a position. */
Star create_star(double position_x, double position_y)
{
    return Star{Box{position_x, position_y, STAR_SIZE, STAR_SIZE}, false};
}

/** @brief Builds the map for one level. */
Level load_level(int index)
{
    Level level = {};
    double start_y = 560 - PLAYER_H;

    if (index == 0)
    {

        level.name = "Level 1";
        level.hint = "Hop the spikes, dodge the slime, climb over the stone wall!";
        add(level.platforms, Box{0, 560, 300, 40});
        add(level.platforms, Box{400, 560, 400, 40});
        add(level.platforms, Box{450, 440, 130, 20});
        add(level.platforms, Box{690, 440, 110, 20});
        add(level.walls, Box{640, 420, 30, 140});
        add(level.ladders, Box{480, 440, 30, 120});
        add(level.ladders, Box{740, 440, 30, 120});
        add(level.spikes, Box{410, 548, 20, 12});
        add(level.monsters, create_monster(520, 610, 560, 1.5));
        add(level.stars, create_star(340, 470));
        add(level.stars, create_star(647, 350));
        add(level.stars, create_star(590, 536));
        level.key_box = {705, 530, 20, 20};
        level.door = {20, 500, 40, 60};
        level.player1_start_x = 90;
        level.player1_start_y = start_y;
        level.player2_start_x = 130;
        level.player2_start_y = start_y;
    }
    else if (index == 1)
    {

        level.name = "Level 2";
        level.hint = "Climb up to the high nook for the key. Mind the spikes by the door!";
        add(level.platforms, Box{0, 560, 200, 40});
        add(level.platforms, Box{300, 560, 200, 40});
        add(level.platforms, Box{600, 560, 200, 40});
        add(level.platforms, Box{300, 380, 140, 20});
        add(level.platforms, Box{240, 270, 160, 20});
        add(level.platforms, Box{230, 190, 80, 20});
        add(level.platforms, Box{20, 240, 160, 20});
        add(level.platforms, Box{620, 220, 180, 20});
        add(level.walls, Box{460, 400, 40, 160});
        add(level.walls, Box{180, 160, 20, 100});
        add(level.ladders, Box{330, 380, 30, 180});
        add(level.ladders, Box{370, 270, 30, 110});
        add(level.ladders, Box{650, 220, 30, 340});
        add(level.spikes, Box{700, 208, 24, 12});
        add(level.monsters, create_monster(690, 752, 560, 1.2));
        add(level.stars, create_star(240, 480));
        add(level.stars, create_star(262, 120));
        add(level.stars, create_star(772, 536));
        level.key_box = {40, 210, 20, 20};
        level.door = {740, 160, 40, 60};
        level.player1_start_x = 30;
        level.player1_start_y = start_y;
        level.player2_start_x = 70;
        level.player2_start_y = start_y;
    }
    else
    {

        level.name = "Level 3";
        level.hint = "Jump into the cave from its lower right. Then go over the tower!";
        add(level.platforms, Box{0, 560, 140, 40});
        add(level.platforms, Box{640, 560, 160, 40});
        add(level.platforms, Box{210, 500, 70, 20});
        add(level.platforms, Box{350, 450, 70, 20});
        add(level.platforms, Box{490, 380, 70, 20});
        add(level.platforms, Box{0, 300, 160, 20});
        add(level.platforms, Box{220, 240, 70, 20});
        add(level.platforms, Box{340, 180, 70, 20});
        add(level.platforms, Box{470, 120, 70, 20});
        add(level.walls, Box{420, 295, 100, 20});
        add(level.walls, Box{400, 215, 20, 100});
        add(level.walls, Box{400, 215, 140, 20});
        add(level.walls, Box{600, 180, 40, 380});
        add(level.ladders, Box{90, 300, 30, 260});
        add(level.spikes, Box{116, 548, 24, 12});
        add(level.monsters, create_monster(650, 706, 560, 1.0));
        add(level.stars, create_star(378, 360));
        add(level.stars, create_star(612, 148));
        add(level.stars, create_star(700, 470));
        level.key_box = {450, 265, 20, 20};
        level.door = {740, 500, 40, 60};
        level.player1_start_x = 20;
        level.player1_start_y = start_y;
        level.player2_start_x = 55;
        level.player2_start_y = start_y;
    }
    return level;
}

/** @brief Creates a player at a spawn point. */
Player create_player(double position_x, double position_y, color body_color, const string &label,
                     const Controls &controls)
{
    Player player;
    player.position_x = position_x;
    player.position_y = position_y;
    player.velocity_x = 0;
    player.velocity_y = 0;
    player.width = PLAYER_W;
    player.height = PLAYER_H;
    player.start_x = position_x;
    player.start_y = position_y;
    player.on_ground = false;
    player.climbing = false;
    player.facing = 1;
    player.body_color = body_color;
    player.label = label;
    player.controls = controls;
    return player;
}

/** @brief Gets player 1's keys: A/D move, W jump or climb up, S climb down, SPACE jump. */
Controls player1_controls()
{
    return Controls{A_KEY, D_KEY, W_KEY, S_KEY, SPACE_KEY};
}

/** @brief Gets player 2's keys: LEFT/RIGHT move, UP jump or climb up, DOWN climb down. */
Controls player2_controls()
{
    return Controls{LEFT_KEY, RIGHT_KEY, UP_KEY, DOWN_KEY, UP_KEY};
}

/** @brief Updates one player for one frame. */
double update_player(Player &player, const Level &level)
{
    bool left = key_down(player.controls.left);
    bool right = key_down(player.controls.right);
    bool up = key_down(player.controls.up);
    bool down = key_down(player.controls.down);
    bool jump_key = key_down(player.controls.jump);
    bool has_separate_jump = (player.controls.jump != player.controls.up);
    double climbed = 0;
    double previous_y = player.position_y;
    bool used_ladder_motion = false;

    player.velocity_x = 0;
    if (left)
    {
        player.velocity_x -= MOVE_SPEED;
    }
    if (right)
    {
        player.velocity_x += MOVE_SPEED;
    }
    if (player.velocity_x > 0)
    {
        player.facing = 1;
    }
    if (player.velocity_x < 0)
    {
        player.facing = -1;
    }
    if (player.climbing)
    {
        player.velocity_x *= 0.5;
    }

    int ladder_index = find_touching_ladder(player, level);
    if (ladder_index == -1)
    {
        player.climbing = false;
    }
    else if (!player.climbing)
    {
        const Box &ladder = level.ladders[ladder_index];
        double bottom = player.position_y + player.height;
        if (up && bottom > ladder.position_y + 4)
        {
            player.climbing = true;
        }
        else if (down && bottom < ladder.position_y + ladder.height - 4)
        {
            player.climbing = true;
        }
    }

    if (player.climbing)
    {
        player.velocity_y = 0;
        if (up)
        {
            player.velocity_y = -CLIMB_SPEED;
        }
        if (down)
        {
            player.velocity_y = CLIMB_SPEED;
        }
        used_ladder_motion = true;
        if (has_separate_jump && jump_key)
        {
            player.climbing = false;
            used_ladder_motion = false;
            player.velocity_y = JUMP_SPEED;
        }
    }
    else
    {
        if ((jump_key || up) && player.on_ground)
        {
            player.velocity_y = JUMP_SPEED;
            player.on_ground = false;
        }
        player.velocity_y += GRAVITY;
        if (player.velocity_y > MAX_FALL)
        {
            player.velocity_y = MAX_FALL;
        }
    }

    player.position_x += player.velocity_x;
    resolve_wall_collisions_x(player, level);

    double previous_bottom = player.position_y + player.height;
    player.position_y += player.velocity_y;
    player.on_ground = false;

    if (player.climbing && ladder_index != -1)
    {
        const Box &ladder = level.ladders[ladder_index];
        if (player.velocity_y < 0 && player.position_y + player.height <= ladder.position_y)
        {
            player.position_y = ladder.position_y - player.height;
            player.velocity_y = 0;
            player.climbing = false;
            player.on_ground = true;
        }
        else if (player.velocity_y > 0 &&
                 player.position_y + player.height >= ladder.position_y + ladder.height)
        {
            player.position_y = ladder.position_y + ladder.height - player.height;
            player.velocity_y = 0;
            player.climbing = false;
            player.on_ground = true;
        }
    }
    else
    {
        resolve_platform_collisions(player, level, previous_bottom);
    }

    resolve_wall_collisions_y(player, level);
    apply_bounds(player);
    if (used_ladder_motion)
    {
        climbed = fabs(player.position_y - previous_y);
    }
    return climbed;
}

/** @brief Moves every monster along its patrol path. */
void update_monsters(dynamic_array<Monster> &monsters)
{
    for (int item_index_6 = 0; item_index_6 < length(monsters); item_index_6++)
    {
        Monster &monster = monsters[item_index_6];
        monster.position_x += monster.speed * monster.direction;
        if (monster.position_x < monster.min_x)
        {
            monster.position_x = monster.min_x;
            monster.direction = 1;
        }
        else if (monster.position_x > monster.max_x)
        {
            monster.position_x = monster.max_x;
            monster.direction = -1;
        }
    }
}

/** @brief Creates a locked achievement. */
Achievement make_achievement(const string &name, const string &description)
{
    return Achievement{name, description, false, false};
}

/** @brief Fills in all achievements with their names and descriptions. */
void init_achievements(Game &game)
{
    game.achievements[ACH_FIRST_DEATH] = make_achievement("First Death", "Die for the first time.");
    game.achievements[ACH_SPEEDRUNNER] = make_achievement("Speedrunner", "Clear a level in under 2 min.");
    game.achievements[ACH_LEVEL_MASTER] = make_achievement("Level Master", "Clear all 3 levels in one mode.");
    game.achievements[ACH_FIRST_STEPS] = make_achievement("First Steps", "Clear your very first level.");
    game.achievements[ACH_KEY_FINDER] = make_achievement("Key Finder", "Pick up your first key.");
    game.achievements[ACH_LIGHTNING] = make_achievement("Lightning", "Clear a level in under 30 sec.");
    game.achievements[ACH_FLAWLESS] = make_achievement("Flawless", "Clear a level without dying.");
    game.achievements[ACH_TEAMWORK] = make_achievement("Teamwork", "Clear a level in 2P mode.");
    game.achievements[ACH_NEVER_GIVE_UP] = make_achievement("Never Give Up", "Die 10 times. Keep going!");
    game.achievements[ACH_CHAMPION] = make_achievement("Champion", "Clear all levels in both modes.");
    game.achievements[ACH_CLIMBER] = make_achievement("Climber", "Climb 3000 px of ladders.");
    game.achievements[ACH_STAR_CATCHER] = make_achievement("Star Catcher", "Clear a level with 3/3 stars.");
    game.achievements[ACH_STAR_HOARDER] = make_achievement("Star Hoarder", "3 stars on every level in a mode.");
    game.achievements[ACH_COLLECTOR] = make_achievement("Collector", "Unlock all other achievements.");
}

/** @brief Unlocks an achievement the first time its condition is met. */
void unlock_achievement(Game &game, int index)
{
    if (game.achievements[index].unlocked)
    {
        return;
    }
    game.achievements[index].unlocked = true;
    game.achievements[index].is_new = true;
    add(game.notifications, "New Achievement: " + game.achievements[index].name);
}

/** @brief Checks whether every level of a mode has been cleared. */
bool mode_all_cleared(const Game &game, int mode)
{
    for (int item_index = 0; item_index < LEVEL_COUNT; item_index++)
    {
        if (!game.completed[mode][item_index])
        {
            return false;
        }
    }
    return true;
}

/** @brief Checks whether every level of a mode has a best result of 3 stars. */
bool mode_all_stars(const Game &game, int mode)
{
    for (int item_index = 0; item_index < LEVEL_COUNT; item_index++)
    {
        if (game.best_stars[mode][item_index] < STARS_PER_LEVEL)
        {
            return false;
        }
    }
    return true;
}

/** @brief Checks whether at least one level has been cleared in a mode. */
bool any_level_cleared_in_mode(const Game &game, int mode)
{
    for (int item_index = 0; item_index < LEVEL_COUNT; item_index++)
    {
        if (game.completed[mode][item_index])
        {
            return true;
        }
    }
    return false;
}

/** @brief Checks every achievement condition and unlocks any that are met. */
void check_achievements(Game &game)
{
    if (game.total_deaths >= 1)
    {
        unlock_achievement(game, ACH_FIRST_DEATH);
    }
    if (game.fastest_clear < SPEEDRUN_LIMIT_MS)
    {
        unlock_achievement(game, ACH_SPEEDRUNNER);
    }
    if (mode_all_cleared(game, game.mode))
    {
        unlock_achievement(game, ACH_LEVEL_MASTER);
    }
    if (game.total_clears >= 1)
    {
        unlock_achievement(game, ACH_FIRST_STEPS);
    }
    if (game.total_keys >= 1)
    {
        unlock_achievement(game, ACH_KEY_FINDER);
    }
    if (game.fastest_clear < LIGHTNING_LIMIT_MS)
    {
        unlock_achievement(game, ACH_LIGHTNING);
    }
    if (game.flawless_clear)
    {
        unlock_achievement(game, ACH_FLAWLESS);
    }
    if (any_level_cleared_in_mode(game, 1))
    {
        unlock_achievement(game, ACH_TEAMWORK);
    }
    if (game.total_deaths >= DEATH_GOAL)
    {
        unlock_achievement(game, ACH_NEVER_GIVE_UP);
    }
    if (mode_all_cleared(game, 0) && mode_all_cleared(game, 1))
    {
        unlock_achievement(game, ACH_CHAMPION);
    }
    if (game.climb_distance >= CLIMB_GOAL)
    {
        unlock_achievement(game, ACH_CLIMBER);
    }
    if (game.perfect_star_clear)
    {
        unlock_achievement(game, ACH_STAR_CATCHER);
    }
    if (mode_all_stars(game, 0) || mode_all_stars(game, 1))
    {
        unlock_achievement(game, ACH_STAR_HOARDER);
    }

    bool all_others = true;
    for (int item_index = 0; item_index < ACH_COUNT; item_index++)
    {
        if (item_index != ACH_COLLECTOR && !game.achievements[item_index].unlocked)
        {
            all_others = false;
        }
    }
    if (all_others)
    {
        unlock_achievement(game, ACH_COLLECTOR);
    }
}

/** @brief Removes the "new" red dot from every achievement. */
void clear_new_marks(Game &game)
{
    for (int item_index = 0; item_index < ACH_COUNT; item_index++)
    {
        game.achievements[item_index].is_new = false;
    }
}

/** @brief Advances the achievement pop-up queue. */
void update_notifications(Game &game)
{
    if (length(game.notifications) == 0)
    {
        return;
    }
    if (game.notify_start == 0)
    {
        game.notify_start = current_ticks();
    }
    if (current_ticks() - game.notify_start > NOTIFY_DURATION_MS)
    {
        remove_at(game.notifications, 0);
        game.notify_start = 0;
    }
}

/** @brief Puts the players, key, stars and monsters back to their starting state. */
void reset_level_state(Game &game)
{
    for (int item_index_7 = 0; item_index_7 < length(game.players); item_index_7++)
    {
        Player &player = game.players[item_index_7];
        player.position_x = player.start_x;
        player.position_y = player.start_y;
        player.velocity_x = 0;
        player.velocity_y = 0;
        player.on_ground = false;
        player.climbing = false;
        player.facing = 1;
    }
    for (int item_index_8 = 0; item_index_8 < length(game.level.stars); item_index_8++)
    {
        Star &star = game.level.stars[item_index_8];
        star.collected = false;
    }
    for (int item_index_9 = 0; item_index_9 < length(game.level.monsters); item_index_9++)
    {
        Monster &monster = game.level.monsters[item_index_9];
        monster.position_x = monster.start_x;
        monster.direction = 1;
    }
    game.key_collected = false;
    game.key_holder = -1;
}

/** @brief Loads and starts a level. */
void start_level(Game &game, int index)
{
    game.level_index = index;
    game.level = load_level(index);

    clear(game.players);
    add(game.players, create_player(game.level.player1_start_x, game.level.player1_start_y, col_p1(), "P1",
                                    player1_controls()));
    if (game.mode == 1)
    {
        add(game.players, create_player(game.level.player2_start_x, game.level.player2_start_y, col_p2(),
                                        "P2", player2_controls()));
    }

    reset_level_state(game);
    game.level_deaths = 0;
    game.lives = LIVES_PER_LEVEL;
    game.last_hurt_ticks = 0;
    stop_timer(game.level_timer);
    start_timer(game.level_timer);
    game.has_paused_level = false;
    game.final_time = 0;
    game.final_stars = 0;
    game.state = STATE_PLAYING;
}

/** @brief Handles a player getting hurt (falling, spikes or a monster). */
/** @brief Keeps the current level in memory and pauses its clock. */
void pause_level(Game &game)
{
    if (game.state != STATE_PLAYING)
    {
        return;
    }
    game.paused_mode = game.mode;
    game.paused_level_index = game.level_index;
    game.has_paused_level = true;
    // Timer API reference: https://splashkit.io/api/timers/
    pause_timer(game.level_timer);
    game.menu_index = 2;
    game.state = STATE_MAIN_MENU;
}

/** @brief Resumes the same players, items, lives and clock. */
void resume_paused_level(Game &game)
{
    if (!game.has_paused_level)
    {
        return;
    }
    game.mode = game.paused_mode;
    game.level_index = game.paused_level_index;
    game.has_paused_level = false;
    // Timer API reference: https://splashkit.io/api/timers/
    resume_timer(game.level_timer);
    game.state = STATE_PLAYING;
}

void handle_death(Game &game)
{
    game.total_deaths++;
    game.level_deaths++;
    game.lives--;
    game.last_hurt_ticks = elapsed_time(game);
    if (audio_ready() && has_sound_effect("hurt"))
    {
        play_sound_effect("hurt", 0.35);
    }
    if (game.lives <= 0)
    {
        game.lives = 0;
        game.final_time = elapsed_time(game);
        pause_timer(game.level_timer);
        game.has_paused_level = false;
        game.state = STATE_GAME_OVER;
    }
    else
    {
        reset_level_state(game);
    }
}

/** @brief Records the result of a cleared level. */
void complete_level(Game &game)
{
    int mode_index = game.mode;
    int level_index = game.level_index;

    game.final_time = elapsed_time(game);
    pause_timer(game.level_timer);
    game.has_paused_level = false;
    game.final_stars = collected_star_count(game.level);
    game.completed[mode_index][level_index] = true;
    game.total_clears++;

    if (game.level_deaths == 0)
    {
        game.flawless_clear = true;
    }
    if (game.final_stars == STARS_PER_LEVEL)
    {
        game.perfect_star_clear = true;
    }
    if (game.final_time < game.best_time[mode_index][level_index])
    {
        game.best_time[mode_index][level_index] = game.final_time;
    }
    if (game.final_stars > game.best_stars[mode_index][level_index])
    {
        game.best_stars[mode_index][level_index] = game.final_stars;
    }
    if (game.final_time < game.fastest_clear)
    {
        game.fastest_clear = game.final_time;
    }

    game.state = STATE_LEVEL_COMPLETE;
}

/** @brief Lets players pick up the key and any stars they are touching. */
void collect_items(Game &game)
{
    for (int item_index = 0; item_index < length(game.players); item_index++)
    {
        Box player_bounds = player_box(game.players[item_index]);

        if (!game.key_collected && boxes_overlap(player_bounds, game.level.key_box))
        {
            game.key_collected = true;
            game.key_holder = (int)item_index;
            game.total_keys++;
        }

        for (int item_index_10 = 0; item_index_10 < length(game.level.stars); item_index_10++)
        {
            Star &star = game.level.stars[item_index_10];
            if (!star.collected && boxes_overlap(player_bounds, star.box))
            {
                star.collected = true;
            }
        }
    }
}

/** @brief Checks whether every player is standing in the door. */
bool all_players_at_door(const Game &game)
{
    for (int item_index_11 = 0; item_index_11 < length(game.players); item_index_11++)
    {
        const Player &player = game.players[item_index_11];
        if (!boxes_overlap(player_box(player), game.level.door))
        {
            return false;
        }
    }
    return true;
}

/** @brief Runs one frame of gameplay. */
void update_level(Game &game)
{
    if (key_typed(ESCAPE_KEY) || key_typed(P_KEY))
    {
        pause_level(game);
        return;
    }

    for (int item_index_12 = 0; item_index_12 < length(game.players); item_index_12++)
    {
        Player &player = game.players[item_index_12];
        game.climb_distance += update_player(player, game.level);
    }
    update_monsters(game.level.monsters);

    for (int item_index_13 = 0; item_index_13 < length(game.players); item_index_13++)
    {
        const Player &player = game.players[item_index_13];
        if (player.position_y > GAME_H || player_hit_hazard(player, game.level))
        {
            handle_death(game);
            return;
        }
    }

    collect_items(game);

    if (game.key_collected && all_players_at_door(game))
    {
        complete_level(game);
    }
}

/** @brief Checks whether a "move up" key (UP or W) was pressed this frame. */
bool menu_up_pressed()
{
    return key_typed(UP_KEY) || key_typed(W_KEY);
}

/** @brief Checks whether a "move down" key (DOWN or S) was pressed this frame. */
bool menu_down_pressed()
{
    return key_typed(DOWN_KEY) || key_typed(S_KEY);
}

/** @brief Moves a menu selection up or down, wrapping around at the ends. */
int move_selection(int index, int option_count)
{
    if (menu_up_pressed())
    {
        index = (index + option_count - 1) % option_count;
    }
    if (menu_down_pressed())
    {
        index = (index + 1) % option_count;
    }
    return index;
}

/** @brief Handles input on the main menu. */
/** @brief Handles mode selection, Continue, achievements and exit. */
void handle_main_menu(Game &game)
{
    int option_count = 4;
    if (game.has_paused_level)
    {
        option_count = 5;
    }
    if (game.menu_index >= option_count)
    {
        game.menu_index = 0;
    }
    game.menu_index = move_selection(game.menu_index, option_count);
    if (key_typed(RETURN_KEY))
    {
        int selected_action = game.menu_index;
        if (!game.has_paused_level && selected_action >= 2)
        {
            selected_action++;
        }
        switch (selected_action)
        {
        case 0:
            game.mode = 0;
            game.level_select_index = 0;
            game.state = STATE_LEVEL_SELECT;
            break;
        case 1:
            game.mode = 1;
            game.level_select_index = 0;
            game.state = STATE_LEVEL_SELECT;
            break;
        case 2:
            resume_paused_level(game);
            break;
        case 3:
            game.state = STATE_ACHIEVEMENTS;
            break;
        case 4:
            game.quit = true;
            break;
        }
    }
    if (key_typed(ESCAPE_KEY))
    {
        game.quit = true;
    }
}

/** @brief Handles input on the level select menu. */
void handle_level_select(Game &game)
{
    game.level_select_index = move_selection(game.level_select_index, LEVEL_COUNT + 1);
    if (key_typed(RETURN_KEY))
    {
        if (game.level_select_index < LEVEL_COUNT)
        {
            start_level(game, game.level_select_index);
        }
        else
        {
            game.state = STATE_MAIN_MENU;
        }
    }
    if (key_typed(ESCAPE_KEY))
    {
        game.state = STATE_MAIN_MENU;
    }
}

/** @brief Handles input on the level complete screen. */
/** @brief Checks actual progress before showing all-level completion. */
void handle_level_complete(Game &game)
{
    if (key_typed(RETURN_KEY))
    {
        if (mode_all_cleared(game, game.mode))
        {
            game.state = STATE_MODE_COMPLETE;
        }
        else if (game.level_index + 1 < LEVEL_COUNT)
        {
            start_level(game, game.level_index + 1);
        }
        else
        {
            game.level_select_index = game.level_index;
            game.state = STATE_LEVEL_SELECT;
        }
    }
    if (key_typed(ESCAPE_KEY))
    {
        game.level_select_index = game.level_index;
        game.state = STATE_LEVEL_SELECT;
    }
}

/** @brief Handles input on the Game Over screen. */
void handle_game_over(Game &game)
{
    if (key_typed(RETURN_KEY))
    {
        start_level(game, game.level_index);
    }
    if (key_typed(ESCAPE_KEY))
    {
        game.level_select_index = game.level_index;
        game.state = STATE_LEVEL_SELECT;
    }
}

/** @brief Handles input on the mode complete screen. */
void handle_mode_complete(Game &game)
{
    if (key_typed(RETURN_KEY) || key_typed(ESCAPE_KEY))
    {
        game.state = STATE_MAIN_MENU;
    }
}

/** @brief Handles input on the achievements page. */
void handle_achievements_page(Game &game)
{
    if (key_typed(RETURN_KEY) || key_typed(ESCAPE_KEY))
    {
        clear_new_marks(game);
        game.state = STATE_MAIN_MENU;
    }
}

/** @brief Sets every field of the game state to its starting value. */
void init_game(Game &game)
{
    game.state = STATE_MAIN_MENU;
    clear(game.players);
    clear(game.notifications);
    game.quit = false;
    game.menu_index = 0;
    game.level_select_index = 0;
    game.mode = 0;
    game.level_index = 0;
    game.key_collected = false;
    game.key_holder = -1;
    game.level_timer = create_timer("beans_level_clock");
    game.has_paused_level = false;
    game.paused_mode = 0;
    game.paused_level_index = 0;
    game.final_time = 0;
    game.final_stars = 0;
    game.level_deaths = 0;
    game.lives = LIVES_PER_LEVEL;
    game.last_hurt_ticks = 0;
    game.total_deaths = 0;
    game.total_clears = 0;
    game.total_keys = 0;
    game.climb_distance = 0;
    game.flawless_clear = false;
    game.perfect_star_clear = false;
    game.fastest_clear = NO_TIME;
    for (int mode_index = 0; mode_index < MODE_COUNT; mode_index++)
    {
        for (int level_index = 0; level_index < LEVEL_COUNT; level_index++)
        {
            game.completed[mode_index][level_index] = false;
            game.best_time[mode_index][level_index] = NO_TIME;
            game.best_stars[mode_index][level_index] = 0;
        }
    }
    game.notify_start = 0;
    init_achievements(game);
}

/** @brief Runs the logic for one frame. */
void update_game(Game &game)
{
    switch (game.state)
    {
    case STATE_MAIN_MENU:
        handle_main_menu(game);
        break;
    case STATE_LEVEL_SELECT:
        handle_level_select(game);
        break;
    case STATE_PLAYING:
        update_level(game);
        break;
    case STATE_LEVEL_COMPLETE:
        handle_level_complete(game);
        break;
    case STATE_MODE_COMPLETE:
        handle_mode_complete(game);
        break;
    case STATE_ACHIEVEMENTS:
        handle_achievements_page(game);
        break;
    case STATE_GAME_OVER:
        handle_game_over(game);
        break;
    }
    check_achievements(game);
    update_notifications(game);
}
