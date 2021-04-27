/*
 * Rogue definitions and variable declarations
 *
 * @(#)rogue.h	5.42 (Berkeley) 08/06/83
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include "extern.h"
#include "dmalloc.h"

#undef lines 

#define NOOP(...) 0
#define CCHAR(x) ( (char) (x & A_CHARTEXT) )

#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE
#define FALSE false
#endif

#define hw (void *)0
// #define win (void *)0
#define WINDOW void

#define MAXINP 30



#define ERR 0

#define KEY_LEFT (char)16
#define KEY_RIGHT (char)0
#define KEY_UP (char)1
#define KEY_DOWN (char)2
#define KEY_HOME (char)3
#define KEY_PPAGE (char)4
#define KEY_NPAGE (char)5
#define KEY_END (char)6
#define KEY_A1 (char)7
#define KEY_A2 (char)8
#define KEY_A3 (char)9
#define KEY_B1 (char)10
#define KEY_B2 (char)11
#define KEY_B3 (char)12
#define KEY_C1 (char)13
#define KEY_C2 (char)14
#define KEY_C3 (char)15


/*
 * Maximum number of different things
 */
#define MAXROOMS	9
#define MAXTHINGS	9
#define MAXOBJ		9
#define MAXPACK		26      /* must be 26 (dependency on 'a'-'z') */
#define MAXTRAPS	10
#define AMULETLEVEL	26
#define	NUMTHINGS	7	/* number of types of things */
#define MAXPASS		13	/* upper limit on number of passages */
#define	NUMLINES	24
#define	NUMCOLS		80
#define STATLINE		(NUMLINES - 1)
#define BORE_LEVEL	50

/*
 * return values for get functions
 */
#define	NORM	0	/* normal exit */
#define	QUIT	1	/* quit option setting */
#define	MINUS	2	/* back up one option */

/*
 * inventory types
 */
#define	INV_OVER	0
#define	INV_SLOW	1
#define	INV_CLEAR	2

/*
 * All the fun defines
 */
#define when		break;case
#define otherwise	break;default
#define until(expr)	while(!(expr))
#define next(ptr)	(*ptr).l_next
#define prev(ptr)	(*ptr).l_prev
#define winat(y,x)	(moat(y,x) != NULL ? moat(y,x)->t_disguise : chat(y,x))
#define ce(a,b)		((a).x == (b).x && (a).y == (b).y)
#define hero		player.t_pos
#define pstats		player.t_stats
#define pack		player.t_pack
#define proom		player.t_room
#define max_hp		player.t_stats.s_maxhp
#undef max
#define max(a,b)	((a) > (b) ? (a) : (b))
#define on(thing,flag)	((bool)(((thing).t_flags & (flag)) != 0))
#define GOLDCALC	(rnd(50 + 10 * level) + 2)
#define ISRING(h,r)	(cur_ring[h] != NULL && cur_ring[h]->o_which == r)
#define ISWEARING(r)	(ISRING(LEFT, r) || ISRING(RIGHT, r))
#define ISMULT(type) 	(type == POTION || type == SCROLL || type == FOOD)
#define INDEX(y,x)	(&places[((x) << 5) + (y)])
#define chat(y,x)	(places[((x) << 5) + (y)].p_ch)
#define flat(y,x)	(places[((x) << 5) + (y)].p_flags)
#define moat(y,x)	(places[((x) << 5) + (y)].p_monst)
#define unc(cp)		(cp).y, (cp).x
#ifdef MASTER
#define debug		if (wizard) msg
#endif

/*
 * things that appear on the screens
 */
#define PASSAGE		'#'
#define DOOR		'+'
#define FLOOR		'.'
#define PLAYER		'@'
#define TRAP		'^'
#define STAIRS		'%'
#define GOLD		'*'
#define POTION		'!'
#define SCROLL		'?'
#define MAGIC		'$'
#define FOOD		':'
#define WEAPON		')'
#define ARMOR		']'
#define AMULET		','
#define RING		'='
#define STICK		'/'
#define CALLABLE	-1
#define R_OR_S		-2

/*
 * Various constants
 */
#define BEARTIME	spread(3)
#define SLEEPTIME	spread(5)
#define HOLDTIME	spread(2)
#define WANDERTIME	spread(70)
#define BEFORE		spread(1)
#define AFTER		spread(2)
#define HEALTIME	30
#define HUHDURATION	20
#define SEEDURATION	850
#define HUNGERTIME	1300
#define MORETIME	150
#define STOMACHSIZE	2000
#define STARVETIME	850
#define ESCAPE		27
#define LEFT		0
#define RIGHT		1
#define BOLT_LENGTH	6
#define LAMPDIST	3
#ifdef MASTER
#ifndef PASSWD
#define	PASSWD		"mTfolnRzp/33s" /* hicsuntleones */
#endif
#endif

/*
 * Version
 */
#define VERSION_5_3	'3'
#define VERSION_5_4	'4'
#define ISVERSION_5_3()	(rogue_version == VERSION_5_3)
#define ISVERSION_5_4()	(rogue_version == VERSION_5_4)

/*
 * Save against things
 */
#define VS_POISON	00
#define VS_PARALYZATION	00
#define VS_DEATH	00
#define VS_BREATH	02
#define VS_MAGIC	03

/*
 * Various flag bits
 */
/* flags for rooms */
#define ISDARK	0000001		/* room is dark */
#define ISGONE	0000002		/* room is gone (a corridor) */
#define ISMAZE	0000004		/* room is gone (a corridor) */

/* flags for objects */
#define ISCURSED 000001		/* object is cursed */
#define ISKNOW	0000002		/* player knows details about the object */
#define ISMISL	0000004		/* object is a missile type */
#define ISMANY	0000010		/* object comes in groups */
/*	ISFOUND 0000020		...is used for both objects and creatures */
#define	ISPROT	0000040		/* armor is permanently protected */
#define ISREVEAL 000100         /* vorpalized weapon: we know who the enemy is */

/* flags for creatures */
#define CANHUH	0000001		/* creature can confuse */
#define CANSEE	0000002		/* creature can see invisible creatures */
#define ISBLIND	0000004		/* creature is blind */
#define ISCANC	0000010		/* creature has special qualities cancelled */
#define ISLEVIT	0000010		/* hero is levitating */
#define ISFOUND	0000020		/* creature has been seen (used for objects) */
#define ISGREED	0000040		/* creature runs to protect gold */
#define ISHASTE	0000100		/* creature has been hastened */
#define ISTARGET 000200		/* creature is the target of an 'f' command */
#define ISHELD	0000400		/* creature has been held */
#define ISHUH	0001000		/* creature is confused */
#define ISINVIS	0002000		/* creature is invisible */
#define ISMEAN	0004000		/* creature can wake when player enters room */
#define ISHALU	0004000		/* hero is on acid trip */
#define ISREGEN	0010000		/* creature can regenerate */
#define ISRUN	0020000		/* creature is running at the player */
#define SEEMONST 040000		/* hero can detect unseen monsters */
#define ISFLY	0040000		/* creature can fly */
#define ISSLOW	0100000		/* creature has been slowed */

/*
 * Flags for level map
 */
#define F_PASS		0x80		/* is a passageway */
#define F_SEEN		0x40		/* have seen this spot before */
#define F_DROPPED	0x20		/* object was dropped here */
#define F_LOCKED	0x20		/* door is locked */
#define F_REAL		0x10		/* what you see is what you get */
#define F_PNUM		0x0f		/* passage number mask */
#define F_TMASK		0x07		/* trap number mask */

/*
 * Trap types
 */
#define T_DOOR	00
#define T_ARROW	01
#define T_SLEEP	02
#define T_BEAR	03
#define T_TELEP	04
#define T_DART	05
#define T_RUST  06 /* 5.4 only */
#define T_MYST  07 /* 5.4 only */
#define NTRAPS_5_3	6 /* NTRAPS version 5.3 */
#define NTRAPS_5_4	8 /* NTRAPS version 5.4 */
#define NTRAPS (ISVERSION_5_3()	? NTRAPS_5_3 : NTRAPS_5_4)

/*
 * Potion types
 */
#define P_CONFUSE	0
#define P_POT_1		1
#define P_PARALYZE_5_3	1
#define P_LSD_5_4	1
#define P_POISON	2
#define P_STRENGTH	3
#define P_SEEINVIS	4
#define P_HEALING	5
#define P_MFIND		6
#define	P_TFIND 	7
#define	P_RAISE		8
#define P_XHEAL		9
#define P_HASTE		10
#define P_RESTORE	11
#define P_BLIND		12
#define P_POT_13	13
#define P_NOP_5_3	13
#define P_LEVIT_5_4	13
#define MAXPOTIONS	14

/*
 * Scroll types
 */
#define S_CONFUSE	0
#define S_MAP		1
#define S_HOLD		2
#define S_SLEEP		3
#define S_ARMOR		4
#define S_SCARE		5
#define S_TELEP		6
#define S_ENCH		7
#define S_CREATE	8
#define S_REMOVE	9
#define S_AGGR		10
/* 5.3 */
#define S_IDENT_5_3	11
#define S_GFIND_5_3	12
#define S_NOP_5_3	13
#define S_VORPAL_5_3	14
#define S_NOP2_5_3	15 /* duplicate to fill to 18 - has zero probability */
#define S_NOP3_5_3	16 /* duplicate to fill to 18 - has zero probability */
#define S_NOP4_5_3	17 /* duplicate to fill to 18 - has zero probability */
/* 5.4 */
#define S_ID_POTION_5_4	11
#define S_ID_SCROLL_5_4	12
#define S_ID_WEAPON_5_4	13
#define S_ID_ARMOR_5_4	14
#define S_ID_R_OR_S_5_4	15
#define S_FDET_5_4	16
#define S_PROTECT_5_4	17
#define MAXSCROLLS	18

/*
 * Weapon types
 */
#define MACE		0
#define SWORD		1
#define BOW		2
#define ARROW		3
#define DAGGER		4
#define TWOSWORD	5
#define DART		6
/*
 * Shiraken is duplicated in 5.4 so to have same number of objects than in 5.3.
 * Duplicate entry has probability of zero.
 */
#define CROSSBOW_5_3	7
#define BOLT_5_3	8
#define SHIRAKEN_5_4	7
#define SHIRAKEN_DP_5_4	8
#define SPEAR		9
#define FLAME		10	/* fake entry for dragon breath (ick) */
#define MAXWEAPONS	10	/* this should equal FLAME */

/*
 * Armor types
 */
#define LEATHER		0
#define RING_MAIL	1
#define STUDDED_LEATHER	2
#define SCALE_MAIL	3
#define CHAIN_MAIL	4
#define SPLINT_MAIL	5
#define BANDED_MAIL	6
#define PLATE_MAIL	7
#define MAXARMORS	8

/*
 * Ring types
 */
#define R_PROTECT	0
#define R_ADDSTR	1
#define R_SUSTSTR	2
#define R_SEARCH	3
#define R_SEEINVIS	4
#define R_NOP		5
#define R_AGGR		6
#define R_ADDHIT	7
#define R_ADDDAM	8
#define R_REGEN		9
#define R_DIGEST	10
#define R_TELEPORT	11
#define R_STEALTH	12
#define R_SUSTARM	13
#define MAXRINGS	14

/*
 * Rod/Wand/Staff types
 */
#define WS_LIGHT	0
#define WS_ELECT	1
#define WS_SLOT_2	2
#define WS_HIT_5_3	2
#define WS_INVIS_5_4	2
#define WS_FIRE		3
#define WS_COLD		4
#define WS_POLYMORPH	5
#define WS_MISSILE	6
#define WS_HASTE_M	7
#define WS_SLOW_M	8
#define WS_DRAIN	9
#define WS_NOP		10
#define WS_TELAWAY	11
#define WS_TELTO	12
#define WS_CANCEL	13
#define MAXSTICKS	14

/*
 * Now we define the structures and types
 */

/*
 * Help list
 */
struct h_list {
    char h_ch;
    char *h_desc;
    bool h_print;
};

/*
 * Coordinate data type
 */
typedef struct {
    int x;
    int y;
} coord;

typedef unsigned int str_t;

/*
 * Stuff about objects
 */
struct obj_info {
    char *oi_name;
    int oi_prob;
    int oi_worth;
    char *oi_guess;
    bool oi_know;
};

/*
 * Room structure
 */
struct room {
    coord r_pos;			/* Upper left corner */
    coord r_max;			/* Size of room */
    coord r_gold;			/* Where the gold is */
    int r_goldval;			/* How much the gold is worth */
    short r_flags;			/* info about the room */
    int r_nexits;			/* Number of exits */
    coord r_exit[12];			/* Where the exits are */
};

/*
 * Structure describing a fighting being
 */
struct stats {
    str_t s_str;			/* Strength */
    int s_exp;				/* Experience */
    int s_lvl;				/* level of mastery */
    int s_arm;				/* Armor class */
    int s_hpt;				/* Hit points */
    char s_dmg[16];			/* String describing damage done */
    int  s_maxhp;			/* Max hit points */
};

/*
 * Structure for monsters and player
 */
union thing {
    struct {
	union thing *_l_next, *_l_prev;	/* Next pointer in link */
	coord _t_pos;			/* Position */
	bool _t_turn;			/* If slowed, is it a turn to move */
	char _t_type;			/* What it is */
	char _t_disguise;		/* What mimic looks like */
	char _t_oldch;			/* Character that was where it was */
	coord *_t_dest;			/* Where it is running to */
	short _t_flags;			/* State word */
	struct stats _t_stats;		/* Physical description */
	struct room *_t_room;		/* Current room for thing */
	union thing *_t_pack;		/* What the thing is carrying */
	int _t_reserved;
    } _t;
    struct {
	union thing *_l_next, *_l_prev;	/* Next pointer in link */
	int _o_type;			/* What kind of object it is */
	coord _o_pos;			/* Where it lives on the screen */
	char *_o_text;			/* What it says if you read it */
	int  _o_launch;			/* What you need to launch it */
	char _o_packch;			/* What character it is in the pack */
	char _o_damage[8];		/* Damage if used like sword */
	char _o_hurldmg[8];		/* Damage if thrown */
	int _o_count;			/* Count for plural objects */
	int _o_which;			/* Which object of a type it is */
	int _o_hplus;			/* Plusses to hit */
	int _o_dplus;			/* Plusses to damage */
	int _o_arm;			/* Armor protection */
	int _o_flags;			/* Information about objects */
	int _o_group;			/* group number for this object */
	char *_o_label;			/* Label for object */
	int _o_enemy;			/* Vorpalized Weapon enemy */
    } _o;
};

typedef union thing THING;

#define l_next		_t._l_next
#define l_prev		_t._l_prev
#define t_pos		_t._t_pos
#define t_turn		_t._t_turn
#define t_type		_t._t_type
#define t_disguise	_t._t_disguise
#define t_oldch		_t._t_oldch
#define t_dest		_t._t_dest
#define t_flags		_t._t_flags
#define t_stats		_t._t_stats
#define t_pack		_t._t_pack
#define t_room		_t._t_room
#define t_reserved      _t._t_reserved
#define o_type		_o._o_type
#define o_pos		_o._o_pos
#define o_text		_o._o_text
#define o_launch	_o._o_launch
#define o_packch	_o._o_packch
#define o_damage	_o._o_damage
#define o_hurldmg	_o._o_hurldmg
#define o_count		_o._o_count
#define o_which		_o._o_which
#define o_hplus		_o._o_hplus
#define o_dplus		_o._o_dplus
#define o_arm		_o._o_arm
#define o_charges	o_arm
#define o_goldval	o_arm
#define o_flags		_o._o_flags
#define o_group		_o._o_group
#define o_label		_o._o_label
#define o_enemy         _o._o_enemy

/*
 * describe a place on the level map
 */
typedef struct {
    char p_ch;
    char p_flags;
    THING *p_monst;
} PLACE;

/*
 * Array containing information on all the various types of monsters
 */
struct monster {
    char *m_name;			/* What to call the monster */
    int m_carry;			/* Probability of carrying something */
    short m_flags;			/* things about the monster */
    struct stats m_stats;		/* Initial stats */
};

/*
 * External variables
 */

extern bool	after, again, allscore, amulet, door_stop, fight_flush,
		firstmove, has_hit, inv_describe, jump, kamikaze,
		lower_msg, move_on, msg_esc, pack_used[],
		passgo, playing, q_comm, running, save_msg, see_floor,
		seenstairs, stat_msg, terse, to_death, tombstone,
                rookie_mode;

extern char	dir_ch, file_name[], home[], huh[], *inv_t_name[],
		l_last_comm, l_last_dir, last_comm, last_dir, *Numname,
		outbuf[], *p_colors[], *r_stones[], *release, runch,
		*s_names[], take, *tr_name[], *ws_made[], *ws_type[],
		rogue_version;

extern int	a_class[], count, food_left, hungry_state, inpack,
		inv_type, lastscore, level, max_hit, max_level, mpos,
		n_objs, no_command, no_food, no_move, noscore, ntraps, purse,
		quiet, vf_hit;

extern unsigned int	numscores;

extern int	dnum, e_levels[], seed;

// extern WINDOW	*hw;

extern coord	delta, oldpos, stairs;

extern PLACE	places[];

extern THING	*cur_armor, *cur_ring[], *cur_weapon, *l_last_pick,
		*last_pick, *lvl_obj, *mlist, player;

extern struct h_list	helpstr[];

extern struct room	*oldrp, passages[], rooms[];

extern struct stats	max_stats;

/* extern struct monster	monsters[]; */
extern struct monster *get_monsters();
#define monsters (get_monsters())

extern struct obj_info	arm_info[];
extern struct obj_info	things[];
extern struct obj_info	arm_info[];
extern struct obj_info *get_weap_info();
#define weap_info (get_weap_info())
extern struct obj_info *get_pot_info();
#define pot_info (get_pot_info())
extern struct obj_info *get_scr_info();
#define scr_info (get_scr_info())
extern struct obj_info *get_ws_info();
#define ws_info (get_ws_info())
extern struct obj_info	ring_info[]; /* TODO */

/*
 * Function types
 */
void	attach(THING **list, THING *item);
void	detach(THING **list, THING *item);
void	free_list(THING **ptr);
void	addmsg(char *fmt, ...);
bool	add_haste(bool potion);
void	add_pack(THING *obj, bool silent);
void	add_pass();
void	add_str(str_t *sp, int amt);
void	accnt_maze(int y, int x, int ny, int nx);
void	aggravate();
int	attack(THING *mp);
void	badcheck(char *name, struct obj_info *info, int bound);
void	bounce(THING *weap, char *mname, bool noend);
void	call();
void	call_it(struct obj_info *info);
void    make_known(THING *obj);
bool	cansee(int y, int x);
int	center(char *str);
void	chg_str(int amt);
void	check_level();
void	conn(int r1, int r2);
void	command(char);
void	create_obj();
int     pick_mons();

void	current(THING *cur, char *how, char *where);
void	d_level();
void	death(char monst);
char	death_monst();
void	dig(int y, int x);
void	discard(THING *item);
void	discovered();
int	dist(int y1, int x1, int y2, int x2);
int	dist_cp(coord *c1, coord *c2);
int	do_chase(THING *th);
void	do_daemons(int flag);
void	do_fuses(int flag);
void	do_maze(struct room *rp);
void	do_motion(THING *obj, int ydelta, int xdelta);
void	do_move(int dy, int dx);
void	do_passages();
void	do_pot(int type, bool knowit);
void	do_rooms();
void	populate_rooms();
void	do_run(char ch);
void	do_zap();
void	doadd(char *fmt, va_list args);
void	door(struct room *rm, coord *cp);
void	door_open(struct room *rp);
void	drain();
void	draw_room(struct room *rp);
void	drop();
void	eat();
size_t  encread(char *start, size_t size, FILE *inf);
size_t	encwrite(char *start, size_t size, FILE *outf);
int	endmsg();
void	enter_room(coord *cp);
void	erase_lamp(coord *pos, struct room *rp);
int	exp_add(THING *tp);
void	extinguish(void (*func)());
void	fall(THING *obj, bool pr);
void	fire_bolt(coord *start, coord *dir, char *name);
char	floor_at();
void	flush_type();
int	fight(coord *mp, THING *weap, bool thrown);
int	fight_at(coord *mp, THING *weap, bool thrown, coord *delta);
void	fix_stick(THING *cur);
void	fuse(void (*func)(), int arg, int time, int type);
bool	get_dir();
int	gethand();
void	give_pack(THING *tp);
void	help();
void	hit(char *er, char *ee, bool noend);
void	horiz(struct room *rp, int starty);
void	leave_room(coord *cp);
void	lengthen(void (*func)(), int xtime);
void	look(bool wakeup);
int	hit_monster(int y, int x, THING *obj);
void	identify();
bool    is_trap(coord *tc);
void	illcom(int ch);
void	init_check();
void	init_colors();
void	init_materials();
void	init_names();
void	init_player();
void	init_probs();
void	init_stones();
void	init_weapon(THING *weap, int which);
bool	inventory(THING *list, int type);
void	check_inventory(THING *list);
void	invis_on();
void	killed(THING *tp, bool pr);
void	kill_daemon(void (*func)());
bool	lock_sc();
void	miss(char *er, char *ee, bool noend);
void	missile(int ydelta, int xdelta);
void    th_effect(THING *obj, THING *tp);
void	money(int value);
int	move_monst(THING *tp);
void	move_msg(THING *obj);
int	msg(char *fmt, ...);
void    fatal(char *fmt, ...);
void	nameit(THING *obj, char *type, char *which, struct obj_info *op, char *(*prfunc)(THING *));
void	new_level();
void	new_monster(THING *tp, char type, coord *cp);
void	numpass(int y, int x);
void	option();
void	open_score();
void	parse_opts(char *str);
void 	passnum();
char	*pick_color(char *col);
int	pick_one(struct obj_info *info, int nitems);
void	pick_up(char ch);
void	picky_inven();
void	pr_spec(struct obj_info *info, int nitems);
void	pr_list();
void	put_bool(void *b);
void	put_inv_t(void *ip);
void	put_str(void *str);
void	put_things();
void    put_amulet();
void	putpass(coord *cp);
void	quaff();
void	raise_level();
char	randmonster(bool wander);
void	read_scroll();
void    relocate(THING *th, coord *new_loc);
void	remove_mon(coord *mp, THING *tp, bool waskill);
void	reset_last();
bool	restore(char *file, char **envp);
int	ring_eat(int hand);
void	ring_on();
void	ring_off();
int	rnd(int range);
int	rnd_room();
int	roll(int number, int sides);
int	rs_save_file(FILE *savef);
int	rs_restore_file(FILE *inf);
void	runto(coord *runner);
void	rust_armor(THING *arm);
int	save(int which);
void	save_file(FILE *savef);
void	save_game();
int	save_throw(int which, THING *tp);
void	score(int amount, int flags, char monst);
void	search();
void	set_know(THING *obj, struct obj_info *info);
void	set_oldch(THING *tp, coord *cp);
void	setup();
void	shell();
bool	show_floor();
void	show_map();
//void	show_win(char *message);
int	sign(int nm);
int	spread(int nm);
void	start_daemon(void (*func)(), int arg, int type);
void	start_score();
void	status();
int	step_ok(int ch);
void	strucpy(char *s1, char *s2, int len);
int	swing(int at_lvl, int op_arm, int wplus);
void	take_off();
void	teleport();
void	total_winner();
void	thunk(THING *weap, char *mname, bool noend);
void	treas_room();
void	turnref();
void	u_level();
void	uncurse(THING *obj);
void	unlock_sc();
void	vert(struct room *rp, int startx);
void	wait_for(int ch);
THING  *wake_monster(int y, int x);
void	wanderer();
void	waste_time();
void	wear();
void	whatis(bool insist, int type);
void	wield();

bool	chase(THING *tp, coord *ee);
bool	diag_ok(coord *sp, coord *ep);
bool	dropcheck(THING *obj);
bool	fallpos(coord *pos, coord *newpos);
#define NOLIMIT                 0
#define MAXTRIES              900
#define MAXTRIES_TREAS        200
#define FF_NOTRAP             0x1       /* do not set coordinates on a trap */
#define FF_NOHERO             0x2       /* do not set coordinates on hero */
bool	find_floor(struct room *rp, coord *cp, int limit, bool monst, int flags);
bool	is_magic(THING *obj);
bool    is_symlink(char *sp); 
bool	levit_check();
bool	pack_room(bool from_floor, THING *obj, bool need_pack_char, bool silent);
bool	roll_em(THING *thatt, THING *thdef, THING *weap, bool hurl);
bool	see_monst(THING *mp);
bool	seen_stairs();
bool	turn_ok(int y, int x);
bool	turn_see(bool turn_off);
bool	is_current(THING *obj);
int	passwd();

char	be_trapped(coord *tc);
char	floor_ch();
char	pack_char();
char	readchar();
char	rnd_thing();

char	*charge_str(THING *obj);
char	*choose_str(char *ts, char *ns);
char	*inv_name(THING *obj, bool drop);
char	*nullstr(THING *ignored);
char	*num(int n1, int n2, char type);
char	*ring_num(THING *obj);
char	*set_mname(THING *tp);
char	*vowelstr(char *str);

// int	get_bool(void *vp, WINDOW *win);
// int	get_inv_t(void *vp, WINDOW *win);
// int	get_num(void *vp, WINDOW *win);
// int	get_sf(void *vp, WINDOW *win);
// int	get_str(void *vopt, WINDOW *win);

#define get_bool NULL
#define get_inv_t NULL
#define get_num NULL
#define get_sf NULL
#define get_str NULL

int	trip_ch(int y, int x, int ch);

coord	*find_dest(THING *tp);
coord	*rndmove(THING *who);

THING	*find_obj(int y, int x);
THING	*get_item(char *purpose, int type);
THING	*leave_pack(THING *obj, bool all);
THING	*new_item();
THING	*new_thing(bool initial_object);

struct room	*roomin(coord *cp);

#define MAXDAEMONS 20

extern struct delayed_action {
    int d_type;
    void (*d_func)();
    int d_arg;
    int d_time;
} d_list[MAXDAEMONS];

typedef struct {
    char	*st_name;
    int		st_value;
} STONE;

extern int      total;
extern int      between;
extern int      group;
extern coord    nh;
extern char     *rainbow[];
extern int      cNCOLORS;
extern STONE    stones[];
extern int      cNSTONES;
extern char     *wood[];
extern int      cNWOOD;
extern char     *metal[];
extern int      cNMETAL;