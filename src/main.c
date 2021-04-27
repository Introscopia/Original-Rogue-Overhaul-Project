/*
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 *
 * @(#)main.c	4.22 (Berkeley) 02/05/99
 */

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>

#include "config.h"
#include "rogue.h"
#include "basics.h"
#include "ok_lib.h"
#include "util.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *gFont;

char sprite_names[42][12] = {"FLOOR", "CORRIDOR", "STAIRS", "DOOR", "TRAP", "PLAYER", "GOLD", "WEAPON",
                             "ARMOR", "FOOD", "SCROLL", "WAND", "RING", "POTION", "PACK", "EMPTYSCROLL",
                             "aquator", "bat", "centaur", "dragon", "emu", "venus", "griffin", "hobgoblin", "ice", "jabberwock", "kestrel", "leprechaun", "medusa",
                             "nymph", "orc", "phantom", "quagga", "rattlesnake", "snake", "troll", "black", "vampire", "wraith", "xeroc", "yeti", "zombie"};

typedef struct font_data_struct
{
    map_int_int charIDs;
    int pitch, w, h;
    int offset_x;
    int *color_offset_y;
} font_data;

SDL_Rect font_src(font_data *fd, char C, int color)
{
    int id = ok_map_get(&(fd->charIDs), C) - 1;
    return (SDL_Rect){(fd->offset_x + (id % fd->pitch)) * fd->w,
                      (fd->color_offset_y[color] + (id / fd->pitch)) * fd->h,
                      fd->w, fd->h};
}

void display_string(SDL_Renderer *renderer, SDL_Texture *sheet, font_data *fd, char *string, int x, int y, int scale, int color)
{
    int len = strlen(string);
    int w = fd->w * scale;
    int h = fd->h * scale;
    for (int i = 0; i < len; ++i)
    {
        SDL_Rect src = font_src(fd, string[i], color);
        if (src.x < 0)
            continue;
        SDL_RenderCopy(renderer, sheet, &src, &(SDL_Rect){x + (i * w), y, w, h});
    }
}

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

map_str_int sprite_id_map;
SDL_Texture *sheet;
SDL_Rect wall_srcs[16];
SDL_Rect sprite_srcs[42];
SDL_Rect map_wall_srcs[16];
SDL_Rect map_icon_srcs[14];
map_int_int char_sprite_map;
font_data FD;
//map_str_int font_colors_map; for this we would have to save the names of the color blocks too...
SDL_Rect AMULET_src;// = (SDL_Rect){0, 0, 0, 0};
SDL_Rect LOGO_src;// = (SDL_Rect){0, 0, 0, 0};
SDL_Rect RIP_src;// = (SDL_Rect){0, 0, 0, 0};
SDL_Rect GRAVE_src;// = (SDL_Rect){0, 0, 0, 0};
SDL_Rect GRAVE_ORNAMENTS_srcs[3];
//for (int i = 0; i < 3; ++i)
//    GRAVE_ORNAMENTS_srcs[i] = (SDL_Rect){0, 0, 0, 0};


void load_spritesheet_metadata( char *filename ){

    FILE *f = fopen( filename, "r");

    if (f == NULL)
    {
        printf("Can't open sprite sheet metadata\n");
        exit(1);
    }

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

                fscanf( f, "%s  %d, %d", str, &x, &y );
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
                                if( c == ' ' || c == '\r' ){
                                    int id = ok_map_get( &sprite_id_map, str )-1;
                                    //printf("\tr %s (%d), y= %d\n", str, id, y );
                                    sprite_srcs[id] = (SDL_Rect){ (x+R)*size, y*size, size, size };
                                    R++;
                                    if( c == '\r' ){
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
                    case 2://walls
                        {
                            fscanf( f, "%d x %d", &w, &h );
                            printf("\tW %d, %d, %d, %d\n", x, y, w, h );
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

                fscanf( f, "%s  %d, %d", str, &x, &y );
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
                                if( c == ' ' || c == '\r' ){
                                    int id = ok_map_get( &sprite_id_map, str )-1;
                                    //printf("\tr %s (%d), y= %d\n", str, id, y );
                                    map_icon_srcs[id] = (SDL_Rect){ (x+R)*size, y*size, size, size };
                                    R++;
                                    if( c == '\r' ){
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
                    //printf(":>%d: %s %d, %d, %d, %d\n", i, str, x, y, w, h );
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
    out:

    ok_map_deinit( &labels );
    
    fclose( f );
}

/*
 * main:e
 *	The main program, of course
 */
int main(int argc, char **argv)
{
    char *env;
    int lowtime;

    md_init();

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

    
    ok_map_init_with_capacity(&sprite_id_map, 42);
    for (int i = 0; i < 42; ++i)
    {
        ok_map_put(&sprite_id_map, sprite_names[i], i + 1);
    }

    char map_chars[] = ".#%+^@*)]:?/=!";
    ok_map_init_with_capacity(&char_sprite_map, 14);
    for (int i = 0; i < 14; ++i)
    {
        ok_map_put(&char_sprite_map, map_chars[i], i + 1);
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Init Failed\n");
        exit(1);
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        printf("IMG_Init Failed\n");
        exit(1);
    }

    if (TTF_Init() < 0)
    {
        printf("TTF_Init Failed\n");
        exit(1);
    }

    SDL_CreateWindowAndRenderer(60 * 32, 24 * 32, SDL_WINDOW_MAXIMIZED, &window, &renderer);
    SDL_SetWindowTitle(window, PACKAGE_STRING);

    sheet = IMG_LoadTexture(renderer, "./assets/loveable_rogue_sheet.png");

    if (sheet == NULL)
    {
        printf("Can't open sprite sheet\n");
        exit(1);
    }

    

    load_spritesheet_metadata( "assets/loveable_rogue_data.txt" );

    // gFont = TTF_OpenFont("dos.ttf", 16);
    // if (gFont == NULL)
    // {
    //     printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    //     exit(1);
    // }

    /*
     * FIXME: do better arg processing
     */
    while (argc >= 2)
    {
#ifdef MASTER
        if (strcmp(argv[1], "-w") == 0)
        {
            if (strcmp(PASSWD, md_crypt(md_getpass("wizard's password: "), "mT")) == 0)
            {
                wizard = TRUE;
                player.t_flags |= SEEMONST;
            }
            argc--;
            argv++;
            continue;
        }
#endif
        if (strcmp(argv[1], "-r") == 0)
        {
            rookie_mode = TRUE;
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[1], "-5.3") == 0)
        {
            rogue_version = VERSION_5_3;
            argc--;
            argv++;
            continue;
        }
        if (strcmp(argv[1], "-5.4") == 0)
        {
            rogue_version = VERSION_5_4;
            argc--;
            argv++;
            continue;
        }
        if (strncmp(argv[1], "-", 1) == 0)
        {
            printf("rogue: WARNING: ignoring unknown option %s\n", argv[1]);
            argc--;
            argv++;
            continue;
        }
        break;
    }

    /*
     * get home and options from environment
     */

    strncpy(home, md_gethomedir(), MAXSTR);

    strcpy(file_name, home);
    strcat(file_name, "rogue.save");

    if ((env = getenv("ROGUEOPTS")) != NULL)
        parse_opts(env);
    if (env == NULL || whoami[0] == '\0')
        strucpy(whoami, md_getusername(), (int)strlen(md_getusername()));
    lowtime = (int)time(NULL);
#ifdef MASTER
    if (wizard && getenv("SEED") != NULL)
        dnum = atoi(getenv("SEED"));
    else
#endif
        dnum = lowtime + md_getpid();
    seed = dnum;

    open_score();

    /* 
     * Drop setuid/setgid after opening the scoreboard file. 
     */
    md_normaluser();

    /*
     * check for print-score option
     */

    if (argc == 2 && strcmp(argv[1], "-s") == 0)
    {
        noscore = TRUE;
        score(0, -1, 0);
        md_deinit();
        exit(0);
    }

    printf("Hello %s %s, just a moment while I dig the dungeon...", (rookie_mode ? "rookie" : "warrior"), whoami);
#ifdef MASTER
    if (wizard)
        printf(" #%d", dnum);
#endif
    fflush(stdout);

    // md_sleep(1); /* allow enough time to see the message */

    initscr();        /* Start up cursor package */
    init_probs();     /* Set up prob tables for objects */
    init_names();     /* Set up names of scrolls */
    init_colors();    /* Set up colors of potions */
    init_stones();    /* Set up stone settings of rings */
    init_materials(); /* Set up materials of wands */
    /*
     * Must call this after all of the above.
     */
    init_player(); /* Set up initial player stats and objects */
    // setup();

    /*
     * The screen must be at least NUMLINES x NUMCOLS
     */
    if (LINES < NUMLINES || COLS < NUMCOLS)
    {
        endwin();
        printf("\nsorry, the screen is %dx%d, must be at least %dx%d\n", LINES, COLS, NUMLINES, COLS, NUMCOLS);
        fflush(stdout);
        my_exit(1);
    }

    /*
     * The screen must be no larger than NUMLINES+2 x NUMCOLS+2
     */
    if (LINES > (NUMLINES + 2) || COLS > (NUMCOLS + 2))
    {
        endwin();
        printf("\nsorry, the screen is %dx%d, must be no larger than %dx%d\n", LINES, COLS, NUMLINES + 2, NUMCOLS + 2);
        fflush(stdout);
        my_exit(1);
    }

    /*
     * Set up windows
     */
    // hw = newwin(LINES, COLS, 0, 0);
    idlok(stdscr, TRUE);
    idlok(hw, TRUE);
#ifdef MASTER
    noscore = wizard;
#endif
    new_level(); /* Draw current level */
    /*
     * Start up daemons and fuses
     */
    start_daemon(runners, 0, AFTER);
    start_daemon(doctor, 0, AFTER);
    fuse(swander, 0, WANDERTIME, AFTER);
    start_daemon(stomach, 0, AFTER);
    playit();
    return (0);
}

/*
 * endit:
 *	Exit the program abnormally.
 */

void endit(int sig)
{
    NOOP(sig);
    fatal("Okay, bye bye!\n");
}

/*
 * rnd:
 *	Pick a very random number.
 */
int rnd(int range)
{
    return range == 0 ? 0 : abs((int)RN) % range;
}

/*
 * roll:
 *	Roll a number of dice
 */
int roll(int number, int sides)
{
    int dtotal = 0;

    while (number--)
        dtotal += rnd(sides) + 1;
    return dtotal;
}

SDL_Texture *letters[256];

SDL_Texture *surfaceForChar(char c)
{
    if (letters[c] != NULL)
    {
        return letters[c];
    }
    else
    {
        SDL_Color color = {255, 255, 255};

        char *pChar = malloc(2 * sizeof(char));
        pChar[0] = c;
        pChar[1] = '\0';

        //Render text surface
        SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, pChar, color);
        //Create texture from surface pixels
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        //Get rid of old surface
        SDL_FreeSurface(textSurface);
        free(pChar);

        letters[c] = texture;
        return texture;
    }
}

/*
 * playit:
 *	The main loop of the program.  Loop until the game is over,
 *	refreshing things and looking at the proper times.
 */

void playit()
{
    char *opts;

    if (md_hasclreol())
        inv_type = INV_CLEAR;

    /*
     * parse environment declaration of options
     */
    if ((opts = getenv("ROGUEOPTS")) != NULL)
        parse_opts(opts);

    oldpos = hero;
    oldrp = roomin(&hero);

    SDL_Event event;

    while (playing)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                playing = 0;
                break;
            case SDL_KEYDOWN:
                // command('a'); /* Command execution */

                command(tolower((char)*SDL_GetKeyName(event.key.keysym.sym)));
                break;
            case SDL_KEYUP:
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (int j = 0; j < MAXLINES; j++)
        {
            for (int i = 0; i < MAXCOLS; i++)
            {
                char p = INDEX(j, i)->p_ch;

                if (p == ' ')
                    continue;
                if (p == '-' || p == '|')
                {
                    int neighborhood = 0;
                    if (INDEX(j - 1, i)->p_ch == '-' || INDEX(j - 1, i)->p_ch == '|')
                        neighborhood = 1;
                    if (INDEX(j, i + 1)->p_ch == '-' || INDEX(j, i + 1)->p_ch == '|')
                        neighborhood |= 2;
                    if (INDEX(j + 1, i)->p_ch == '-' || INDEX(j + 1, i)->p_ch == '|')
                        neighborhood |= 4;
                    if (INDEX(j, i - 1)->p_ch == '-' || INDEX(j, i - 1)->p_ch == '|')
                        neighborhood |= 8;
                    SDL_RenderCopy(renderer, sheet, wall_srcs + neighborhood, &(SDL_Rect){i * TILE_WIDTH, j * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT});
                }
                else
                {
                    int id = ok_map_get(&char_sprite_map, p) - 1;
                    SDL_RenderCopy(renderer, sheet, sprite_srcs + id, &(SDL_Rect){i * TILE_WIDTH, j * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT});
                }
            }
        }

        SDL_Rect rect;
        rect.x = hero.x * 9;
        rect.y = hero.y * 16;
        rect.w = 9;
        rect.h = 16;

        SDL_RenderPresent(renderer);
    }
}

/*
 * quit:
 *	Have player make certain, then exit.
 */

void quit(int sig)
{
    int oy, ox;

    NOOP(sig);

    /*
     * Reset the signal in case we got here via an interrupt
     */
    if (!q_comm)
        mpos = 0;
    getyx(curscr, oy, ox);
    msg("really quit?");
    if (readchar() == 'y')
    {
        signal(SIGINT, leave);
        clear();
        mvprintw(LINES - 2, 0, "You quit with %d gold pieces", purse);
        move(LINES - 1, 0);
        refresh();
        score(purse, 1, 0);
        my_exit(0);
    }
    else
    {
        move(0, 0);
        clrtoeol();
        status();
        move(oy, ox);
        refresh();
        mpos = 0;
        count = 0;
        to_death = FALSE;
    }
}

/*
 * leave:
 *	Leave quickly, but curteously
 */

void leave(int sig)
{
    static char buf[BUFSIZ];

    NOOP(sig);

    setbuf(stdout, buf); /* throw away pending output */

    if (!isendwin())
    {
        mvcur(0, COLS - 1, LINES - 1, 0);
        endwin();
    }

    putchar('\n');
    my_exit(0);
}

/*
 * my_exit:
 *	Leave the process properly
 */

void my_exit(int st)
{
    resetltchars();
    md_deinit();
    exit(st);
}
