#pragma once

#include <Core/Core.h>
using namespace Upp;
#include "Object.h"

NAMESPACE_HTMLCTRL

class Document;
class Element;

typedef ArrayMap<String, String>							StrMap;
typedef Vector< ObjectPtr<Element> >						ElementsVector;
typedef Vector<int>											IntVector;
typedef Vector<String>										StringVector;

inline int FindPos(const ElementsVector& vec, const ObjectPtr<Element>& el) {
	for(int i = 0; i < vec.GetCount(); i++) {
		if (((const Element*)vec[i]) == (const Element*)el)
			return i;
	}
	return -1;
}

const unsigned int font_decoration_none			= 0x00;
const unsigned int font_decoration_underline	= 0x01;
const unsigned int font_decoration_linethrough	= 0x02;
const unsigned int font_decoration_overline		= 0x04;

typedef unsigned char	byte;
typedef unsigned int	ucode_t;

struct Margins
{
	int	left;
	int	right;
	int top;
	int bottom;

	Margins()
	{
		left = right = top = bottom = 0;
	}

	int Width()		const	{ return left + right; } 
	int Height()	const	{ return top + bottom; } 
};

/*
struct size
{
	int		width;
	int		height;

	size()
	{
		width	= 0;
		height	= 0;
	}
};*/

struct Position : public Moveable<Position>
{
	typedef Vector<Position>	vector;

	int	x;
	int	y;
	int	width;
	int	height;
	
	operator Rect() const {return RectC(x, y, width, height);}
	
	Position()
	{
		x =0;
		y = 0;
		width = 0;
		height = 0;
	}

	Position(int x, int y, int width, int height)
	{
		this->x			= x;
		this->y			= y;
		this->width		= width;
		this->height	= height;
	}
	
	Rect GetRect() const {return RectC(x, y, width, height);}

	int Right()		const		{ return x + width;		}
	int Bottom()	const		{ return y + height;	}
	int Left()		const		{ return x;				}
	int Top()		const		{ return y;				}

	void operator+=(const Margins& mg)
	{
		x		-= mg.left;
		y		-= mg.top;
		width	+= mg.left + mg.right;
		height	+= mg.top + mg.bottom;
	}
	void operator-=(const Margins& mg)
	{
		x		+= mg.left;
		y		+= mg.top;
		width	-= mg.left + mg.right;
		height	-= mg.top + mg.bottom;
	}

	void Clear()
	{
		x = y = width = height = 0;
	}

	void operator=(const Size& sz)
	{
		width	= sz.cx;
		height	= sz.cy;
	}

	void MoveTo(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	bool DoesIntersect() const {return true;}
	bool DoesIntersect(const Position& val) const {
		//if(!val) return true;

		return (
			Left()			<= val.Right()		&& 
			Right()			>= val.Left()		&& 
			Bottom()		>= val.Top()		&& 
			Top()			<= val.Bottom()	)
			|| (
			val.Left()		<= Right()			&& 
			val.Right()		>= Left()			&& 
			val.Bottom()	>= Top()			&& 
			val.Top()		<= Bottom()			);
	}

	bool Empty() const
	{
		if(!width && !height)
		{
			return true;
		}
		return false;
	}

	bool IsPointInside(int x, int y) const
	{
		if(x >= Left() && x <= Right() && y >= Top() && y <= Bottom())
		{
			return true;
		}
		return false;
	}
};

struct FontMetrics
{
	int		height;
	int		ascent;
	int		descent;
	int		x_height;
	bool	draw_spaces;

	FontMetrics()
	{
		height			= 0;
		ascent			= 0;
		descent			= 0;
		x_height		= 0;
		draw_spaces		= true;
	}
	int BaseLine()	{ return descent; }
};

struct FontItem
{
	uint32		font;
	FontMetrics	metrics;
};

typedef ArrayMap<String, FontItem>	FontsMap;

enum DrawFlag
{
	draw_root,
	draw_block,
	draw_floats,
	draw_inlines,
	draw_positioned,
};

#define  style_display_strings		"none;block;inline;inline-block;inline-table;list-item;table;table-caption;table-cell;table-column;table-column-group;table-footer-group;table-header-group;table-row;table-row-group"

enum StyleDisplay
{
	display_none,
	display_block,
	display_inline,
	display_inline_block,
	display_inline_table,
	display_list_item,
	display_table,
	display_table_caption,
	display_TableCell,
	display_TableColumn,
	display_TableColumn_group,
	display_table_footer_group,
	display_table_header_group,
	display_TableRow,
	display_TableRow_group,
	display_inline_text,
};

enum StyleBorder
{
	BorderNope,
	BorderNone,
	BorderHidden,
	BorderDotted,
	BorderDashed,
	BorderSolid,
	BorderDouble
};

#define  FontSize_strings		"xx-small;x-small;small;medium;large;x-large;xx-large;smaller;larger"

enum FontSize
{
	fontSize_xx_small,
	fontSize_x_small,
	fontSize_small,
	fontSize_medium,
	fontSize_large,
	fontSize_x_large,
	fontSize_xx_large,
	fontSize_smaller,
	fontSize_larger,
};

#define  Fontstyle_strings		"normal;italic"

enum FontStyle
{
	fontStyleNormal,
	fontStyleItalic
};

#define  FontVariant_strings		"normal;small-caps"

enum FontVariant
{
	FontVariant_normal,
	FontVariant_italic
};

#define  FontWeight_strings	"normal;bold;bolder;lighter100;200;300;400;500;600;700"

enum FontWeight
{
	fontWeightNormal,
	fontWeightBold,
	fontWeightBolder,
	fontWeightLighter,
	fontWeight100,
	fontWeight200,
	fontWeight300,
	fontWeight400,
	fontWeight500,
	fontWeight600,
	fontWeight700
};

#define  list_style_type_strings	"none;circle;disc;square;armenian;cjk-ideographic;decimal;decimal-leading-zero;georgian;hebrew;hiragana;hiragana-iroha;katakana;katakana-iroha;lower-alpha;lower-greek;lower-latin;lower-roman;upper-alpha;upper-latin;upper-roman"

enum ListStyleType
{
	list_style_type_none,
	list_style_type_circle,
	list_style_type_disc,
	list_style_type_square,
	list_style_type_armenian,
	list_style_type_cjk_ideographic,
	list_style_type_decimal,
	list_style_type_decimal_leading_zero,
	list_style_type_georgian,
	list_style_type_hebrew,
	list_style_type_hiragana,
	list_style_type_hiragana_iroha,
	list_style_type_katakana,
	list_style_type_katakana_iroha,
	list_style_type_lower_alpha,
	list_style_type_lower_greek,
	list_style_type_lower_latin,
	list_style_type_lower_roman,
	list_style_type_upper_alpha,
	list_style_type_upper_latin,
	list_style_type_upper_roman,
};

#define  ListStyleposition_strings	"inside;outside"

enum ListStylePosition
{
	ListStyleposition_inside,
	ListStyleposition_outside
};

#define  VerticalAlign_strings	"baseline;sub;super;top;text-top;middle;bottom;text-bottom"

enum VerticalAlign
{
	va_baseline,
	va_sub,
	va_super,
	va_top,
	va_text_top,
	va_middle,
	va_bottom,
	va_text_bottom
};

#define  border_width_strings	"thin;medium;thick"

enum border_width
{
	border_width_thin,
	border_width_medium,
	border_width_thick
};

#define  border_style_strings	"none;hidden;dotted;dashed;solid;double;groove;ridge;inset;outset"

enum BorderStyle
{
	border_style_none,
	border_style_hidden,
	border_style_dotted,
	border_style_dashed,
	border_style_solid,
	border_style_double,
	border_style_groove,
	border_style_ridge,
	border_style_inset,
	border_style_outset
};

#define  ElementFloat_strings	"none;left;right"

enum ElementFloat
{
	float_none,
	float_left,
	float_right
};

#define  ElementClear_strings	"none;left;right;both"

enum ElementClear
{
	clear_none,
	clear_left,
	clear_right,
	clear_both
};

#define  css_units_strings	"none;%;in;cm;mm;em;ex;pt;pc;px;dpi;dpcm;vw;vh;vmin;vmax"

enum CssUnits
{
	css_units_none,
	css_units_percentage,
	css_units_in,
	css_units_cm,
	css_units_mm,
	css_units_em,
	css_units_ex,
	css_units_pt,
	css_units_pc,
	css_units_px,
	css_units_dpi,
	css_units_dpcm,
	css_units_vw,
	css_units_vh,
	css_units_vmin,
	css_units_vmax,
};

#define  BackgroundAttachment_strings	"scroll;fixed"

enum BackgroundAttachment
{
	BackgroundAttachment_scroll,
	BackgroundAttachment_fixed
};

#define  background_repeat_strings	"repeat;repeat-x;repeat-y;no-repeat"

enum BackgroundRepeat
{
	background_repeat_repeat,
	background_repeat_repeat_x,
	background_repeat_repeat_y,
	background_repeat_no_repeat
};

#define  background_box_strings	"border-box;padding-box;content-box"

enum BackgroundBox
{
	background_box_border,
	background_box_padding,
	background_box_content
};

#define element_position_strings	"static;relative;absolute;fixed"

enum ElementPosition
{
	element_position_static,
	element_position_relative,
	element_position_absolute,
	element_position_fixed,
};

#define text_align_strings		"left;right;center;justify"

enum text_align
{
	text_align_left,
	text_align_right,
	text_align_center,
	text_align_justify
};

#define TextTransform_strings		"none;capitalize;uppercase;lowercase"

enum TextTransform
{
	TextTransform_none,
	TextTransform_capitalize,
	TextTransform_uppercase,
	TextTransform_lowercase
};

#define white_space_strings		"normal;nowrap;pre;pre-line;pre-wrap"

enum WhiteSpace
{
	white_space_normal,
	white_space_nowrap,
	white_space_pre,
	white_space_pre_line,
	white_space_pre_wrap
};

#define overflow_strings		"visible;hidden;scroll;auto;no-display;no-content"

enum Overflow
{
	overflow_visible,
	overflow_hidden,
	overflow_scroll,
	overflow_auto,
	overflow_no_display,
	overflow_no_content
};

#define background_size_strings		"auto;cover;contain"

enum BackgroundSize
{
	background_size_auto,
	background_size_cover,
	background_size_contain,
};

#define visibility_strings			"visible;hidden;collapse"

enum Visibility
{
	visibility_visible,
	visibility_hidden,
	visibility_collapse,
};

#define border_collapse_strings		"collapse;separate"

enum BorderCollapse
{
	border_collapse_collapse,
	border_collapse_separate,
};


#define PseudoClass_strings		"only-child;only-of-type;first-child;first-of-type;last-child;last-of-type;nth-child;nth-of-type;nth-last-child;nth-last-of-type;not"

enum PseudoClass
{
	PseudoClass_only_child,
	PseudoClass_only_of_type,
	PseudoClass_first_child,
	PseudoClass_first_of_type,
	PseudoClass_last_child,
	PseudoClass_last_of_type,
	PseudoClass_nth_child,
	PseudoClass_nth_of_type,
	PseudoClass_nth_last_child,
	PseudoClass_nth_last_of_type,
	PseudoClass_not,
};

#define ContentProperty_string		"none;normal;open-quote;close-quote;no-open-quote;no-close-quote"

enum ContentProperty
{
	ContentProperty_none,
	ContentProperty_normal,
	ContentProperty_open_quote,
	ContentProperty_close_quote,
	ContentProperty_no_open_quote,
	ContentProperty_no_close_quote,
};


struct FloatedBox : public Moveable<FloatedBox>
{
	typedef Vector<FloatedBox>	vector;

	Position		pos;
	ElementFloat	float_side;
	ElementClear	clear_floats;
	Element*		el;
};

struct IntIntCache
{
	int		hash;
	int		val;
	bool	is_valid;
	bool	is_default;

	IntIntCache()
	{
		hash		= 0;
		val			= 0;
		is_valid	= false;
		is_default	= false;
	}
	void invalidate()
	{
		is_valid	= false;
		is_default	= false;
	}
	void SetValue(int vHash, int vVal)
	{
		hash		= vHash;
		val			= vVal;
		is_valid	= true;
	}
};

enum SelectResult
{
	select_no_match				= 0x00,
	select_match				= 0x01,
	select_match_pseudo_class	= 0x02,
	select_match_with_before	= 0x10,
	select_match_with_after		= 0x20,
};

template<class T>
class DefValue
{
	T		m_val;
	bool	m_is_default;
public:
	DefValue(T def_val)
	{
		m_is_default	= true;
		m_val			= def_val;
	}
	void reset(T def_val)
	{
		m_is_default	= true;
		m_val			= def_val;
	}
	bool is_default()
	{
		return m_is_default;
	}
	T operator=(T new_val)
	{
		m_val			= new_val;
		m_is_default	= false;
		return m_val;
	}
	operator T()
	{
		return m_val;
	}
};


#define MediaOrientation_strings		"portrait;landscape"

enum MediaOrientation
{
	MediaOrientation_portrait,
	MediaOrientation_landscape,
};

#define media_feature_strings		"none;width;min-width;max-width;height;min-height;max-height;device-width;min-device-width;max-device-width;device-height;min-device-height;max-device-height;orientation;aspect-ratio;min-aspect-ratio;max-aspect-ratio;device-aspect-ratio;min-device-aspect-ratio;max-device-aspect-ratio;color;min-color;max-color;color-index;min-color-index;max-color-index;monochrome;min-monochrome;max-monochrome;resolution;min-resolution;max-resolution"

enum MediaFeature
{
	media_feature_none,

	media_feature_width,
	media_feature_min_width,
	media_feature_max_width,

	media_feature_height,
	media_feature_min_height,
	media_feature_max_height,

	media_feature_device_width,
	media_feature_min_device_width,
	media_feature_max_device_width,

	media_feature_device_height,
	media_feature_min_device_height,
	media_feature_max_device_height,

	media_feature_orientation,

	media_feature_aspect_ratio,
	media_feature_min_aspect_ratio,
	media_feature_max_aspect_ratio,

	media_feature_device_aspect_ratio,
	media_feature_min_device_aspect_ratio,
	media_feature_max_device_aspect_ratio,

	media_feature_color,
	media_feature_min_color,
	media_feature_max_color,

	media_feature_color_index,
	media_feature_min_color_index,
	media_feature_max_color_index,

	media_feature_monochrome,
	media_feature_min_monochrome,
	media_feature_max_monochrome,

	media_feature_resolution,
	media_feature_min_resolution,
	media_feature_max_resolution,
};

#define box_sizing_strings		"content-box;border-box"

enum BoxSizing
{
	box_sizing_content_box,
	box_sizing_border_box,
};


#define media_type_strings		"none;all;screen;print;braille;embossed;handheld;projection;speech;tty;tv"

enum MediaType
{
	media_type_none,
	media_type_all,
	media_type_screen,
	media_type_print,
	media_type_braille,
	media_type_embossed,
	media_type_handheld,
	media_type_projection,
	media_type_speech,
	media_type_tty,
	media_type_tv,
};

struct MediaFeatures
{
	MediaType	type;
	int			width;			// (pixels) For continuous media, this is the width of the viewport including the size of a rendered scroll bar (if any). For paged media, this is the width of the page Box.
	int			height;			// (pixels) The height of the targeted display area of the output device. For continuous media, this is the height of the viewport including the size of a rendered scroll bar (if any). For paged media, this is the height of the page Box.
	int			device_width;	// (pixels) The width of the rendering surface of the output device. For continuous media, this is the width of the screen. For paged media, this is the width of the page sheet size.
	int			device_height;	// (pixels) The height of the rendering surface of the output device. For continuous media, this is the height of the screen. For paged media, this is the height of the page sheet size.
	int			color;			// The number of bits per color component of the output device. If the device is not a color device, the value is zero.
	int			color_index;	// The number of entries in the color lookup table of the output device. If the device does not use a color lookup table, the value is zero.
	int			monochrome;		// The number of bits per pixel in a monochrome frame buffer. If the device is not a monochrome device, the output device value will be 0.
	int			resolution;		// The resolution of the output device (in DPI)
};

enum RenderType
{
	render_all,
	render_no_fixed,
	render_fixed_only,
};

// List of the Void Elements (can't have any contents)
const char* const void_elements = "area;base;br;col;command;embed;hr;img;input;keygen;link;meta;param;source;track;wbr";


END_HTMLCTRL_NAMESPACE
