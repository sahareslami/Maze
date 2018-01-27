#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "object.h"

#define MAXN 1000




enum KEYS{up = 0 , down , left , right};
int key[4] = {0 , 0 , 0 , 0};

int init();
void wellcome();
void read_map();
void draw_map();
void player_move_keyboard();
void move_up(PLAYER *player);
void move_down(PLAYER *player);
void move_right(PLAYER *player);
void move_left(PLAYER *player);
void back_screen();
//void player_move_mouse();
void init_player(PLAYER *player);
void draw_player(PLAYER *player);
void convert_display_map(int x_dispaly,int y_display,int *x_map,int *y_map);
//void convert_map_display(int x_map,int y_map , int *x_dispaly , int *y_display);
//void init_enemy(ENEMIES *enemy);
/*void draw_enemy(ENEMIES *enemy);
void move_enemy(ENEMIES *enemy);
int make_random(int s,int e);
void detection();*/







ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;

int cnt_of_move; // player should win in at last cnt_of_move step
int n,m,map[MAXN][MAXN];  // n * m is size of map in real
int N,M; // N * M is size of map in computer
int W = 700 ,H = 700; //size of display
int s_cell = 100 , l_line = 6; // size of cell in map is s_cell * s_cell , length of each line is l_line
int st_x , st_y; // start point of map in allegro display
int w,h; // w * h is the size of map in allegro display
int FPS = 60; // frames per second

int main(void){
	PLAYER player;
	read_map();
	//wellcome();
	if(!init())
		return -1;
	draw_map();
	draw_player(&player);
	init_player(&player);
	player_move_keyboard(&player);
	//player_move_mouse();
	al_destroy_display(display);
	return 0;
}
int init(){
	al_init();
	if(!al_init()){
		//al_show_native_message_box(NULL , NULL , NULL , "failed to initialize allegro!" , NULL , NULL);
		return 0;
	}
	display = al_create_display(W / 2 , H / 2
		);
	if(!display){
		//al_show_native_message_box(NULL , NULL , NULL , "failed to initialize display!" , NULL , NULL);
		return 0;
	}
	return 1;

}
void wellcome(){
	al_init_font_addon();
	al_init_ttf_addon();
	ALLEGRO_FONT *arial = al_load_font("arial.ttf" , 100 , 0);
	ALLEGRO_FONT *papyrus = al_load_font("PAPYRUS.TTF" , 100 , 0);

	al_clear_to_color(al_map_rgb(12,12 ,12));
	al_draw_text(arial , al_map_rgb(255 , 155 ,149) , W / 2 , H / 2 , 0 , "wellcome");
	al_flip_display();
	al_rest(2.0);

	al_clear_to_color(al_map_rgb(12 , 12, 12));
	
	al_draw_text(arial , al_map_rgb(0 , 0 ,149) , W / 2 , H / 2 , 0 , "to my game");
	al_flip_display();
	al_rest(2.0);
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
void draw_map(){	
	w = (s_cell + l_line) * m;
	h = (s_cell + l_line) * n;
	st_x = W / 2 - w / 2 ;
	st_y = H / 2 - h / 2 ;
	al_clear_to_color(al_map_rgb(10 , 10 , 10));
	al_draw_rectangle(st_x , st_y , st_x + w  , st_y + h  , al_map_rgb(100 , 100 , 100) , l_line);
	printf("%d %d %d %d\n",st_x,st_y,st_x+w,st_y + h);
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


	al_flip_display();
	//al_rest(15.0);
}
void back_screen(){
	w = (s_cell + l_line) * m;
	h = (s_cell + l_line) * n;
	st_x = W / 2 - w / 2;
	st_y = H / 2 - h / 2;
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
void player_move_keyboard(PLAYER *player){
	int redraw = 0;
	int done = 0;
	al_install_keyboard();
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);
	al_register_event_source(event_queue , al_get_keyboard_event_source());
	al_register_event_source(event_queue , al_get_display_event_source(display));
	al_register_event_source(event_queue , al_get_timer_event_source(timer));
	al_start_timer(timer);
	while(!done){
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue , &ev);
		/*if(ev.type == ALLEGRO_EVENT_TIMER){
			redraw = 1;
			if(key[up] == 1)
				move_up(player);
			if(key[down] == 1)
				move_down(player);
			if(key[left] == 1)
				move_left(player);
			if(key[right] == 1)
				move_right(player);
		}*/
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
		}
		/*else if(ev.type == ALLEGRO_EVENT_KEY_UP){
			switch(ev.keyboard.keycode){
				case ALLEGRO_KEY_UP:
					key[up] = 0;
					break;
				case ALLEGRO_KEY_DOWN:
					key[down] = 0;
					break;
				case ALLEGRO_KEY_LEFT:
					key[left] = 0;
					break;
				case ALLEGRO_KEY_RIGHT:
					key[right] = 0;
					break;
			}
		}*/

		draw_player(player);
		al_flip_display();
		back_screen();
	}
}
/*void player_move_mouse(){
	int pos_x = st_x + 30 , pos_y = st_y + 30;
	int done = 0;

	al_install_mouse();
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
	printf("%d %d %d %d %d %d %d %d %d\n",st_x,st_y,player->x,player->y,x_map,y_map,x_map - 1,y_map,map[x_map - 1][y_map]);
	if(map[x_map - 1][y_map] == 0)
			player->y -= (s_cell + l_line);
}
void move_down(PLAYER *player){
	int x_map , y_map;
	convert_display_map(player->x , player->y , &x_map , &y_map);
	printf("%d %d %d %d %d %d %d %d %d\n",st_x,st_y,player->x,player->y,x_map,y_map,x_map + 1,y_map,map[x_map + 1][y_map]);
	if(map[x_map + 1][y_map] == 0)
			player->y += (s_cell + l_line);

}
void move_right(PLAYER *player){
	int x_map , y_map;
	convert_display_map(player->x , player->y , &x_map , &y_map);
	printf("%d %d %d %d %d %d %d %d %d\n",st_x,st_y,player->x,player->y,x_map,y_map,x_map,y_map + 1,map[x_map][y_map + 1]);
	if(map[x_map][y_map + 1] == 0)
			player->x += (s_cell + l_line);
}
void move_left(PLAYER *player){
	int x_map , y_map;
	convert_display_map(player->x , player->y , &x_map , &y_map);
	printf("%d %d %d %d %d %d %d %d %d\n",st_x,st_y,player->x,player->y,x_map,y_map,x_map,y_map - 1,map[x_map][y_map - 1]);
	if(map[x_map][y_map - 1] == 0)
			player->x -= (s_cell + l_line);
}
void convert_display_map(int x_dispaly,int y_display,int *x_map,int *y_map){
	*x_map = ((x_dispaly - st_x) / (s_cell + l_line)) * 2 + 1;
	*y_map = ((y_display - st_y) / (s_cell + (l_line))) * 2 + 1;
}
//void convert_map_display(int x_map,int y_map,int *x_dispaly , int *y_display)
void draw_player(PLAYER *player){
	al_draw_filled_circle(player->x + 50 , player->y + 50 , 50 , al_map_rgb(56, 45 , 90));
}

/*int make_random(int s,int e){
	// this function make a random natural number in [a , b];	
	int x = rand() / RAND_MAX;
	x =  x * (e - s + 1) + s;
	x = floor(x);
	x = (x == e + 1) ? e;
	return x;

			
}
void init_enemy(ENEMIES *enemy){
		enemy->x = make_random(0 , n - 1);
		enemy->y = make_random(0 , m - 1);
		enemy->x = enemy->x * 2 + 1;
		enemy->y = enemy->y * 2 + 1;
		if(map[enemy->x][(enemy->y) - 1] == 0)
			enemy->D = up;
		else if(map[(enemy->x) + 1][(enemy->y)] == 0)
			enemy->D = right;
		else if(map[(enemy->x)][(enemy->y) + 1] == 0)
			enemy->D = down;
		else if(map[(enemy->x) - 1][(enemy->y)] == 0)
			enemy->D = left;
		convert_display_map(((enemy->x) - 1) / 2 , ((enemy->y) - 1) / 2 , &(enemy->x)  , &(enemy->y));
}
void draw_enemy(ENEMIES *enemy){
	al_draw_filled_circle(enemy->x + 50 ,enemy->y + 50 , 50 , al_map_rgb(255 , 0 , 0));
}
*/

/*
sample tests 
test #1;
1 3 10 
._._._.
| | . |
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