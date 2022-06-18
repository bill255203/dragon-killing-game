#include "global.h"
#include "charater.h"

void menu_init();
void menu_process(ALLEGRO_EVENT event);
void menu_draw();
void menu_destroy();

void choose_init();
void choose_draw();
void choose_destroy();

void game_scene_init();
void game_scene_draw();
void game_scene_destroy();

void boss_scene_init();
void boss_scene_draw();
void boss_scene_destroy();

void end_scene_init();
void end_scene_process(ALLEGRO_EVENT event);
void end_scene_draw();
void end_scene_destroy();

void video_init();
void video_begin();
void video_display();
void video_destroy();
void play_credit_video();

