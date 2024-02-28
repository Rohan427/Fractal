#pragma once

#include <SDL.h>
#include "SDL_ttf.h"
#include "ErrorHandler.h"


class WindowRef
{
	private:
	SDL_Window* m_window = nullptr;
	ErrorHandler m_errHdlr = {};
	SDL_Renderer* m_renderer = nullptr;
	Uint32 mWindowID = 0;

	//Window dimensions
	int mWidth = 0;
	int mHeight = 0;

	//Window focus
	bool mMouseFocus = false;
	bool mKeyboardFocus = false;
	bool mFullScreen = false;
	bool mMinimized = false;
	bool mShown = false;
	bool mUpdateMenu = false;

	point mousePos;

	color_t defaultColor = {0x00, 0x00, 0x00, 0xff};
	color_t previousColor = defaultColor;

	struct textBox
	{
		SDL_Surface* surface = nullptr;
		TTF_Font *font = nullptr;
		SDL_Color color;
		SDL_Texture *text = nullptr;
		SDL_Rect textRect;
	};


	TTF_Font* defaultFont;

	textBox mode;
	textBox imageX;
	textBox imageY;
	textBox imageW;
	textBox imageH;
	textBox imageErr;

	static const int RECT_MODE = 0;
	static const int RECT_IMG_X = 1;
	static const int RECT_IMG_Y = 2;
	static const int RECT_IMG_W = 3;
	static const int RECT_IMG_H = 4;
	static const int RECT_ERR = 5;

	public:
	ErrorHandler getError ();
	SDL_Window* getWindow ();
	ErrorHandler initWindow (Uint32 flags);
	void* createWindow (const char* title,
						int x, int y, int w,
						int h, Uint32 flags
	);
	ErrorHandler drawImage ();
	void show ();
	void cleanUp ();
	void clear (const color_t color);
	ErrorHandler createRenderer (color_t color);
	SDL_Renderer* getRenderer ();
	ErrorHandler plotPoint (unsigned x, unsigned y, unsigned color);
	ErrorHandler plotPoint (unsigned x, unsigned y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	void delay (Uint32 delay);
	ErrorHandler setColor (unsigned color);
	ErrorHandler setColor (Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	ErrorHandler drawRectangle (int orgX, int orgY, int length, int height, color_t color);
	point getMousePos ();
	ErrorHandler resetColor ();
	void setPreviousColor (color_t color);
	ErrorHandler clearToDefault ();
	void hideWindow ();
	void showWindow ();
	void focus ();

	int initMenu ();
	ErrorHandler output (int rect, const char *text);
	bool isUpdateMenu ();

	Uint32 getID ();
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
};
