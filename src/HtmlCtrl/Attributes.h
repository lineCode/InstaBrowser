#pragma once

#include "Types.h"

NAMESPACE_HTMLCTRL

struct attr_color
{
	unsigned char    rgbBlue;
	unsigned char    rgbGreen;
	unsigned char    rgbRed;
	unsigned char    rgbAlpha;
	attr_color()
	{
		rgbAlpha	= 255;
		rgbBlue		= 0;
		rgbGreen	= 0;
		rgbRed		= 0;
	}
};

struct attr_border
{
	StyleBorder	Border;
	int				width;
	attr_color		color;

	attr_border()
	{
		Border	= BorderNone;
		width	= 0;
	}
};

END_HTMLCTRL_NAMESPACE
