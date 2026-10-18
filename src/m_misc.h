// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//
//    
//-----------------------------------------------------------------------------


#ifndef __M_MISC__
#define __M_MISC__


#include "doomtype.h"
//
// MISC
//
#define KEY_START 4
#define SENSITIVITY_START (KEY_START + 12)
#define LIQUID_MODE_INDEX (31)

extern int	key_right;
extern int	key_left;
extern int	key_up;
extern int	key_down;

extern int	key_strafeleft;
extern int	key_straferight;

extern int	key_fire;
extern int	key_use;
extern int	key_strafe;
extern int	key_speed;

extern int	mousebfire;
extern int	mousebstrafe;
extern int	mousebforward;

extern int	joybfire;
extern int	joybstrafe;
extern int	joybuse;
extern int	joybspeed;

extern int	viewwidth;
extern int	viewheight;

extern int	mouseSensitivity;
extern int	showMessages;

extern int	detailLevel;

extern int	screenblocks;
extern int liquidwarping;

extern int	showMessages;

typedef struct
{
    char* name;
    int* location;
    int		defaultvalue;
    int		scantranslate;		// PC scan code hack
    int		untranslated;		// lousy hack
} default_t;
extern default_t	defaults[];

boolean
M_WriteFile
( char const*	name,
  void*		source,
  int		length );

int
M_ReadFile
( char const*	name,
  byte**	buffer );

void M_ScreenShot (void);

void M_LoadDefaults (void);

void M_SaveDefaults (void);


int
M_DrawText
( int		x,
  int		y,
  boolean	direct,
  char*		string );


#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
