#include "global.h"
#include "charater.h"
#include "scene.h"

int Game_establish();
void game_init();
void game_begin();
void game_update();
int process_event();
void game_draw();
int game_run();
void game_destroy();

void video_display(ALLEGRO_VIDEO *video);
void video_init();
void video_begin();
void destroy_video();
void song_play();
