#pragma once
#include "CssLength.h"

NAMESPACE_HTMLCTRL

struct CssOffsets
{
	CssLength	left;
	CssLength	top;
	CssLength	right;
	CssLength	bottom;

	CssOffsets()
	{

	}

	CssOffsets(const CssOffsets& val)
	{
		left	= val.left;
		top		= val.top;
		right	= val.right;
		bottom	= val.bottom;
	}

	CssOffsets& operator=(const CssOffsets& val)
	{
		left	= val.left;
		top		= val.top;
		right	= val.right;
		bottom	= val.bottom;
		return *this;
	}
};

END_HTMLCTRL_NAMESPACE
