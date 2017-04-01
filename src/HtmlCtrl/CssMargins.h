#pragma once
#include "CssLength.h"

NAMESPACE_HTMLCTRL

struct CssMargins
{
	CssLength	left;
	CssLength	right;
	CssLength	top;
	CssLength	bottom;

	CssMargins()
	{

	}

	CssMargins(const CssMargins& val)
	{
		left	= val.left;
		right	= val.right;
		top		= val.top;
		bottom	= val.bottom;
	}

	CssMargins& operator=(const CssMargins& val)
	{
		left	= val.left;
		right	= val.right;
		top		= val.top;
		bottom	= val.bottom;
		return *this;
	}
};

END_HTMLCTRL_NAMESPACE
