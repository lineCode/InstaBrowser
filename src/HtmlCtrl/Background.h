#pragma once

#include <Core/Core.h>
using namespace Upp;

#include "Types.h"
#include "Attributes.h"
#include "CssLength.h"
#include "CssPosition.h"
#include "WebColor.h"
#include "Borders.h"

NAMESPACE_HTMLCTRL

class Background
{
public:
	int						cache_id;
	String					m_image;
	String					m_baseurl;
	WebColor				m_color;
	BackgroundAttachment	m_attachment;
	CssPosition				m_position;
	BackgroundRepeat		m_repeat;
	BackgroundBox			m_clip;
	BackgroundBox			m_origin;
	CssBorderRadius			m_radius;

public:
	Background(void);
	Background(const Background& val);
	~Background(void);

	Background& operator=(const Background& val);
};

class BackgroundPaint
{
public:
	int						cache_id;
	String					image;
	String					baseurl;
	BackgroundAttachment	attachment;
	BackgroundRepeat		repeat;
	WebColor				color;
	Position				clip_box;
	Position				origin_box;
	Position				border_box;
	BorderRadiuses			border_radius;
	Size					image_size;
	int						position_x;
	int						position_y;
	bool					is_root;
public:
	BackgroundPaint();
	BackgroundPaint(const BackgroundPaint& val);
	void operator=(const Background& val);
};


END_HTMLCTRL_NAMESPACE
