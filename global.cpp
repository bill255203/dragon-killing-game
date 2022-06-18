#include "global.h"

// variables for global usage
const float FPS = 60.0;
const int WIDTH = 1400;
const int HEIGHT = 900;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *fps = NULL;
bool key_state[ALLEGRO_KEY_MAX] = {false};
bool change_window = false;
int current_window = 0;

int character_bought[4];
int money;



