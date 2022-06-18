#include "global.h"
// character and choose
void character_init();
void choose_process(ALLEGRO_EVENT event);
void choose_update();
void choose_buy_draw();
void character_process(ALLEGRO_EVENT event);
void character_update();
void character_draw();
void character_destroy();

// candy
void candy_init();
void candy_collide();
void candy_draw();
void candy_destroy();

// score
void score_draw(bool end);
void score_destroy(bool end);

// boss
void boss_init();
void boss_scene_process(ALLEGRO_EVENT event);
void boss_update();
void boss_draw();
void boss_destroy();

// minion
void minion_init();
void minion_update();
void minion_collide();
void minion_draw();
void minion_destroy();

// character for boss stage
void character_update2();
void health_draw();

// real init
void real_choose_init();
void real_game_init();
void real_boss_init();
