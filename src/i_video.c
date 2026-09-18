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
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <io.h>

#include "i_system.h"
#include <stdarg.h>

#include <sys/types.h>

#include <errno.h>
#include <signal.h>

#include "doomstat.h"

#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

#define POINTER_WARP_COUNTDOWN	1

SDL_Color colors[256];
 SDL_Surface*	mainSurface;
 SDL_Surface* imageSurface;

 SDL_Surface* UISurface;
 byte UI_image[320 * 200];
 SDL_Window*		mainWindow;
 SDL_Event		mainEvent;
 SDL_Renderer* mainRenderer;
 SDL_Texture* renderTexture;
//Colormap	X_cmap;
//Visual*		X_visual;
//SDL_Context		X_gc;

int		X_screen;
//XVisualInfo	X_visualinfo;
//XImage*		image;
int		X_width;
int		X_height;

// MIT SHared Memory extension.
bool		doShm;

//XShmSegmentInfo	X_shminfo;
int		X_shmeventtype;

// Fake mouse handling.
// This cannot work properly w/o DGA.
// Needs an invisible mouse cursor at least.
bool		grabMouse;
int		doPointerWarp = POINTER_WARP_COUNTDOWN;

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int	multiply=1;


//
//  Translates the key currently in X_event
//
SDL_Window* I_GetWindow(void)
{
    return mainWindow;
}
SDL_Surface* I_GetRenderSurface(void)
{
    return imageSurface;
}

int xlatekey(void)
{

    int rc;
rc=SDL_GetKeyName( mainEvent.key.key)[0];
    switch( mainEvent.key.key)
    {
      case SDLK_LEFT:	rc = KEY_LEFTARROW; break;
      case SDLK_RIGHT:	rc = KEY_RIGHTARROW;	break;
      case SDLK_DOWN:	rc = KEY_DOWNARROW;	break;
      case SDLK_UP:	rc = KEY_UPARROW;	break;
      case SDLK_ESCAPE:	rc = KEY_ESCAPE;	break;
      case SDLK_RETURN:	rc = KEY_ENTER;		break;
      case SDLK_TAB:	rc = KEY_TAB;		break;
      case SDLK_F1:	rc = KEY_F1;		break;
      case SDLK_F2:	rc = KEY_F2;		break;
      case SDLK_F3:	rc = KEY_F3;		break;
      case SDLK_F4:	rc = KEY_F4;		break;
      case SDLK_F5:	rc = KEY_F5;		break;
      case SDLK_F6:	rc = KEY_F6;		break;
      case SDLK_F7:	rc = KEY_F7;		break;
      case SDLK_F8:	rc = KEY_F8;		break;
      case SDLK_F9:	rc = KEY_F9;		break;
      case SDLK_F10:	rc = KEY_F10;		break;
      case SDLK_F11:	rc = KEY_F11;		break;
      case SDLK_F12:	rc = KEY_F12;		break;

      case SDLK_BACKSPACE: rc = KEY_BACKSPACE;	break;
      case SDLK_DELETE:	rc = KEY_BACKSPACE;	break;

      case SDLK_PAUSE:	rc = KEY_PAUSE;		break;

      case SDLK_KP_EQUALS: rc = KEY_EQUALS;	break;
      case SDLK_EQUALS:	rc = KEY_EQUALS;	break;

      case SDLK_KP_MINUS: rc = KEY_MINUS;		break;
      case SDLK_MINUS:	rc = KEY_MINUS;		break;
          case SDLK_SPACE:
        rc =' ';
        break;
      case SDLK_LSHIFT: rc = KEY_RSHIFT; break;
      case SDLK_RSHIFT:
	rc = KEY_RSHIFT;
	break;

      case SDLK_LCTRL:
          rc = KEY_RCTRL;
	break;
      case SDLK_RCTRL:
	rc = KEY_RCTRL;
	break;

      case SDLK_LALT:
          rc = KEY_RALT;
	    break;
    case SDLK_RALT:
	rc = KEY_RALT;
	break;
//	case SDLK_SPACE: KEY_UPARROW break;

      default:

	if (rc >= SDLK_SPACE && rc <= SDLK_TILDE)
	    rc = rc - SDLK_SPACE + ' ';
	if (rc >= 'A' && rc <= 'Z')
	    rc = rc - 'A' + 'a';
        break;
    }
    return rc;

}

void I_ShutdownGraphics(void)
{

  // Paranoia.
  //image->data = NULL;
  SDL_DestroyWindow(mainWindow);
  SDL_DestroyRenderer(mainRenderer);

}



//
// I_StartFrame
//
void I_StartFrame (void)
{
    // er?

}

static int	lastmousex = 0;
static int	lastmousey = 0;
bool		mousemoved = false;
bool		shmFinished;

void I_GetEvent(void)
{

    while (SDL_PollEvent(&mainEvent))
    {
        event_t event;
        // put event-grabbing stuff in here
        //XNextEvent(X_display, &X_event);
        switch (mainEvent.type)
        {
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            {
                I_Quit();
                break;
            }

            case SDL_EVENT_KEY_DOWN:
            {
                event.type = ev_keydown;
                event.data1 = xlatekey();//mainEvent.key.scancode;
                D_PostEvent(&event);
                // fprintf(stderr, "k");
                break;
            }
            case SDL_EVENT_KEY_UP:
            {
                event.type = ev_keyup;
                event.data1 = xlatekey();// mainEvent.key.scancode;
                D_PostEvent(&event);
                // fprintf(stderr, "ku");
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                event.type = ev_mouse;
                int mbuttons = SDL_GetMouseState(NULL, NULL);
                event.data1 = mbuttons;
              //      (mainEvent.button.button & SDL_BUTTON_LMASK)
              //               | (mainEvent.button.button & SDL_BUTTON_RMASK? 2 : 0)
              //               | (mainEvent.button.button & SDL_BUTTON_MMASK ? 4 : 0);
                event.data2 = event.data3 = 0;

                D_PostEvent(&event);

                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                event.type = ev_mouse;
                int mbuttons = SDL_GetMouseState(NULL, NULL);
                event.data1 = mbuttons;
                //      (mainEvent.button.button & SDL_BUTTON_LMASK)
                //               | (mainEvent.button.button & SDL_BUTTON_RMASK? 2 : 0)
                //               | (mainEvent.button.button & SDL_BUTTON_MMASK ? 4 : 0);
                event.data2 = event.data3 = 0;
                D_PostEvent(&event);

                break;
            }

            case SDL_EVENT_MOUSE_MOTION:
            {
                event.type = ev_mouse;
                event.data1 =
                    (mainEvent.motion.state & SDL_BUTTON_LMASK)
                    | (mainEvent.motion.state & SDL_BUTTON_RMASK ? 2 : 0)
                    | (mainEvent.motion.state & SDL_BUTTON_MMASK ? 4 : 0);
                //   event.data1 =
              //         (mainEvent.motion.state & SDL_BUTTON_LMASK)
              //         | (mainEvent.motion.state & SDL_BUTTON_RMASK? 2 : 0)
             //          | (mainEvent.motion.state & SDL_BUTTON_MMASK ? 4 : 0);
                 //  event.data2 = (mainEvent.motion.x - lastmousex);//<< 2;
                 //  event.data3 = (mainEvent.motion.y - lastmousey);//<< 2;

                event.data2 = (int)(mainEvent.motion.xrel)<<4;//<< 2;
                event.data3 = (int)(-mainEvent.motion.yrel)<<4;//<< 2;
                D_PostEvent(&event);

             //   float mouseX, mouseY;
             //   SDL_GetRelativeMouseState(&mouseX, &mouseY);
            //    event.data2 = mouseX;
             //       event.data3 = mouseY;
                /*
                if (event.data2 || event.data3)
                {
                    mousemoved = true;
                    D_PostEvent(&event);


                }
                else
                {
                    event.data2 = event.data3 = 0;

                    mousemoved = false;
                }
                */
                break;
            }


        }

    }
}

//
// I_StartTic
//
void I_StartTic (void)
{

    //if (!X_display)
	//return;

   // while (SDL_WaitEvent(&mainEvent))
	I_GetEvent();

    // Warp the pointer back to the middle of the window
    //  or it will wander off - that is, the game will
    //  loose input focus within X11.
    if (grabMouse)
    {
	if (!--doPointerWarp)
	{
		/*
	    XWarpPointer( X_display,
			  None,
			  X_mainWindow,
			  0, 0,
			  0, 0,
			  X_width/2, X_height/2);
			  */
	    doPointerWarp = POINTER_WARP_COUNTDOWN;
	}
    }

    mousemoved = false;

}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{

    static int	lasttic;
    int		tics;
    int		i;
    //int j;
    // UNUSED static unsigned char *bigscreen=0;

    // draws little dots on the bottom of the screen
    if (devparm)
    {

	i = I_GetTime();
	tics = i - lasttic;
	lasttic = i;
	if (tics > 20) tics = 20;

	for (i=0 ; i<tics*2 ; i+=2)
	    screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0xff;
	for ( ; i<20*2 ; i+=2)
	    screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;

    }
I_UpdateScreen();
    // scales the screen size before blitting it
    //SDL_BlitSurface()



}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    SDL_memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}



//
// I_SetPalette
//
SDL_Palette* palette;
void I_SetPalette (byte* pal)
{
    palette = SDL_CreatePalette(256);
    
    for(int i = 0; i < 256; i++)
    {
        
        colors[i].r = pal[(3*i)];
        colors[i].g = pal[(3*i)+1];
        colors[i].b = pal[(3*i)+2];
        colors[i].a = 255;
         
    }
   
    SDL_SetPaletteColors(palette, colors,0,256);
    SDL_SetSurfacePalette(UISurface, palette);

SDL_SetSurfacePalette(imageSurface,palette);
   // UploadNewPalette(X_cmap, palette);

}


//
// This function is probably redundant,
//  if XShmDetach works properly.
// ddt never detached the XShm memory,
//  thus there might have been stale
//  handles accumulating.
//


void I_InitGraphics(void)
{
    printf("I_InitGraphics\n");
	char* displayname;
	char* d;
	int			n;
	int			pnum;
	int			x = 0;
	int			y = 0;

	// warning: char format, different type arg
	char		xsign = ' ';
	char		ysign = ' ';

	int			oktodraw;
	unsigned long	attribmask;

	int			valuemask;
	static int		firsttime = 1;

	if (!firsttime)
		return;
	firsttime = 0;

	X_width = SCREENWIDTH;
	X_height = SCREENHEIGHT;

	// check for command-line display name
	if ((pnum = M_CheckParm("-disp"))) // suggest parentheses around assignment
		displayname = myargv[pnum + 1];
	else
		displayname = 0;

	// check if the user wants to grab the mouse (quite unnice)
	grabMouse = !!M_CheckParm("-grabmouse");

	// check for command-line geometry

	// open the display
	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO );
    UISurface = SDL_CreateSurfaceFrom(320, 200, SDL_PIXELFORMAT_INDEX8, UI_image, NULL);
    SDL_CreateWindowAndRenderer("Custom Doom", X_width*3, X_height*3,  SDL_WINDOW_FULLSCREEN, &mainWindow,&mainRenderer);
	mainSurface = SDL_GetWindowSurface(mainWindow);
//SDL_SetS
	imageSurface = SDL_CreateSurface(X_width, X_height, SDL_PIXELFORMAT_INDEX8 );
    // use the default visual
    renderTexture = SDL_CreateTexture(mainRenderer,SDL_PIXELFORMAT_INDEX8,SDL_TEXTUREACCESS_STREAMING,X_width,X_height);
	SDL_ShowWindow(mainWindow);
    // wait until it is OK to draw
    SDL_SetSurfaceBlendMode(imageSurface, SDL_BLENDMODE_NONE);
    SDL_SetSurfaceBlendMode(mainSurface, SDL_BLENDMODE_NONE);
    
    SDL_SetWindowRelativeMouseMode(mainWindow, true);
imageSurface->pixels = (void*) screens[0];
//SDL_SetWindowSize(mainWindow,X_width*3, X_height*3);

}

void I_UpdateScreen(void)
{

 //    SDL_BlitSurface(imageSurface,NULL,mainSurface,NULL);
    SDL_Rect destRect = { viewwidth,viewheight };
    
     SDL_BlitSurfaceScaled(imageSurface, NULL, mainSurface, NULL, SDL_SCALEMODE_PIXELART);
  //   SDL_BlitSurfaceScaled(UISurface, NULL, mainSurface, &destRect, SDL_SCALEMODE_PIXELART);


SDL_UpdateWindowSurface(mainWindow);



}


