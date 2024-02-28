#include "MouseManager.h"
#include <iostream>


MouseManager::MouseManager ()
{
	rectangle.color = &(rectColors.at (DEF_RECT_COL));
}

void MouseManager::setMode (int mode)
{
	mouseMode = mode;
}

void MouseManager::setMousePos (double x, double y)
{
	mousePos.x = x;
	mousePos.y = y;
}

void MouseManager::setRectPoint1 (double x, double y)
{
	rectangle.startX = x;
	rectangle.startY = y;
}

void MouseManager::setRectPoint2 (double x, double y)
{
	rectangle.endX = x;
	rectangle.endY = y;
}

void MouseManager::finishRect ()
{
	// Adjust length based on which X user selected first
	if (rectangle.startX > rectangle.endX)
	{
		rectangle.length = rectangle.startX - rectangle.endX;
	}
	else
	{
		rectangle.length = rectangle.endX - rectangle.startX;
	}

	// Adjust width based on which Y user selected first
	if (rectangle.startY > rectangle.endY)
	{
		rectangle.width = rectangle.startY - rectangle.endY;
	}
	else
	{
		rectangle.width = rectangle.endY - rectangle.startY;
	}
}

MouseManager::mousePosStruct MouseManager::getMousePos ()
{
	return mousePos;
}

MouseManager::rectStruct MouseManager::getRectangle ()
{
	return rectangle;
}

void MouseManager::resetColor ()
{
	rectangle.color = &(rectColors.at (0));
}

void MouseManager::resetSize ()
{
	rectangle.startX = WIN_WIDTH / 2;
	rectangle.startY = WIN_HEIGHT / 2;
	rectangle.endX = rectangle.startX + RECT_LENGTH;
	rectangle.endY = rectangle.startY + RECT_HEIGHT;
	rectangle.length = RECT_LENGTH;
	rectangle.width = RECT_HEIGHT;
}

void MouseManager::initRect ()
{
	rectStruct newRect;

	rectangle = newRect;
}

int MouseManager::getMode ()
{
	return mouseMode;
}

color_t MouseManager::getRectColor ()
{
	return *rectangle.color;
}

void MouseManager::incColor ()
{
	if (rectangle.color == &(rectColors.back ()))
	{
		rectangle.color = &rectColors.front ();
	}
	else
	{
		rectangle.color++;
	}
	// else do nothing
}

void MouseManager::decColor ()
{
	if (rectangle.color == &(rectColors.front ()))
	{
		rectangle.color = &rectColors.back ();
	}
	else
	{
		rectangle.color--;
	}
	// else do nothing
}
