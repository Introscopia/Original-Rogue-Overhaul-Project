#ifndef I_CURSES_INCLUDED
#define I_CURSES_INCLUDED

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "ok_lib.h"


typedef struct font_data_struct
{
    map_int_int charIDs;
    int pitch, w, h;
    int offset_x;
    int *color_offset_y;
} font_data;


extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *gFont;

extern map_str_int sprite_id_map;
extern SDL_Texture *sheet;
extern SDL_Rect wall_srcs[16];
extern SDL_Rect sprite_srcs[42];
extern SDL_Rect map_wall_srcs[16];
extern SDL_Rect map_icon_srcs[14];
extern map_int_int char_sprite_map;
extern font_data FD;
//map_str_int font_colors_map; for this we would have to save the names of the color blocks too...
extern SDL_Rect AMULET_src;// = (SDL_Rect){0, 0, 0, 0};
extern SDL_Rect LOGO_src;// = (SDL_Rect){0, 0, 0, 0};
extern SDL_Rect RIP_src;// = (SDL_Rect){0, 0, 0, 0};
extern SDL_Rect GRAVE_src;// = (SDL_Rect){0, 0, 0, 0};
extern SDL_Rect GRAVE_ORNAMENTS_srcs[3];
//for (int i = 0; i < 3; ++i)
//    GRAVE_ORNAMENTS_srcs[i] = (SDL_Rect){0, 0, 0, 0};

#define TILE_WIDTH 16
#define TILE_HEIGHT 16



#define WINDOW void

#define stdscr NULL

//#define wmove NOOP
int wmove(WINDOW *win, int y, int x);

//#define waddch NOOP
int waddch(WINDOW *win, char ch);

#define waddnstr NOOP

//#define refresh NOOP
int refresh(void);

#define wrefresh NOOP
#define LINES 24
#define wattrset 0
#define curscr 0
#define clear NOOP
#define wclear NOOP
#define COLS 80
#define winch NOOP
#define unctrl NOOP
#define clearok NOOP
#define wtouchln 0
#define wclrtoeol 0
#define raw NOOP
#define printw NOOP
#define mvwprintw NOOP
#define mvcur NOOP
#define keypad NOOP
#define delwin NOOP
#define wprintw NOOP
#define noecho NOOP
#define newwin NOOP
#define leaveok NOOP
#define nocbreak NOOP
#define mvwin NOOP
#define mvprintw NOOP
#define initscr NOOP
#define idlok NOOP
#define wgetnstr NOOP
#define wgetch 0
#define werase NOOP
#define erasechar NOOP
#define subwin NOOP
#define killchar NOOP
#define isendwin NOOP
#define halfdelay NOOP
#define flushinp NOOP
#define ESCDELAY 0
#define cur_term 0
#define baudrate NOOP
#define mvaddch NOOP
#define move NOOP
#define addch NOOP
#define standout NOOP
#define standend NOOP
#define mvinch NOOP
#define mvwinch NOOP
#define touchwin NOOP
#define A_CHARTEXT 0
#define inch NOOP
#define endwin NOOP
#define addstr NOOP
#define waddstr NOOP
#define clrtoeol NOOP
#define mvaddstr NOOP
#define getyx NOOP
#define tcgetattr NOOP
#define getgid NOOP
#define getloadavg NOOP
#define mvwaddch NOOP
#define getmaxx NOOP
#define getmaxy NOOP

//from io.c
#define show_win NOOP

#endif