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

    map_str_int sprite_id_map;
    ok_map_init_with_capacity(&sprite_id_map, 42);
    for (int i = 0; i < 42; ++i)
    {
        ok_map_put(&sprite_id_map, sprite_names[i], i + 1);
    }

    //printf(">> %d, %d\n",PLAYER, ok_map_get(&sprite_id_map, "PLAYER")-1 );

    char map_chars[] = ".#%+^@*)]:?/=!";
    map_int_int char_sprite_map;
    ok_map_init_with_capacity(&char_sprite_map, 14);
    for (int i = 0; i < 14; ++i)
    {
        ok_map_put(&char_sprite_map, map_chars[i], i + 1);
    }

    IMG_Init(IMG_INIT_PNG);

    SDL_Texture *sheet = IMG_LoadTexture(renderer, "Assets/loveable_rogue_sheet.png");
    SDL_Rect wall_srcs[16];
    SDL_Rect sprite_srcs[42];
    SDL_Rect map_wall_srcs[16];
    SDL_Rect map_icon_srcs[14];
    font_data FD;
    //map_str_int font_colors_map; for this we would have to save the names of the color blocks too...

    SDL_Rect AMULET_src = (SDL_Rect){0, 0, 0, 0};
    SDL_Rect LOGO_src = (SDL_Rect){0, 0, 0, 0};
    SDL_Rect RIP_src = (SDL_Rect){0, 0, 0, 0};
    SDL_Rect GRAVE_src = (SDL_Rect){0, 0, 0, 0};
    SDL_Rect GRAVE_ORNAMENTS_srcs[3];
    for (int i = 0; i < 3; ++i)
        GRAVE_ORNAMENTS_srcs[i] = (SDL_Rect){0, 0, 0, 0};

    FILE *f = fopen("assets/lovable-rogue-data.txt", "r");

    if (f == NULL)
    {
        perror("Can't open lovable");
        exit(1);
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        perror("SDL_Init Failed");
        exit(1);
    }

    if (TTF_Init() < 0)
    {
        perror("TTF_Init Failed");
        exit(1);
    }

    gFont = TTF_OpenFont("dos.ttf", 16);
    if (gFont == NULL)
    {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
        exit(1);
    }

    // Create an application window with the following settings:
    SDL_CreateWindowAndRenderer(80 * 9, 24 * 16 + 4 * 16, 0, &window, &renderer);
    SDL_SetWindowTitle(window, PACKAGE_STRING);

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

                command(tolower((char) *SDL_GetKeyName(event.key.keysym.sym)));
                break;
            case SDL_KEYUP:
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (int i = 0; i < MAXLINES; i++)
        {
            for (int j = 0; j < MAXCOLS; j++)
            {
                char p = INDEX(i, j)->p_ch;

                if (INDEX(i, j)->p_flags & F_REAL == 0)
                    continue;

                SDL_Rect rect;
                rect.x = j * 9;
                rect.y = i * 16;
                rect.w = 9;
                rect.h = 16;

                SDL_RenderCopy(renderer, surfaceForChar(p), NULL, &rect);
            }
        }

        SDL_Rect rect;
        rect.x = hero.x * 9;
        rect.y = hero.y * 16;
        rect.w = 9;
        rect.h = 16;

        SDL_RenderCopy(renderer, surfaceForChar('@'), NULL, &rect);

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
