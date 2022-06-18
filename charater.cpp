#include "charater.h"

// character and choose
#define CN 4
// the state of character
enum {STOP = 0, MOVE, ATK};
typedef struct character
{
    int x, y; // the position of image
    int width, height; // the width and height of image
    bool dir; // left: false, right: true
    bool jump, double_jump;
    int state; // the state of character
    int on_plat;
    int health;

    int buy_price[4] = {0, 10, 20, -1};
    ALLEGRO_BITMAP *img_move[CN][2], *img_atk[CN][2], *img_jump[CN], *img_die;
    ALLEGRO_FONT *font1, *font2;

    int choosed_character;
    ALLEGRO_SAMPLE_INSTANCE *atk_sound[CN], *jump_sound[CN], *choose_sound, *buy_sound, *hit_dragon_sound;
    int anime; // counting the time of animation
    int choosing_anime; // indicate which character the player choose
    int anime_time; // indicate how long the animation is
    int choosing_anime_time; // indicate how long the choosing animation is

    int jump_anime;
    int jump_anime_time;
}Character;
Character chara;
ALLEGRO_SAMPLE *sample = NULL;
char temp[50];
void character_init(){
    chara.font1 = al_load_ttf_font("./font/pirulen.ttf", 20, 0);
    chara.font2 = al_load_ttf_font("./font/pirulen.ttf", 27, 0);
    // load character images
    for(int i = 1 ; i <= CN ; i++){
        for(int j = 1 ; j <= 2 ; j++){
            sprintf( temp, "./image/char%d_move%d.png", i, j );
            chara.img_move[i-1][j-1] = al_load_bitmap(temp);
        }
        for(int j = 1 ; j <= 2 ; j++){
            sprintf( temp, "./image/char%d_atk%d.png", i, j );
            chara.img_atk[i-1][j-1] = al_load_bitmap(temp);
        }
        sprintf( temp, "./image/char%d_jump.png", i);
        chara.img_jump[i-1] = al_load_bitmap(temp);
    }
    chara.img_die = al_load_bitmap("./image/tomb.png");
    chara.choosed_character = 0;
    // load effective sound
    for(int i = 1 ; i <= CN ; i++){
        sprintf( temp, "./sound/char%d_atk.mp3", i);
        sample = al_load_sample(temp);
        chara.atk_sound[i-1] = al_create_sample_instance(sample);
        sprintf( temp, "./sound/char%d_jump.mp3", i);
        sample = al_load_sample(temp);
        chara.jump_sound[i-1] = al_create_sample_instance(sample);
        al_set_sample_instance_playmode(chara.atk_sound[i-1], ALLEGRO_PLAYMODE_ONCE);
        al_set_sample_instance_playmode(chara.jump_sound[i-1], ALLEGRO_PLAYMODE_ONCE);
        al_attach_sample_instance_to_mixer(chara.atk_sound[i-1], al_get_default_mixer());
        al_attach_sample_instance_to_mixer(chara.jump_sound[i-1], al_get_default_mixer());
    }
    sprintf( temp, "./sound/click.mp3");
    sample = al_load_sample(temp);
    chara.choose_sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(chara.choose_sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(chara.choose_sound, al_get_default_mixer());
    sprintf( temp, "./sound/buy.mp3");
    sample = al_load_sample(temp);
    chara.buy_sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(chara.buy_sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(chara.buy_sound, al_get_default_mixer());
    sprintf( temp, "./sound/Gotcha Bitch.mp3");
    sample = al_load_sample(temp);
    chara.hit_dragon_sound = al_create_sample_instance(sample);
    al_set_sample_instance_gain(chara.hit_dragon_sound, 10);
    al_set_sample_instance_playmode(chara.hit_dragon_sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(chara.hit_dragon_sound, al_get_default_mixer());

    // initial the geometric information of character
    chara.width = al_get_bitmap_width(chara.img_move[0][0]);
    chara.height = al_get_bitmap_height(chara.img_move[0][0]);
}
int show_count = 0;
void choose_buy_draw(){
    char text[26];
    sprintf( text, "money: %3d", money);
    al_draw_text(chara.font1, al_map_rgb(0, 0, 0), 1100, 600, ALLEGRO_ALIGN_CENTRE, text);
    if(character_bought[chara.choosed_character] == 0){
        if(chara.choosed_character == 0) strcpy(text, "BUY? $0");
        else if(chara.choosed_character == 1) strcpy(text, "BUY? $10");
        else if(chara.choosed_character == 2) strcpy(text, "BUY? $20");
        else if(chara.choosed_character == 3) strcpy(text, "DEFEAT THE BOSS TO UNLOCK");
    }else{
        strcpy(text, "OWNED");
    }
    al_draw_text(chara.font1, al_map_rgb(255, 255, 255), 380, 300, ALLEGRO_ALIGN_CENTRE, text);
    if(show_count % 100 > 50 ) al_draw_text(chara.font1, al_map_rgb(0, 0, 0), 700, 50, ALLEGRO_ALIGN_CENTER, "Press Enter to Continue");
    show_count++, show_count %= 100;
}
// new function for choosing character page
void choose_process(ALLEGRO_EVENT event){
    // animation
    if( event.type == ALLEGRO_EVENT_TIMER ){
        if( event.timer.source == fps ){
            chara.anime++;
            chara.anime %= chara.anime_time;
            chara.choosing_anime++;
            chara.choosing_anime %= chara.choosing_anime_time;
        }
    }
    // jumping
    if ( chara.jump ){
        if ( chara.jump_anime >= -15 ){
            // if chara.jump_anime >0 character go up <0 character go down
            chara.y -= chara.jump_anime;
            chara.jump_anime--;
        }else chara.jump = false;
    }
    if( event.type == ALLEGRO_EVENT_KEY_UP ){
        // next character
        if( event.keyboard.keycode == ALLEGRO_KEY_RIGHT ){
            al_play_sample_instance(chara.choose_sound);
            chara.choosed_character++;
            if(chara.choosed_character == CN) chara.choosed_character = 0;
        // last character
        }else if( event.keyboard.keycode == ALLEGRO_KEY_LEFT ){
            al_play_sample_instance(chara.choose_sound);
            chara.choosed_character--;
            if(chara.choosed_character < 0) chara.choosed_character = CN-1;
        // buy character
        }else if( event.keyboard.keycode == ALLEGRO_KEY_Y ){
            if(character_bought[chara.choosed_character] == 0 && chara.buy_price[chara.choosed_character] <= money && chara.choosed_character != 3){
                al_play_sample_instance(chara.buy_sound);
                money -= chara.buy_price[chara.choosed_character];
                character_bought[chara.choosed_character] = 1;
            }
        // next window
        }else if( event.keyboard.keycode == ALLEGRO_KEY_ENTER && character_bought[chara.choosed_character] == 1){
            current_window = 2;
            change_window = true;
        }
    }
}
// make the character do stop -> move -> attack with animation of both sides
void choose_update(){
    if( chara.choosing_anime == 0 || character_bought[chara.choosed_character] == 0){
        chara.x = 320;
        chara.y = 500;
        chara.state = STOP;
        chara.jump_anime = -15;
        chara.jump = false;
        chara.dir = true;
    }else if( chara.choosing_anime == 200 ){
        chara.state = MOVE;
        chara.dir = true;
    }else if( chara.choosing_anime == 400 ){
        chara.state = ATK;
        chara.dir = true;
    }else if( chara.choosing_anime == 600 || chara.choosing_anime == 650 ){
        chara.state = STOP;
        chara.jump = true;
        chara.dir = true;
        chara.jump_anime = 15;
    }else if( chara.choosing_anime == 700 || chara.choosing_anime == 750 ){
        chara.state = STOP;
        chara.jump = true;
        chara.dir = false;
        chara.jump_anime = 15;
    }else if( chara.choosing_anime == 100 || chara.choosing_anime == 300 || chara.choosing_anime == 500){
        chara.dir = false;
    }
}
void character_process(ALLEGRO_EVENT event){
    // process the animation
    if( event.type == ALLEGRO_EVENT_TIMER ){
        if( event.timer.source == fps ){
            chara.anime++;
            chara.anime %= chara.anime_time;
        }
    // process the keyboard event
    }else if( event.type == ALLEGRO_EVENT_KEY_DOWN ){
        key_state[event.keyboard.keycode] = true;
        chara.anime = 0;
    }else if( event.type == ALLEGRO_EVENT_KEY_UP ){
        key_state[event.keyboard.keycode] = false;
        if( event.keyboard.keycode == ALLEGRO_KEY_ENTER || chara.state == 100){
            current_window = 3;
            change_window = true;
        }
    }
}
void character_update(){
    // use the idea of finite state machine to deal with different state
    if( key_state[ALLEGRO_KEY_W] ){
        if( !chara.jump ) chara.jump_anime = 15, chara.jump = true;
    }else if( key_state[ALLEGRO_KEY_A] ){
        chara.dir = false;
        chara.x -= 5;
        chara.state = MOVE;
    }else if( key_state[ALLEGRO_KEY_D] ){
        chara.dir = true;
        chara.x += 5;
        chara.state = MOVE;
    }else if( key_state[ALLEGRO_KEY_SPACE] ){
        chara.state = ATK;
    }else if( chara.anime == chara.anime_time-1 ){
        chara.anime = 0;
        chara.state = STOP;
    }else if ( chara.anime == 0 ){
        chara.state = STOP;
    }
    // for jump function
    if ( chara.jump ){
        if ( chara.jump_anime >= -15 ){
            // if chara.jump_anime >0 character go up <0 character go down
            chara.y -= chara.jump_anime;
            chara.jump_anime--;
        }
        else chara.jump = false;
    }
    // for attack candy
    if (chara.state == ATK) candy_collide();
    // for border
    if (chara.x < -150 && chara.y > 600) chara.x = -150;
    else if (chara.x > WIDTH && chara.y < 300) chara.state = 100;
    // for climbing
    else if (chara.x > WIDTH && chara.y > 300) chara.y -= 300, chara.x = 0;
    else if (chara.x < -150 && chara.y < 600) chara.y += 300, chara.x = WIDTH;
}
void character_draw(){
    // with the state, draw corresponding image
    if( chara.state != 3){
        if( chara.state == ATK ){
            al_play_sample_instance(chara.atk_sound[chara.choosed_character]);
            if( chara.dir ){
                if( chara.anime < chara.anime_time/2 ){
                    al_draw_bitmap(chara.img_atk[chara.choosed_character][0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
                }else{
                    al_draw_bitmap(chara.img_atk[chara.choosed_character][1], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
                }
            }else{
                if( chara.anime < chara.anime_time/2 ){
                    al_draw_bitmap(chara.img_atk[chara.choosed_character][0], chara.x, chara.y, 0);
                }else{
                    al_draw_bitmap(chara.img_atk[chara.choosed_character][1], chara.x, chara.y, 0);
                }
            }
        }else if( chara.jump ){
            al_play_sample_instance(chara.jump_sound[chara.choosed_character]);
            if( chara.dir ){
                al_draw_bitmap(chara.img_jump[chara.choosed_character], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
            }else{
                al_draw_bitmap(chara.img_jump[chara.choosed_character], chara.x, chara.y, 0);
            }
        }else if( chara.state == STOP ){
            if( chara.dir )
                al_draw_bitmap(chara.img_move[chara.choosed_character][0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
            else
                al_draw_bitmap(chara.img_move[chara.choosed_character][0], chara.x, chara.y, 0);
        }else if( chara.state == MOVE ){
            if( chara.dir ){
                if( chara.anime < chara.anime_time/2 ){
                    al_draw_bitmap(chara.img_move[chara.choosed_character][0], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
                }else{
                    al_draw_bitmap(chara.img_move[chara.choosed_character][1], chara.x, chara.y, ALLEGRO_FLIP_HORIZONTAL);
                }
            }else{
                if( chara.anime < chara.anime_time/2 ){
                    al_draw_bitmap(chara.img_move[chara.choosed_character][0], chara.x, chara.y, 0);
                }else{
                    al_draw_bitmap(chara.img_move[chara.choosed_character][1], chara.x, chara.y, 0);
                }
            }
        }
    }else{
        al_draw_bitmap(chara.img_die, chara.x, chara.y, 0);
    }
}
void character_destroy(){
    for(int i = 0 ; i < CN ; i++){
        for(int j = 0 ; j <= 1 ; j++){
            al_destroy_bitmap(chara.img_move[i][j]);
        }
        for(int j = 0 ; j <= 1 ; j++){
            al_destroy_bitmap(chara.img_atk[i][j]);
        }
        al_destroy_sample_instance(chara.atk_sound[i]);
        al_destroy_sample_instance(chara.jump_sound[i]);
    }
}


// candy
typedef struct candy{
    bool eaten;
    int x, y;
    ALLEGRO_BITMAP *img[4];
}Candy;

// max number of candies is 10
Candy candies[10];
int candies_number = 10;
int candies_exist_num = 3;

void candy_init(){
    for(int i=0; i<candies_number; i++){
        candies[i].eaten = false;
        for(int j=1; j<=4; j++){
            sprintf( temp, "./image/char%d_coin.png", j);
            candies[i].img[j-1] = al_load_bitmap(temp);
        }
    }
}

// check for collision
void candy_collide(){
    for(int i=0; i<candies_number; i++){
        if(chara.state == ATK && abs(chara.x - candies[i].x) < 100 && abs(chara.y - candies[i].y) < 100 && !candies[i].eaten){
            money ++;
            candies[i].eaten = true;
            candies_exist_num --;
        }
    }
}

void candy_draw(){
    // with the coordinates draw corresponding candies
    for(int i=0; i<candies_number; i++){
        if(!candies[i].eaten){
            al_draw_bitmap(candies[i].img[chara.choosed_character], candies[i].x, candies[i].y, 0);
        }
    }
}

void candy_destroy(){
    for(int i=0; i<4; i++) al_destroy_bitmap(candies[0].img[i]);
}

// score
void score_draw(bool end){
    char text[13];
    sprintf( text, "money: %3d", money);
    if( !end ){
        al_draw_text(chara.font1, al_map_rgb(255, 255, 255), 100, 20, ALLEGRO_ALIGN_CENTRE, text);
        if(show_count % 100 > 50 ){
                al_draw_text(chara.font1, al_map_rgb(0, 0, 0), 700, 50, ALLEGRO_ALIGN_CENTER, "Press Enter to Pass");
        }
        show_count++;
    }
    else al_draw_text(chara.font2, al_map_rgb(0, 0, 0), 1020, 400, ALLEGRO_ALIGN_CENTRE, text);
}

void score_destroy(bool end){
    if( !end ) al_destroy_font(chara.font1);
    else al_destroy_font(chara.font2);
}


// boss

typedef struct boss{
    int x, y;
    int health;
    int status; // 0 for rest 1 for attack 2 for tired
    bool eat_candy;
    ALLEGRO_BITMAP *img[4];
    ALLEGRO_SAMPLE_INSTANCE *atk_sound, *weak_sound;
    int anime;
    int anime_time;
    int tired_anime;
    int tired_anime_time;
}Boss;

Boss dragon;

void boss_init(){
    for(int i=1; i<=4; i++){
        sprintf( temp, "./image/boss%d.png", i);
        dragon.img[i-1] = al_load_bitmap(temp);
    }
    sprintf( temp, "./sound/boss_atk.mp3");
    sample = al_load_sample(temp);
    dragon.atk_sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(dragon.atk_sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(dragon.atk_sound, al_get_default_mixer());
    sprintf( temp, "./sound/Onii chan.mp3");
    sample = al_load_sample(temp);
    dragon.weak_sound = al_create_sample_instance(sample);
    al_set_sample_instance_gain(dragon.weak_sound, 10);
    al_set_sample_instance_playmode(dragon.weak_sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(dragon.weak_sound, al_get_default_mixer());
}


// minion

typedef struct minion{
    bool exist;
    int x, y;
    int dir;
    int anime;
    int anime_time;
    int plat;
    int width, height;
    ALLEGRO_BITMAP *move_img[2];
    ALLEGRO_SAMPLE_INSTANCE *die_sound;
}Minion;

Minion goomba[10];
int goomba_exist_num = 0;

void minion_init(){
    for(int i=0; i<10; i++){
        goomba[i].anime = 0;
        goomba[i].anime_time = 200;
        goomba[i].exist = false;
        goomba[i].move_img[0] = al_load_bitmap("./image/minion_move1.png");
        goomba[i].move_img[1] = al_load_bitmap("./image/minion_move2.png");
        goomba[i].width = al_get_bitmap_width(goomba[0].move_img[0]);
        goomba[i].height = al_get_bitmap_height(goomba[0].move_img[0]);
        sprintf( temp, "./sound/minion_die.mp3");
        sample = al_load_sample(temp);
        goomba[i].die_sound = al_create_sample_instance(sample);
        al_set_sample_instance_playmode(goomba[i].die_sound, ALLEGRO_PLAYMODE_ONCE);
        al_attach_sample_instance_to_mixer(goomba[i].die_sound, al_get_default_mixer());
    }
    for(int i=0; i<candies_number; i++){
        candies[i].eaten = true;
    }
}


// boss function
void boss_scene_process(ALLEGRO_EVENT event){

    // anime
    if( event.type == ALLEGRO_EVENT_TIMER ){
        if( event.timer.source == fps ){
            chara.anime++;
            if(chara.state != 3) chara.anime %= chara.anime_time;
            dragon.anime++;
            if(dragon.status == 4 && dragon.anime > 0) dragon.anime = -120;
            if(dragon.status != 4) dragon.anime %= dragon.anime_time;
            for(int i=0; i<10; i++){
                if(goomba[i].exist){
                    goomba[i].anime++;
                    goomba[i].anime %= goomba[i].anime_time;
                }

            }
            if( dragon.tired_anime != -1) dragon.tired_anime++;
            if( dragon.tired_anime == dragon.tired_anime_time){
                dragon.status = 0;
                dragon.tired_anime = -1;
                dragon.eat_candy = false;
            }
        }
    // process the keyboard event
    }else if( event.type == ALLEGRO_EVENT_KEY_DOWN ){
        key_state[event.keyboard.keycode] = true;
        chara.anime = 0;
    }else if( event.type == ALLEGRO_EVENT_KEY_UP ){
        key_state[event.keyboard.keycode] = false;
        if( event.keyboard.keycode == ALLEGRO_KEY_ENTER ){
            current_window = 4;
        }
    }
    // hit dragon
    if(chara.state == ATK && abs(chara.x - dragon.x) < 100 && abs(chara.y - dragon.y) < 100 && dragon.status == 3){
        dragon.health--;
        al_play_sample_instance(chara.hit_dragon_sound);
        if(dragon.health <= 0) dragon.health = 0, character_bought[3] = 1;
    }

}

void boss_update(){
    if( dragon.health > 0){
        if( dragon.anime == 300 && !dragon.eat_candy ){
            dragon.status = 1;
        }else if ( dragon.anime == 350 && dragon.status == 1 ){
            dragon.status = 2;
        }else if( dragon.eat_candy && candies_exist_num == 0 ){
            dragon.status = 3;
        }
        if( dragon.status == 0 ){
            if( dragon.anime%100 < 50 && dragon.anime%2 == 0){
                dragon.y += 1;
                dragon.x += 1;
            }else if( dragon.anime%100 > 50 && dragon.anime%2 == 0 ){
                dragon.y -= 1;
                dragon.x -= 1;
            }
        }else if( dragon.status == 1 || dragon.status == 2){
            if( dragon.anime < 400 ){
                dragon.y -= 2;
            }else{
                dragon.y += 2;
            }
            if( dragon.anime == 400){
                al_play_sample_instance(dragon.atk_sound);
            }else if( dragon.anime == 499){
                dragon.eat_candy = true;
                int goomba_add = 0;
                for(int i=0; i<10; i++){
                    if(!goomba[i].exist){
                        goomba[i].exist = true;
                        int plat = rand() % 4; // 0 1 2 3
                        goomba[i].plat = plat;
                        if(plat == 0){
                            goomba[i].x = rand() % 350;
                            goomba[i].y = 380;
                        }else if(plat == 1){
                            goomba[i].x = 755 + rand() % 350;
                            goomba[i].y = 350;
                        }else{
                            goomba[i].x = rand() % WIDTH;
                            goomba[i].y = 710;
                        }
                        goomba_add ++;
                        goomba_exist_num ++;
                        if(goomba_add == 5) break;
                    }
                }
                for(int i=0; i<candies_number; i++){
                    candies[i].eaten = false;
                    int level = rand() % 3; // 0 1 2
                    candies[i].x = 100 + rand() % (WIDTH-200);
                    candies[i].y = HEIGHT - 140 - (level * 200 + (rand() % 140));
                }
                candies_exist_num = candies_number;
                dragon.status = 0;
            }
        }else if( dragon.status == 3 && dragon.tired_anime == -1){
            dragon.tired_anime = 0;
        }
    }else{
        if(dragon.status != 4){
            al_play_sample_instance(dragon.weak_sound);
            character_bought[3] = 1;
            dragon.anime = -120;
            dragon.status = 4;
        }else{
            if(dragon.anime == 0 && goomba_exist_num == 0){
                current_window = 4;
                change_window = true;
            }
        }
    }
}

void boss_draw(){
    if(dragon.health > 0) al_draw_bitmap(dragon.img[dragon.status], dragon.x, dragon.y, 0);
}

void boss_destroy(){
    for(int i=0; i<3; i++) al_destroy_bitmap(dragon.img[i]);
    al_destroy_sample_instance(dragon.atk_sound);
}

// minion function

void minion_update(){
    for(int i=0; i<10; i++){
        if(goomba[i].exist){
            // move
            if(goomba[i].dir) goomba[i].x--;
            else goomba[i].x++;
            // border
            if(goomba[i].plat == 0){
                if(goomba[i].x + goomba[i].width/2 < 0) goomba[i].dir = false;
                else if (goomba[i].x + goomba[i].width/2 > 350) goomba[i].dir = true;
            }else if(goomba[i].plat == 1){
                if (goomba[i].x+goomba[i].width/2 < 755) goomba[i].dir = false;
                else if (goomba[i].x+goomba[i].width/2 > 1105) goomba[i].dir = true;
            }else{
                if(goomba[i].x < 0) goomba[i].dir = false;
                else if(goomba[i].x > WIDTH) goomba[i].dir = true;
            }
        }
    }

}

void minion_collide(){
    for(int i=0; i<10; i++){
        // attack by minion
        if(abs(chara.x + 20 - goomba[i].x) < 25 && abs(chara.y - goomba[i].y) < 50 && goomba[i].exist){
            chara.health--;
            if(chara.health < 0) chara.health = 0;
        }
        // end game stuff
        if(dragon.status == 4 && chara.state == ATK && abs(chara.x + 20 - goomba[i].x) < 75 && abs(chara.y - goomba[i].y) < 75 && goomba[i].exist){
            goomba[i].exist = false;
            goomba_exist_num--;
            al_play_sample_instance(dragon.weak_sound);
        }
        // step on minion
        if(abs(chara.x + 20 - goomba[i].x) < 50 && chara.y < goomba[i].y-chara.height && abs(chara.y - (goomba[i].y - chara.height)) < 75 && chara.jump && chara.jump_anime < 0 && goomba[i].exist){
            goomba[i].exist = false;
            goomba_exist_num--;
            al_play_sample_instance(goomba[i].die_sound);
        }
    }
}

void minion_draw(){
    for(int i=0; i<10; i++){
        if(goomba[i].exist){
            if(goomba[i].dir){
                if(goomba[i].anime % 100 > 50) al_draw_bitmap(goomba[i].move_img[0], goomba[i].x, goomba[i].y, 0);
                else al_draw_bitmap(goomba[i].move_img[1], goomba[i].x, goomba[i].y, 0);
            }else{
                if(goomba[i].anime % 100 > 50) al_draw_bitmap(goomba[i].move_img[0], goomba[i].x, goomba[i].y, ALLEGRO_FLIP_HORIZONTAL);
                else al_draw_bitmap(goomba[i].move_img[1], goomba[i].x, goomba[i].y, ALLEGRO_FLIP_HORIZONTAL);
            }

        }
    }
}

void minion_destroy(){
    al_destroy_bitmap(goomba[0].move_img[0]);
    al_destroy_bitmap(goomba[0].move_img[1]);
}

// character for boss stage

void character_update2(){
    if( chara.health > 0 ){
        // use the idea of finite state machine to deal with different state
        if( key_state[ALLEGRO_KEY_W] && !chara.jump ){
            chara.jump_anime = 15, chara.jump = true, chara.double_jump = false;
        }else if( key_state[ALLEGRO_KEY_S] && !chara.jump && !chara.double_jump){
            chara.jump_anime = 15, chara.jump = true, chara.double_jump = true;
        }else if( key_state[ALLEGRO_KEY_A] ){
            chara.dir = false;
            chara.x -= 10;
            chara.state = MOVE;
        }else if( key_state[ALLEGRO_KEY_D] ){
            chara.dir = true;
            chara.x += 10;
            chara.state = MOVE;
        }else if( key_state[ALLEGRO_KEY_SPACE] ){
            chara.state = ATK;
        }else if( chara.anime == chara.anime_time-1 ){
            chara.anime = 0;
            chara.state = STOP;
        }else if ( chara.anime == 0 ){
            chara.state = STOP;
        }
        // for jump function
        if ( chara.jump ){
            if ( chara.jump_anime >= -15){
                // if chara.jump_anime >0 character go up <0 character go down
                if( chara.double_jump ) chara.y -= 2*chara.jump_anime;
                chara.y -= chara.jump_anime;
                chara.jump_anime--;
            }
            else chara.jump = false, chara.double_jump = false;
        }
        // for attack candy
        candy_collide();
        minion_collide();
        // for boarder
        if ( chara.y == 350 && chara.jump_anime < 0 && !chara.on_plat && ((chara.x+chara.width/2 >= 0 && chara.x+chara.width/2 <= 350) || (chara.x+chara.width/2 >= 755 && chara.x+chara.width/2 <= 1105))){
            chara.jump = false, chara.double_jump = false;
            chara.state = STOP;
            chara.on_plat = true;
        }
        if ( chara.y == 350 && chara.on_plat && !((chara.x+chara.width/2 >= 0 && chara.x+chara.width/2 <= 350) || (chara.x+chara.width/2 >= 755 && chara.x+chara.width/2 <= 1105))){
            chara.jump = true, chara.double_jump = true;
            chara.jump_anime = 0;
            chara.on_plat = false;
        }
    }else{
        if(chara.state != 3){
            chara.anime = -120;
            chara.state = 3;
        }else{
            if(chara.anime == 0){
                current_window = 4;
                change_window = true;
            }
        }
    }
}

void health_draw(){
    char text[13];
    // al_draw_text(chara.font2, al_map_rgb(180, 0, 0), 1250, 20, ALLEGRO_ALIGN_CENTRE, text);
    al_draw_filled_rectangle(1050, 10, 1350-dragon.health, 30, al_map_rgb(255, 255, 255));
    al_draw_filled_rectangle(1350-dragon.health, 10, 1350, 30, al_map_rgb(180, 0, 0));
    al_draw_text(chara.font1, al_map_rgb(0, 0, 0), 1350, 50, ALLEGRO_ALIGN_RIGHT, "Dragon health");

    al_draw_filled_rectangle(50, 10, 50+chara.health*3, 30, al_map_rgb(180, 0, 0));
    al_draw_filled_rectangle(50+chara.health*3, 10, 350, 30, al_map_rgb(255, 255, 255));
    al_draw_text(chara.font1, al_map_rgb(0, 0, 0), 50, 50, ALLEGRO_ALIGN_LEFT, "Player health");
    // al_draw_text(chara.font2, al_map_rgb(120, 0, 0), 300, 20, ALLEGRO_ALIGN_CENTRE, text);


    sprintf( text, "money: %3d", money);
    al_draw_text(chara.font1, al_map_rgb(0, 0, 0), 700, 50, ALLEGRO_ALIGN_CENTER, text);
}

void real_choose_init(){
    chara.x = 320;
    chara.y = 500;
    chara.dir = false;
    // initial the animation component
    chara.state = STOP;
    chara.anime = 0;
    chara.choosing_anime = 0;
    chara.anime_time = 30;
    chara.choosing_anime_time = 800;

    // initial jump component
    chara.jump = false;
    chara.double_jump = false;
    chara.jump_anime = -15; // which frame the jump animation is in
}
void real_game_init(){
    chara.x = WIDTH/2 - chara.width/2;
    chara.y = HEIGHT - 60 - chara.height;;
    chara.dir = false;
    // initial the animation component
    chara.state = STOP;
    chara.anime = 0;
    chara.anime_time = 30;

    // initial jump component
    chara.jump = false;
    chara.double_jump = false;
    chara.jump_anime = -15; // which frame the jump animation is in
    chara.on_plat = false;

    int level;
    candies_number = 10;
    srand( time(NULL) );
    for(int i=0; i<candies_number; i++){
        candies[i].eaten = false;
        level = rand() % 3; // 0 1 2
        candies[i].x = 100 + rand() % (WIDTH-200);
        candies[i].y = level * 300 + (rand() % 140);
    }
}
void real_boss_init(){
    chara.x = WIDTH/2 - chara.width/2;
    chara.y = HEIGHT - 60 - chara.height;
    chara.dir = false;
    // initial the animation component
    chara.state = STOP;
    chara.anime = 0;
    chara.anime_time = 30;

    // initial jump component
    chara.jump = false;
    chara.double_jump = false;
    chara.jump_anime = -15; // which frame the jump animation is in
    chara.on_plat = false;
    chara.health = 100;
    for(int i=0; i<candies_number; i++){
        candies[i].eaten = true;
    }
    candies_number = 3;
    for(int i=0; i<10; i++){
        goomba[i].exist = false;
    }
    goomba_exist_num = 0;
    dragon.x = 800;
    dragon.y = 0;
    dragon.health = 300;
    dragon.status = 0;
    dragon.eat_candy = false;
    dragon.anime = 0;
    dragon.anime_time = 500;
    dragon.tired_anime = -1;
    dragon.tired_anime_time = 300;
}
