#include "Html.h"
#include "ElImage.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElImage::ElImage( Document& doc ) : HtmlTag(doc)
{
	m_display = display_inline_block;
	cache_id = -1;
}

ElImage::~ElImage( void )
{

}

void ElImage::GetContentSize( Size& sz, int max_width )
{
	GetDocument()->GetHtmlFile()->GetImageSize(m_src, "", sz);
}

int ElImage::LineHeight() const
{
	return Height();
}

bool ElImage::IsReplaced() const
{
	return true;
}

int ElImage::Render( int x, int y, int max_width, bool second_pass )
{
	int higher_width = max_width;

	CalcOutlines(higher_width);

	m_pos.MoveTo(x, y);

	Upp::Size sz;
	GetDocument()->GetHtmlFile()->GetImageSize(m_src, "", sz);

	m_pos.width		= sz.cx;
	m_pos.height	= sz.cy;

	if(m_css_height.IsPredefined() && m_css_width.IsPredefined())
	{
		m_pos.height	= sz.cy;
		m_pos.width		= sz.cx;

		// check for max-height
		if(!m_css_max_width.IsPredefined())
		{
			int max_width = GetDocument()->CvtUnits(m_css_max_width, m_font_size, higher_width);
			if(m_pos.width > max_width)
			{
				m_pos.width = max_width;
			}
			if(sz.cx)
			{
				m_pos.height = (int) ((float) m_pos.width * (float) sz.cy / (float)sz.cx);
			} else
			{
				m_pos.height = sz.cy;
			}
		}

		// check for max-height
		if(!m_css_max_height.IsPredefined())
		{
			int max_height = GetDocument()->CvtUnits(m_css_max_height, m_font_size);
			if(m_pos.height > max_height)
			{
				m_pos.height = max_height;
			}
			if(sz.cy)
			{
				m_pos.width = (int) (m_pos.height * (float)sz.cx / (float)sz.cy);
			} else
			{
				m_pos.width = sz.cx;
			}
		}
	} else if(!m_css_height.IsPredefined() && m_css_width.IsPredefined())
	{
		if (!GetPredefinedHeight(m_pos.height))
		{
			m_pos.height = (int)m_css_height.Value();
		}

		// check for max-height
		if(!m_css_max_height.IsPredefined())
		{
			int max_height = GetDocument()->CvtUnits(m_css_max_height, m_font_size);
			if(m_pos.height > max_height)
			{
				m_pos.height = max_height;
			}
		}

		if(sz.cy)
		{
			m_pos.width = (int) (m_pos.height * (float)sz.cx / (float)sz.cy);
		} else
		{
			m_pos.width = sz.cx;
		}
	} else if(m_css_height.IsPredefined() && !m_css_width.IsPredefined())
	{
		m_pos.width = (int) m_css_width.CalcPercent(higher_width);

		// check for max-width
		if(!m_css_max_width.IsPredefined())
		{
			int max_width = GetDocument()->CvtUnits(m_css_max_width, m_font_size, higher_width);
			if(m_pos.width > max_width)
			{
				m_pos.width = max_width;
			}
		}

		if(sz.cx)
		{
			m_pos.height = (int) ((float) m_pos.width * (float) sz.cy / (float)sz.cx);
		} else
		{
			m_pos.height = sz.cy;
		}
	} else
	{
		m_pos.width		= (int) m_css_width.CalcPercent(higher_width);
		m_pos.height	= 0;
		if (!GetPredefinedHeight(m_pos.height))
		{
			m_pos.height = (int)m_css_height.Value();
		}

		// check for max-height
		if(!m_css_max_height.IsPredefined())
		{
			int max_height = GetDocument()->CvtUnits(m_css_max_height, m_font_size);
			if(m_pos.height > max_height)
			{
				m_pos.height = max_height;
			}
		}

		// check for max-height
		if(!m_css_max_width.IsPredefined())
		{
			int max_width = GetDocument()->CvtUnits(m_css_max_width, m_font_size, higher_width);
			if(m_pos.width > max_width)
			{
				m_pos.width = max_width;
			}
		}
	}

	CalcAutoMargins(higher_width);

	m_pos.x	+= ContentMarginsLeft();
	m_pos.y += ContentMarginsTop();

	return m_pos.width + ContentMarginsLeft() + ContentMarginsRight();
}

void ElImage::ParseAttributes()
{
	m_src = GetAttr("src", "");

	String attr_height = GetAttr("height");
	if(attr_height.GetCount()) {
		m_style.AddProperty("height", attr_height, "", false);
	}
	String attr_width = GetAttr("height");
	if (attr_width.GetCount()) {
		m_style.AddProperty("width", attr_width, "", false);
	}
}

void ElImage::Paint( Draw& hdc, int x, int y, const Position& clip )
{
	Position pos = m_pos;
	pos.x += x;
	pos.y += y;

	Position el_pos = pos;
	el_pos += m_padding;
	el_pos += m_borders;

	// draw standard background here
	if (el_pos.DoesIntersect(clip))
	{
		Background* bg = GetBackground();
		if (bg)
		{
			BackgroundPaint bg_paint;
			InitBackgroundPaint(pos, bg_paint, bg);

			GetDocument()->GetHtmlFile()->DrawBackground(hdc, bg_paint);
		}
	}

	// draw image as background
	if(pos.DoesIntersect(clip))
	{
		BackgroundPaint bg;
		bg.cache_id				= cache_id;
		if (cache_id == -1)
			bg.image			= m_src;
		bg.clip_box				= pos;
		bg.origin_box			= pos;
		bg.border_box			= pos;
		bg.border_box			+= m_padding;
		bg.border_box			+= m_borders;
		bg.repeat				= background_repeat_no_repeat;
		bg.image_size.cx		= pos.width;
		bg.image_size.cy		= pos.height;
		bg.border_radius		= m_css_borders.radius.CalcPercents(bg.border_box.width, bg.border_box.height);
		bg.position_x			= pos.x;
		bg.position_y			= pos.y;
		GetDocument()->GetHtmlFile()->DrawBackground(hdc, bg);
	}

	// draw Borders
	if (el_pos.DoesIntersect(clip))
	{
		Position border_box = pos;
		border_box += m_padding;
		border_box += m_borders;

		Borders bdr = m_css_borders;
		bdr.radius = m_css_borders.radius.CalcPercents(border_box.width, border_box.height);

		GetDocument()->GetHtmlFile()->DrawBorders(hdc, bdr, border_box, GetParentElement() ? false : true);
	}
}

void ElImage::ParseStyles( bool is_reparse /*= false*/ )
{
	HtmlTag::ParseStyles(is_reparse);

	if(!m_src.IsEmpty() && cache_id == -1)
	{
		if(!m_css_height.IsPredefined() && !m_css_width.IsPredefined())
		{
			GetDocument()->GetHtmlFile()->LoadImage(m_src, "", true);
		} else
		{
			GetDocument()->GetHtmlFile()->LoadImage(m_src, "", false);
		}
		cache_id = GetCache().FindCacheObject(m_src, GetBaseUrl());
	}
}

END_HTMLCTRL_NAMESPACE
