#pragma once

#include <SDL.h>
#include "WindowRef.h"

class Window
{
	private:
	WindowRef m_window = {};
	point mousePos;

	public:
	ErrorHandler init (Uint32 flags);
	ErrorHandler createWindow (int x, int y);
	void* setWindowRef (const char* title,
						int x, int y, int w,
						int h, Uint32 flags
	);
	WindowRef getWindow ();
	ErrorHandler createRenderer (color_t color);
	void* getRenderer ();
	ErrorHandler getError ();
	ErrorHandler plotPoint (unsigned x, unsigned y, unsigned color);
	ErrorHandler plotPoint (unsigned x, unsigned y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	void show ();
	void clear (color_t color);
	ErrorHandler plotTest ();
	void clearWindow ();
	ErrorHandler drawRectangle (int x, int y, int length, int width, color_t color);
	point getMousePos ();
	ErrorHandler resetColor ();
	void setPreviousColor (color_t color);
	ErrorHandler clearToDefault ();
	void handleWindowEvent (SDL_Event ev);
	ErrorHandler output (int rect, const char *text);
	bool isWindowUpdate ();
	int initMenu ();

	Uint32 getWindowID ();
	int getWidth ();
	int getHeight ();
	bool getMFocus ();
	bool getKFocus ();
	bool getFullScreen ();
	bool getMinimized ();
	bool getShown ();

	void setWidth (int width);
	void setHeight (int height);
	void setMFocus (bool focus);
	void setKFocus (bool focus);
	void setFullScreen (bool full);
	void setMinimized (bool min);
	void setShown (bool show);

	void hideWindow ();
	void showWindow ();
	void focus ();
};
