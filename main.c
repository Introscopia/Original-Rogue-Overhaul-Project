#include "basics.h"
#include <SDL.h>
#include <SDL_image.h>
//#include <SDL_ttf.h>
#include "SDL2_gfx/SDL2_framerate.h"

#include "ok_lib.h"

#ifdef WIN32
#define _WIN32_WINNT 0x0500
#include <windows.h>
#endif


Uint32 time_passed = 0;

typedef enum{ FLOOR, CORRIDOR, STAIRS, DOOR, TRAP, PLAYER, GOLD, WEAPON, 
			  ARMOR, FOOD, SCROLL, WAND, RING, POTION, PACK, EMPTYSCROLL, 
			  aquator, bat, centaur, dragon, emu,         venus, griffin, hobgoblin, ice,     jabberwock, kestrel, leprechaun, medusa, 
			  nymph,   orc, phantom, quagga, rattlesnake, snake, troll,   black,     vampire, wraith,     xeroc,   yeti,       zombie
			} sprite_id;
char sprite_names [42][12] = { "FLOOR", "CORRIDOR", "STAIRS", "DOOR", "TRAP", "PLAYER", "GOLD", "WEAPON", 
							   "ARMOR", "FOOD", "SCROLL", "WAND", "RING", "POTION", "PACK", "EMPTYSCROLL", 
							   "aquator", "bat", "centaur", "dragon", "emu", "venus", "griffin", "hobgoblin", "ice", "jabberwock", "kestrel", "leprechaun", "medusa", 
							   "nymph",   "orc", "phantom", "quagga", "rattlesnake", "snake", "troll", "black", "vampire", "wraith", "xeroc", "yeti", "zombie" };

typedef struct font_data_struct{
	map_int_int charIDs;
	int pitch, w, h;
	int offset_x;
	int *color_offset_y;
} font_data;

SDL_Rect font_src( font_data *fd, char C, int color ){
	int id = ok_map_get( &(fd->charIDs), C )-1;
	return (SDL_Rect){              (fd->offset_x + (id % fd->pitch)) * fd->w, 
		               (fd->color_offset_y[color] + (id / fd->pitch)) * fd->h, 
		               fd->w, fd->h };
}
void display_string( SDL_Renderer *renderer, SDL_Texture *sheet, font_data *fd, char *string, int x, int y, int scale, int color ){
	int len = strlen( string );
	int w = fd->w * scale;
	int h = fd->h * scale;
	for (int i = 0; i < len; ++i){
		SDL_Rect src = font_src( fd, string[i], color );
		if( src.x < 0 ) continue;
		SDL_RenderCopy( renderer, sheet, &src, &(SDL_Rect){ x+(i*w), y, w, h } );
	}
}

typedef struct {
    char p_ch;
    char p_flags;
    //THING *p_monst;
} PLACE;
#define MAXLINES 16
#define MAXCOLS 32

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~O~~~~~~~~~~| M A I N |~~~~~~~~~~~O~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(int argc, char *argv[]){

	//HWND hwnd_win = GetConsoleWindow();
	//ShowWindow(hwnd_win,SW_HIDE);
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;
	int width = 640;
	int height = 480;
	bool loop = 1;

	Uint32 then, now, frames;
	FPSmanager fpsm;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}
	if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED, &window, &renderer)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		return 3;
	}
	//SDL_MaximizeWindow( window );
	SDL_SetWindowTitle( window, "Rogue Overhaul" );
	SDL_GetWindowSize( window, &width, &height );

	//srand (time(NULL));// prime the random number generator

	const SDL_Color black = {0, 0, 0, 255};
	const SDL_Color white = {255, 255, 255, 255};
	//Uint32 rmask, gmask, bmask, amask;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
	#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
	#endif


	map_str_int sprite_id_map;
	ok_map_init_with_capacity(&sprite_id_map, 42);
	for (int i = 0; i < 42; ++i){
		ok_map_put( &sprite_id_map, sprite_names[i], i+1 );
	}

	//printf(">> %d, %d\n",PLAYER, ok_map_get(&sprite_id_map, "PLAYER")-1 );

	
	char map_chars [] = ".#%+^@*)]:?/=!";
	map_int_int char_sprite_map;
	ok_map_init_with_capacity(&char_sprite_map, 14);
	for (int i = 0; i < 14; ++i){
		ok_map_put( &char_sprite_map, map_chars[i], i+1 );
	}

	IMG_Init(IMG_INIT_PNG);

	SDL_Texture *sheet = IMG_LoadTexture( renderer, "Assets/loveable_rogue_sheet.png" );
	SDL_Rect wall_srcs [16];
	SDL_Rect sprite_srcs [42];
	SDL_Rect map_wall_srcs [16];
	SDL_Rect map_icon_srcs [14];
	font_data FD;
	//map_str_int font_colors_map; for this we would have to save the names of the color blocks too...

	SDL_Rect AMULET_src = (SDL_Rect){0,0,0,0};
	SDL_Rect LOGO_src = (SDL_Rect){0,0,0,0};
	SDL_Rect RIP_src = (SDL_Rect){0,0,0,0};
	SDL_Rect GRAVE_src = (SDL_Rect){0,0,0,0};
	SDL_Rect GRAVE_ORNAMENTS_srcs[3];
	for (int i = 0; i < 3; ++i) GRAVE_ORNAMENTS_srcs[i] = (SDL_Rect){0,0,0,0};

	//LOADING THE DATA ABOUT WHERE EVERYTHING IS IN THE SPRITESHEET
	FILE *f = fopen( "Assets/lovable_rogue data.txt", "r" );
	if( f != NULL ){

		char mode = 0;
		int size = 0;
		char str [100];
		int x, y, w, h;
		char c;

		map_str_int labels;
		ok_map_init(&labels);
		ok_map_put( &labels, "row",    1 );
		ok_map_put( &labels, "WALLS",  2 );
		ok_map_put( &labels, "AMULET", 3 );
		ok_map_put( &labels, "LOGO",   4 );
		ok_map_put( &labels, "RIP",    5 );
		ok_map_put( &labels, "GRAVE",  6 );
		ok_map_put( &labels, "GRAVE_ORNAMENTS_1", 7 );
		ok_map_put( &labels, "GRAVE_ORNAMENTS_2", 8 );
		ok_map_put( &labels, "GRAVE_ORNAMENTS_3", 9 );

		while( !feof( f ) ){

			switch( mode ){
				case 0:
					fscanf( f, "%s size %d", str, &size );
					mode = str[0];
					//printf("#%s, %d\n", str, size );
					getc( f );
					break;
				case 'S':
					if( empty_line( f ) ){
						mode = 0;
						break;
					}

					fscanf( f, "%s	%d, %d", str, &x, &y );
					//printf("S %s %d, %d\n", str, x, y );
					int L = ok_map_get( &labels, str );
					switch( L ){
						case 1://row
							{	
								int R = 0;//row index
								int i = 0;
								memset(str, 0, 99);
								getc( f );
								c = getc( f );
								while( c != EOF ){
									if( c == ' ' || c == '\n' ){
										int id = ok_map_get( &sprite_id_map, str )-1;
										//printf("\tr %s (%d), y= %d\n", str, id, y );
										sprite_srcs[id] = (SDL_Rect){ (x+R)*size, y*size, size, size };
										R++;
										if( c == '\n' ) break;
										i = 0;
										memset(str, 0, 99);
									}
									else{
										str[i++] = c;
									}
									c = getc( f );
								}
							}
							break;
						case 2://walls
							{
								fscanf( f, "%d x %d\n", &w, &h );
								//printf("\tW %d, %d, %d, %d\n", x, y, w, h );
								for (int i = 0; i < 16; ++i){
									wall_srcs[i] = (SDL_Rect){ (x+(i%w))*size, (y+(i/w))*size, size, size };
									//printf("%d, %d, %d, %d\n", wall_srcs[i].x, wall_srcs[i].y, wall_srcs[i].w, wall_srcs[i].h );
								}
							}
							break;
						case 3://amulet
							fscanf( f, "%d x %d", &w, &h );
							AMULET_src = (SDL_Rect){ x, y, w, h };
							break;
						case 4://logo
							fscanf( f, "%d x %d", &w, &h );
							LOGO_src = (SDL_Rect){ x, y, w, h };
							break;
						case 5://RIP
							fscanf( f, "%d x %d", &w, &h );
							RIP_src = (SDL_Rect){ x, y, w, h };
							break;
						case 6://Grave
							fscanf( f, "%d x %d", &w, &h );
							GRAVE_src = (SDL_Rect){ x, y, w, h };
							break;
						case 7://GRAVE_ORNAMENTS_1
							fscanf( f, "%d x %d", &w, &h );
							GRAVE_ORNAMENTS_srcs[0] = (SDL_Rect){ x, y, w, h };
							break;
						case 8://GRAVE_ORNAMENTS_2
							fscanf( f, "%d x %d", &w, &h );
							GRAVE_ORNAMENTS_srcs[1] = (SDL_Rect){ x, y, w, h };
							break;
						case 9://GRAVE_ORNAMENTS_3
							fscanf( f, "%d x %d", &w, &h );
							GRAVE_ORNAMENTS_srcs[2] = (SDL_Rect){ x, y, w, h };
							break;
					}
					break;
				case 'M':
					if( empty_line( f ) ){
						mode = 0;
						break;
					}

					fscanf( f, "%s	%d, %d", str, &x, &y );
					//printf("M %s %d, %d\n", str, x, y );
					L = ok_map_get( &labels, str );
					switch( L ){
						case 1://map row
							{	
								int R = 0;//row index
								int i = 0;
								memset(str, 0, 99);
								getc( f );
								c = getc( f );
								while( c != EOF ){
									if( c == ' ' || c == '\n' ){
										int id = ok_map_get( &sprite_id_map, str )-1;
										//printf("\tr %s (%d), y= %d\n", str, id, y );
										map_icon_srcs[id] = (SDL_Rect){ (x+R)*size, y*size, size, size };
										R++;
										if( c == '\n' ){
											ungetc( c, f );
											break;
										}
										i = 0;
										memset(str, 0, 99);
									}
									else{
										str[i++] = c;
									}
									c = getc( f );
								}
							}
							break;
						case 2://map walls
							{
								fscanf( f, "%d x %d", &w, &h );
								//printf("\tW %d, %d, %d, %d\n", x, y, w, h );
								for (int i = 0; i < 16; ++i){
									map_wall_srcs[i] = (SDL_Rect){ (x+(i%w))*size, (y+(i/w))*size, size, size };
									//printf("\t%d, %d, %d, %d\n", map_wall_srcs[i].x, map_wall_srcs[i].y, map_wall_srcs[i].w, map_wall_srcs[i].h );
								}
							}
							break;
					}
					break;
				case 'F':
					memset(str, 0, 99);
					fscanf( f, "layout %s", str );
					int len = strlen( str );
					ok_map_init_with_capacity( &(FD.charIDs), len );
					for (int i = 0; i < len; ++i){
						ok_map_put(  &(FD.charIDs), str[i], i+1 );
					}
					int i = 0;
					int cs = 2;
					FD.color_offset_y = malloc( cs * sizeof(int) );
					//ok_map_init( &font_colors_map );
					while( !feof( f ) ){
						memset(str, 0, 99);
						fscanf( f, "%s\t%d, %d\t%d x %d\n", str, &x, &y, &w, &h );
						printf(":>%d: %s %d, %d, %d, %d\n", i, str, x, y, w, h );
						//ok_map_put( &font_colors_map, str, i+1 );
						if( i >= cs ){
							cs *= 2;
							FD.color_offset_y = realloc( FD.color_offset_y, cs * sizeof(int) );
						}
						FD.color_offset_y[i++] = y;
						//printf("(%d)\n", FD.color_offset_y[i-1] );
						FD.offset_x = x;
						FD.pitch = w;
					}
					FD.w = size;
					FD.h = size;
					goto out;
					break;
			}

		}

		ok_map_deinit( &labels );
	}
	out:

	fclose( f );

	


	bool refresh = 1;
	SDL_initFramerate(&fpsm);
	SDL_setFramerate(&fpsm, 2);
	puts("<<Entering Loop>>");
	while ( loop ) { //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> L O O P <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
		while( SDL_PollEvent(&event) ){
			switch (event.type) {
				case SDL_QUIT:
					loop = 0;
					break;
				case SDL_KEYDOWN:
					//     if( event.key.keysym.sym == 'w' || event.key.keysym.sym == SDLK_UP )    
					break;
				case SDL_KEYUP:
						 
					break;
				case SDL_MOUSEMOTION:
					//mouseX = event.motion.x;
					//mouseY = event.motion.y;
					/*if( SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE) ){
						tx += event.motion.xrel;
						ty += event.motion.yrel;
					}*/
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					break;
				case SDL_MOUSEWHEEL:;
					
					break;
			}
		}

		if( refresh ){
			refresh = 0;
			SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255);
			SDL_RenderClear( renderer );

			// LOADING TEST MAP
			PLACE places[MAXLINES*MAXCOLS];
			f = fopen( "Assets/test map.txt", "r" );
			for (int j = 0; j < MAXLINES; ++j){
				for (int i = 0; i < MAXCOLS; ++i){
					places[i + (j*MAXCOLS)].p_ch = getc(f);
				}
				getc(f);
			}
			fclose(f);

			//RENDERING MAP
			for (int j = 0; j < MAXLINES; ++j){
				for (int i = 0; i < MAXCOLS; ++i){
					int p = i + (j*MAXCOLS);
					if( places[p].p_ch == ' ' ) continue;
					if( places[p].p_ch == '-' || places[p].p_ch == '|' ){
						int neighborhood = 0;
						if( places[p-MAXCOLS].p_ch == '-' || places[p-MAXCOLS].p_ch == '|' ) neighborhood = 1;
						if( places[p+1].p_ch == '-' || places[p+1].p_ch == '|' ) neighborhood |= 2;
						if( places[p+MAXCOLS].p_ch == '-' || places[p+MAXCOLS].p_ch == '|' ) neighborhood |= 4;
						if( places[p-1].p_ch == '-' || places[p-1].p_ch == '|' ) neighborhood |= 8;
						SDL_RenderCopy( renderer, sheet,wall_srcs + neighborhood, &(SDL_Rect){i*32, j*32, 32, 32} );
					}
					else{
						int id = ok_map_get( &char_sprite_map, places[p].p_ch)-1;
						SDL_RenderCopy( renderer, sheet, sprite_srcs + id, &(SDL_Rect){i*32, j*32, 32, 32} );
					}
				}
			}

			//RENDERING MINIMAP
			for (int j = 0; j < MAXLINES; ++j){
				for (int i = 0; i < MAXCOLS; ++i){
					int p = i + (j*MAXCOLS);
					if( places[p].p_ch == ' ' ) continue;
					if( places[p].p_ch == '-' || places[p].p_ch == '|' ){
						int neighborhood = 0;
						if( places[p-MAXCOLS].p_ch == '-' || places[p-MAXCOLS].p_ch == '|' ) neighborhood = 1;
						if( places[p+1].p_ch == '-' || places[p+1].p_ch == '|' ) neighborhood |= 2;
						if( places[p+MAXCOLS].p_ch == '-' || places[p+MAXCOLS].p_ch == '|' ) neighborhood |= 4;
						if( places[p-1].p_ch == '-' || places[p-1].p_ch == '|' ) neighborhood |= 8;
						SDL_RenderCopy( renderer, sheet, map_wall_srcs + neighborhood, &(SDL_Rect){1000+ i*8, j*8, 8, 8} );
					}
					else{
						int id = ok_map_get( &char_sprite_map, places[p].p_ch)-1;
						SDL_RenderCopy( renderer, sheet, map_icon_srcs + id, &(SDL_Rect){1000+ i*8, j*8, 8, 8} );
					}
				}
			}

			//RENDERING SOME TEXT
			display_string( renderer,sheet, &FD, "Rogue: Exploring the Dungeons of Doom", 100, 550, 3, 0 );//ok_map_get(&font_colors_map, "WHITE")-1


			SDL_RenderPresent(renderer);
			//printf("%d\n", time_passed );
			time_passed = SDL_framerateDelay(&fpsm);
		}
	}//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> / L O O P <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	ok_map_deinit( &char_sprite_map );
	ok_map_deinit( &sprite_id_map );
	ok_map_deinit( &(FD.charIDs) );
	SDL_DestroyTexture( sheet );

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}