#pragma once

#include "Constants.h"
#include <cmath>
#include <vector>

class MouseManager
{
	private:
	/*
	* Used to track the state of the mouse for button presses and movement
	*
	* Normal/initial mode
	* 0		Move cursor. Moves the rectangle around the window. Button clicks
	*		will select one of the other modes:
	*			Left click:		Resize mode
	*			Right click:	Point selection mode
	*			Middle click:	Rectangle color mode
	*		<Enter> draws the fractal based on the current rectangle position
	*		and size.
	*
	* r		Reset image to full set
	* p		Cycle through color palettes and redraw current image
	*
	* Resize mode
	* 1		Mouse movement resizes the rectangle. <Enter> key exits and sets
	*		mode back to 0
	*
	* Point selection mode
	* 2		Point selection 1. Select the rectangle starting point (upper
	*		left corner) by moving the mouse pointer and clicking a point.
	* 3		Point selection 2. Select the rectangle ending point (lower
	*		right corner) by moving the mouse pointer and clicking a point.
	*
	* Rectangle color mode
	* 4
	*			Left click:		Changes the rectangle color UP (see Color Table
	*							for details.
	*			Right click:	Changes the rectangle color UP (see Color Table
	*							for details.
	*			Middle click:	Reset to original color
	*/
	int mouseMode = MOUSE_NORM;

	color_t redMouse =
	{
		0xff,
		0x00,
		0x00,
		0xff
	};

	color_t whiteMouse =
	{
		0xff,
		0xff,
		0xff,
		0xff
	};

	color_t blackMouse =
	{
		0x00,
		0x00,
		0x00,
		0xff
	};

	color_t greenMouse =
	{
		0x00,
		0xff,
		0x00,
		0xff
	};

	color_t blueMouse =
	{
		0x00,
		0x00,
		0xff,
		0xff
	};

	// Mouse location coordinates. Defaults to window center
	struct mousePosStruct
	{
		double x = WIN_WIDTH / 2;
		double y = WIN_HEIGHT / 2;
	} mousePos;

	// Rectangle points. Defaults to window center, 20 x 20 pixels
	struct rectStruct
	{
		double startX = 0;// WIN_WIDTH / 2;
		double startY = 0;// WIN_HEIGHT / 2;
		double endX = startX + WIN_WIDTH - 1;// RECT_LENGTH;
		double endY = startY + WIN_HEIGHT - 1;// RECT_HEIGHT;
		double length = WIN_WIDTH;
		double width = WIN_HEIGHT;
		color_t* color;
	} rectangle;

	// Color Table
	std::vector<color_t> rectColors = {redMouse, greenMouse, blueMouse, whiteMouse, blackMouse};

	public:
	void setMode (int mode);
	void setMousePos (double x, double y);
	void setRectPoint1 (double x, double y);
	void setRectPoint2 (double x, double y);
	mousePosStruct getMousePos ();
	rectStruct getRectangle ();
	void resetColor ();
	void resetSize ();
	int getMode ();
	color_t getRectColor ();
	void incColor ();
	void decColor ();
	void finishRect ();
	void initRect ();
	MouseManager ();
};
