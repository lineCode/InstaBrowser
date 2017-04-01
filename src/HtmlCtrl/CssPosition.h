#pragma once
#include "CssLength.h"

NAMESPACE_HTMLCTRL

struct CssPosition
{
	CssLength	x;
	CssLength	y;
	CssLength	width;
	CssLength	height;

	CssPosition()
	{

	}

	CssPosition(const CssPosition& val)
	{
		x		= val.x;
		y		= val.y;
		width	= val.width;
		height	= val.height;
	}

	CssPosition& operator=(const CssPosition& val)
	{
		x		= val.x;
		y		= val.y;
		width	= val.width;
		height	= val.height;
		return *this;
	}
};

END_HTMLCTRL_NAMESPACE
