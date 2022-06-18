#include "scene.h"

ALLEGRO_BITMAP *background_1 = NULL, *background_2 = NULL, *background_3 = NULL, *background_4 = NULL, *background_5 = NULL;
ALLEGRO_TIMER *timer;
ALLEGRO_VIDEO *video;
ALLEGRO_BITMAP *al_get_video_frame(ALLEGRO_VIDEO *video);
static ALLEGRO_DISPLAY *screen;
static char const *filename;
ALLEGRO_EVENT_QUEUE *que;
ALLEGRO_EVENT eventt;
ALLEGRO_SAMPLE *song1 = NULL, *song2 = NULL, *song3 = NULL, *song4 = NULL;
ALLEGRO_SAMPLE_INSTANCE *sample_instance1;

// function of menu
void menu_init(){
    background_1 = al_load_bitmap("./image/menu.png");
}

int option = -1;
void menu_process(ALLEGRO_EVENT event){
    if( event.type == ALLEGRO_EVENT_MOUSE_AXES ){
        if( (event.mouse.x>=420) && (event.mouse.x<=960) ){
            if ( (event.mouse.y>=520) && (event.mouse.y<=600) ) option = 2; // game
            else if ( (event.mouse.y>=640) && (event.mouse.y<=720) ) option = 1; // choose
            else if ( (event.mouse.y>=750) && (event.mouse.y<=830) ) option = 6; // quit
            else option = -1;
        }else option = -1;
    }
    if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
        if(event.mouse.button == 1){
            if(option != -1){
                current_window = option;
                change_window = true;
            }
        }
    }
}
void menu_draw(){
    al_draw_bitmap(background_1, 0, 0, 0);
}
void menu_destroy(){
    al_destroy_bitmap(background_1);
}
// function of choosing character
void choose_init(){
    background_2 = al_load_bitmap("./image/choose.png");
}
void choose_draw(){
    al_draw_bitmap(background_2, 0, 0, 0);
    choose_buy_draw();
    character_draw();
}
void choose_destroy(){
    al_destroy_bitmap(background_2);
}
// function of game scene
void game_scene_init(){
    background_3 = al_load_bitmap("./image/game_stage.png");
}
void game_scene_draw(){
    al_draw_bitmap(background_3, 0, 0, 0);
    candy_draw();
    character_draw();
    score_draw(false);
}
void game_scene_destroy(){
    al_destroy_bitmap(background_3);
}
// function of boss scene
void boss_scene_init(){
    background_4 = al_load_bitmap("./image/boss_stage.png");
}
void boss_scene_draw(){
    al_draw_bitmap(background_4, 0, 0, 0);
    candy_draw();
    minion_draw();
    boss_draw();
    character_draw();
    health_draw();
}
void boss_scene_destroy(){
    al_destroy_bitmap(background_4);
    character_destroy();
    candy_destroy();
    minion_destroy();
    boss_destroy();
    score_destroy(false);
}
// function of end scene
void end_scene_init(){
    background_5 = al_load_bitmap("./image/end.png");
}

void end_scene_process(ALLEGRO_EVENT event){
    if( event.type == ALLEGRO_EVENT_MOUSE_AXES ){
        if( (event.mouse.x>=500) && (event.mouse.x<=900) ){
            if ( (event.mouse.y>=550) && (event.mouse.y<=640) ) option = 0; // menu
            else if ( (event.mouse.y>=680) && (event.mouse.y<=770) ) option = 5; // credit
            else option = -1;
        }else option = -1;
    }
    if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
        if(event.mouse.button == 1){
            if(option != -1){
                current_window = option;
                change_window = true;
            }
        }
    }
}
void end_scene_draw(){
    al_draw_bitmap(background_5, 0, 0, 0);
    score_draw(true);
}
void end_scene_destroy(){
    al_destroy_bitmap(background_5);
    score_destroy(true);
}

// function of video

void video_init(){
    al_init();
    al_init_video_addon();
    al_install_audio();
    timer = al_create_timer(1.0 / 60);
    //al_set_new_display_flags(ALLEGRO_RESIZABLE);
    //al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
    screen = al_create_display(1400, 900);
    // linear interpolation for scaling images
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    filename = "./video/credit1.ogv";
    printf("reading video.....\n");
    video = al_open_video(filename);
    if( video )
        printf("read video succeed\n");
    else
        printf("read video fail!!!!\n");
    que = al_create_event_queue();
    // register video event
    ALLEGRO_EVENT_SOURCE *temp = al_get_video_event_source(video);
    al_register_event_source(que, temp);
    al_register_event_source(que, al_get_display_event_source(screen));
    al_register_event_source(que, al_get_timer_event_source(timer));

}
void video_begin(){
    al_reserve_samples(1);
    al_start_video(video, al_get_default_mixer());
    al_start_timer(timer);
}
void video_display(ALLEGRO_VIDEO *video) {
    ALLEGRO_BITMAP *frame = al_get_video_frame(video);
    // check if we get the frame successfully
    // Note the this code is necessary
    if ( !frame )
        return;
    // Display the frame.
    // rescale the frame into the size of screen
    al_draw_scaled_bitmap(frame,
                          // the rescale position of the frame
                          0, 0,
                          // the width and height you want to rescale
                          al_get_bitmap_width(frame),
                          al_get_bitmap_height(frame),
                          // the position of result image
                          0, 0,
                          // the width and height of result image
                          al_get_display_width(screen),
                          al_get_display_height(screen), 0);
    al_flip_display();
}
void destroy_video(){
    al_destroy_display(screen);
    al_destroy_event_queue(que);
}

void play_credit_video(){
    video_init();
    video_begin();
    while( 1 ){
        al_wait_for_event(que, &eventt);
        if( eventt.type == ALLEGRO_EVENT_TIMER ) {
            video_display(video);
        } else if( eventt.type == ALLEGRO_EVENT_DISPLAY_CLOSE ) {
            al_close_video(video);
            break;
        } else if( eventt.type == ALLEGRO_EVENT_VIDEO_FINISHED ) {
            break;
        }
    }
    destroy_video();
}
