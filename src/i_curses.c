#include "i_curses.h"

static int cursor_x;
static int cursor_y;

int wmove( WINDOW *win, int x, int y ){
	cursor_x = x;
	cursor_y = y;
}

int waddch( WINDOW *win, char ch ){

    if (ch == ' ') return 1;
    if (ch == '-' || ch == '|')
    {
    	//how are going to access the map state from here?...

        //int neighborhood = 0;
        //if (INDEX(cursor_y - 1, cursor_x)->p_ch == '-' || INDEX(cursor_y - 1, cursor_x)->p_ch == '|')
        //    neighborhood = 1;
        //if (INDEX(cursor_y, cursor_x + 1)->p_ch == '-' || INDEX(cursor_y, cursor_x + 1)->p_ch == '|')
        //    neighborhood |= 2;
        //if (INDEX(cursor_y + 1, cursor_x)->p_ch == '-' || INDEX(cursor_y + 1, cursor_x)->p_ch == '|')
        //    neighborhood |= 4;
        //if (INDEX(cursor_y, cursor_x - 1)->p_ch == '-' || INDEX(cursor_y, cursor_x - 1)->p_ch == '|')
        //    neighborhood |= 8;
        SDL_RenderCopy(renderer, sheet, wall_srcs + 0, &(SDL_Rect){cursor_x * TILE_WIDTH, cursor_y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT});
    }
    else
    {
        int id = ok_map_get(&char_sprite_map, ch) - 1;
        SDL_RenderCopy(renderer, sheet, sprite_srcs + id, &(SDL_Rect){cursor_x * TILE_WIDTH, cursor_y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT});
    }
    return 0;
}


int refresh(void){
	SDL_RenderPresent(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}