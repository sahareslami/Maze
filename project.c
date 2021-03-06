#include <stdio.h>
#include <math.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "object.h"

#define MAXN 1000
#define MAX_ENEMY 4
#define size_score 10




enum KEYS{up = 0 , down , left , right};
int key[4] = {0 , 0 , 0 , 0};

//prototype
int init();
void wellcome();
void read_map();
void draw_map(ENEMIES enemy[],int E);
void player_move_keyboard(PLAYER *player,ENEMIES enemy[],int size);
void move_up(PLAYER *player);
void move_down(PLAYER *player);
void move_right(PLAYER *player);
void move_left(PLAYER *player);
int move_up_enemy(ENEMIES *enemy);
int move_down_enemy(ENEMIES *enemy);
int move_right_enemy(ENEMIES *enemy);
int move_left_enemy(ENEMIES *enemy);
void back_screen();
//void player_move_mouse();
void init_player(PLAYER *player);
void draw_player(PLAYER *player);
void convert_display_map(int x_dispaly,int y_display,int *x_map,int *y_map);
void convert_map_display(int x_map,int y_map , int *x_dispaly , int *y_display);
void call_enemy(ENEMIES enemy[],int size);
void init_enemy(ENEMIES *enemy);
void draw_enemy(ENEMIES *enemy,int idx);
void move_enemy(ENEMIES enemy[],int size);
int make_random(int min,int max);
void init_images();
void init_voices();
void destroy_unistall();
int cnt_of_enemy();
void save_score();
void open_score(int score_now);
void show_score(int score_now);
void sort(int a,int size);
void swap(int *a,int *b);






//Allegro variables
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *image_player = NULL;
ALLEGRO_BITMAP *image_enemy0 = NULL;
ALLEGRO_BITMAP *image_enemy1 = NULL;
ALLEGRO_BITMAP *image_enemy2 = NULL;
ALLEGRO_BITMAP *image_enemy3 = NULL;
ALLEGRO_BITMAP *image_over = NULL;
ALLEGRO_BITMAP *image_win = NULL;
ALLEGRO_BITMAP *image_background = NULL;
ALLEGRO_SAMPLE *voice_win = NULL;
ALLEGRO_SAMPLE *voice_fail = NULL;
ALLEGRO_SAMPLE *voice_wall = NULL;



//variables
int cnt_of_move; // player should win in at last cnt_of_move step
int n,m,map[MAXN][MAXN] = {0};  // n * m is size of map in real
int scoreboard[size_score + 7] = {0}; //score
int N,M; // N * M is size of map in computer
int W = 700 ,H = 700; //size of display
int s_cell = 100 , l_line = 6; // size of cell in map is s_cell * s_cell , length of each line is l_line
int st_x , st_y; // start point of map in allegro display
int w,h; // w * h is the size of map in allegro display
int FPS = 60; // frames per second
int E;//random number of enemies
int image_width_player,image_height_player;
int image_width_enemy0 , image_height_enemy0;
int image_width_enemy1 , image_height_enemy1;
int image_width_enemy2 , image_height_enemy2;
int image_width_enemy3 , image_height_enemy3;
int image_width_win , image_height_win;
int image_width_over , image_height_over;
int image_width_background , image_height_background;



FILE *input_map , *out_score , *in_score;




int main(void){
	//time 
	srand(time(NULL));
	//object variables
	PLAYER player; 
	ENEMIES enemy[MAX_ENEMY];

	read_map();
	//Allegro Module Init
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	//al_init_primitves_addon();
	al_init_acodec_addon();

	//Allegro Install 
	//al_install_mouse();
	al_install_audio();

	//very very important variables about location of map
	w = (s_cell + l_line) * m;
	h = (s_cell + l_line) * n;
	st_x = (W / 2) - (w / 2);
	st_y = (H / 2) - (h / 2);


	if(!init())
		return -1;
	al_reserve_samples(1);
	init_voices();
	init_images();
	
	wellcome();
	E = make_random(1 , MAX_ENEMY);
	call_enemy(enemy , E);
	draw_map(enemy , E);
	draw_player(&player);
	init_player(&player);
	
	player_move_keyboard(&player,enemy,E);
	//player_move_mouse();	

	open_score(player.score);
	save_score();
	destroy_unistall();
	return 0;
}
int init(){
	al_init();
	if(!al_init()){
		//al_show_native_message_box(NULL , NULL , NULL , "failed to initialize allegro!" , NULL , NULL);
		return 0;
	}
	display = al_create_display(W  / 2, H / 2);
	if(!display){
		//al_show_native_message_box(NULL , NULL , NULL , "failed to initialize display!" , NULL , NULL);
		return 0;
	}
	return 1;

}
void wellcome(){
	al_draw_scaled_bitmap(image_background , 0 , 0 , image_width_background , image_height_background , 0  , 0 , W  , H ,  0);
	al_flip_display();
	al_rest(1.5);
}
void read_map(){
	scanf("%d %d\n", &n , &m);
	scanf("%d\n" , &cnt_of_move);
	N = 2 * n + 1 , M = 2 * m + 1;
	for(int i = 0 ; i < N ; i++){
		char now;
		for(int j = 0 ; j < M ; j++){
			scanf("%c" , &now);
			switch (now){
				case '_':
					map[i][j] = 1;
					break;
				case '|':
					map[i][j] = 2;
					break;
				case ' ':
					map[i][j] = 0;
					break;
				case '.':
					map[i][j] = 0;
					break;
				default:
					map[i][j] = 0;
					break;
			}
		}
		scanf("%c" , &now);
	}
	printf("\n");
	for(int i = 0 ; i < M ; i++)
		map[0][i] = 1 , map[N-1][i] = 1;
	for(int i = 0 ; i < N ; i++)
		map[i][0] = 1 , map[i][M-1] = 1;
	for(int i = 0 ; i < N ; i++){
		for(int j = 0 ; j < M ; j++)
			printf("%d ",map[i][j]);
		printf("\n");
	}
}
void draw_map(ENEMIES enemy[],int E){	
	al_clear_to_color(al_map_rgb(10 , 10 , 10));
	al_draw_rectangle(st_x , st_y , st_x + w  , st_y + h  , al_map_rgb(100 , 100 , 100) , l_line);
	for(int i = 1 ; i < n  ; i++){
		int st_line_y = st_y + i * (s_cell + l_line);
		al_draw_line(st_x , st_line_y , st_x + w , st_line_y  , al_map_rgb( 200 , 200 , 200) , l_line);
	}
	for(int i = 1 ; i < m ; i++){
		int st_line_x = st_x + (s_cell + l_line) * i;
		al_draw_line(st_line_x , st_y , st_line_x , st_y + h , al_map_rgb(200 , 200 ,200) , l_line);
	}
	for(int i = 1 ; i < N - 1 ; i++){
		int t = (i % 2) + 1;
		for(int j = t  ; j < M - 1  ; j+=t){
			if(map[i][j] == 1){
				int st_line_x = st_x + (j / 2) * s_cell + (j / 2) * l_line + l_line / 2;
				int st_line_y = st_y + (i / 2) * s_cell + (i / 2) * l_line + l_line / 2;
				al_draw_line(st_line_x , st_line_y ,st_line_x + s_cell ,st_line_y ,al_map_rgb(100 , 200 , 255) , 2 * l_line);
			}
			if(map[i][j] == 2){
				int st_line_x = st_x + (j / 2) * s_cell + (j / 2) * l_line + l_line / 2;
				int st_line_y = st_y + (i / 2) * s_cell + (i / 2) * l_line + l_line / 2;
				al_draw_line(st_line_x , st_line_y ,st_line_x ,st_line_y + s_cell ,al_map_rgb(100 , 200 , 255) , 2 * l_line);
			}
		}
	}
	al_draw_scaled_bitmap(image_player , 0 , 0 , image_width_player , image_height_player , st_x + l_line  , st_y + l_line , s_cell  , s_cell ,  0);
	for(int i = 0 ; i < E ; i++)
			draw_enemy(&enemy[i],i);
	al_flip_display();
}
void back_screen(){
	al_clear_to_color(al_map_rgb(10 , 10 , 10));
	al_draw_rectangle(st_x , st_y , st_x + w  , st_y + h  , al_map_rgb(100 , 100 , 100) , l_line);
	for(int i = 1 ; i < n  ; i++){
		int st_line_y = st_y + i * (s_cell + l_line);
		al_draw_line(st_x , st_line_y , st_x + w , st_line_y  , al_map_rgb( 200 , 200 , 200) , l_line);
	}
	for(int i = 1 ; i < m ; i++){
		int st_line_x = st_x + (s_cell + l_line) * i;
		al_draw_line(st_line_x , st_y , st_line_x , st_y + h , al_map_rgb(200 , 200 ,200) , l_line);
	}
	for(int i = 1 ; i < N - 1 ; i++){
		int t = (i % 2) + 1;
		for(int j = t  ; j < M - 1  ; j+=t){
			if(map[i][j] == 1){
				int st_line_x = st_x + (j / 2) * s_cell + (j / 2) * l_line + l_line / 2;
				int st_line_y = st_y + (i / 2) * s_cell + (i / 2) * l_line + l_line / 2;
				al_draw_line(st_line_x , st_line_y ,st_line_x + s_cell ,st_line_y ,al_map_rgb(100 , 200 , 255) , 2 * l_line);
			}
			if(map[i][j] == 2){
				int st_line_x = st_x + (j / 2) * s_cell + (j / 2) * l_line + l_line / 2;
				int st_line_y = st_y + (i / 2) * s_cell + (i / 2) * l_line + l_line / 2;
				al_draw_line(st_line_x , st_line_y ,st_line_x ,st_line_y + s_cell ,al_map_rgb(100 , 200 , 255) , 2 * l_line);
			}
		}
	}
}
void player_move_keyboard(PLAYER *player,ENEMIES enemy[],int size){
	int done = 0;
	int mve = 0;
	int number_of_step = 0;
	al_install_keyboard();
	event_queue = al_create_event_queue();
	al_register_event_source(event_queue , al_get_keyboard_event_source());
	al_register_event_source(event_queue , al_get_display_event_source(display));
	while(!done){
		printf("%d \n",player->score);
		ALLEGRO_EVENT ev;

		al_wait_for_event(event_queue , &ev);

		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
					done = 1;
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
			switch(ev.keyboard.keycode){
				case ALLEGRO_KEY_UP:
					move_up(player);
					break;
				case ALLEGRO_KEY_DOWN:
					move_down(player);
					break;
				case ALLEGRO_KEY_LEFT:
					move_left(player);
					break;
				case ALLEGRO_KEY_RIGHT:
					move_right(player);
					break;
			}
			move_enemy(enemy,size);
			if(!mve){
				(player->score) += (n * 1000) + (E * 500);
				mve = 1;
			}
		}

		///lose condition
		for(int i = 0 ; i < size ; i++){
			if((enemy[i].x == player->x &&  enemy[i].y == player->y) || (player->R_move <= 0)){
				player->score = 0;
				al_play_sample(voice_fail , 1 , 0 , 1 , ALLEGRO_PLAYMODE_ONCE , NULL);
				al_draw_scaled_bitmap(image_over , 0 , 0 , image_width_over , image_height_over , 0  , 0, W , H ,  0);
				al_flip_display();
				al_clear_to_color(al_map_rgb(100 , 30 , 30));
				al_rest(4.0);
				return;
			}
		}

		int x_map,y_map;
		convert_display_map(player->x,player->y,&x_map,&y_map);
		//win condition
		if(x_map == 2 * n - 1 && y_map == 2 * m - 1){
			al_play_sample(voice_win , 1 , 0 , 1 , ALLEGRO_PLAYMODE_ONCE , NULL);
			al_draw_scaled_bitmap(image_win , 0 , 0 , image_width_win , image_height_win , 0  , 0, W , H ,  0);
			al_flip_display();
			//al_clear_to_color(al_map_rgb(30 , 30 , 100));
			al_rest(4.0);
			return;
		}


		back_screen();
		for(int i = 0 ; i < size ;i++)
				draw_enemy(&enemy[i],i);
		draw_player(player);
		al_flip_display();
		back_screen();
	}
}
/*void player_move_mouse(){
	int pos_x = st_x + 30 , pos_y = st_y + 30;
	int done = 0;
	al_register_event_source(event_queue , al_get_display_event_source(display));
	al_register_event_source(event_queue , al_get_mouse_event_source());
	while(!done){
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue , &ev);
		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
					done = 1;
		else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES){
			pos_x = ev.mouse.x;
			pos_y = ev.mouse.y;
		}
		draw_player(player);
		al_flip_display();
		back_screen();
	}
}*/
void init_player(PLAYER *player){
	player->x = st_x;
	player->y = st_y;
	player->score = 0;
	player->size_of_map = n;
	player->R_move = cnt_of_move;
}
void move_up(PLAYER *player){
	int x_map , y_map;
	convert_display_map(player->x , player->y , &x_map , &y_map);
	//printf("UP%d %d %d %d %d %d %d %d %d\n",st_x,st_y,player->x,player->y,x_map,y_map,x_map,y_map - 1,map[x_map][y_map - 1]);
	if(map[x_map - 1][y_map] == 0){
			player->y -= (s_cell + l_line);
			player->R_move -= 1;
			(player->score) -= 100;
			return;
	}
	al_play_sample(voice_wall , 1 , 0 , 1 , ALLEGRO_PLAYMODE_ONCE , NULL);
}
void move_down(PLAYER *player){
	int x_map , y_map;
	convert_display_map(player->x , player->y , &x_map , &y_map);
	//printf("DOWN%d %d %d %d %d %d %d %d %d\n",st_x,st_y,player->x,player->y,x_map,y_map,x_map + 1,y_map,map[x_map][y_map + 1]);
	if(map[x_map + 1][y_map] == 0){
			player->y += (s_cell + l_line);
			player->R_move -= 1;
			(player->score) -= 100;
			return;
	}
	al_play_sample(voice_wall , 1 , 0 , 1 , ALLEGRO_PLAYMODE_ONCE , NULL);
}
void move_right(PLAYER *player){
	int x_map , y_map;
	convert_display_map(player->x , player->y , &x_map , &y_map);
	//printf("RIGHT%d %d %d %d %d %d %d %d %d\n",st_x,st_y,player->x,player->y,x_map,y_map,x_map,y_map + 1,map[x_map][y_map + 1]);
	if(map[x_map][y_map + 1] == 0){
			player->x += (s_cell + l_line);
			player->R_move -= 1;
			(player->score) -= 100;
			return;
	}
	al_play_sample(voice_wall , 1 , 0 , 1 , ALLEGRO_PLAYMODE_ONCE , NULL);
}
void move_left(PLAYER *player){
	int x_map , y_map;
	convert_display_map(player->x , player->y , &x_map , &y_map);
	//printf("LEFT%d %d %d %d %d %d %d %d %d\n",st_x,st_y,player->x,player->y,x_map,y_map,x_map,y_map - 1,map[x_map][y_map - 1]);
	if(map[x_map][y_map - 1] == 0){
			player->x -= (s_cell + l_line);
			player->R_move -= 1;
			(player->score) -= 100;
			return;
	}
	al_play_sample(voice_wall , 1 , 0 , 1 , ALLEGRO_PLAYMODE_ONCE , NULL);
}
void convert_display_map(int x_dispaly,int y_display,int *x_map,int *y_map){
	*y_map = ((x_dispaly - st_x) / (s_cell + l_line)) * 2 + 1;
	*x_map = ((y_display - st_y) / (s_cell + (l_line))) * 2 + 1;
}

void convert_map_display(int x_map,int y_map,int *x_display , int *y_display){
	*x_display = st_x + (s_cell + l_line) * x_map;
	*y_display = st_y + (s_cell + l_line) * y_map;
}
void draw_player(PLAYER *player){
	//al_draw_filled_circle(player->x + 50 , player->y + 50 , 50 , al_map_rgb(56, 45 , 90));
	al_draw_scaled_bitmap(image_player , 0 , 0 , image_width_player , image_height_player , player->x + l_line  , player->y + l_line , s_cell  , s_cell ,  0);
}
int make_random(int min,int max){
	//return random number between min and max
    int r;
    int range = 1 + max - min;
    int buckets = RAND_MAX / range;
    int limit = buckets * range;
    do
    {
        r = rand();
    } while (r >= limit);

    return min + (r / buckets);
}

void call_enemy(ENEMIES enemy[] , int size){
	for(int i = 0 ; i < size ; i++){
		init_enemy(&enemy[i]);
		//draw_enemy(&enemy[i],i);
	}
}
void init_enemy(ENEMIES *enemy){
		enemy->x = make_random(0 , m - 1);
		enemy->y = make_random(0 , n - 1);
		if(enemy->x == 0 && enemy->y == 0){
			enemy->x = 1;
			enemy->y = 1;
		}
		enemy->x = enemy->x * 2 + 1;
		enemy->y = enemy->y * 2 + 1;
		if(map[(enemy->x) - 1][(enemy->y)] == 0)
			enemy->D = up;
		else if(map[(enemy->x)][(enemy->y) + 1] == 0)
			enemy->D = right;
		else if(map[(enemy->x) + 1][(enemy->y)] == 0)
			enemy->D = down;
		else if(map[(enemy->x)][(enemy->y) - 1] == 0)
			enemy->D = left;
		convert_map_display(((enemy->x) - 1) / 2 , ((enemy->y) - 1) / 2 , &(enemy->x)  , &(enemy->y));
}
void draw_enemy(ENEMIES *enemy,int idx){
	//al_draw_filled_circle(enemy->x + 50 ,enemy->y + 50 , 40 , al_map_rgb(255 , 10 , 10));
	if(idx == 0) al_draw_scaled_bitmap(image_enemy0 , 0 , 0 , image_width_enemy0 , image_height_enemy0 , enemy->x + l_line  , enemy->y + l_line , s_cell  , s_cell ,  0);
	if(idx == 1) al_draw_scaled_bitmap(image_enemy1 , 0 , 0 , image_width_enemy1 , image_height_enemy1 , enemy->x + l_line  , enemy->y + l_line , s_cell  , s_cell ,  0);
	if(idx == 2) al_draw_scaled_bitmap(image_enemy2 , 0 , 0 , image_width_enemy2 , image_height_enemy2 , enemy->x + l_line  , enemy->y + l_line , s_cell  , s_cell ,  0);
	if(idx == 3) al_draw_scaled_bitmap(image_enemy3 , 0 , 0 , image_width_enemy3 , image_height_enemy3 , enemy->x + l_line  , enemy->y + l_line , s_cell  , s_cell ,  0);

}
int move_up_enemy(ENEMIES *enemy){
	int x_map , y_map;
	convert_display_map(enemy->x, enemy->y,&x_map,&y_map);
	if(map[x_map - 1][y_map] == 0){
			enemy->y -= (s_cell + l_line);
				return 1;
	}
	return 0;

}
int move_down_enemy(ENEMIES *enemy){
	int x_map , y_map;
	convert_display_map(enemy->x, enemy->y,&x_map,&y_map);
	if(map[x_map + 1][y_map] == 0){
			enemy->y += (s_cell + l_line);
			return 1;
	}
	return 0;
}
int move_right_enemy(ENEMIES *enemy){
	int x_map , y_map;
	convert_display_map(enemy->x, enemy->y,&x_map,&y_map);
	if(map[x_map][y_map + 1] == 0){
			enemy->x += (s_cell + l_line);
			return 1;
	}
	return 0;
}
int move_left_enemy(ENEMIES *enemy){
	int x_map , y_map;
	convert_display_map(enemy->x, enemy->y,&x_map,&y_map);
	if(map[x_map][y_map - 1] == 0){
			enemy->x -= (s_cell + l_line);
			return 1;
	}
	return 0;
}
void move_enemy(ENEMIES enemy[] , int size){
	for(int i = 0 ; i < size ; i++){
		if(enemy[i].D == up){
			if(move_up_enemy(&enemy[i])== 0){
				move_down_enemy(&enemy[i]);
				enemy[i].D = down;
			}
		}
		else if(enemy[i].D == down){
			if(move_down_enemy(&enemy[i])== 0){
				move_up_enemy(&enemy[i]);
				enemy[i].D = up;
			}
		}
		else if(enemy[i].D == right){
			if(move_right_enemy(&enemy[i])== 0){
				move_left_enemy(&enemy[i]);
				enemy[i].D = left;
			}
		}
		else if(enemy[i].D == left){
			if(move_left_enemy(&enemy[i]) == 0){
				move_right_enemy(&enemy[i]);
				enemy[i].D = right;
			}
		}
	}
}
void init_images(){
	image_player = al_load_bitmap("P.jpg");

	image_width_player = al_get_bitmap_width(image_player);
	image_height_player = al_get_bitmap_height(image_player);


	image_enemy0 = al_load_bitmap("PEO.jpg");

	image_width_enemy0 = al_get_bitmap_width(image_enemy0);
	image_height_enemy0 = al_get_bitmap_height(image_enemy0);


	image_enemy1 = al_load_bitmap("PER.jpg");

	image_width_enemy1 = al_get_bitmap_width(image_enemy1);
	image_height_enemy1 = al_get_bitmap_height(image_enemy1);

	image_enemy2 = al_load_bitmap("PEB.jpg");

	image_width_enemy2 = al_get_bitmap_width(image_enemy2);
	image_height_enemy2 = al_get_bitmap_height(image_enemy2);

	image_enemy3 = al_load_bitmap("PEP.jpg");
	image_width_enemy3 = al_get_bitmap_width(image_enemy3);
	image_height_enemy3 = al_get_bitmap_height(image_enemy3);

	image_over = al_load_bitmap("gameover.jpg");
	image_width_over = al_get_bitmap_width(image_over);
	image_height_over = al_get_bitmap_height(image_over);

	image_win = al_load_bitmap("win.jpg");
	image_width_win = al_get_bitmap_width(image_win);
	image_height_win = al_get_bitmap_height(image_win);

	image_background = al_load_bitmap("background.jpg");
	image_width_background = al_get_bitmap_width(image_background);
	image_height_background = al_get_bitmap_height(image_background);
}

void init_voices(){
	voice_win = al_load_sample("victory.ogg");
	voice_fail = al_load_sample("fail.ogg");
	voice_wall = al_load_sample("walli.ogg");
}

void destroy_unistall(){
	al_destroy_bitmap(image_player);
	al_destroy_bitmap(image_enemy0);
	al_destroy_bitmap(image_enemy1);
	al_destroy_bitmap(image_enemy2);
	al_destroy_bitmap(image_enemy3);
	al_destroy_bitmap(image_over);
	al_destroy_bitmap(image_background);
	al_destroy_bitmap(image_win);
	al_destroy_sample(voice_win);
	al_destroy_display(display);

}
void swap(int *a,int *b){
	int tmp = *a;
	*a = *b;
	*b = tmp;
}
void sort(int a,int size){
	scoreboard[size] = a;
	for(int i = 0 ; i < size + 1 ; i++)
			for(int j = i ; j < size + 1; j++)
				if(scoreboard[i] < scoreboard[j])
					swap(&scoreboard[i],&scoreboard[j]);
}
void open_score(int score_now){
	int i = 0;
	in_score = fopen("score.txt" , "r");
	if (in_score == NULL){
    	printf("Error opening file!\n");
    	exit(1);
	}
	while(fscanf(in_score , "%d\n" , &scoreboard[i]) != EOF){
        i++;
    }
    fclose(in_score);
    sort(score_now , size_score);
    show_score(score_now);

}
void show_score(int score_now){
	int dis_between_line = H / (size_score + 1);
	ALLEGRO_FONT *arial = al_load_font("arial.ttf" , 60 , 0);

	al_clear_to_color(al_map_rgb(0 , 0 , 0));

	al_draw_textf(arial , al_map_rgb(245 , 189 , 74) ,W / 2 , 0 , ALLEGRO_ALIGN_CENTER , "YOUR SCORE = %i" , score_now);

	for(int i = 1 ; i <= size_score ; i++)
		al_draw_textf(arial , al_map_rgb(245 , 189 , 74) ,W / 2 , dis_between_line * i , ALLEGRO_ALIGN_CENTER , "SCORE %i = %i" , i , scoreboard[i - 1]);

	al_flip_display();
	al_clear_to_color(al_map_rgb(0 , 0 ,0));
	al_rest(5.0);
}
void save_score(){
	out_score = fopen("score.txt" , "w");
	for(int i = 0 ; i < size_score ; i++)
		fprintf(out_score, "%d\n", scoreboard[i]);
	fclose(out_score);
}

/*

sample tests 
test #1;
2 3 10 
._._._.
| | . |
. ._. .
| . . |
._._._.

 

test #2;

3 4 15
._._._._.
| | . . |
. . ._. .
| . . . |
._._. ._.
| . . . |
._._._._.


*/















