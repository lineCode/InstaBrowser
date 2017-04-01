#include "Html.h"
#include "Background.h"

NAMESPACE_HTMLCTRL

Background::Background(void)
{
	m_attachment	= BackgroundAttachment_scroll;
	m_repeat		= background_repeat_repeat;
	m_clip			= background_box_border;
	m_origin		= background_box_padding;
	m_color.alpha	= 0;
	m_color.red		= 0;
	m_color.green	= 0;
	m_color.blue	= 0;
	cache_id		= -1;
}

Background::Background( const Background& val )
{
	m_image			= val.m_image;
	m_baseurl		= val.m_baseurl;
	m_color			= val.m_color;
	m_attachment	= val.m_attachment;
	m_position		= val.m_position;
	m_repeat		= val.m_repeat;
	m_clip			= val.m_clip;
	m_origin		= val.m_origin;
	cache_id		= -1;
}

Background::~Background(void)
{
}

Background& Background::operator=( const Background& val )
{
	m_image			= val.m_image;
	m_baseurl		= val.m_baseurl;
	m_color			= val.m_color;
	m_attachment	= val.m_attachment;
	m_position		= val.m_position;
	m_repeat		= val.m_repeat;
	m_clip			= val.m_clip;
	m_origin		= val.m_origin;
	return *this;
}


BackgroundPaint::BackgroundPaint() : color(0, 0, 0, 0)
{
	position_x		= 0;
	position_y		= 0;
	attachment		= BackgroundAttachment_scroll;
	repeat			= background_repeat_repeat;
	is_root			= false;
	cache_id		= -1;
}

BackgroundPaint::BackgroundPaint( const BackgroundPaint& val )
{
	image			= val.image;
	baseurl			= val.baseurl;
	attachment		= val.attachment;
	repeat			= val.repeat;
	color			= val.color;
	clip_box		= val.clip_box;
	origin_box		= val.origin_box;
	border_box		= val.border_box;
	border_radius	= val.border_radius;
	image_size		= val.image_size;
	position_x		= val.position_x;
	position_y		= val.position_y;
	is_root			= val.is_root;
}

void BackgroundPaint::operator=( const Background& val )
{
	attachment	= val.m_attachment;
	baseurl		= val.m_baseurl;
	image		= val.m_image;
	repeat		= val.m_repeat;
	color		= val.m_color;
	cache_id	= val.cache_id;
}

END_HTMLCTRL_NAMESPACE
