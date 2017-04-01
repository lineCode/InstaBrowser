#pragma once
#include "CssLength.h"
#include "Types.h"

NAMESPACE_HTMLCTRL

struct CssBorder
{
	CssLength		width;
	BorderStyle		style;
	WebColor		color;

	CssBorder()
	{
		style = border_style_none;
	}

	CssBorder(const CssBorder& val)
	{
		width	= val.width;
		style	= val.style;
		color	= val.color;
	}

	CssBorder& operator=(const CssBorder& val)
	{
		width	= val.width;
		style	= val.style;
		color	= val.color;
		return *this;
	}
};

struct Border
{
	int				width;
	BorderStyle		style;
	WebColor		color;

	Border()
	{
		width = 0;
	}
	Border(const Border& val)
	{
		width = val.width;
		style = val.style;
		color = val.color;
	}
	Border(const CssBorder& val)
	{
		width = (int) val.width.Value();
		style = val.style;
		color = val.color;
	}
	Border& operator=(const Border& val)
	{
		width = val.width;
		style = val.style;
		color = val.color;
		return *this;
	}
	Border& operator=(const CssBorder& val)
	{
		width = (int) val.width.Value();
		style = val.style;
		color = val.color;
		return *this;
	}
};

struct BorderRadiuses
{
	int	top_left_x;
	int	top_left_y;

	int	top_right_x;
	int	top_right_y;

	int	bottom_right_x;
	int	bottom_right_y;

	int	bottom_left_x;
	int	bottom_left_y;

	BorderRadiuses()
	{
		top_left_x = 0;
		top_left_y = 0;
		top_right_x = 0;
		top_right_y = 0;
		bottom_right_x = 0;
		bottom_right_y = 0;
		bottom_left_x = 0;
		bottom_left_y = 0;
	}
	BorderRadiuses(const BorderRadiuses& val)
	{
		top_left_x = val.top_left_x;
		top_left_y = val.top_left_y;
		top_right_x = val.top_right_x;
		top_right_y = val.top_right_y;
		bottom_right_x = val.bottom_right_x;
		bottom_right_y = val.bottom_right_y;
		bottom_left_x = val.bottom_left_x;
		bottom_left_y = val.bottom_left_y;
	}
	BorderRadiuses& operator = (const BorderRadiuses& val)
	{
		top_left_x = val.top_left_x;
		top_left_y = val.top_left_y;
		top_right_x = val.top_right_x;
		top_right_y = val.top_right_y;
		bottom_right_x = val.bottom_right_x;
		bottom_right_y = val.bottom_right_y;
		bottom_left_x = val.bottom_left_x;
		bottom_left_y = val.bottom_left_y;
		return *this;
	}
	void operator += (const Margins& mg)
	{
		top_left_x += mg.left;
		top_left_y += mg.top;
		top_right_x += mg.right;
		top_right_y += mg.top;
		bottom_right_x += mg.right;
		bottom_right_y += mg.bottom;
		bottom_left_x += mg.left;
		bottom_left_y += mg.bottom;
		fix_values();
	}
	void operator -= (const Margins& mg)
	{
		top_left_x -= mg.left;
		top_left_y -= mg.top;
		top_right_x -= mg.right;
		top_right_y -= mg.top;
		bottom_right_x -= mg.right;
		bottom_right_y -= mg.bottom;
		bottom_left_x -= mg.left;
		bottom_left_y -= mg.bottom;
		fix_values();
	}
	void fix_values()
	{
		if (top_left_x < 0)	top_left_x = 0;
		if (top_left_y < 0)	top_left_y = 0;
		if (top_right_x < 0) top_right_x = 0;
		if (bottom_right_x < 0) bottom_right_x = 0;
		if (bottom_right_y < 0) bottom_right_y = 0;
		if (bottom_left_x < 0) bottom_left_x = 0;
		if (bottom_left_y < 0) bottom_left_y = 0;
	}
};

struct CssBorderRadius
{
	CssLength	top_left_x;
	CssLength	top_left_y;

	CssLength	top_right_x;
	CssLength	top_right_y;

	CssLength	bottom_right_x;
	CssLength	bottom_right_y;

	CssLength	bottom_left_x;
	CssLength	bottom_left_y;

	CssBorderRadius()
	{

	}

	CssBorderRadius(const CssBorderRadius& val)
	{
		top_left_x		= val.top_left_x;
		top_left_y		= val.top_left_y;
		top_right_x		= val.top_right_x;
		top_right_y		= val.top_right_y;
		bottom_left_x	= val.bottom_left_x;
		bottom_left_y	= val.bottom_left_y;
		bottom_right_x	= val.bottom_right_x;
		bottom_right_y	= val.bottom_right_y;
	}

	CssBorderRadius& operator=(const CssBorderRadius& val)
	{
		top_left_x		= val.top_left_x;
		top_left_y		= val.top_left_y;
		top_right_x		= val.top_right_x;
		top_right_y		= val.top_right_y;
		bottom_left_x	= val.bottom_left_x;
		bottom_left_y	= val.bottom_left_y;
		bottom_right_x	= val.bottom_right_x;
		bottom_right_y	= val.bottom_right_y;
		return *this;
	}
	BorderRadiuses CalcPercents(int width, int height)
	{
		BorderRadiuses ret;
		ret.bottom_left_x = bottom_left_x.CalcPercent(width);
		ret.bottom_left_y = bottom_left_y.CalcPercent(height);
		ret.top_left_x = top_left_x.CalcPercent(width);
		ret.top_left_y = top_left_y.CalcPercent(height);
		ret.top_right_x = top_right_x.CalcPercent(width);
		ret.top_right_y = top_right_y.CalcPercent(height);
		ret.bottom_right_x = bottom_right_x.CalcPercent(width);
		ret.bottom_right_y = bottom_right_y.CalcPercent(height);
		return ret;
	}
};

struct CssBorders
{
	CssBorder			left;
	CssBorder			top;
	CssBorder			right;
	CssBorder			bottom;
	CssBorderRadius	radius;

	CssBorders()
	{

	}

	CssBorders(const CssBorders& val)
	{
		left	= val.left;
		right	= val.right;
		top		= val.top;
		bottom	= val.bottom;
		radius	= val.radius;
	}

	CssBorders& operator=(const CssBorders& val)
	{
		left	= val.left;
		right	= val.right;
		top		= val.top;
		bottom	= val.bottom;
		radius	= val.radius;
		return *this;
	}
};

struct Borders
{
	Border			left;
	Border			top;
	Border			right;
	Border			bottom;
	BorderRadiuses	radius;

	Borders()
	{

	}

	Borders(const Borders& val)
	{
		left = val.left;
		right = val.right;
		top = val.top;
		bottom = val.bottom;
		radius = val.radius;
	}

	Borders(const CssBorders& val)
	{
		left = val.left;
		right = val.right;
		top = val.top;
		bottom = val.bottom;
	}

	Borders& operator=(const Borders& val)
	{
		left = val.left;
		right = val.right;
		top = val.top;
		bottom = val.bottom;
		radius = val.radius;
		return *this;
	}

	Borders& operator=(const CssBorders& val)
	{
		left = val.left;
		right = val.right;
		top = val.top;
		bottom = val.bottom;
		return *this;
	}
};

END_HTMLCTRL_NAMESPACE

