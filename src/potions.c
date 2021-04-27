/*
 * Function(s) for dealing with potions
 *
 * @(#)potions.c	4.46 (Berkeley) 06/07/83
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

// #include <curses.h>
#include <ctype.h>
#include "rogue.h"

typedef struct
{
    int pa_flags;
    void (*pa_daemon)();
    int pa_time;
    char *pa_high, *pa_straight;
} PACT;

static PACT p_actions[] =
{
	{ ISHUH,	unconfuse,	HUHDURATION,	/* P_CONFUSE */
		"what a tripy feeling!",
		"wait, what's going on here. Huh? What? Who?" },
	/*
	 * This PACT is only used by version 5.4 (P_LSD - hallucination).
	 */
	{ ISHALU,	come_down,	SEEDURATION,	/* P_LSD */
		"Oh, wow!  Everything seems so cosmic!",
		"Oh, wow!  Everything seems so cosmic!" },
	{ 0,		NULL,	0 },			/* P_POISON */
	{ 0,		NULL,	0 },			/* P_STRENGTH */
	{ CANSEE,	unsee,	SEEDURATION,		/* P_SEEINVIS */
		prbuf,
		prbuf },
	{ 0,		NULL,	0 },			/* P_HEALING */
	{ 0,		NULL,	0 },			/* P_MFIND */
	{ 0,		NULL,	0 },			/* P_TFIND  */
	{ 0,		NULL,	0 },			/* P_RAISE */
	{ 0,		NULL,	0 },			/* P_XHEAL */
	{ 0,		NULL,	0 },			/* P_HASTE */
	{ 0,		NULL,	0 },			/* P_RESTORE */
	{ ISBLIND,	sight,	SEEDURATION,		/* P_BLIND */
		"oh, bummer!  Everything is dark!  Help!",
		"a cloak of darkness falls around you" },
	/*
	 * This PACT is only used by version 5.4 (P_LEVIT - levitation).
	 */
	{ ISLEVIT,	land,	HEALTIME,		/* P_LEVIT */
		"oh, wow!  You're floating in the air!",
		"you start to float in the air" }
};

/*
 * quaff:
 *	Quaff a potion from the pack
 */

void
quaff()
{
    THING *obj, *tp, *mp;
    bool show, trip;

    obj = get_item("quaff", POTION);
    /*
     * Make certain that it is somethings that we want to drink
     */
    if (obj == NULL)
	return;
    if (obj->o_type != POTION)
    {
	if (!terse)
	    msg("yuk! Why would you want to drink that?");
	else
	    msg("that's undrinkable");
	return;
    }
    if (obj == cur_weapon)
	cur_weapon = NULL;

    /*
     * Calculate the effect it has on the poor guy.
     */
    trip = on(player, ISHALU);
    obj = leave_pack(obj, FALSE);
    switch (obj->o_which)
    {
	case P_CONFUSE:
	    do_pot(P_CONFUSE, !trip);
	when P_POISON:
	    pot_info[P_POISON].oi_know = TRUE;
	    if (ISWEARING(R_SUSTSTR))
		msg("you feel momentarily sick");
	    else
	    {
		chg_str(-(rnd(3) + 1));
		msg("you feel very sick now");
		come_down();
	    }
	when P_HEALING:
	    pot_info[P_HEALING].oi_know = TRUE;
	    if ((pstats.s_hpt += roll(pstats.s_lvl, 4)) > max_hp)
		pstats.s_hpt = ++max_hp;
	    sight();
	    msg("you begin to feel better");
	when P_STRENGTH:
	    pot_info[P_STRENGTH].oi_know = TRUE;
	    chg_str(1);
	    msg("you feel stronger, now.  What bulging muscles!");
	when P_MFIND:
	    player.t_flags |= SEEMONST;
	    fuse((void(*)())turn_see, TRUE, HUHDURATION, AFTER);
	    if (!turn_see(FALSE))
		msg("you have a %s feeling for a moment, then it passes",
		    choose_str("normal", "strange"));
	when P_TFIND:
	    /*
	     * Potion of magic detection.  Show the potions and scrolls
	     */
	    show = FALSE;
	    if (lvl_obj != NULL)
	    {
		wclear(hw);
		for (tp = lvl_obj; tp != NULL; tp = next(tp))
		{
		    if (is_magic(tp))
		    {
			show = TRUE;
			wmove(hw, tp->o_pos.y, tp->o_pos.x);
			waddch(hw, MAGIC);
			pot_info[P_TFIND].oi_know = TRUE;
		    }
		}
		for (mp = mlist; mp != NULL; mp = next(mp))
		{
		    for (tp = mp->t_pack; tp != NULL; tp = next(tp))
		    {
			if (is_magic(tp))
			{
			    show = TRUE;
			    wmove(hw, mp->t_pos.y, mp->t_pos.x);
			    waddch(hw, MAGIC);
			}
		    }
		}
	    }
	    if (show)
	    {
		pot_info[P_TFIND].oi_know = TRUE;
		show_win("You sense the presence of magic on this level.--More--");
	    }
	    else
		msg("you have a %s feeling for a moment, then it passes",
		    choose_str("normal", "strange"));
	when P_POT_1:
	/* version 5.3: P_PARALYZE_5_3 */
	/* version 5.4: P_LSD_5_4 */
            if (ISVERSION_5_3()) {
	        /* version 5.3: P_PARALYZE_5_3 */
		player.t_flags &= ~ISRUN;
	        no_command += HOLDTIME;
		msg("you can't move");
	    } else {
	       /* version 5.4: P_LSD_5_4 */
	       if (!trip) {
		   if (on(player, SEEMONST))
		       turn_see(FALSE);
		   start_daemon(visuals, 0, BEFORE);
		   seenstairs = seen_stairs();
	       }
	       do_pot(P_LSD_5_4, TRUE);
	    }
	when P_SEEINVIS:
	    sprintf(prbuf, "this potion tastes like %s juice", fruit);
	    show = on(player, CANSEE);
	    do_pot(P_SEEINVIS, FALSE);
	    if (!show)
		invis_on();
	    sight();
	when P_RAISE:
	    pot_info[P_RAISE].oi_know = TRUE;
	    msg("you suddenly feel much more skillful");
	    raise_level();
	when P_XHEAL:
	    pot_info[P_XHEAL].oi_know = TRUE;
	    if ((pstats.s_hpt += roll(pstats.s_lvl, 8)) > max_hp)
	    {
		if (pstats.s_hpt > max_hp + pstats.s_lvl + 1)
		    ++max_hp;
		pstats.s_hpt = ++max_hp;
	    }
	    sight();
	    come_down();
	    msg("you begin to feel much better");
	when P_HASTE:
	    pot_info[P_HASTE].oi_know = TRUE;
	    after = FALSE;
	    if (add_haste(TRUE))
		msg("you feel yourself moving much faster");
	when P_RESTORE:
	    if (ISRING(LEFT, R_ADDSTR))
		add_str(&pstats.s_str, -cur_ring[LEFT]->o_arm);
	    if (ISRING(RIGHT, R_ADDSTR))
		add_str(&pstats.s_str, -cur_ring[RIGHT]->o_arm);
	    if (pstats.s_str < max_stats.s_str)
		pstats.s_str = max_stats.s_str;
	    if (ISRING(LEFT, R_ADDSTR))
		add_str(&pstats.s_str, cur_ring[LEFT]->o_arm);
	    if (ISRING(RIGHT, R_ADDSTR))
		add_str(&pstats.s_str, cur_ring[RIGHT]->o_arm);
	    msg("hey, this tastes great.  It make you feel warm all over");
	when P_BLIND:
	    do_pot(P_BLIND, TRUE);
        when P_POT_13:
	/* version 5.3: P_NOP_5_3 */
	/* version 5.4: P_LEVIT_5_4 */
            if (ISVERSION_5_3()) {
		/* version 5.3: P_NOP_5_3 */
		msg("this potional tastes extremely dull");
	    } else {
		/* version 5.4: P_LEVIT_5_4 */
	        do_pot(P_LEVIT_5_4, TRUE);
	    }
	otherwise:
	    fatal("what an odd tasting potion #%d!", obj->o_which);
	    return;
    }
    status();
    /*
     * Throw the item away
     */
    call_it(&pot_info[obj->o_which]);
    discard(obj);
}

/*
 * is_magic:
 *	Returns true if an object radiates magic
 */
bool
is_magic(THING *obj)
{
    switch (obj->o_type)
    {
	case ARMOR:
	    return (bool)((obj->o_flags&ISPROT) || obj->o_arm != a_class[obj->o_which]);
	case WEAPON:
	    return (bool)(obj->o_hplus != 0 || obj->o_dplus != 0);
	case POTION:
	case SCROLL:
	case STICK:
	case RING:
	case AMULET:
	    return TRUE;
    }
    return FALSE;
}

/*
 * invis_on:
 *	Turn on the ability to see invisible
 */

void
invis_on()
{
    THING *mp;

    player.t_flags |= CANSEE;
    for (mp = mlist; mp != NULL; mp = next(mp))
	if (on(*mp, ISINVIS) && see_monst(mp) && !on(player, ISHALU))
	    mvaddch(mp->t_pos.y, mp->t_pos.x, mp->t_disguise);
}

/*
 * turn_see:
 *	Put on or off seeing monsters on this level
 */
bool
turn_see(bool turn_off)
{
    THING *mp;
    bool can_see, add_new;

    add_new = FALSE;
    for (mp = mlist; mp != NULL; mp = next(mp))
    {
	move(mp->t_pos.y, mp->t_pos.x);
	can_see = see_monst(mp);
	if (turn_off)
	{
	    if (!can_see)
		addch(mp->t_oldch);
	}
	else
	{
	    if (!can_see)
		standout();
	    if (!on(player, ISHALU))
		addch(mp->t_type);
	    else
		addch(rnd(26) + 'A');
	    if (!can_see)
	    {
		standend();
		add_new++;
	    }
	}
    }
    if (turn_off)
	player.t_flags &= ~SEEMONST;
    else
	player.t_flags |= SEEMONST;
    return add_new;
}

/*
 * seen_stairs:
 *	Return TRUE if the player has seen the stairs
 */
bool
seen_stairs()
{
    THING	*tp;

    move(stairs.y, stairs.x);
    if (inch() == STAIRS)			/* it's on the map */
	return TRUE;
    if (ce(hero, stairs))			/* It's under him */
	return TRUE;

    /*
     * if a monster is on the stairs, this gets hairy
     */
    if ((tp = moat(stairs.y, stairs.x)) != NULL)
    {
	if (see_monst(tp) && on(*tp, ISRUN))	/* if it's visible and awake */
	    return TRUE;			/* it must have moved there */

	if (on(player, SEEMONST)		/* if she can detect monster */
	    && tp->t_oldch == STAIRS)		/* and there once were stairs */
		return TRUE;			/* it must have moved there */
    }
    return FALSE;
}

/*
 * raise_level:
 *	The guy just magically went up a level.
 */

void
raise_level()
{
    pstats.s_exp = e_levels[pstats.s_lvl-1] + 1L;
    check_level();
}

/*
 * do_pot:
 *	Do a potion with standard setup.  This means it uses a fuse and
 *	turns on a flag
 */

void
do_pot(int type, bool knowit)
{
    PACT *pp;
    int t;

    pp = &p_actions[type];
    if (!pot_info[type].oi_know)
	pot_info[type].oi_know = knowit;
    t = spread(pp->pa_time);
    if (!on(player, pp->pa_flags))
    {
	player.t_flags |= pp->pa_flags;
	fuse(pp->pa_daemon, 0, t, AFTER);
	look(FALSE);
    }
    else
	lengthen(pp->pa_daemon, t);
    msg(choose_str(pp->pa_high, pp->pa_straight));
}

/*
 * th_effect:
 *	Compute the effect of this potion hitting a monster.
 *	Only called in case of 5.3 version.
 */
void
th_effect(THING *obj, THING *tp)
{
    switch (obj->o_which)
    {
	when P_CONFUSE:
	case P_BLIND:
	    tp->t_flags |= ISHUH;
	    msg("the %s appears confused", monsters[tp->t_type-'A'].m_name);
	when P_PARALYZE_5_3:
	    tp->t_flags &= ~ISRUN;
	    tp->t_flags |= ISHELD;
	when P_HEALING:
	case P_XHEAL:
	    if ((tp->t_stats.s_hpt += rnd(8)) > tp->t_stats.s_maxhp)
		tp->t_stats.s_hpt = ++tp->t_stats.s_maxhp;
	when P_RAISE:
		tp->t_stats.s_hpt += 8;
		tp->t_stats.s_maxhp += 8;
		tp->t_stats.s_lvl++;
	when P_HASTE:
		tp->t_flags |= ISHASTE;
    }
}
