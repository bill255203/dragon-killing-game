#include "GameWindow.h"

bool draw = false;

const char *title = "EAT CANDY";

// ALLEGRO Variables
ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_SAMPLE *song=NULL;
ALLEGRO_SAMPLE_INSTANCE *sample_instance;

ALLEGRO_EVENT_QUEUE *queue;
ALLEGRO_EVENT event;

int Game_establish() {
    int msg = 0;

    game_init();
    game_begin();

    while ( msg != GAME_TERMINATE ) {
        msg = game_run();
        if ( msg == GAME_TERMINATE )
            printf( "Game Over\n" );
    }

    game_destroy();
    return msg;
}

void game_init() {
    printf( "Game Initializing...\n" );
    al_init();
    // init audio
    al_install_audio();
    al_init_acodec_addon();
    // Create display
    display = al_create_display(WIDTH, HEIGHT);
    // create event queue
    event_queue = al_create_event_queue();
    // Initialize Allegro settings
    al_set_window_position(display, 0, 0);
    al_set_window_title(display, title);
    al_init_primitives_addon();
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon(); // initialize the ttf (True Type Font) addon
    al_init_image_addon(); // initialize the image addon
    al_init_acodec_addon(); // initialize acodec addon
    al_install_keyboard(); // install keyboard event
    al_install_mouse();    // install mouse event
    al_install_audio();    // install audio event
    // Register event
    al_register_event_source(event_queue, al_get_display_event_source( display ));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    fps  = al_create_timer( 1.0 / FPS );
    al_register_event_source(event_queue, al_get_timer_event_source( fps )) ;
    // initialize the icon on the display
    ALLEGRO_BITMAP *icon = al_load_bitmap("./image/icon.png");
    al_set_display_icon(display, icon);
    // load in stats
    FILE *fptr = fopen("stats.txt","r");
    fscanf(fptr,"money: %d\ncharacter bought %d %d %d %d", &money, &character_bought[0], &character_bought[1],
           &character_bought[2], &character_bought[3]);
    fclose(fptr);
}

void game_begin() {
    song_play();
    al_start_timer(fps);
    // initialize the menu before entering the loop
    menu_init();
    choose_init();
    game_scene_init();
    boss_scene_init();
    end_scene_init();
    character_init();
    candy_init();
    boss_init();
    minion_init();
}
void game_update(){
    if( change_window ){
        if(current_window <= 3) song_play();
        if (current_window == 1) real_choose_init();
        else if (current_window == 2) real_game_init();
        else if (current_window == 3) real_boss_init();
        change_window = false;
    }else if( current_window == 5 ){
        al_destroy_sample(song);
        play_credit_video();
        current_window = 6;
        change_window = true;
    }else if( current_window == 6 ){
        menu_destroy();
        choose_destroy();
        game_scene_destroy();
        boss_scene_destroy();
        end_scene_destroy();
    }
    if( draw ){
        if( current_window == 1 ){
            choose_update();
        }else if( current_window == 2 ){
            character_update();
        }else if( current_window == 3 ){
            character_update2();
            boss_update();
            minion_update();
        }
    }
}
int process_event(){
    // Request the event
    ALLEGRO_EVENT event;
    al_wait_for_event(event_queue, &event);
    // process the event of other component
    if( current_window == 0 ){
        menu_process(event);
    }else if( current_window == 1 ){
        choose_process(event);
    }else if( current_window == 2 ){
        character_process(event);
    }else if( current_window == 3 ){
        boss_scene_process(event);
    }else if( current_window == 4 ){
        end_scene_process(event);
    }

    // Shutdown our program
    if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        return GAME_TERMINATE;
    else if(event.type == ALLEGRO_EVENT_TIMER)
        if(event.timer.source == fps)
            draw = true;
    game_update();
    return 0;
}
void game_draw(){
    if( current_window == 0 ){
        menu_draw();
    }else if(current_window == 1 ){
        choose_draw();
    }else if( current_window == 2 ){
        game_scene_draw();
    }else if( current_window == 3 ){
        boss_scene_draw();
    }else if( current_window == 4 ){
        end_scene_draw();
    }
    al_flip_display();
}
int game_run() {
    int error = 0;
    if( draw ){
        game_draw();
        draw = false;
    }
    if ( !al_is_event_queue_empty(event_queue) ) {
        error = process_event();
    }
    return error;
}

void game_destroy() {
    // Make sure you destroy all things
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    end_scene_destroy();
    FILE *fptr = fopen("stats.txt","w+");
    fprintf(fptr,"money: %d\ncharacter bought %d %d %d %d", money, character_bought[0], character_bought[1],
            character_bought[2], character_bought[3]);
    fclose(fptr);
}

void song_play(){
    if(song != NULL) al_destroy_sample(song);
    // Load sound
    if(current_window == 0) song = al_load_sample("./sound/menu.mp3");
    else if(current_window == 1)song = al_load_sample("./sound/shop.mp3");
    else if(current_window == 2)song = al_load_sample("./sound/game.mp3");
    else if(current_window == 3)song = al_load_sample("./sound/battle.mp3");
    al_reserve_samples(20);
    sample_instance = al_create_sample_instance(song);
    // Loop the song until the display closes
    al_set_sample_instance_playmode(sample_instance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(sample_instance, al_get_default_mixer());
    // set the volume of instance
    al_set_sample_instance_gain(sample_instance, 0.3);
    al_play_sample_instance(sample_instance);
}
