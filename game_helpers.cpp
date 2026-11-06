#include "splashkit.h"
#include "game_types.h"

color col_text()
{
    return rgb_color(105, 65, 115);
}
color col_text_soft()
{
    return rgb_color(160, 120, 170);
}
color col_pink()
{
    return rgb_color(255, 150, 190);
}
color col_mint()
{
    return rgb_color(150, 225, 195);
}
color col_peach()
{
    return rgb_color(255, 200, 150);
}
color col_lavender()
{
    return rgb_color(200, 175, 245);
}
color col_gold()
{
    return rgb_color(255, 205, 40);
}
color col_star_off()
{
    return rgb_color(210, 205, 220);
}
color col_red_dot()
{
    return rgb_color(240, 60, 75);
}
color col_p1()
{
    return rgb_color(110, 165, 245);
}
color col_p2()
{
    return rgb_color(250, 120, 150);
}

/** @brief Adds a box when there is free space in the array. */
string pad_two(unsigned int value)
{
    string text = std::to_string(value);
    if (value < 10)
    {
        text = "0" + text;
    }
    return text;
}

/** @brief Wraps a finite animation value using division and multiplication. */
double wrapped_value(double value, double period)
{
    if (period <= 0)
    {
        return 0;
    }
    long long cycles = (long long)(value / period);
    return value - cycles * period;
}

/** @brief Converts a time in milliseconds to the text "mm:ss.cc". */
string format_time(unsigned int milliseconds)
{
    if (milliseconds == NO_TIME)
    {
        return "--:--.--";
    }
    unsigned int minutes = milliseconds / 60000;
    unsigned int seconds = (milliseconds / 1000) % 60;
    unsigned int centiseconds = (milliseconds / 10) % 100;
    return pad_two(minutes) + ":" + pad_two(seconds) + "." + pad_two(centiseconds);
}

/** @brief Gets how long the current level has been running. */
unsigned int elapsed_time(const Game &game)
{
    return timer_ticks(game.level_timer);
}

/** @brief Gets the current program time in seconds. */
double seconds_now()
{
    return current_ticks() / 1000.0;
}

/** @brief Counts the achievements that are unlocked but not yet viewed. */
int new_achievement_count(const Game &game)
{
    int count = 0;
    for (int item_index = 0; item_index < ACH_COUNT; item_index++)
    {
        if (game.achievements[item_index].is_new)
        {
            count++;
        }
    }
    return count;
}

/** @brief Counts how many stars have been collected in a level. */
int collected_star_count(const Level &level)
{
    int count = 0;
    for (int item_index_1 = 0; item_index_1 < length(level.stars); item_index_1++)
    {
        const Star &star = level.stars[item_index_1];
        if (star.collected)
        {
            count++;
        }
    }
    return count;
}

/** @brief Gets the display name of a game mode. */
string mode_name(int mode)
{
    if (mode == 0)
    {
        return "Single Player";
    }
    else
    {
        return "Two Players";
    }
}
