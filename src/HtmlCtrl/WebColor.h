#pragma once

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include <HtmlCtrl/Common.h>

NAMESPACE_HTMLCTRL

struct DefColor
{
	String	name;
	String	rgb;
};

extern DefColor g_DefColors[];

struct WebColor
{
	byte    blue;
	byte    green;
	byte    red;
	byte    alpha;

	WebColor(byte r, byte g, byte b, byte a = 255)
	{
		blue	= b;
		green	= g;
		red		= r;
		alpha	= a;
	}

	WebColor()
	{
		blue	= 0;
		green	= 0;
		red		= 0;
		alpha	= 0xFF;
	}

	WebColor(const WebColor& val)
	{
		blue	= val.blue;
		green	= val.green;
		red		= val.red;
		alpha	= val.alpha;
	}

	WebColor& operator=(const WebColor& val)
	{
		blue	= val.blue;
		green	= val.green;
		red		= val.red;
		alpha	= val.alpha;
		return *this;
	}
	static WebColor			FromString(const String& str);
	static String			ResolveName(const String& name);
	static bool				IsColor(const String& str);
	
	operator Color () const {return Color(red, green, blue);}
	
	WebColor& operator=(const Color& c) {blue = c.GetB(); green = c.GetG(); red = c.GetR(); alpha = 255; return *this;}
};

END_HTMLCTRL_NAMESPACE
