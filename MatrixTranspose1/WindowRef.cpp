#include "WindowRef.h"
#include <iostream> // for testing

ErrorHandler WindowRef::getError ()
{
	return m_errHdlr;
}

SDL_Window* WindowRef::getWindow ()
{
	return m_window;
}

ErrorHandler WindowRef::initWindow (Uint32 flags)
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		m_errHdlr.setErrorData (ERR_VIDERR, ERR_VIDINIT, SDL_GetError (), REND_ERR);
	}
	else
	{
		m_errHdlr.setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);
	}

	return m_errHdlr;
}

void* WindowRef::createWindow (const char* title,
							   int x, int y, int w,
							   int h, Uint32 flags)
{
	m_window = SDL_CreateWindow (title, x, y, w, h, flags);

	if (m_window != nullptr)
	{
		mWindowID = SDL_GetWindowID (m_window);

		if (mWindowID == 0)
		{
			SDL_DestroyWindow (m_window);
			m_window = nullptr;
		}
		else
		{
			mMouseFocus = true;
			mKeyboardFocus = true;
			mFullScreen = false;
			mMinimized = false;
			mShown = true;
		}
	}
	// else do nothing

	return m_window;
}

ErrorHandler WindowRef::drawImage ()
{
	//windowSurface = SDL_GetWindowSurface (m_window);
	//imageSurface = SDL_LoadBMP ("test.bmp");

	//if (imageSurface == NULL)
	//{
	//	m_errHdlr.setErrorData (ERR_IMGERR, ERR_IMGLOAD, SDL_GetError());
	//}
	//// TODO: Handle the errors
	//else
	//{
	//	SDL_BlitSurface (imageSurface, NULL, windowSurface, NULL);
	//	SDL_UpdateWindowSurface (m_window);
	//}

	return m_errHdlr;
}

ErrorHandler WindowRef::createRenderer (color_t color)
{
	m_renderer = SDL_CreateRenderer (m_window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	m_errHdlr.setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);

	if (m_renderer == nullptr)
	{
		std::cout << "FAILED TO CREATE RENDERER" << std::endl;
		m_errHdlr.setErrorData (ERR_RENDINIT, ERR_RENDFAIL, SDL_GetError (), REND_ERR);
	}
	else
	{
		std::cout << "Renderer created" << std::endl;

		clear (color);
	}

	return m_errHdlr;
}

SDL_Renderer* WindowRef::getRenderer ()
{
	return m_renderer;
}

ErrorHandler WindowRef::plotPoint (unsigned x, unsigned y, unsigned color)
{
	int red = (color & 0xff000000) >> 24;
	int green = (color & 0x00ff0000) >> 16;
	int blue = (color & 0x0000ff00) >> 8;
	int alpha = (color & 0x000000ff);

	m_errHdlr.setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);

	if (m_renderer == nullptr)
	{
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETCOLOR, ERR_RENDFAIL, REND_ERR);
	}

	// Set our color for the draw functions
	else if (SDL_SetRenderDrawColor (m_renderer, red, green, blue, SDL_ALPHA_OPAQUE) != 0)
	{
		std::cout << "DEBUG: failed to set color" << std::endl;
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETCOLOR, SDL_GetError (), REND_ERR);
	}
	else
	{
		// Now we can draw our point
		if (SDL_RenderDrawPoint (m_renderer, x, y) != 0)
		{
			std::cout << "DEBUG: failed to draw point" << std::endl;
			m_errHdlr.setErrorData (ERR_PLOT, ERR_PLOTFAIL, SDL_GetError (), REND_ERR);
		}
	}

	return m_errHdlr;
}

ErrorHandler WindowRef::plotPoint (unsigned x, unsigned y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
	m_errHdlr.setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);

	if (m_renderer == nullptr)
	{
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETCOLOR, ERR_RENDFAIL, REND_ERR);
	}

	// Set our color for the draw functions
	else if (SDL_SetRenderDrawColor (m_renderer, red, green, blue, alpha) != 0)
	{
		std::cout << "DEBUG: failed to set color" << std::endl;
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETCOLOR, SDL_GetError (), REND_ERR);
	}
	else
	{
		// Now we can draw our point
		if (SDL_RenderDrawPoint (m_renderer, x, y) != 0)
		{
			std::cout << "DEBUG: failed to draw point" << std::endl;
			m_errHdlr.setErrorData (ERR_PLOT, ERR_PLOTFAIL, SDL_GetError (), REND_ERR);
		}
	}

	return m_errHdlr;
}

ErrorHandler WindowRef::resetColor ()
{
	if (m_renderer == nullptr)
	{
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETCOLOR, ERR_RENDFAIL, REND_ERR);
	}
	// Set the color to what it was before
	else if (SDL_SetRenderDrawColor (m_renderer, previousColor.red, previousColor.green, previousColor.blue, previousColor.alpha) != 0)
	{
		std::cout << "DEBUG: failed to reset color" << std::endl;
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETRSTCLR, SDL_GetError (), REND_ERR);
	}
	else
	{
		m_errHdlr.setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);
	}
	// else do nothing - it all worked

	return m_errHdlr;
}

void WindowRef::setPreviousColor (color_t color)
{
	previousColor = color;
}

ErrorHandler WindowRef::clearToDefault ()
{
	if (m_renderer == nullptr)
	{
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETCOLOR, ERR_RENDFAIL, REND_ERR);
	}
	// Set the color to what it was before
	else if (SDL_SetRenderDrawColor (m_renderer, defaultColor.red, defaultColor.green, defaultColor.blue, defaultColor.alpha) != 0)
	{
		std::cout << "DEBUG: failed to clear to defaul color" << std::endl;
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETRSTCLR, SDL_GetError (), REND_ERR);
	}
	else
	{
		m_errHdlr.setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);
	}
	// else do nothing - it all worked

	return m_errHdlr;
}

void WindowRef::show ()
{
	//Draw only if not minimized
	if (!mMinimized)
	{
		SDL_RenderPresent (m_renderer);
	}
}

void WindowRef::clear (const color_t color)
{
	SDL_Color newColor = {color.red, color.green, color.blue, color.alpha};

	m_errHdlr.setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);

	if (SDL_SetRenderDrawColor (m_renderer, newColor.r, newColor.g, newColor.b, newColor.a) != 0)
	{
		std::cout << "DEBUG: failed to reset color" << std::endl;
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETRSTCLR, SDL_GetError (), REND_ERR);
	}

	if (SDL_RenderClear (m_renderer) != ERR_SUCCESS)
	{
		m_errHdlr.setErrorData (ERR_RENDINIT, ERR_SETCLR, SDL_GetError (), REND_ERR);
	}
}

void WindowRef::delay (Uint32 delay)
{
	SDL_Delay (delay);
}

void WindowRef::cleanUp ()
{
	SDL_DestroyRenderer (m_renderer);
	//SDL_FreeSurface (imageSurface);
	SDL_DestroyWindow (m_window);

	//imageSurface = NULL;
	//windowSurface = NULL;
	m_window = NULL;
	m_renderer = NULL;
}

ErrorHandler WindowRef::setColor (unsigned color)
{
	int red = (color & 0xff000000) >> 24;
	int green = (color & 0x00ff0000) >> 16;
	int blue = (color & 0x0000ff00) >> 8;
	int alpha = (color & 0x000000ff);

	m_errHdlr.setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);

	if (m_renderer == NULL)
	{
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETCOLOR, ERR_RENDFAIL, REND_ERR);
	}

	// Set our color for the draw functions
	if (SDL_SetRenderDrawColor (m_renderer, red, green, blue, SDL_ALPHA_OPAQUE) != 0)
	{
		std::cout << "DEBUG: failed to set color" << std::endl;
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETCOLOR, SDL_GetError (), REND_ERR);
	}

	return m_errHdlr;
}

ErrorHandler WindowRef::setColor (Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{

	m_errHdlr.setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);

	if (m_renderer == NULL)
	{
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETCOLOR, ERR_RENDFAIL, REND_ERR);
	}

	// Set our color for the draw functions
	if (SDL_SetRenderDrawColor (m_renderer, red, green, blue, alpha) != 0)
	{
		std::cout << "DEBUG: failed to set color" << std::endl;
		m_errHdlr.setErrorData (ERR_PLOT, ERR_SETCOLOR, SDL_GetError (), REND_ERR);
	}

	return m_errHdlr;
}

ErrorHandler WindowRef::drawRectangle (int orgX, int orgY, int length, int height, color_t color)
{
	int startx = orgX;
	int starty = orgY;
	int endx = startx + length;
	int endy = orgY;


	SDL_GetRenderDrawColor (m_renderer, &previousColor.red, &previousColor.green, &previousColor.blue, &previousColor.alpha);

	// set the color
	setColor (color.red, color.green, color.blue, color.alpha);
	// top
	SDL_RenderDrawLine (m_renderer, startx, starty, endx, endy);
	startx = endx;

	// right
	endy = starty + height;
	SDL_RenderDrawLine (m_renderer, startx, starty, endx, endy);
	starty = endy;

	// bottom
	endx = orgX;
	SDL_RenderDrawLine (m_renderer, startx, starty, endx, endy);
	startx = endx;

	// left
	endy = orgY;
	SDL_RenderDrawLine (m_renderer, startx, starty, endx, endy);

	// show
	show ();

	resetColor ();

	return m_errHdlr;
}

point WindowRef::getMousePos ()
{
	SDL_GetMouseState (&mousePos.x, &mousePos.y);
	return mousePos;
}

Uint32 WindowRef::getID ()
{
	return mWindowID;
}
int WindowRef::getWidth ()
{
	return mWidth;
}

int WindowRef::getHeight ()
{
	return mHeight;
}

bool WindowRef::getMFocus ()
{
	return mMouseFocus;
}

bool WindowRef::getKFocus ()
{
	return mKeyboardFocus;
}

bool WindowRef::getFullScreen ()
{
	return mFullScreen;
}

bool WindowRef::getMinimized ()
{
	return mMinimized;
}

bool WindowRef::getShown ()
{
	return mShown;
}

void WindowRef::setWidth (int width)
{
	mWidth = width;
}

void WindowRef::setHeight (int height)
{
	mHeight = height;
}

void WindowRef::setMFocus (bool focus)
{
	mMouseFocus = focus;
}

void WindowRef::setKFocus (bool focus)
{
	mKeyboardFocus = focus;
}

void WindowRef::setFullScreen (bool full)
{
	mFullScreen = full;
}

void WindowRef::setMinimized (bool min)
{
	mMinimized = min;
}

void WindowRef::setShown (bool show)
{
	mShown = show;
}

void WindowRef::hideWindow ()
{
	SDL_HideWindow (m_window);
	mMinimized = true;
}

void WindowRef::showWindow ()
{
	SDL_ShowWindow (m_window);
	mMinimized = false;
	mShown = true;
}

void WindowRef::focus ()
{
	if (!mShown)
	{
		showWindow ();
	}
	// else continue

	SDL_RaiseWindow (m_window);
}

int WindowRef::initMenu ()
{
	int result = 0;
	SDL_Color color;
	SDL_Surface *textSurface;

	//std::cout << "Initialize menu" << std::endl;


	if (TTF_Init () != 0)
	{
		std::cout << "Failed font init " << std::endl;
		return -1;
	}
	else
	{
		defaultFont = TTF_OpenFont ("lazy.ttf", 20);

		if (defaultFont == NULL)
		{
			std::cout << "Failed to load font " << std::endl;
			return -2;
		}
		// else continue

		color = {0xff, 0xff, 0xff, 0xff};

		// Mode
		mode.textRect.x = 0;
		mode.textRect.y = 0;
		mode.color = color;
		mode.font = defaultFont;
		textSurface = TTF_RenderText_Solid (mode.font, "READY", mode.color);

		if (textSurface == NULL)
		{
			std::cout << "Failed to create text surface " << std::endl;
			return -3;
		}
		// else continue

		mode.text = SDL_CreateTextureFromSurface (m_renderer, textSurface);

		if (mode.text == nullptr)
		{
			std::cout << "Failed to create texture" << std::endl;
			return -4;
		}

		if (SDL_QueryTexture (mode.text, NULL, NULL, &mode.textRect.w, &mode.textRect.h) != 0)
		{
			std::cout << "Failed to query texture " << std::endl;
			return -5;
		}

		// Image x
		//imageX.textRect.x = 0;
		//imageX.textRect.y = 50;
		//imageX.color = color;
		//imageX.font = defaultFont;

		//imageX.text = SDL_CreateTextureFromSurface (m_renderer, textSurface);

		//// Image y
		//imageY.textRect.x = 0;
		//imageY.textRect.y = 100;
		//imageY.color = color;
		//imageY.font = defaultFont;

		//imageY.text = SDL_CreateTextureFromSurface (m_renderer, textSurface);

		//// Image W
		//imageW.textRect.x = 0;
		//imageW.textRect.y = 150;
		//imageW.color = color;
		//imageW.font = defaultFont;

		//imageW.text = SDL_CreateTextureFromSurface (m_renderer, textSurface);

		//// Image H
		//imageH.textRect.x = 0;
		//imageH.textRect.y = 200;
		//imageH.color = color;
		//imageH.font = defaultFont;

		//imageH.text = SDL_CreateTextureFromSurface (m_renderer, textSurface);

		//// Error
		//imageErr.textRect.x = 0;
		//imageErr.textRect.y = 250;
		//imageErr.color = color;
		//imageErr.font = defaultFont;

		//imageErr.text = SDL_CreateTextureFromSurface (m_renderer, textSurface);
	}

	mUpdateMenu = true;

	SDL_FreeSurface (textSurface);

	return result;
}

ErrorHandler WindowRef::output (int rect, const char *text)
{
	SDL_Surface *textSurface = nullptr;
	clear (DEFAULTCOLOR);

	std::cout << "update menu" << std::endl;

	switch (rect)
	{
		case RECT_MODE:
			if (text == "DEFAULT")
			{
				SDL_RenderCopy (m_renderer, mode.text, NULL, &mode.textRect);
			}
			else
			{
				textSurface = TTF_RenderText_Solid (mode.font, text, mode.color);

				if (textSurface == NULL)
				{
					std::cout << "Failed to create text surface " << std::endl;
				}
				// else continue
				else
				{
					mode.text = SDL_CreateTextureFromSurface (m_renderer, textSurface);

					if (mode.text == nullptr)
					{
						std::cout << "Failed to create texture" << std::endl;
					}
					else
					{
						if (SDL_QueryTexture (mode.text, NULL, NULL, &mode.textRect.w, &mode.textRect.h) != 0)
						{
							std::cout << "Failed to query texture " << std::endl;
						}
						else
						{
							SDL_RenderCopy (m_renderer, mode.text, NULL, &mode.textRect);
						}
					}
				}
			}

			break;

		case RECT_IMG_X:
			//imageX.text = text;
			break;

		case RECT_IMG_Y:
			//imageY.text = text;
			break;

		case RECT_IMG_W:
			//imageW.text = text;
			break;

		case RECT_IMG_H:
			//imageH.text = text;
			break;

		case RECT_ERR:
			//imageErr.text = text;
			break;

		default:
			break;
	}

	mUpdateMenu = true;
	if (textSurface != nullptr)
	{
		SDL_FreeSurface (textSurface);
	}
	// else do nothing

	return m_errHdlr;
}

bool WindowRef::isUpdateMenu ()
{
	return mUpdateMenu;
}