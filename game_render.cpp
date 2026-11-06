#include "splashkit.h"
#include "game_types.h"

// AI-assisted addition: help me improve the map design more beautiful.
/** @brief Draws a filled rectangle on the canvas. */
void canvas_rect(bitmap canvas, color draw_color, double position_x, double position_y, double width,
                 double height)
{
    fill_rectangle(draw_color, position_x, position_y, width, height, option_draw_to(canvas));
}

/** @brief Draws a rectangle outline on the canvas. */
void canvas_outline(bitmap canvas, color draw_color, double position_x, double position_y, double width,
                    double height)
{
    draw_rectangle(draw_color, position_x, position_y, width, height, option_draw_to(canvas));
}

/** @brief Draws a filled circle on the canvas. */
void canvas_circle(bitmap canvas, color draw_color, double position_x, double position_y, double radius)
{
    fill_circle(draw_color, position_x, position_y, radius, option_draw_to(canvas));
}

/** @brief Draws a filled ellipse on the canvas. */
void canvas_ellipse(bitmap canvas, color draw_color, double position_x, double position_y, double width,
                    double height)
{
    fill_ellipse(draw_color, position_x, position_y, width, height, option_draw_to(canvas));
}

/** @brief Draws a filled triangle on the canvas. */
void canvas_triangle(bitmap canvas, color draw_color, double first_x, double first_y, double second_x,
                     double second_y, double third_x, double third_y)
{
    fill_triangle(draw_color, first_x, first_y, second_x, second_y, third_x, third_y, option_draw_to(canvas));
}

/** @brief Draws a straight line on the canvas. */
void canvas_line(bitmap canvas, color draw_color, double first_x, double first_y, double second_x,
                 double second_y)
{
    draw_line(draw_color, first_x, first_y, second_x, second_y, option_draw_to(canvas));
}

/** @brief Draws text on the canvas using SplashKit's default font. */
void canvas_text(bitmap canvas, const string &text, color draw_color, double position_x, double position_y)
{
    draw_text(text, draw_color, position_x, position_y, option_draw_to(canvas));
}

/** @brief Draws text horizontally centred on the canvas. */
void canvas_text_centered(bitmap canvas, const string &text, color draw_color, double position_y)
{
    canvas_text(canvas, text, draw_color, GAME_W / 2.0 - text.length() * 4.0, position_y);
}

/** @brief Draws text centred inside a rectangle. */
void canvas_text_in_box(bitmap canvas, const string &text, color draw_color, double position_x,
                        double position_y, double width, double height)
{
    canvas_text(canvas, text, draw_color, position_x + width / 2 - text.length() * 4.0,
                position_y + height / 2 - 4);
}

/** @brief Shows the finished canvas on screen, scaled to fit the window. */
void present_canvas(bitmap canvas)
{
    double win_w = current_window_width();
    double win_h = current_window_height();
    double scale = fmin(win_w / GAME_W, win_h / GAME_H);

    clear_screen(rgb_color(45, 30, 55));
    draw_bitmap(canvas, (win_w - GAME_W) / 2.0, (win_h - GAME_H) / 2.0, option_scale_bmp(scale, scale));
    refresh_screen(60);
}

/** @brief Draws a filled rectangle with rounded corners. */
void draw_round_rect(bitmap canvas, color draw_color, double position_x, double position_y, double width,
                     double height, double radius)
{
    if (radius * 2 > height)
    {
        radius = height / 2;
    }
    if (radius * 2 > width)
    {
        radius = width / 2;
    }
    canvas_rect(canvas, draw_color, position_x + radius, position_y, width - 2 * radius, height);
    canvas_rect(canvas, draw_color, position_x, position_y + radius, width, height - 2 * radius);
    canvas_circle(canvas, draw_color, position_x + radius, position_y + radius, radius);
    canvas_circle(canvas, draw_color, position_x + width - radius, position_y + radius, radius);
    canvas_circle(canvas, draw_color, position_x + radius, position_y + height - radius, radius);
    canvas_circle(canvas, draw_color, position_x + width - radius, position_y + height - radius, radius);
}

/** @brief Draws a heart from two circles and a triangle. */
void draw_heart(bitmap canvas, double center_x, double center_y, double size, color draw_color)
{
    double radius = size * 0.3;
    canvas_circle(canvas, draw_color, center_x - radius, center_y, radius);
    canvas_circle(canvas, draw_color, center_x + radius, center_y, radius);
    canvas_triangle(canvas, draw_color, center_x - 2 * radius, center_y + radius * 0.3, center_x + 2 * radius,
                    center_y + radius * 0.3, center_x, center_y + size * 0.75);
}

/** @brief Draws a six-pointed star made of two overlapping triangles. */
void draw_star_shape(bitmap canvas, double center_x, double center_y, double radius, color draw_color)
{
    canvas_triangle(canvas, draw_color, center_x, center_y - radius, center_x - radius * 0.87,
                    center_y + radius * 0.5, center_x + radius * 0.87, center_y + radius * 0.5);
    canvas_triangle(canvas, draw_color, center_x, center_y + radius, center_x - radius * 0.87,
                    center_y - radius * 0.5, center_x + radius * 0.87, center_y - radius * 0.5);
}

/** @brief Draws a row of three stars. Earned stars are gold, the rest grey. */
void draw_star_row(bitmap canvas, double position_x, double center_y, double radius, int earned)
{
    for (int item_index = 0; item_index < STARS_PER_LEVEL; item_index++)
    {
        double center_x = position_x + radius + item_index * (radius * 2.4);
        draw_star_shape(canvas, center_x, center_y, radius + 1.5, rgb_color(255, 255, 255));
        color star_color;
        if (item_index < earned)
        {
            star_color = col_gold();
        }
        else
        {
            star_color = col_star_off();
        }
        draw_star_shape(canvas, center_x, center_y, radius, star_color);
    }
}

/** @brief Draws a row of three hearts. Remaining hearts are pink, lost ones grey. */
void draw_heart_row(bitmap canvas, double position_x, double center_y, double size, int remaining)
{
    for (int item_index = 0; item_index < LIVES_PER_LEVEL; item_index++)
    {
        double center_x = position_x + size * 0.6 + item_index * (size * 1.5);
        draw_heart(canvas, center_x, center_y - size * 0.2, size + 3, rgb_color(255, 255, 255));
        color heart_color;
        if (item_index < remaining)
        {
            heart_color = rgb_color(255, 85, 130);
        }
        else
        {
            heart_color = col_star_off();
        }
        draw_heart(canvas, center_x, center_y - size * 0.15, size, heart_color);
    }
}

/** @brief Draws the red "new" dot with a white border and a small shine. */
void draw_red_dot(bitmap canvas, double center_x, double center_y, double radius)
{
    canvas_circle(canvas, rgb_color(255, 255, 255), center_x, center_y, radius + 2);
    canvas_circle(canvas, col_red_dot(), center_x, center_y, radius);
    canvas_circle(canvas, rgba_color(255, 255, 255, 130), center_x - radius * 0.35, center_y - radius * 0.35,
                  radius * 0.3);
}

/** @brief Draws a fluffy white cloud from three circles and a rectangle. */
void draw_cloud(bitmap canvas, double position_x, double position_y, double cloud_scale)
{
    color white = rgb_color(255, 255, 255);
    canvas_circle(canvas, white, position_x, position_y, 18 * cloud_scale);
    canvas_circle(canvas, white, position_x + 22 * cloud_scale, position_y - 10 * cloud_scale,
                  24 * cloud_scale);
    canvas_circle(canvas, white, position_x + 46 * cloud_scale, position_y, 18 * cloud_scale);
    canvas_rect(canvas, white, position_x, position_y, 46 * cloud_scale, 18 * cloud_scale);
}

/** @brief Draws a small cross-shaped sparkle. */
void draw_sparkle(bitmap canvas, double position_x, double position_y, double size, int alpha)
{
    color draw_color = rgba_color(255, 255, 255, alpha);
    canvas_rect(canvas, draw_color, position_x - size, position_y - 1, size * 2, 2);
    canvas_rect(canvas, draw_color, position_x - 1, position_y - size, 2, size * 2);
}

/** @brief Draws the pastel background used on menu screens. */
void draw_pastel_background(bitmap canvas, double animation_time)
{

    for (int position_y = 0; position_y < GAME_H; position_y += 4)
    {
        double gradient_ratio = (double)position_y / GAME_H;
        canvas_rect(canvas,
                    rgb_color((int)(255 - 45 * gradient_ratio), (int)(205 + 30 * gradient_ratio),
                              (int)(230 + 20 * gradient_ratio)),
                    0, position_y, GAME_W, 4);
    }

    for (int item_index = 0; item_index < 4; item_index++)
    {
        double center_x =
            wrapped_value(item_index * 240 + animation_time * (12 + item_index * 6), GAME_W + 200) - 120;
        draw_cloud(canvas, center_x, 50 + (item_index * 67) % 140, 0.8 + 0.2 * (item_index % 2));
    }

    for (int item_index = 0; item_index < 14; item_index++)
    {
        double sparkle_ratio = (sin(animation_time * 2.2 + item_index * 1.3) + 1) / 2;
        draw_sparkle(canvas, wrapped_value(item_index * 97.3 + 40, GAME_W),
                     30 + wrapped_value(item_index * 53.7, 420), 3 + 4 * sparkle_ratio,
                     (int)(80 + 175 * sparkle_ratio));
    }
}

/** @brief Draws sparkle_ratio bumpy grass field with small flowers. */
void draw_grass_ground(bitmap canvas, double ground_y)
{
    color grass = rgb_color(150, 220, 130);
    for (double position_x = 0; position_x <= GAME_W + 40; position_x += 40)
    {
        canvas_circle(canvas, grass, position_x, ground_y + 10, 26);
    }
    canvas_rect(canvas, grass, 0, ground_y + 10, GAME_W, GAME_H - ground_y);
    canvas_rect(canvas, rgb_color(120, 195, 110), 0, ground_y + 45, GAME_W, GAME_H - ground_y);

    color petals[4] = {rgb_color(255, 170, 200), rgb_color(255, 255, 255), rgb_color(200, 175, 245),
                       rgb_color(255, 210, 150)};
    for (int item_index = 0; item_index < 9; item_index++)
    {
        double fx = 30 + item_index * 90;
        double fy = ground_y + 22 + (item_index % 3) * 6;
        canvas_circle(canvas, petals[item_index % 4], fx - 4, fy, 3.5);
        canvas_circle(canvas, petals[item_index % 4], fx + 4, fy, 3.5);
        canvas_circle(canvas, petals[item_index % 4], fx, fy - 4, 3.5);
        canvas_circle(canvas, petals[item_index % 4], fx, fy + 4, 3.5);
        canvas_circle(canvas, col_gold(), fx, fy, 2.5);
    }
}

/** @brief Draws a cute bean character with eyes, blush and a small smile. */
void draw_cute_blob(bitmap canvas, double position_x, double position_y, double width, double height,
                    color body, int facing, bool blink)
{
    color ink = rgb_color(60, 40, 70);

    draw_round_rect(canvas, body, position_x, position_y, width, height, width * 0.35);
    canvas_circle(canvas, rgba_color(255, 255, 255, 90), position_x + width * 0.28, position_y + height * 0.2, width * 0.12);

    double off = facing * width * 0.07;
    double eye_r = width * 0.12;
    double eye_y = position_y + height * 0.42;
    double letter_x = position_x + width * 0.33 + off;
    double rx = position_x + width * 0.67 + off;
    if (blink)
    {
        canvas_rect(canvas, ink, letter_x - eye_r, eye_y, eye_r * 2, width * 0.05 + 1);
        canvas_rect(canvas, ink, rx - eye_r, eye_y, eye_r * 2, width * 0.05 + 1);
    }
    else
    {
        canvas_circle(canvas, ink, letter_x, eye_y, eye_r);
        canvas_circle(canvas, ink, rx, eye_y, eye_r);
        canvas_circle(canvas, rgb_color(255, 255, 255), letter_x - eye_r * 0.3, eye_y - eye_r * 0.3,
                      eye_r * 0.4);
        canvas_circle(canvas, rgb_color(255, 255, 255), rx - eye_r * 0.3, eye_y - eye_r * 0.3, eye_r * 0.4);
    }

    color blush = rgba_color(255, 120, 150, 150);
    canvas_ellipse(canvas, blush, position_x + width * 0.08 + off, position_y + height * 0.56, width * 0.2,
                   height * 0.09);
    canvas_ellipse(canvas, blush, position_x + width * 0.72 + off, position_y + height * 0.56, width * 0.2,
                   height * 0.09);

    double mx = position_x + width / 2 + off;
    double my = position_y + height * 0.6;
    canvas_line(canvas, ink, mx - width * 0.07, my, mx, my + width * 0.05);
    canvas_line(canvas, ink, mx, my + width * 0.05, mx + width * 0.07, my);
}

/** @brief Draws a gold key (round head, shaft and two teeth). */
void draw_key_shape(bitmap canvas, double position_x, double position_y, double size)
{
    double radius = size * 0.3;
    canvas_circle(canvas, col_gold(), position_x + radius, position_y + size / 2, radius);
    canvas_circle(canvas, rgb_color(255, 245, 200), position_x + radius, position_y + size / 2, radius * 0.4);
    canvas_rect(canvas, col_gold(), position_x + radius, position_y + size / 2 - 2, size - radius, 4);
    canvas_rect(canvas, col_gold(), position_x + size - 4, position_y + size / 2, 3, 5);
    canvas_rect(canvas, col_gold(), position_x + size - 9, position_y + size / 2, 3, 4);
}

/** @brief Draws a rounded menu button. */
void draw_cute_button(bitmap canvas, const string &text, double position_x, double position_y, double width,
                      double height, color base, bool selected, double animation_time)
{
    if (selected)
    {
        position_y += sin(animation_time * 6) * 3;
        position_x -= 8;
        width += 16;
        draw_round_rect(canvas, rgba_color(120, 70, 130, 60), position_x + 3, position_y + 5, width, height,
                        height / 2);
        draw_round_rect(canvas, rgb_color(255, 255, 255), position_x - 3, position_y - 3, width + 6,
                        height + 6, height / 2 + 3);
        draw_round_rect(canvas, base, position_x, position_y, width, height, height / 2);
        draw_heart(canvas, position_x + 24, position_y + height / 2 - 4, 16, rgb_color(255, 90, 140));
        draw_heart(canvas, position_x + width - 24, position_y + height / 2 - 4, 16, rgb_color(255, 90, 140));
    }
    else
    {
        draw_round_rect(canvas, rgba_color(120, 70, 130, 40), position_x + 2, position_y + 4, width, height, height / 2);
        draw_round_rect(canvas, rgb_color(255, 250, 252), position_x, position_y, width, height, height / 2);
        draw_round_rect(canvas, base, position_x + 4, position_y + 4, width - 8, height - 8, height / 2 - 4);
    }
    canvas_text_in_box(canvas, text, col_text(), position_x, position_y, width, height);
}

/** @brief Draws a white rounded panel with a soft shadow. */
void draw_panel(bitmap canvas, double position_x, double position_y, double width, double height)
{
    draw_round_rect(canvas, rgba_color(120, 70, 130, 50), position_x + 4, position_y + 6, width, height, 18);
    draw_round_rect(canvas, rgb_color(255, 252, 254), position_x, position_y, width, height, 18);
}

/** @brief Gets the 5x7 pixel pattern of a capital letter for the title. */
Glyph glyph_for(char character)
{
    switch (character)
    {
    case 'A':
        return Glyph{{".###.", "#...#", "#...#", "#####", "#...#", "#...#", "#...#"}};
    case 'B':
        return Glyph{{"####.", "#...#", "#...#", "####.", "#...#", "#...#", "####."}};
    case 'C':
        return Glyph{{".####", "#....", "#....", "#....", "#....", "#....", ".####"}};
    case 'D':
        return Glyph{{"####.", "#...#", "#...#", "#...#", "#...#", "#...#", "####."}};
    case 'E':
        return Glyph{{"#####", "#....", "#....", "####.", "#....", "#....", "#####"}};
    case 'N':
        return Glyph{{"#...#", "##..#", "#.#.#", "#..##", "#...#", "#...#", "#...#"}};
    case 'R':
        return Glyph{{"####.", "#...#", "#...#", "####.", "#.#..", "#..#.", "#...#"}};
    case 'S':
        return Glyph{{".####", "#....", "#....", ".###.", "....#", "....#", "####."}};
    case 'T':
        return Glyph{{"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "..#.."}};
    case 'U':
        return Glyph{{"#...#", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."}};
    case 'V':
        return Glyph{{"#...#", "#...#", "#...#", "#...#", "#...#", ".#.#.", "..#.."}};
    default:
        return Glyph{{".....", ".....", ".....", ".....", ".....", ".....", "....."}};
    }
}

/** @brief Draws a large, colourful pixel-art title that waves up and down. */
void draw_pixel_title(bitmap canvas, const string &title, double position_y, double pixel_size,
                      int color_offset, double animation_time)
{
    const double letter_w = 5 * pixel_size;
    const double spacing = pixel_size * 1.5;
    double start_x = GAME_W / 2.0 - (title.length() * (letter_w + spacing) - spacing) / 2;
    color palette[6] = {rgb_color(255, 110, 165), rgb_color(255, 160, 80), rgb_color(245, 195, 40),
                        rgb_color(80, 200, 160),  rgb_color(90, 165, 240), rgb_color(165, 120, 230)};

    for (size_t item_index = 0; item_index < title.length(); item_index++)
    {
        if (title[item_index] == ' ')
        {
            continue;
        }
        Glyph glyph = glyph_for(title[item_index]);
        double letter_x = start_x + item_index * (letter_w + spacing);
        double letter_y = position_y + sin(animation_time * 3 + item_index * 0.7 + color_offset) * 5;
        color draw_color = palette[(item_index + color_offset) % 6];
        for (int row = 0; row < 7; row++)
        {
            for (int col = 0; col < 5; col++)
            {
                if (glyph.rows[row][col] != '#')
                {
                    continue;
                }
                double button_x = letter_x + col * pixel_size;
                double button_y = letter_y + row * pixel_size;
                canvas_rect(canvas, rgba_color(110, 60, 120, 90), button_x + 3, button_y + 3, pixel_size - 1,
                            pixel_size - 1);
                canvas_rect(canvas, draw_color, button_x, button_y, pixel_size - 1, pixel_size - 1);
                canvas_rect(canvas, rgba_color(255, 255, 255, 140), button_x + 1, button_y + 1, 2, 2);
            }
        }
    }
}

/** @brief Draws the main menu (title screen). */
void draw_main_menu(bitmap canvas, const Game &game)
{
    double animation_time = seconds_now();
    draw_pastel_background(canvas, animation_time);
    draw_grass_ground(canvas, 490);

    draw_pixel_title(canvas, "CUTE BEANS", 65, 8, 0, animation_time);
    draw_pixel_title(canvas, "ADVENTURE", 135, 6, 3, animation_time);
    canvas_text_centered(canvas, "~ two little beans, one big adventure ~", col_text_soft(), 195);
    draw_heart(canvas, GAME_W / 2.0 - 180, 195, 12, col_pink());
    draw_heart(canvas, GAME_W / 2.0 + 180, 195, 12, col_pink());

    string options[5] = {"Single Player", "Two Players", "Achievements", "Exit", ""};
    color colors[5] = {col_mint(), col_peach(), col_lavender(), col_pink(), col_mint()};
    int option_count = 4;
    int achievements_index = 2;
    double button_spacing = 58;
    if (game.has_paused_level)
    {
        options[2] =
            "Continue " + mode_name(game.paused_mode) + " L" + std::to_string(game.paused_level_index + 1);
        options[3] = "Achievements";
        options[4] = "Exit";
        colors[2] = col_mint();
        colors[3] = col_lavender();
        colors[4] = col_pink();
        option_count = 5;
        achievements_index = 3;
        button_spacing = 49;
    }
    double button_width = 260, button_height = 44, button_x = GAME_W / 2.0 - button_width / 2;
    int new_count = new_achievement_count(game);

    for (int item_index = 0; item_index < option_count; item_index++)
    {
        double button_y = 225 + item_index * button_spacing;
        draw_cute_button(canvas, options[item_index], button_x, button_y, button_width, button_height,
                         colors[item_index], game.menu_index == item_index, animation_time);
        if (item_index == achievements_index && new_count > 0)
        {
            double selection_offset;
            if (game.menu_index == item_index)
            {
                selection_offset = 10;
            }
            else
            {
                selection_offset = 0;
            }
            double dot_x = button_x + button_width - 6 + selection_offset;
            string number_text = std::to_string(new_count);
            draw_red_dot(canvas, dot_x, button_y + 4, 11);
            canvas_text(canvas, number_text, rgb_color(255, 255, 255), dot_x - number_text.length() * 4.0, button_y);
        }
    }

    double bounce1 = fabs(sin(animation_time * 3)) * 28;
    double bounce2 = fabs(sin(animation_time * 3 + 1.4)) * 28;
    canvas_ellipse(canvas, rgba_color(60, 120, 60, 60), 82, 492, 66, 12);
    canvas_ellipse(canvas, rgba_color(60, 120, 60, 60), 652, 492, 66, 12);
    draw_cute_blob(canvas, 85, 425 - bounce1, 60, 70, col_p1(), 1, wrapped_value(animation_time, 3.2) < 0.15);
    draw_cute_blob(canvas, 655, 425 - bounce2, 60, 70, col_p2(), -1,
                   wrapped_value(animation_time + 1.1, 2.7) < 0.15);
    draw_key_shape(canvas, 100, 400 - bounce1, 30);
    draw_star_shape(canvas, 685, 400 - bounce2, 13, col_gold());

    canvas_text_centered(canvas, "W/S or UP/DOWN to choose   ENTER to start   F11 fullscreen", col_text(), 570);
}

/** @brief Draws the level select menu for the current mode. */
void draw_level_select(bitmap canvas, const Game &game)
{
    double animation_time = seconds_now();
    draw_pastel_background(canvas, animation_time);

    draw_panel(canvas, 150, 30, 500, 46);
    canvas_text_in_box(canvas, mode_name(game.mode) + "  -  Choose a Level", col_text(), 150, 30, 500, 46);
    draw_star_shape(canvas, 185, 53, 10, col_gold());
    draw_star_shape(canvas, 615, 53, 10, col_gold());

    color colors[4] = {col_mint(), col_peach(), col_lavender(), col_pink()};
    double button_width = 320, button_height = 44, button_x = GAME_W / 2.0 - button_width / 2;

    for (int item_index = 0; item_index < LEVEL_COUNT; item_index++)
    {
        double button_y = 100 + item_index * 82;
        draw_cute_button(canvas, "Level " + std::to_string(item_index + 1), button_x, button_y, button_width,
                         button_height, colors[item_index], game.level_select_index == item_index,
                         animation_time);

        draw_star_row(canvas, button_x + button_width - 78, button_y + button_height / 2, 9,
                      game.best_stars[game.mode][item_index]);

        string time_text = "Best time: " + format_time(game.best_time[game.mode][item_index]);
        color time_color;
        if (game.completed[game.mode][item_index])
        {
            time_color = col_text();
        }
        else
        {
            time_color = col_text_soft();
        }
        canvas_text_centered(canvas, time_text, time_color, button_y + button_height + 10);
    }
    draw_cute_button(canvas, "Back", button_x, 100 + LEVEL_COUNT * 82, button_width, button_height, colors[3],
                     game.level_select_index == LEVEL_COUNT, animation_time);

    draw_panel(canvas, 70, 410, 660, 180);
    canvas_text_centered(canvas, "How to Play", col_text(), 424);
    canvas_text_centered(canvas, "P1 (blue): A/D move, W jump/climb up, S climb down, SPACE jump",
                         col_text_soft(), 448);
    if (game.mode == 1)
    {
        canvas_text_centered(canvas, "P2 (pink): LEFT/RIGHT move, UP jump/climb up, DOWN climb down",
                             col_text_soft(), 468);
    }
    if (game.mode == 1)
    {
        canvas_text_centered(canvas, "Grab the key, then BOTH players must stand in the door.",
                             col_text_soft(), 494);
    }
    else
    {
        canvas_text_centered(canvas, "Grab the key, then walk into the door. Collect 3 stars!",
                             col_text_soft(), 494);
    }
    canvas_text_centered(canvas, "3 hearts per level. Spikes, slimes or falling = lose 1 heart.",
                         col_text_soft(), 516);
    canvas_text_centered(canvas, "ENTER: new run    ESC: back", col_text(), 546);
    canvas_text_centered(canvas, "In game: ESC/P pauses. Main menu Continue resumes. New run replaces pause.",
                         col_text_soft(), 568);
}

/** @brief Draws a wooden ladder (two side rails and evenly spaced rungs). */
void draw_ladder(bitmap canvas, const Box &ladder)
{
    color wood = rgb_color(190, 130, 80);
    canvas_rect(canvas, wood, ladder.position_x, ladder.position_y, 4, ladder.height);
    canvas_rect(canvas, wood, ladder.position_x + ladder.width - 4, ladder.position_y, 4, ladder.height);
    for (double ry = ladder.position_y + 6; ry < ladder.position_y + ladder.height; ry += 12)
    {
        canvas_rect(canvas, wood, ladder.position_x, ry, ladder.width, 3);
    }
}

/** @brief Draws a platform as a block of soil with grass on top. */
void draw_platform(bitmap canvas, const Box &platform)
{
    draw_round_rect(canvas, rgb_color(200, 150, 110), platform.position_x, platform.position_y,
                    platform.width, platform.height, 5);
    draw_round_rect(canvas, rgb_color(140, 215, 120), platform.position_x, platform.position_y,
                    platform.width, 7, 3);
}

/** @brief Draws a solid stone wall with a brick pattern. */
void draw_wall(bitmap canvas, const Box &wall)
{
    color stone = rgb_color(165, 160, 185);
    color line = rgb_color(125, 120, 150);
    canvas_rect(canvas, stone, wall.position_x, wall.position_y, wall.width, wall.height);
    int row = 0;
    for (double position_y = wall.position_y; position_y < wall.position_y + wall.height;
         position_y += 10, row++)
    {
        canvas_line(canvas, line, wall.position_x, position_y, wall.position_x + wall.width, position_y);
        double start;
        if (row % 2 == 0)
        {
            start = 0;
        }
        else
        {
            start = 10;
        }
        for (double position_x = wall.position_x + start; position_x < wall.position_x + wall.width;
             position_x += 20)
        {
            canvas_line(canvas, line, position_x, position_y, position_x,
                        fmin(position_y + 10, wall.position_y + wall.height));
        }
    }
    canvas_outline(canvas, line, wall.position_x, wall.position_y, wall.width, wall.height);
    canvas_rect(canvas, rgb_color(195, 190, 215), wall.position_x, wall.position_y, wall.width, 3);
}

/** @brief Draws a strip of silver spikes (a row of 8 px wide triangles). */
void draw_spikes(bitmap canvas, const Box &spike)
{
    double tooth = 8;
    for (double position_x = spike.position_x; position_x + tooth <= spike.position_x + spike.width + 0.1;
         position_x += tooth)
    {
        canvas_triangle(canvas, rgb_color(150, 145, 170), position_x, spike.position_y + spike.height,
                        position_x + tooth, spike.position_y + spike.height, position_x + tooth / 2,
                        spike.position_y);
        canvas_triangle(canvas, rgb_color(235, 235, 245), position_x + tooth / 2, spike.position_y + 2,
                        position_x + tooth / 2, spike.position_y + spike.height, position_x + tooth * 0.2,
                        spike.position_y + spike.height);
    }
}

/** @brief Draws a purple jelly slime with angry eyes and two small fangs. */
void draw_monster(bitmap canvas, const Monster &monster, double animation_time)
{
    double squish = sin(animation_time * 8 + monster.start_x) * 2;
    double position_x = monster.position_x - squish / 2;
    double position_y = monster.position_y + squish;
    double width = MONSTER_W + squish;
    double height = MONSTER_H - squish;
    color ink = rgb_color(60, 30, 70);

    draw_round_rect(canvas, rgb_color(170, 110, 220), position_x, position_y, width, height, 9);
    canvas_circle(canvas, rgba_color(255, 255, 255, 100), position_x + 7, position_y + 5, 3);

    double off = monster.direction * 2;
    canvas_circle(canvas, rgb_color(255, 255, 255), position_x + width * 0.33 + off,
                  position_y + height * 0.45, 4);
    canvas_circle(canvas, rgb_color(255, 255, 255), position_x + width * 0.67 + off,
                  position_y + height * 0.45, 4);
    canvas_circle(canvas, ink, position_x + width * 0.33 + off * 1.5, position_y + height * 0.48, 2);
    canvas_circle(canvas, ink, position_x + width * 0.67 + off * 1.5, position_y + height * 0.48, 2);
    canvas_line(canvas, ink, position_x + width * 0.2 + off, position_y + height * 0.2,
                position_x + width * 0.42 + off, position_y + height * 0.32);
    canvas_line(canvas, ink, position_x + width * 0.8 + off, position_y + height * 0.2,
                position_x + width * 0.58 + off, position_y + height * 0.32);
    canvas_triangle(canvas, rgb_color(255, 255, 255), position_x + width * 0.4, position_y + height * 0.72,
                    position_x + width * 0.48, position_y + height * 0.72, position_x + width * 0.44,
                    position_y + height * 0.9);
    canvas_triangle(canvas, rgb_color(255, 255, 255), position_x + width * 0.52, position_y + height * 0.72,
                    position_x + width * 0.6, position_y + height * 0.72, position_x + width * 0.56,
                    position_y + height * 0.9);
}

/** @brief Draws a floating gold star with a sparkle, if it has not been collected. */
void draw_level_star(bitmap canvas, const Star &star, double animation_time)
{
    if (star.collected)
    {
        return;
    }
    double bob = sin(animation_time * 4 + star.box.position_x) * 3;
    double center_x = star.box.position_x + star.box.width / 2;
    double center_y = star.box.position_y + star.box.height / 2 + bob;
    draw_star_shape(canvas, center_x, center_y, 10, rgb_color(255, 255, 255));
    draw_star_shape(canvas, center_x, center_y, 8, col_gold());
    draw_sparkle(canvas, center_x + 9, center_y - 8, 3, (int)(150 + 100 * sin(animation_time * 5)));
}

/** @brief Draws the exit door. */
void draw_door(bitmap canvas, const Box &door, bool unlocked, bool two_players)
{
    if (unlocked)
    {
        draw_round_rect(canvas, rgb_color(130, 210, 150), door.position_x, door.position_y, door.width,
                        door.height, 8);
        draw_round_rect(canvas, rgb_color(70, 130, 90), door.position_x + 6, door.position_y + 6,
                        door.width - 12, door.height - 6, 5);
        canvas_text(canvas, "OPEN", col_text(), door.position_x + 4, door.position_y - 24);
        if (two_players)
        {
            canvas_text(canvas, "both!", col_text_soft(), door.position_x, door.position_y - 12);
        }
    }
    else
    {
        draw_round_rect(canvas, rgb_color(190, 120, 90), door.position_x, door.position_y, door.width,
                        door.height, 8);
        canvas_rect(canvas, col_gold(), door.position_x + door.width / 2 - 5,
                    door.position_y + door.height / 2 - 4, 10, 10);
        canvas_text(canvas, "LOCK", col_text(), door.position_x + 4, door.position_y - 12);
    }
    canvas_circle(canvas, col_gold(), door.position_x + door.width - 8,
                  door.position_y + door.height / 2 + 10, 3);
}

/** @brief Draws a player bean with its label, and the key above its head if held. */
void draw_player(bitmap canvas, const Player &player, bool holding_key, double animation_time)
{
    bool blink = wrapped_value(animation_time + player.start_x * 0.01, 3.0) < 0.12;
    draw_cute_blob(canvas, player.position_x, player.position_y, player.width, player.height,
                   player.body_color, player.facing, blink);
    canvas_text(canvas, player.label, col_text(), player.position_x + 4, player.position_y - 12);
    if (holding_key)
    {
        draw_key_shape(canvas, player.position_x + 2, player.position_y - 30, 20);
    }
}

/** @brief Draws the top information bar during gameplay. */
void draw_hud(bitmap canvas, const Game &game)
{
    canvas_rect(canvas, rgb_color(255, 240, 246), 0, 0, GAME_W, HUD_H);
    canvas_rect(canvas, col_pink(), 0, HUD_H - 3, GAME_W, 3);

    int stars = collected_star_count(game.level);
    canvas_text(canvas, mode_name(game.mode) + " - " + game.level.name, col_text(), 10, 8);
    unsigned int display_time = elapsed_time(game);
    if (game.state == STATE_LEVEL_COMPLETE || game.state == STATE_GAME_OVER)
    {
        display_time = game.final_time;
    }
    canvas_text(canvas, "Time " + format_time(display_time), rgb_color(230, 110, 60), 205, 8);
    draw_heart_row(canvas, 345, 13, 12, game.lives);
    draw_star_row(canvas, 440, 12, 6, stars);
    canvas_text(canvas, std::to_string(stars) + "/3", col_text(), 488, 8);

    string key_text = "Key ";
    if (game.key_collected && game.key_holder >= 0)
    {
        key_text += game.players[game.key_holder].label;
    }
    else
    {
        key_text += "none";
    }
    color key_color;
    if (game.key_collected)
    {
        key_color = rgb_color(210, 150, 0);
    }
    else
    {
        key_color = col_text();
    }
    canvas_text(canvas, key_text, key_color, 540, 8);
    canvas_text(canvas, "ESC Pause", col_text_soft(), 712, 8);
    canvas_text(canvas, game.level.hint, col_text_soft(), 10, 24);
}

/** @brief Draws the banner shown for the first few seconds of a level. */
void draw_level_banner(bitmap canvas, const Game &game)
{
    if (elapsed_time(game) > BANNER_DURATION_MS)
    {
        return;
    }
    int stars = collected_star_count(game.level);
    double position_x = GAME_W / 2.0 - 160, position_y = HUD_H + 50, width = 320, height = 76;
    draw_panel(canvas, position_x, position_y, width, height);
    canvas_text_in_box(canvas, game.level.name + "  -  Stars " + std::to_string(stars) + "/3", col_text(),
                       position_x, position_y + 4, width, 30);
    draw_star_row(canvas, position_x + 50, position_y + 52, 10, stars);
    draw_heart_row(canvas, position_x + 190, position_y + 54, 16, game.lives);
}

/** @brief Draws a red flash over the screen right after a player is hurt. */
void draw_hurt_flash(bitmap canvas, const Game &game)
{
    if (game.last_hurt_ticks == 0)
    {
        return;
    }
    unsigned int since = elapsed_time(game) - game.last_hurt_ticks;
    if (since > HURT_FLASH_MS)
    {
        return;
    }
    int alpha = (int)(110 * (1.0 - (double)since / HURT_FLASH_MS));
    canvas_rect(canvas, rgba_color(255, 80, 110, alpha), 0, 0, GAME_W, GAME_H);
}

/** @brief Draws a speech bubble above a player standing at the door. */
void draw_door_messages(bitmap canvas, const Game &game)
{
    for (int item_index = 0; item_index < length(game.players); item_index++)
    {
        const Player &player = game.players[item_index];
        if (!boxes_overlap(player_box(player), game.level.door))
        {
            continue;
        }

        string message;
        if (!game.key_collected)
        {
            message = "Need the key!";
        }
        else if (!all_players_at_door(game))
        {
            message = "Waiting for " + game.players[1 - item_index].label + "...";
        }
        if (message.empty())
        {
            continue;
        }

        double width = message.length() * 8 + 20;
        double position_x =
            fmax(4, fmin(player.position_x + player.width / 2 - width / 2, GAME_W - 4 - width));
        draw_round_rect(canvas, rgb_color(255, 255, 255), position_x, player.position_y - 58, width, 22, 11);
        canvas_text(canvas, message, col_text(), position_x + 10, player.position_y - 51);
    }
}

/** @brief Draws the full gameplay screen. */
void draw_level(bitmap canvas, const Game &game)
{
    double animation_time = seconds_now();

    for (int position_y = 0; position_y < GAME_H; position_y += 4)
    {
        double gradient_ratio = (double)position_y / GAME_H;
        canvas_rect(canvas,
                    rgb_color((int)(190 + 40 * gradient_ratio), (int)(225 + 10 * gradient_ratio), 250), 0,
                    position_y, GAME_W, 4);
    }
    for (int item_index = 0; item_index < 3; item_index++)
    {
        draw_cloud(canvas, wrapped_value(item_index * 300 + animation_time * 8, GAME_W + 200) - 120,
                   90 + item_index * 50, 0.7);
    }
    canvas_rect(canvas, rgb_color(230, 120, 140), 0, GAME_H - 5, GAME_W, 5);

    for (int item_index_1 = 0; item_index_1 < length(game.level.ladders); item_index_1++)
    {
        const Box &ladder = game.level.ladders[item_index_1];
        draw_ladder(canvas, ladder);
    }
    for (int item_index_2 = 0; item_index_2 < length(game.level.platforms); item_index_2++)
    {
        const Box &platform = game.level.platforms[item_index_2];
        draw_platform(canvas, platform);
    }
    for (int item_index_3 = 0; item_index_3 < length(game.level.walls); item_index_3++)
    {
        const Box &wall = game.level.walls[item_index_3];
        draw_wall(canvas, wall);
    }
    for (int item_index_4 = 0; item_index_4 < length(game.level.spikes); item_index_4++)
    {
        const Box &spike = game.level.spikes[item_index_4];
        draw_spikes(canvas, spike);
    }

    draw_door(canvas, game.level.door, game.key_collected, length(game.players) > 1);

    if (!game.key_collected)
    {
        double bob = sin(animation_time * 4) * 3;
        draw_key_shape(canvas, game.level.key_box.position_x, game.level.key_box.position_y + bob,
                       game.level.key_box.width);
        draw_sparkle(canvas, game.level.key_box.position_x + 18, game.level.key_box.position_y - 2 + bob, 4,
                     220);
    }

    for (int item_index_5 = 0; item_index_5 < length(game.level.stars); item_index_5++)
    {
        const Star &star = game.level.stars[item_index_5];
        draw_level_star(canvas, star, animation_time);
    }
    for (int item_index_6 = 0; item_index_6 < length(game.level.monsters); item_index_6++)
    {
        const Monster &monster = game.level.monsters[item_index_6];
        draw_monster(canvas, monster, animation_time);
    }

    for (int item_index = 0; item_index < length(game.players); item_index++)
    {
        draw_player(canvas, game.players[item_index],
                    game.key_collected && game.key_holder == (int)item_index, animation_time);
    }

    draw_door_messages(canvas, game);
    draw_hurt_flash(canvas, game);
    draw_hud(canvas, game);
    draw_level_banner(canvas, game);
}

/** @brief Draws the "level cleared" screen over a dimmed view of the level. */
void draw_level_complete(bitmap canvas, const Game &game)
{
    draw_level(canvas, game);
    canvas_rect(canvas, rgba_color(80, 40, 90, 120), 0, 0, GAME_W, GAME_H);

    draw_panel(canvas, 200, 150, 400, 280);
    canvas_text_in_box(canvas, game.level.name + " CLEARED!", col_text(), 200, 165, 400, 30);

    draw_star_row(canvas, GAME_W / 2.0 - 64, 230, 20, game.final_stars);
    canvas_text_centered(canvas, "Stars: " + std::to_string(game.final_stars) + " / 3", col_text(), 265);

    canvas_text_centered(canvas, "Time: " + format_time(game.final_time), col_text(), 292);
    canvas_text_centered(canvas, "Best: " + format_time(game.best_time[game.mode][game.level_index]),
                         col_text_soft(), 314);
    canvas_text(canvas, "Hearts left:", col_text_soft(), GAME_W / 2.0 - 90, 333);
    draw_heart_row(canvas, GAME_W / 2.0 + 10, 337, 12, game.lives);
    if (game.final_time < SPEEDRUN_LIMIT_MS)
    {
        canvas_text_centered(canvas, "Under 2 minutes - so speedy!", rgb_color(60, 170, 110), 360);
    }

    string next;
    if (mode_all_cleared(game, game.mode))
    {
        next = "ENTER: All Clear";
    }
    else if (game.level_index + 1 < LEVEL_COUNT)
    {
        next = "ENTER: Next Level";
    }
    else
    {
        next = "ENTER: Level Select";
    }
    canvas_text_centered(canvas, next + "    ESC: Level Select", col_text(), 398);
}

/** @brief Draws the Game Over screen shown when all hearts are lost. */
void draw_game_over(bitmap canvas, const Game &game)
{
    double animation_time = seconds_now();
    draw_level(canvas, game);
    canvas_rect(canvas, rgba_color(80, 40, 90, 140), 0, 0, GAME_W, GAME_H);

    draw_panel(canvas, 210, 160, 380, 260);
    canvas_text_in_box(canvas, "OH NO!  OUT OF HEARTS", col_text(), 210, 175, 380, 30);
    draw_heart_row(canvas, GAME_W / 2.0 - 42, 230, 20, 0);

    double sway = sin(animation_time * 2) * 4;
    color sad_bean_color;
    if (game.mode == 0)
    {
        sad_bean_color = col_p1();
    }
    else
    {
        sad_bean_color = col_p2();
    }
    draw_cute_blob(canvas, GAME_W / 2.0 - 22 + sway, 262, 44, 52, sad_bean_color, 1, true);

    canvas_text_centered(canvas, game.level.name + "   time " + format_time(game.final_time), col_text_soft(),
                         335);
    canvas_text_centered(canvas, "Don't give up, little bean!", col_text_soft(), 357);
    canvas_text_centered(canvas, "ENTER: Try Again    ESC: Level Select", col_text(), 392);
}

/** @brief Draws the celebration screen after all three levels of a mode are cleared. */
void draw_mode_complete(bitmap canvas, const Game &game)
{
    double animation_time = seconds_now();
    draw_pastel_background(canvas, animation_time);
    draw_grass_ground(canvas, 490);

    draw_panel(canvas, 170, 80, 460, 330);
    canvas_text_in_box(canvas, "CONGRATULATIONS!", col_text(), 170, 95, 460, 30);
    canvas_text_centered(canvas, "All 3 levels cleared in " + mode_name(game.mode) + " mode!",
                         col_text_soft(), 135);

    for (int item_index = 0; item_index < LEVEL_COUNT; item_index++)
    {
        double position_y = 180 + item_index * 50;
        canvas_text(canvas, "Level " + std::to_string(item_index + 1), col_text(), 220, position_y);
        draw_star_row(canvas, 300, position_y + 4, 8, game.best_stars[game.mode][item_index]);
        canvas_text(canvas, "best " + format_time(game.best_time[game.mode][item_index]), col_text(), 390,
                    position_y);
    }
    canvas_text_centered(canvas, "Total deaths: " + std::to_string(game.total_deaths), col_text_soft(), 340);
    canvas_text_centered(canvas, "Press ENTER to return to the main menu", col_text(), 380);

    draw_cute_blob(canvas, 330, 425 - fabs(sin(animation_time * 4)) * 25, 50, 60, col_p1(), 1, false);
    draw_cute_blob(canvas, 420, 425 - fabs(sin(animation_time * 4 + 1)) * 25, 50, 60, col_p2(), -1, false);
}

/** @brief Draws the achievements page. */
void draw_achievements_page(bitmap canvas, const Game &game)
{
    double animation_time = seconds_now();
    draw_pastel_background(canvas, animation_time);

    int unlocked_count = 0;
    for (int item_index = 0; item_index < ACH_COUNT; item_index++)
    {
        if (game.achievements[item_index].unlocked)
        {
            unlocked_count++;
        }
    }

    draw_panel(canvas, 220, 14, 360, 46);
    canvas_text_in_box(canvas,
                       "ACHIEVEMENTS  " + std::to_string(unlocked_count) + " / " + std::to_string(ACH_COUNT),
                       col_text(), 220, 14, 360, 46);
    draw_heart(canvas, 250, 34, 14, col_pink());
    draw_heart(canvas, 550, 34, 14, col_pink());

    const double card_w = 360, card_h = 58;
    for (int item_index = 0; item_index < ACH_COUNT; item_index++)
    {
        const Achievement &box_a = game.achievements[item_index];
        double position_x;
        if (item_index % 2 == 0)
        {
            position_x = 30;
        }
        else
        {
            position_x = 410;
        }
        double position_y = 76 + (item_index / 2) * 66;

        draw_round_rect(canvas, rgba_color(120, 70, 130, 40), position_x + 3, position_y + 4, card_w, card_h,
                        14);
        color border_color;
        color background_color;
        if (box_a.unlocked)
        {
            border_color = col_gold();
            background_color = rgb_color(255, 246, 215);
        }
        else
        {
            border_color = rgb_color(200, 195, 215);
            background_color = rgb_color(235, 232, 242);
        }
        draw_round_rect(canvas, border_color, position_x - 2, position_y - 2, card_w + 4, card_h + 4, 15);
        draw_round_rect(canvas, background_color, position_x, position_y, card_w, card_h, 14);

        if (box_a.unlocked)
        {
            canvas_circle(canvas, rgb_color(255, 225, 120), position_x + 32, position_y + card_h / 2, 20);
            draw_star_shape(canvas, position_x + 32, position_y + card_h / 2, 12, rgb_color(255, 150, 40));
        }
        else
        {
            canvas_circle(canvas, rgb_color(205, 200, 220), position_x + 32, position_y + card_h / 2, 20);
            canvas_text(canvas, "?", rgb_color(130, 125, 150), position_x + 29, position_y + card_h / 2 - 4);
        }

        color name_color;
        color description_color;
        color status_color;
        string status_text;
        if (box_a.unlocked)
        {
            name_color = col_text();
            description_color = col_text_soft();
            status_color = rgb_color(220, 140, 0);
            status_text = "UNLOCKED";
        }
        else
        {
            name_color = rgb_color(140, 135, 155);
            description_color = rgb_color(160, 155, 175);
            status_color = rgb_color(160, 155, 175);
            status_text = "LOCKED";
        }
        canvas_text(canvas, std::to_string(item_index + 1) + ". " + box_a.name, name_color, position_x + 62,
                    position_y + 8);
        canvas_text(canvas, box_a.description, description_color, position_x + 62, position_y + 24);
        canvas_text(canvas, status_text, status_color, position_x + 62, position_y + 40);

        if (box_a.is_new)
        {
            draw_red_dot(canvas, position_x + card_w - 6, position_y + 6, 9);
            canvas_text(canvas, "NEW", col_red_dot(), position_x + card_w - 40, position_y + 40);
        }
    }

    canvas_text_centered(canvas,
                         "Deaths: " + std::to_string(game.total_deaths) + "   Ladder climbed: " +
                             std::to_string((int)game.climb_distance) + " px" + "   ENTER/ESC: back",
                         col_text(), 548);
}

/** @brief Draws the current achievement pop-up near the top of the screen. */
void draw_notification(bitmap canvas, const Game &game)
{
    if (length(game.notifications) == 0)
    {
        return;
    }
    const string &message = game.notifications[0];
    double width = message.length() * 8 + 70, height = 38;
    double position_x = GAME_W / 2.0 - width / 2, position_y = HUD_H + 10;
    draw_round_rect(canvas, rgba_color(120, 70, 130, 60), position_x + 3, position_y + 4, width, height, height / 2);
    draw_round_rect(canvas, rgb_color(255, 255, 255), position_x - 2, position_y - 2, width + 4, height + 4, height / 2 + 2);
    draw_round_rect(canvas, rgb_color(255, 225, 235), position_x, position_y, width, height, height / 2);
    draw_red_dot(canvas, position_x + 22, position_y + height / 2, 8);
    draw_star_shape(canvas, position_x + width - 22, position_y + height / 2, 9, col_gold());
    canvas_text(canvas, message, col_text(), position_x + 40, position_y + height / 2 - 4);
}

/** @brief Draws one complete frame onto the canvas. */
void draw_game(bitmap canvas, const Game &game)
{
    clear_bitmap(canvas, rgb_color(255, 255, 255));
    switch (game.state)
    {
    case STATE_MAIN_MENU:
        draw_main_menu(canvas, game);
        break;
    case STATE_LEVEL_SELECT:
        draw_level_select(canvas, game);
        break;
    case STATE_PLAYING:
        draw_level(canvas, game);
        break;
    case STATE_LEVEL_COMPLETE:
        draw_level_complete(canvas, game);
        break;
    case STATE_MODE_COMPLETE:
        draw_mode_complete(canvas, game);
        break;
    case STATE_ACHIEVEMENTS:
        draw_achievements_page(canvas, game);
        break;
    case STATE_GAME_OVER:
        draw_game_over(canvas, game);
        break;
    }
    draw_notification(canvas, game);
}