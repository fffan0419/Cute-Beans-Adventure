#include "splashkit.h"
#include "game_types.h"

/** @brief Starts the game and runs the main loop until the player quits. */
int main()
{

    display main_display = display_details(0);
    window game_window =
        open_window("Cute Beans Adventure", display_width(main_display), display_height(main_display));
    window_toggle_fullscreen(game_window);

    bitmap canvas = create_bitmap("game_canvas", GAME_W, GAME_H);

    Game game;
    init_game(game);

    // AI-assisted addition: help me explain SplashKit Audio Functions.
    // Audio API reference: https://splashkit.io/api/audio/
    if (!audio_ready())
    {
        open_audio();
    }
    music background_music = nullptr;
    if (audio_ready())
    {
        background_music = load_music("beans_background", "beans_breeze.wav");
        if (music_valid(background_music))
        {
            play_music(background_music, 1, 0.35);
        }
    }

    if (audio_ready())
    {
        load_sound_effect("hurt", "hurt.wav");
    }

    while (!quit_requested() && !game.quit)
    {
        process_events();

        if (music_valid(background_music) && !music_playing())
        {
            play_music(background_music, 1, 0.35);
        }
        if (key_typed(F11_KEY))
        {
            window_toggle_fullscreen(game_window);
        }
        update_game(game);
        draw_game(canvas, game);
        present_canvas(canvas);
    }

    if (music_valid(background_music))
    {
        stop_music();
        free_music(background_music);
    }
    free_all_sound_effects();
    free_timer(game.level_timer);
    free_bitmap(canvas);
    close_all_windows();
    return 0;
}
