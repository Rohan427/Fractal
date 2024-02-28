#include "Window.h"
#include <iostream> //for testing
#include <random>
#include <limits>

void* Window::setWindowRef (const char* title,
							int x, int y, int w,
							int h, Uint32 flags)
{
	return m_window.createWindow (title, x, y, w, h, flags);
}

WindowRef Window::getWindow()
{
	return m_window;
}

ErrorHandler Window::getError()
{
	return m_window.getError();
}

ErrorHandler Window::init (Uint32 flags)
{
	return m_window.initWindow (flags);
}

ErrorHandler Window::createWindow (int x, int y)
{
	m_window.getError().setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);

	if (setWindowRef ("Mandelbrot",
					  SDL_WINDOWPOS_UNDEFINED,
					  SDL_WINDOWPOS_UNDEFINED,
					  x,
					  y,
					  SDL_WINDOW_SHOWN
	) == nullptr
		)
	{
		m_window.getError().setErrorData (ERR_WININIT, ERR_WINCREATE, SDL_GetError (), REND_ERR);
	}
	else
	{
		m_window.getError().setErrorData (ERR_SUCCESS, ERR_OK, ERR_NONE, REND_OK);
	}

	return m_window.getError();
}

ErrorHandler Window::plotTest()
{
	ErrorHandler error;
	unsigned color = 0xffffffff;

	std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
	std::mt19937_64 eng (rd ()); //Use the 64-bit Mersenne Twister 19937 generator
							   //and seed it with entropy.

	//Define the distribution, by default it goes from 0 to MAX(unsigned long long)
	//or what have you.
	std::uniform_int_distribution<unsigned> distr;

	for (unsigned y = 0; y < WIN_HEIGHT; y++)
	{
		for (unsigned x = 0; x < WIN_WIDTH; x++)
		{
			color = distr (eng);
			error = Window::plotPoint (x, y, color);

			if (error.getStatus() != ERR_SUCCESS)
			{
				std::cout << "FAILED TO PLOT POINTS" << std::endl;
				return error;
			}
			// else continue
		}
	}

	m_window.show ();

	std::cout << "Plot SUCCESS" << std::endl;
	m_window.delay (2000);
	return error;
}

ErrorHandler Window::plotPoint (unsigned x, unsigned y, unsigned color)
{
	return m_window.plotPoint (x, y, color);
}

ErrorHandler Window::plotPoint (unsigned x, unsigned y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
	return m_window.plotPoint (x, y, red, green, blue, alpha);
}

void Window::show()
{
	m_window.show();
}

void Window::clear (color_t color)
{
	m_window.clear (color);
}

ErrorHandler Window::createRenderer (color_t color)
{
	return m_window.createRenderer (color);
}

void* Window::getRenderer()
{
	return m_window.getRenderer();
}

void Window::clearWindow()
{
	m_window.clear (DEFAULTCOLOR);
}

ErrorHandler Window::drawRectangle (int x, int y, int length, int width, color_t color)
{
	return m_window.drawRectangle (x, y, length, width, color);
}

point Window::getMousePos()
{
	return m_window.getMousePos();
}

ErrorHandler Window::resetColor()
{
	return m_window.resetColor();
}

void Window::setPreviousColor (color_t color)
{
	m_window.setPreviousColor (color);
}
ErrorHandler Window::clearToDefault()
{
	return m_window.clearToDefault();
}

Uint32 Window::getWindowID()
{
	return m_window.getID();
}

int Window::getWidth()
{
	return m_window.getWidth();
}

int Window::getHeight()
{
	return m_window.getHeight();
}

bool Window::getMFocus()
{
	return m_window.getMFocus();
}

bool Window::getKFocus()
{
	return m_window.getMFocus();
}

bool Window::getFullScreen()
{
	return m_window.getFullScreen();
}

bool Window::getMinimized()
{
	return m_window.getMinimized();
}

bool Window::getShown()
{
	return m_window.getShown();
}

void Window::setWidth (int width)
{
	m_window.setWidth (width);
}

void Window::setHeight (int height)
{
	m_window.setHeight (height);
}

void Window::setMFocus (bool focus)
{
	m_window.setMFocus (focus);
}

void Window::setKFocus (bool focus)
{
	m_window.setKFocus (focus);
}

void Window::setFullScreen (bool full)
{
	m_window.setFullScreen (full);
}

void Window::setMinimized (bool min)
{
	m_window.setMinimized (min);
}

void Window::setShown (bool show)
{
	m_window.setShown (show);
}

void Window::hideWindow()
{
	m_window.hideWindow();
}

void Window::showWindow()
{
	m_window.showWindow();
}

void Window::focus()
{
	m_window.focus();
}

void Window::handleWindowEvent (SDL_Event ev)
{
	switch (ev.window.event)
	{
		//Window appeared
		case SDL_WINDOWEVENT_SHOWN:
			m_window.setShown (true);
			break;

			//Window disappeared
		case SDL_WINDOWEVENT_HIDDEN:
			m_window.setShown (false);
			break;

			//Get new dimensions and repaint
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			//mWidth = e.window.data1;
			//mHeight = e.window.data2;
			//SDL_RenderPresent (mRenderer);
			break;

			//Repaint on expose
		case SDL_WINDOWEVENT_EXPOSED:
			//SDL_RenderPresent (mRenderer);
			break;

			//Mouse enter
		case SDL_WINDOWEVENT_ENTER:
			m_window.setMFocus (true);
			//updateCaption = true;
			break;

			//Mouse exit
		case SDL_WINDOWEVENT_LEAVE:
			m_window.setMFocus (false);
			//updateCaption = true;
			break;

			//Keyboard focus gained
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			m_window.setKFocus (true);
			//updateCaption = true;
			break;

			//Keyboard focus lost
		case SDL_WINDOWEVENT_FOCUS_LOST:
			m_window.setKFocus (false);
			//updateCaption = true;
			break;

			//Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			m_window.setMinimized (true);
			break;

			//Window maxized
		case SDL_WINDOWEVENT_MAXIMIZED:
			m_window.setMinimized (false);
			break;

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			m_window.setMinimized (false);
			break;

			//Hide on close
		case SDL_WINDOWEVENT_CLOSE:
			m_window.hideWindow();
			break;
	}
}

ErrorHandler Window::output (int rect, const char *text)
{
	return m_window.output (rect, text);
}

bool Window::isWindowUpdate()
{
	return m_window.isUpdateMenu();
}

int Window::initMenu()
{
	return m_window.initMenu();
}
