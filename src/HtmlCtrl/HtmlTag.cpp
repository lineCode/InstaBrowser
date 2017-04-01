#include "Html.h"
#include "HtmlTag.h"
#include "Document.h"
#include "Iterators.h"
#include "Stylesheet.h"
#include "Table.h"
#include <Core/Core.h>
using namespace Upp;

#include "ElBeforeAfter.h"

#include "ElBody.h"

NAMESPACE_HTMLCTRL

HtmlTag::HtmlTag(Document& doc) : m_style(doc.GetObjectPool()), Element(doc) {Reset();}

void HtmlTag::Reset()
{
	m_box_sizing			= box_sizing_content_box;
	m_z_index				= 0;
	m_overflow				= overflow_visible;
	m_box					= 0;
	m_text_align			= text_align_left;
	m_el_position			= element_position_static;
	m_display				= display_inline;
	m_vertical_align			= va_baseline;
	m_list_style_type			= list_style_type_none;
	m_list_style_position		= ListStyleposition_outside;
	m_float					= float_none;
	m_clear					= clear_none;
	m_font					= 0;
	m_font_size				= 0;
	m_white_space			= white_space_normal;
	m_lh_predefined			= false;
	m_line_height			= 0;
	m_visibility			= visibility_visible;
	m_border_spacing_x		= 0;
	m_border_spacing_y		= 0;
	m_borderCollapse		= border_collapse_separate;
}

HtmlTag::~HtmlTag()
{
	
}

bool HtmlTag::AppendChild( Element* el )
{
	if(el)
	{
		el->MoveTo(*this);
		m_children.Add(el);
		return true;
	}
	return false;
}

String HtmlTag::GetTagName() const
{
	return m_tag;
}

void HtmlTag::SetAttr( const String& name, const String& val ) {
	if(name.GetCount() && val.GetCount()) {
		String tmp;
		for(size_t i = 0; i < name.GetCount(); i++)
		{
			tmp.Cat( ToLower(name[i]) );
		}
		m_attrs.GetAdd(tmp) = val;
	}
}

String HtmlTag::GetAttr( const String& name, const String& def )
{
	int i = m_attrs.Find(name);
	if (i != -1)
		return m_attrs[i];
	return def;
}

Element::ptr HtmlTag::SelectOne( const String& selector )
{
	CssSelector sel(GetObjectPool(), MediaQueryList::ptr(0));
	sel.Parse(selector);

	return SelectOne(sel);
}

Element::ptr HtmlTag::SelectOne( const CssSelector& selector )
{
	if(Select(selector))
	{
		return this;
	}

	for(ElementsVector::Iterator el = m_children.Begin(); el != m_children.End(); el++)
	{
		Element::ptr res = (*el)->SelectOne(selector);
		if(res)
		{
			return res;
		}
	}
	return 0;
}

void HtmlTag::ApplyStylesheet( const Css& stylesheet )
{
	RemoveBeforeAfter();

	for (int i = 0; i < stylesheet.Selectors().GetCount(); i++) {
		const CssSelector::ptr& sel = stylesheet.Selectors()[i];
		
		int apply = Select(*sel, false);

		if(apply != select_no_match)
		{
			UsedSelector::ptr us = new UsedSelector(sel, false);
			m_used_styles.Add(us);

			if(sel->IsMediaValid())
			{
				if(apply & select_match_pseudo_class)
				{
					if(Select(*sel, true))
					{
						AddStyle(sel->m_style);
						us->m_used = true;
					}
				} else if(apply & select_match_with_after)
				{
					Element* el = GetElementAfter();
					if(el)
					{
						el->AddStyle(sel->m_style);
					}
				} else if(apply & select_match_with_before)
				{
					Element* el = GetElementBefore();
					if(el)
					{
						el->AddStyle(sel->m_style);
					}
				} else
				{
					AddStyle(sel->m_style);
					us->m_used = true;
				}
			}
		}
	}

	for (int j = 0; j < m_children.GetCount(); j++) 
	{
		ObjectPtr<Element>& el = m_children[j];
		if(el->GetDisplay() != display_inline_text) {
			el->ApplyStylesheet(stylesheet);
		}
	}
}

void HtmlTag::GetContentSize( Size& sz, int max_width )
{
	sz.cy	= 0;
	if(m_display == display_block)
	{
		sz.cx	= max_width;
	} else
	{
		sz.cx	= 0;
	}
}

void HtmlTag::Paint( Draw& hdc, int x, int y, const Position& clip )
{
	Position pos = m_pos;
	pos.x	+= x;
	pos.y	+= y;

	DrawBackground(hdc, x, y, clip);

	if(m_display == display_list_item && m_list_style_type != list_style_type_none)
	{
		if(m_overflow > overflow_visible)
		{
			Position border_box = pos;
			border_box += m_padding;
			border_box += m_borders;

			BorderRadiuses bdr_radius = m_css_borders.radius.CalcPercents(border_box.width, border_box.height);

			bdr_radius -= m_borders;
			bdr_radius -= m_padding;

			GetDocument()->GetHtmlFile()->SetClip(pos, bdr_radius, true, true);
		}

		DrawListMarker(hdc, pos);

		if(m_overflow > overflow_visible)
		{
			GetDocument()->GetHtmlFile()->DelClip();
		}
	}
}

uint32 HtmlTag::GetFont(FontMetrics* fm)
{
	if(fm)
	{
		*fm = m_font_metrics;
	}
	return m_font;
}
	

String HtmlTag::GetStyleProperty( const String& name, bool inherited, const String& def /*= 0*/ )
{
	String ret = m_style.GetProperty(name);
	bool pass_higher = false;
	
	
	Element* m_higher = GetParentElement();
	if(m_higher) {
	
	
		if(ret.GetCount() && !CompareNoCase(ret, "inherit"))
		{
			pass_higher = true;
		} else if(!ret.GetCount() && inherited)
		{
			pass_higher = true;
		}
	}
	if (pass_higher)
	{
			ret = m_higher->GetStyleProperty(name, inherited, def);
	}

	if(!ret.GetCount())
	{
		return def;
	}

	return ret;
}

int HtmlTag::GetStylePropertyCount() {
	return m_style.GetCount();
}

String HtmlTag::GetStylePropertyKey(int i) {
	return m_style.GetKey(i);
}

String HtmlTag::GetStylePropertyValue(int i) {
	return m_style.GetValue(i);
}

void HtmlTag::ParseStyles(bool is_reparse)
{
	m_id	= GetAttr("id", "");
	m_class	= GetAttr("class", "");

	String style = GetAttr("style");

	if(style.GetCount())
	{
		m_style.add(style, "");
	}

	InitFont();

	m_el_position	= (ElementPosition)	ValueIndex(GetStyleProperty("Position",		false,	"static"),		element_position_strings,	element_position_fixed);
	m_text_align	= (text_align)			ValueIndex(GetStyleProperty("text-align",		true,	"left"),		text_align_strings,			text_align_left);
	m_overflow		= (Overflow)			ValueIndex(GetStyleProperty("Overflow",		false,	"visible"),		overflow_strings,			overflow_visible);
	m_white_space	= (WhiteSpace)			ValueIndex(GetStyleProperty("white-space",	true,	"normal"),		white_space_strings,		white_space_normal);
	m_display		= (StyleDisplay)		ValueIndex(GetStyleProperty("display",		false,	"inline"),		style_display_strings,		display_inline);
	m_visibility	= (Visibility)			ValueIndex(GetStyleProperty("Visibility",	true,	"visible"),		visibility_strings,			visibility_visible);
	m_box_sizing	= (BoxSizing)			ValueIndex(GetStyleProperty("Box-sizing",		false,	"content-box"),	box_sizing_strings,			box_sizing_content_box);

	if(m_el_position != element_position_static)
	{
		String val = GetStyleProperty("z-index", false, "");
		if(val.GetCount())
		{
			m_z_index = StrInt(val);
		}
	}

	String va	= GetStyleProperty("vertical-align", true,	"baseline");
	m_vertical_align = (VerticalAlign) ValueIndex(va, VerticalAlign_strings, va_baseline);

	String fl	= GetStyleProperty("float", false,	"none");
	m_float = (ElementFloat) ValueIndex(fl, ElementFloat_strings, float_none);

	m_clear = (ElementClear) ValueIndex(GetStyleProperty("clear", false, "none"), ElementClear_strings, clear_none);
	
	Document* m_doc = GetDocument();
	
	if (m_float != float_none)
	{
		// reset display in to block for floating elements
		if (m_display != display_none)
		{
			m_display = display_block;
		}
	}
	else if (m_display == display_table ||
		m_display == display_table_caption ||
		m_display == display_TableCell ||
		m_display == display_TableColumn ||
		m_display == display_TableColumn_group ||
		m_display == display_table_footer_group ||
		m_display == display_table_header_group ||
		m_display == display_TableRow ||
		m_display == display_TableRow_group)
	{
		m_doc->AddTabular(this);
	}
	// fix inline Boxes with absolute/fixed Positions
	else if (m_display != display_none && IsInLineBox())
	{
		if (m_el_position == element_position_absolute || m_el_position == element_position_fixed)
		{
			m_display = display_block;
		}
	}

	m_css_text_indent.FromString(	GetStyleProperty("text-indent",	true,	"0"),	"0");

	m_css_width.FromString(			GetStyleProperty("width",			false, "auto"), "auto");
	m_css_height.FromString(		GetStyleProperty("height",			false, "auto"), "auto");

	m_doc->CvtUnits(m_css_width,	m_font_size);
	m_doc->CvtUnits(m_css_height,	m_font_size);

	m_css_min_width.FromString(		GetStyleProperty("min-width",		false,	"0"));
	m_css_min_height.FromString(	GetStyleProperty("min-height",		false,	"0"));

	m_css_max_width.FromString(		GetStyleProperty("max-width",		false,	"none"),	"none");
	m_css_max_height.FromString(	GetStyleProperty("max-height",		false,	"none"),	"none");
	
	m_doc->CvtUnits(m_css_min_width,	m_font_size);
	m_doc->CvtUnits(m_css_min_height,	m_font_size);

	m_css_offsets.left.FromString(		GetStyleProperty("left",				false,	"auto"), "auto");
	m_css_offsets.right.FromString(		GetStyleProperty("right",				false,	"auto"), "auto");
	m_css_offsets.top.FromString(		GetStyleProperty("top",				false,	"auto"), "auto");
	m_css_offsets.bottom.FromString(	GetStyleProperty("bottom",			false,	"auto"), "auto");

	m_doc->CvtUnits(m_css_offsets.left,		m_font_size);
	m_doc->CvtUnits(m_css_offsets.right,	m_font_size);
	m_doc->CvtUnits(m_css_offsets.top,		m_font_size);
	m_doc->CvtUnits(m_css_offsets.bottom,	m_font_size);

	m_css_margins.left.FromString(		GetStyleProperty("margin-left",			false,	"0"), "auto");
	m_css_margins.right.FromString(		GetStyleProperty("margin-right",		false,	"0"), "auto");
	m_css_margins.top.FromString(		GetStyleProperty("margin-top",			false,	"0"), "auto");
	m_css_margins.bottom.FromString(	GetStyleProperty("margin-bottom",		false,	"0"), "auto");

	m_css_padding.left.FromString(		GetStyleProperty("padding-left",		false,	"0"), "");
	m_css_padding.right.FromString(		GetStyleProperty("padding-right",		false,	"0"), "");
	m_css_padding.top.FromString(		GetStyleProperty("padding-top",			false,	"0"), "");
	m_css_padding.bottom.FromString(	GetStyleProperty("padding-bottom",		false,	"0"), "");

	m_css_borders.left.width.FromString(	GetStyleProperty("border-left-width",		false,	"medium"), border_width_strings);
	m_css_borders.right.width.FromString(	GetStyleProperty("border-right-width",		false,	"medium"), border_width_strings);
	m_css_borders.top.width.FromString(		GetStyleProperty("border-top-width",		false,	"medium"), border_width_strings);
	m_css_borders.bottom.width.FromString(	GetStyleProperty("border-bottom-width",		false,	"medium"), border_width_strings);

	m_css_borders.left.color = WebColor::FromString(GetStyleProperty("border-left-color",	false,	""));
	m_css_borders.left.style = (BorderStyle) ValueIndex(GetStyleProperty("border-left-style", false, "none"), border_style_strings, border_style_none);

	m_css_borders.right.color = WebColor::FromString(GetStyleProperty("border-right-color",	false,	""));
	m_css_borders.right.style = (BorderStyle) ValueIndex(GetStyleProperty("border-right-style", false, "none"), border_style_strings, border_style_none);

	m_css_borders.top.color = WebColor::FromString(GetStyleProperty("border-top-color",	false,	""));
	m_css_borders.top.style = (BorderStyle) ValueIndex(GetStyleProperty("border-top-style", false, "none"), border_style_strings, border_style_none);

	m_css_borders.bottom.color = WebColor::FromString(GetStyleProperty("border-bottom-color",	false,	""));
	m_css_borders.bottom.style = (BorderStyle) ValueIndex(GetStyleProperty("border-bottom-style", false, "none"), border_style_strings, border_style_none);

	m_css_borders.radius.top_left_x.FromString(GetStyleProperty("border-top-left-radius-x", false, "0"));
	m_css_borders.radius.top_left_y.FromString(GetStyleProperty("border-top-left-radius-y", false, "0"));

	m_css_borders.radius.top_right_x.FromString(GetStyleProperty("border-top-right-radius-x", false, "0"));
	m_css_borders.radius.top_right_y.FromString(GetStyleProperty("border-top-right-radius-y", false, "0"));

	m_css_borders.radius.bottom_right_x.FromString(GetStyleProperty("border-bottom-right-radius-x", false, "0"));
	m_css_borders.radius.bottom_right_y.FromString(GetStyleProperty("border-bottom-right-radius-y", false, "0"));

	m_css_borders.radius.bottom_left_x.FromString(GetStyleProperty("border-bottom-left-radius-x", false, "0"));
	m_css_borders.radius.bottom_left_y.FromString(GetStyleProperty("border-bottom-left-radius-y", false, "0"));

	m_doc->CvtUnits(m_css_borders.radius.bottom_left_x,			m_font_size);
	m_doc->CvtUnits(m_css_borders.radius.bottom_left_y,			m_font_size);
	m_doc->CvtUnits(m_css_borders.radius.bottom_right_x,			m_font_size);
	m_doc->CvtUnits(m_css_borders.radius.bottom_right_y,			m_font_size);
	m_doc->CvtUnits(m_css_borders.radius.top_left_x,				m_font_size);
	m_doc->CvtUnits(m_css_borders.radius.top_left_y,				m_font_size);
	m_doc->CvtUnits(m_css_borders.radius.top_right_x,				m_font_size);
	m_doc->CvtUnits(m_css_borders.radius.top_right_y,				m_font_size);

	m_doc->CvtUnits(m_css_text_indent,								m_font_size);

	m_margins.left		= m_doc->CvtUnits(m_css_margins.left,		m_font_size);
	m_margins.right		= m_doc->CvtUnits(m_css_margins.right,		m_font_size);
	m_margins.top		= m_doc->CvtUnits(m_css_margins.top,		m_font_size);
	m_margins.bottom	= m_doc->CvtUnits(m_css_margins.bottom,	m_font_size);

	m_padding.left		= m_doc->CvtUnits(m_css_padding.left,		m_font_size);
	m_padding.right		= m_doc->CvtUnits(m_css_padding.right,		m_font_size);
	m_padding.top		= m_doc->CvtUnits(m_css_padding.top,		m_font_size);
	m_padding.bottom	= m_doc->CvtUnits(m_css_padding.bottom,	m_font_size);

	m_borders.left		= m_doc->CvtUnits(m_css_borders.left.width,	m_font_size);
	m_borders.right		= m_doc->CvtUnits(m_css_borders.right.width,	m_font_size);
	m_borders.top		= m_doc->CvtUnits(m_css_borders.top.width,		m_font_size);
	m_borders.bottom	= m_doc->CvtUnits(m_css_borders.bottom.width,	m_font_size);

	CssLength line_height;
	line_height.FromString(GetStyleProperty("line-height",	true,	"normal"), "normal");
	if(line_height.IsPredefined())
	{
		m_line_height = m_font_metrics.height;
		m_lh_predefined = true;
	} else if(line_height.Units() == css_units_none)
	{
		m_line_height = (int) (line_height.Value() * m_font_size);
		m_lh_predefined = false;
	} else
	{
		m_line_height =  m_doc->CvtUnits(line_height,	m_font_size, m_font_size);
		m_lh_predefined = false;
	}


	if(m_display == display_list_item)
	{
		String list_type = GetStyleProperty("list-style-type", true, "disc");
		m_list_style_type = (ListStyleType) ValueIndex(list_type, list_style_type_strings, list_style_type_disc);

		String list_pos = GetStyleProperty("list-style-position", true, "outside");
		m_list_style_position = (ListStylePosition) ValueIndex(list_pos, ListStyleposition_strings, ListStyleposition_outside);

		String list_image = GetStyleProperty("list-style-image", true, "");
		
		if(list_image.GetCount() && list_image[0])
		{
			String url;
			Css::ParseCssUrl(list_image, url);

			String list_image_baseurl = GetStyleProperty("list-style-image-baseurl", true, "");
			m_doc->GetHtmlFile()->LoadImage(url, list_image_baseurl, true);
		}

	}

	ParseBackground();

	if(!is_reparse)
	{
		for(ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
		{
			(*i)->ParseStyles();
		}
	}
}

int HtmlTag::Render( int x, int y, int max_width, bool second_pass )
{
	if (m_display == display_table || m_display == display_inline_table)
	{
		return RenderTable(x, y, max_width, second_pass);
	}

	return RenderBox(x, y, max_width, second_pass);
}

bool HtmlTag::IsWhiteSpace()
{
	return false;
}

int HtmlTag::GetFontSize() const
{
	return m_font_size;
}

int HtmlTag::GetBaseLine()
{
	if(IsReplaced())
	{
		return 0;
	}
	int bl = 0;
	if(!m_boxes.IsEmpty())
	{
		bl = LAST(m_boxes)->Baseline() + ContentMarginsBottom();
	}
	return bl;
}

void HtmlTag::Init()
{
	if (m_display == display_table || m_display == display_inline_table)
	{
		m_grid.Clear();

		go_inside_table 		table_selector;
		TableRowsSelector		row_selector;
		TableCellsSelector	cell_selector;

		ElementsIterator row_iter(this, &table_selector, &row_selector);

		Element* row = row_iter.Next(false);
		while (row)
		{
			m_grid.BeginRow(row);

			ElementsIterator cell_iter(row, &table_selector, &cell_selector);
			Element* cell = cell_iter.Next();
			while (cell)
			{
				m_grid.AddCell(cell);

				cell = cell_iter.Next(false);
			}
			row = row_iter.Next(false);
		}

		m_grid.Finish();
	}
	else
	{
		//remove white spaces
		int i = 0;
		while (i < m_children.GetCount())
		{
			ObjectPtr<Element>& el = m_children[i];
			if (el->IsWhiteSpace())
			{
				i++;
				while ( i < m_children.GetCount())
				{
					ObjectPtr<Element>& el = m_children[i];
					if (!el->IsWhiteSpace()) break;
					m_children.Remove(i);
				}
			}
			else
			{
				i++;
			}
		}
	}

	for (ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		(*i)->Init();
	}
}

int HtmlTag::Select(const CssSelector& selector, bool apply_pseudo)
{
	int right_res = Select(selector.m_right, apply_pseudo);
	if(right_res == select_no_match)
	{
		return select_no_match;
	}
	if(selector.m_left)
	{
		Element* m_higher = GetParentElement();
		if(!m_higher)
		{
			return select_no_match;
		}
		switch(selector.m_combinator)
		{
		case combinator_descendant:
			{
				bool is_pseudo = false;
				Element* res = FindAncestor(*selector.m_left, apply_pseudo, &is_pseudo);
				if(!res)
				{
					return select_no_match;
				} else
				{
					if(is_pseudo)
					{
						right_res |= select_match_pseudo_class;
					}
				}
			}
			break;
		case combinator_child:
			{
				int res = m_higher->Select(*selector.m_left, apply_pseudo);
				if(res == select_no_match)
				{
					return select_no_match;
				} else
				{
					if(right_res != select_match_pseudo_class)
					{
						right_res |= res;
					}
				}
			}
			break;
		case combinator_adjacent_sibling:
			{
				bool is_pseudo = false;
				Element* res =  m_higher->FindAdjacentSibling(this, *selector.m_left, apply_pseudo, &is_pseudo);
				if(!res)
				{
					return select_no_match;
				} else
				{
					if(is_pseudo)
					{
						right_res |= select_match_pseudo_class;
					}
				}
			}
			break;
		case combinator_general_sibling:
			{
				bool is_pseudo = false;
				Element* res =  m_higher->FindSibling(this, *selector.m_left, apply_pseudo, &is_pseudo);
				if(!res)
				{
					return select_no_match;
				} else
				{
					if(is_pseudo)
					{
						right_res |= select_match_pseudo_class;
					}
				}
			}
			break;
		default:
			right_res = select_no_match;
		}
	}
	return right_res;
}

int HtmlTag::Select(const CssElementSelector& selector, bool apply_pseudo)
{
	//LOG("HtmlTag::Select " << selector.m_tag);
	if(!selector.m_tag.IsEmpty() && selector.m_tag != "*")
	{
		if(selector.m_tag != m_tag)
		{
			return select_no_match;
		}
	}

	int res = select_match;
	
	for(CssAttributeSelector::vector::ConstIterator i = selector.m_attrs.Begin(); i != selector.m_attrs.End(); i++)
	{
		String attr_value = GetAttr(i->attribute);
		switch(i->condition)
		{
		case select_exists:
			if(!attr_value.GetCount())
			{
				return select_no_match;
			}
			break;
		case select_equal:
			if(!attr_value.GetCount())
			{
				return select_no_match;
			} else 
			{
				if(i->attribute == "class")
				{
					StringVector tokens1;
					SplitString(attr_value, tokens1, " ");
					StringVector tokens2;
					SplitString(i->val, tokens2, " ");
					bool found = true;
					for(StringVector::Iterator str1 = tokens2.Begin(); str1 != tokens2.End() && found; str1++)
					{
						bool f = false;
						for(StringVector::Iterator str2 = tokens1.Begin(); str2 != tokens1.End() && !f; str2++)
						{
							if( !CompareNoCase(*str1, *str2) )
							{
								f = true;
							}
						}
						if(!f)
						{
							found = false;
						}
					}
					if(!found)
					{
						return select_no_match;
					}
				} else
				{
					if( CompareNoCase(i->val, attr_value) )
					{
						return select_no_match;
					}
				}
			}
			break;
		case select_contain_str:
			if(!attr_value.GetCount())
			{
				return select_no_match;
			} 
			else if(attr_value.Find(i->val) == -1)
			{
				return select_no_match;
			}
			break;
		case select_start_str:
			if(!attr_value.GetCount())
			{
				return select_no_match;
			} else if(attr_value.Compare(i->val))
			{
				return select_no_match;
			}
			break;
		case select_end_str:
			if(!attr_value.GetCount())
			{
				return select_no_match;
			} 
			else if(attr_value.Compare(i->val))
			{
				String s = attr_value.Mid(attr_value.GetCount() - i->val.GetCount() - 1);
				if(s < attr_value)
				{
					return select_no_match;
				}
				if(i->val != s)
				{
					return select_no_match;
				}
			}
			break;
		case select_pseudo_element:
			if(i->val == "after")
			{
				res |= select_match_with_after;
			} else if(i->val == "before")
			{
				res |= select_match_with_before;
			} else
			{
				return select_no_match;
			}
			break;
		case select_pseudo_class:
			if(apply_pseudo)
			{
				Element* m_higher = GetParentElement();
				
				if(!m_higher) return select_no_match;

				String selector_param;
				String	selector_name;

				int begin	= FindFirstOf(i->val, '(');
				int end		= (begin == -1) ? -1 : FindCloseBracket(i->val, begin);
				if(begin != -1 && end != -1)
				{
					selector_param = i->val.Mid(begin + 1, end - begin - 1);
				}
				if(begin != -1)
				{
					selector_name = i->val.Mid(0, begin);
					Trim(selector_name);
				} else
				{
					selector_name = i->val;
				}

				int selector = ValueIndex(selector_name, PseudoClass_strings);
				
				switch(selector)
				{
				case PseudoClass_only_child:
					if( !m_higher->IsOnlyChild(this, false) )
					{
						return select_no_match;
					}
					break;
				case PseudoClass_only_of_type:
					if( !m_higher->IsOnlyChild(this, true) )
					{
						return select_no_match;
					}
					break;
				case PseudoClass_first_child:
					if( !m_higher->IsNthChild(this, 0, 1, false) )
					{
						return select_no_match;
					}
					break;
				case PseudoClass_first_of_type:
					if( !m_higher->IsNthChild(this, 0, 1, true) )
					{
						return select_no_match;
					}
					break;
				case PseudoClass_last_child:
					if( !m_higher->IsNthLastChild(this, 0, 1, false) )
					{
						return select_no_match;
					}
					break;
				case PseudoClass_last_of_type:
					if( !m_higher->IsNthLastChild(this, 0, 1, true) )
					{
						return select_no_match;
					}
					break;
				case PseudoClass_nth_child:
				case PseudoClass_nth_of_type:
				case PseudoClass_nth_last_child:
				case PseudoClass_nth_last_of_type:
					{
						if(selector_param.IsEmpty()) return select_no_match;

						int num = 0;
						int off = 0;

						ParseNthChildParams(selector_param, num, off);
						if(!num && !off) return select_no_match;
						switch(selector)
						{
						case PseudoClass_nth_child:
							if( !m_higher->IsNthChild(this, num, off, false) )
							{
								return select_no_match;
							}
							break;
						case PseudoClass_nth_of_type:
							if( !m_higher->IsNthChild(this, num, off, true) )
							{
								return select_no_match;
							}
							break;
						case PseudoClass_nth_last_child:
							if( !m_higher->IsNthLastChild(this, num, off, false) )
							{
								return select_no_match;
							}
							break;
						case PseudoClass_nth_last_of_type:
							if( !m_higher->IsNthLastChild(this, num, off, true) )
							{
								return select_no_match;
							}
							break;
						}

					}
					break;
				case PseudoClass_not:
					{
						CssElementSelector sel;
						sel.Parse(selector_param);
						if(Select(sel, apply_pseudo))
						{
							return select_no_match;
						}
					}
					break;
				default:
					if(Find(m_pseudo_classes.Begin(), m_pseudo_classes.End(), i->val, StdEqual<String>() ) == m_pseudo_classes.End())
					{
						return select_no_match;
					}
					break;
				}
			} else
			{
				res |= select_match_pseudo_class;
			}
			break;
		}
	}
	return res;
}

Element* HtmlTag::FindAncestor( const CssSelector& selector, bool apply_pseudo, bool* is_pseudo )
{
	Element* m_higher = GetParentElement();
	if(!m_higher)
	{
		return 0;
	}
	int res = m_higher->Select(selector, apply_pseudo);
	if(res != select_no_match)
	{
		if(is_pseudo)
		{
			if(res & select_match_pseudo_class)
			{
				*is_pseudo = true;
			} else
			{
				*is_pseudo = false;
			}
		}
		return m_higher;
	}
	if (m_higher == this)
		return 0;
	return m_higher->FindAncestor(selector, apply_pseudo, is_pseudo);
}

int HtmlTag::GetFloatsHeight(ElementFloat el_float) const
{
	if(IsFloatsHolder())
	{
		int h = 0;

		bool process = false;

		for(FloatedBox::vector::ConstIterator i = m_floats_left.Begin(); i != m_floats_left.End(); i++)
		{
			process = false;
			switch(el_float)
			{
			case float_none:
				process = true;
				break;
			case float_left:
				if(i->clear_floats == clear_left || i->clear_floats == clear_both)
				{
					process = true;
				}
				break;
			case float_right:
				if(i->clear_floats == clear_right || i->clear_floats == clear_both)
				{
					process = true;
				}
				break;
			}
			if(process)
			{
				if(el_float == float_none)
				{
					h = max(h, i->pos.Bottom());
				} else
				{
					h = max(h, i->pos.Top());
				}
			}
		}


		for(FloatedBox::vector::ConstIterator i = m_floats_right.Begin(); i != m_floats_right.End(); i++)
		{
			process = false;
			switch(el_float)
			{
			case float_none:
				process = true;
				break;
			case float_left:
				if(i->clear_floats == clear_left || i->clear_floats == clear_both)
				{
					process = true;
				}
				break;
			case float_right:
				if(i->clear_floats == clear_right || i->clear_floats == clear_both)
				{
					process = true;
				}
				break;
			}
			if(process)
			{
				if(el_float == float_none)
				{
					h = max(h, i->pos.Bottom());
				} else
				{
					h = max(h, i->pos.Top());
				}
			}
		}

		return h;
	}
	int h = GetParentElement()->GetFloatsHeight(el_float);
	return h - m_pos.y;
}

int HtmlTag::GetLeftFloatsHeight() const
{
	if(IsFloatsHolder())
	{
		int h = 0;
		if(!m_floats_left.IsEmpty())
		{
			for(FloatedBox::vector::ConstIterator el = m_floats_left.Begin(); el != m_floats_left.End(); el++)
			{
				h = max(h, el->pos.Bottom());
			}
		}
		return h;
	}
	int h = GetParentElement()->GetLeftFloatsHeight();
	return h - m_pos.y;
}

int HtmlTag::GetRightGloatsHeight() const
{
	if(IsFloatsHolder())
	{
		int h = 0;
		if(!m_floats_right.IsEmpty())
		{
			for(FloatedBox::vector::ConstIterator el = m_floats_right.Begin(); el != m_floats_right.End(); el++)
			{
				h = max(h, el->pos.Bottom());
			}
		}
		return h;
	}
	int h = GetParentElement()->GetRightGloatsHeight();
	return h - m_pos.y;
}

int HtmlTag::GetLineLeft( int y )
{
	if(IsFloatsHolder())
	{
		if(m_cahe_line_left.is_valid && m_cahe_line_left.hash == y)
		{
			return m_cahe_line_left.val;
		}

		int w = 0;
		for(FloatedBox::vector::Iterator el = m_floats_left.Begin(); el != m_floats_left.End(); el++)
		{
			if(y >= el->pos.Top() && y < el->pos.Bottom())
			{
				w = max(w, el->pos.Right());
				if(w < el->pos.Right())
				{
					break;
				}
			}
		}
		m_cahe_line_left.SetValue(y, w);
		return w;
	}
	int w = GetParentElement()->GetLineLeft(y + m_pos.y);
	if(w < 0)
	{
		w = 0;
	}
	return w - (w ? m_pos.x : 0);
}

int HtmlTag::GetLineRight( int y, int def_right )
{
	if(IsFloatsHolder())
	{
		if(m_cahe_line_right.is_valid && m_cahe_line_right.hash == y)
		{
			if(m_cahe_line_right.is_default)
			{
				return def_right;
			} else
			{
				return min(m_cahe_line_right.val, def_right);
			}
		}

		int w = def_right;
		m_cahe_line_right.is_default = true;
		for(FloatedBox::vector::Iterator el = m_floats_right.Begin(); el != m_floats_right.End(); el++)
		{
			if(y >= el->pos.Top() && y < el->pos.Bottom())
			{
				w = min(w, el->pos.Left());
				m_cahe_line_right.is_default = false;
				if(w > el->pos.Left())
				{
					break;
				}
			}
		}
		m_cahe_line_right.SetValue(y, w);
		return w;
	}
	int w = GetParentElement()->GetLineRight(y + m_pos.y, def_right + m_pos.x);
	return w - m_pos.x;
}


void HtmlTag::GetLineLeftRight( int y, int def_right, int& ln_left, int& ln_right )
{
	if(IsFloatsHolder())
	{
		ln_left		= GetLineLeft(y);
		ln_right	= GetLineRight(y, def_right);
	} else
	{
		GetParentElement()->GetLineLeftRight(y + m_pos.y, def_right + m_pos.x, ln_left, ln_right);
		ln_right -= m_pos.x;

		if(ln_left < 0)
		{
			ln_left = 0;
		} else if(ln_left)
		{
			ln_left -= m_pos.x;
		}
	}
}

int HtmlTag::FixLineWidth( int max_width, ElementFloat flt )
{
	int ret_width = 0;
	if(!m_boxes.IsEmpty())
	{
		ElementsVector els;
		LAST(m_boxes)->GetElements(els);
		bool was_cleared = false;
		if(!els.IsEmpty() && els[0]->GetClear() != clear_none)
		{
			if(els[0]->GetClear() == clear_both)
			{
				was_cleared = true;
			} else
			{
				if(	(flt == float_left	&& els[0]->GetClear() == clear_left) ||
					(flt == float_right	&& els[0]->GetClear() == clear_right) )
				{
					was_cleared = true;
				}
			}
		}

		if(!was_cleared)
		{
			REMOVELAST(m_boxes);

			for(ElementsVector::Iterator i = els.Begin(); i != els.End(); i++)
			{
				int rw = PlaceElement((*i), max_width);
				if(rw > ret_width)
				{
					ret_width = rw;
				}
			}
		} else
		{
			int line_top = 0;
			if(LAST(m_boxes)->GetType() == Box_line)
			{
				line_top = LAST(m_boxes)->Top();
			} else
			{
				line_top = LAST(m_boxes)->Bottom();
			}

			int line_left	= 0;
			int line_right	= max_width;
			GetLineLeftRight(line_top, max_width, line_left, line_right);

			if(LAST(m_boxes)->GetType() == Box_line)
			{
				if(m_boxes.GetCount() == 1 && m_list_style_type != list_style_type_none && m_list_style_position == ListStyleposition_inside)
				{
					int sz_font = GetFontSize();
					line_left += sz_font;
				}

				if(m_css_text_indent.Value() != 0)
				{
					bool LineBox_found = false;
					for(Box::vector::Iterator iter = m_boxes.Begin(); iter < m_boxes.End(); iter++)
					{
						if((*iter)->GetType() == Box_line)
						{
							LineBox_found = true;
							break;
						}
					}
					if(!LineBox_found)
					{
						line_left += m_css_text_indent.CalcPercent(max_width);
					}
				}

			}

			ElementsVector els;
			LAST(m_boxes)->NewWidth(line_left, line_right, els);
			for(ElementsVector::Iterator i = els.Begin(); i != els.End(); i++)
			{
				int rw = PlaceElement((*i), max_width);
				if(rw > ret_width)
				{
					ret_width = rw;
				}
			}
		}
	}

	return ret_width;
}

void HtmlTag::AddFloat( Element* el, int x, int y )
{
	if(IsFloatsHolder())
	{
		FloatedBox fb;
		fb.pos.x		= el->Left() + x;
		fb.pos.y		= el->Top()  + y;
		fb.pos.width	= el->Width();
		fb.pos.height	= el->Height();
		fb.float_side	= el->GetFloat();
		fb.clear_floats	= el->GetClear();
		fb.el			= el;

		if(fb.float_side == float_left)
		{
			if(m_floats_left.IsEmpty())
			{
				m_floats_left.Add(fb);
			} else
			{
				bool inserted = false;
				for (int i = 0; i < m_floats_left.GetCount(); i++)
				{
					FloatedBox& ifb = m_floats_left[i];
					if(fb.pos.Right() > ifb.pos.Right())
					{
						m_floats_left.Insert(i, fb);
						inserted = true;
						break;
					}
				}
				if(!inserted)
				{
					m_floats_left.Add(fb);
				}
			}
			m_cahe_line_left.invalidate();
		} else if(fb.float_side == float_right)
		{
			if(m_floats_right.IsEmpty())
			{
				m_floats_right.Add(fb);
			} else
			{
				bool inserted = false;
				for (int i = 0; i < m_floats_right.GetCount(); i++)
				{
					FloatedBox& ifb = m_floats_right[i];
					if(fb.pos.Left() < ifb.pos.Left())
					{
						m_floats_right.Insert(i, fb);
						inserted = true;
						break;
					}
				}
				if(!inserted)
				{
					m_floats_right.Add(fb);
				}
			}
			m_cahe_line_right.invalidate();
		}
	} else
	{
		GetParentElement()->AddFloat(el, x + m_pos.x, y + m_pos.y);
	}
}

int HtmlTag::FindNextLineTop( int top, int width, int def_right )
{
	if(IsFloatsHolder())
	{
		int new_top = top;
		IntVector points;

		for(FloatedBox::vector::Iterator el = m_floats_left.Begin(); el != m_floats_left.End(); el++)
		{
			if(el->pos.Top() >= top)
			{
				if(Find(points.Begin(), points.End(), el->pos.Top(), StdEqual<int>()) == points.End())
				{
					points.Add(el->pos.Top());
				}
			}
			if(el->pos.Bottom() >= top)
			{
				if(Find(points.Begin(), points.End(), el->pos.Bottom(), StdEqual<int>()) == points.End())
				{
					points.Add(el->pos.Bottom());
				}
			}
		}

		for(FloatedBox::vector::Iterator el = m_floats_right.Begin(); el != m_floats_right.End(); el++)
		{
			if(el->pos.Top() >= top)
			{
				if(Find(points.Begin(), points.End(), el->pos.Top(), StdEqual<int>()) == points.End())
				{
					points.Add(el->pos.Top());
				}
			}
			if(el->pos.Bottom() >= top)
			{
				if(Find(points.Begin(), points.End(), el->pos.Bottom(), StdEqual<int>()) == points.End())
				{
					points.Add(el->pos.Bottom());
				}
			}
		}

		if(!points.IsEmpty())
		{
			Sort(points, StdLess<int>() );
			new_top = LAST(points);

			for(IntVector::Iterator i = points.Begin(); i != points.End(); i++)
			{
				int pos_left	= 0;
				int pos_right	= def_right;
				GetLineLeftRight((*i), def_right, pos_left, pos_right);

				if(pos_right - pos_left >= width)
				{
					new_top = (*i);
					break;
				}
			}
		}
		return new_top;
	}
	int new_top = GetParentElement()->FindNextLineTop(top + m_pos.y, width, def_right + m_pos.x);
	return new_top - m_pos.y;
}

void HtmlTag::ParseBackground()
{
	// parse background-color
	m_bg.m_color		= GetColor("background-color", false, WebColor(0, 0, 0, 0));

	// parse background-position
	String str = GetStyleProperty("background-position", false, "0% 0%");

	if(str.GetCount())
	{
		StringVector res;
		SplitString(str, res, " \t");
		if(res.GetCount() > 0)
		{
			if(res.GetCount() == 1)
			{
				if( ValueInList(res[0], "left;right;center"))
				{
					m_bg.m_position.x.FromString(res[0], "left;right;center");
					m_bg.m_position.y.SetValue(50, css_units_percentage);
				} else if( ValueInList(res[0], "top;bottom;center"))
				{
					m_bg.m_position.y.FromString(res[0], "top;bottom;center");
					m_bg.m_position.x.SetValue(50, css_units_percentage);
				} else
				{
					m_bg.m_position.x.FromString(res[0], "left;right;center");
					m_bg.m_position.y.SetValue(50, css_units_percentage);
				}
			} else
			{
				if(ValueInList(res[0], "left;right"))
				{
					m_bg.m_position.x.FromString(res[0], "left;right;center");
					m_bg.m_position.y.FromString(res[1], "top;bottom;center");
				} else if(ValueInList(res[0], "top;bottom"))
				{
					m_bg.m_position.x.FromString(res[1], "left;right;center");
					m_bg.m_position.y.FromString(res[0], "top;bottom;center");
				} else if(ValueInList(res[1], "left;right"))
				{
					m_bg.m_position.x.FromString(res[1], "left;right;center");
					m_bg.m_position.y.FromString(res[0], "top;bottom;center");
				}else if(ValueInList(res[1], "top;bottom"))
				{
					m_bg.m_position.x.FromString(res[0], "left;right;center");
					m_bg.m_position.y.FromString(res[1], "top;bottom;center");
				} else
				{
					m_bg.m_position.x.FromString(res[0], "left;right;center");
					m_bg.m_position.y.FromString(res[1], "top;bottom;center");
				}
			}

			if(m_bg.m_position.x.IsPredefined())
			{
				switch(m_bg.m_position.x.PreDefine())
				{
				case 0:
					m_bg.m_position.x.SetValue(0, css_units_percentage);
					break;
				case 1:
					m_bg.m_position.x.SetValue(100, css_units_percentage);
					break;
				case 2:
					m_bg.m_position.x.SetValue(50, css_units_percentage);
					break;
				}
			}
			if(m_bg.m_position.y.IsPredefined())
			{
				switch(m_bg.m_position.y.PreDefine())
				{
				case 0:
					m_bg.m_position.y.SetValue(0, css_units_percentage);
					break;
				case 1:
					m_bg.m_position.y.SetValue(100, css_units_percentage);
					break;
				case 2:
					m_bg.m_position.y.SetValue(50, css_units_percentage);
					break;
				}
			}
		} else
		{
			m_bg.m_position.x.SetValue(0, css_units_percentage);
			m_bg.m_position.y.SetValue(0, css_units_percentage);
		}
	} else
	{
		m_bg.m_position.y.SetValue(0, css_units_percentage);
		m_bg.m_position.x.SetValue(0, css_units_percentage);
	}

	str = GetStyleProperty("background-size", false, "auto");

	if(str.GetCount())
	{
		StringVector res;
		SplitString(str, res, " \t");
		if(!res.IsEmpty())
		{
			m_bg.m_position.width.FromString(res[0], background_size_strings);
			if(res.GetCount() > 1)
			{
				m_bg.m_position.height.FromString(res[1], background_size_strings);
			} else
			{
				m_bg.m_position.height.PreDefine(background_size_auto);
			}
		} else
		{
			m_bg.m_position.width.PreDefine(background_size_auto);
			m_bg.m_position.height.PreDefine(background_size_auto);
		}
	}

	Document* m_doc = GetDocument();
	m_doc->CvtUnits(m_bg.m_position.x,			m_font_size);
	m_doc->CvtUnits(m_bg.m_position.y,			m_font_size);
	m_doc->CvtUnits(m_bg.m_position.width,		m_font_size);
	m_doc->CvtUnits(m_bg.m_position.height,	m_font_size);

	// parse BackgroundAttachment
	m_bg.m_attachment = (BackgroundAttachment) ValueIndex(
		GetStyleProperty("background-attachment", false, "scroll"), 
		BackgroundAttachment_strings, 
		BackgroundAttachment_scroll);

	// parse BackgroundAttachment
	m_bg.m_repeat = (BackgroundRepeat) ValueIndex(
		GetStyleProperty("background-repeat", false, "repeat"), 
		background_repeat_strings, 
		background_repeat_repeat);

	// parse background_clip
	m_bg.m_clip = (BackgroundBox) ValueIndex(
		GetStyleProperty("background-clip", false, "border-box"), 
		background_box_strings, 
		background_box_border);

	// parse background_origin
	m_bg.m_origin = (BackgroundBox) ValueIndex(
		GetStyleProperty("background-origin", false, "padding-box"), 
		background_box_strings, 
		background_box_content);

	// parse background-image
	Css::ParseCssUrl(GetStyleProperty("background-image", false, ""), m_bg.m_image);
	m_bg.m_baseurl = GetStyleProperty("background-image-baseurl", false, "");

	if(!m_bg.m_image.IsEmpty() && m_bg.cache_id == -1)
	{
		String base_url;
		if (m_bg.m_baseurl.GetCount())
			base_url = m_bg.m_baseurl;
		else
			base_url = GetBaseUrl();
		m_doc->GetHtmlFile()->LoadImage(m_bg.m_image, base_url, true);
		
		m_bg.cache_id = GetCache().FindCacheObject(m_bg.m_image, base_url);
	}
}

void HtmlTag::AddPositioned( Element* el )
{
	Element* m_higher = GetParentElement();
	
	if( m_el_position != element_position_static || (!m_higher) )
	{
		m_positioned.Add(el);
	} else
	{
		m_higher->AddPositioned(el);
	}
}

void HtmlTag::CalcOutlines( int higher_width )
{
	m_padding.left	= m_css_padding.left.CalcPercent(higher_width);
	m_padding.right	= m_css_padding.right.CalcPercent(higher_width);

	m_borders.left	= m_css_borders.left.width.CalcPercent(higher_width);
	m_borders.right	= m_css_borders.right.width.CalcPercent(higher_width);

	m_margins.left	= m_css_margins.left.CalcPercent(higher_width);
	m_margins.right	= m_css_margins.right.CalcPercent(higher_width);

	m_margins.top		= m_css_margins.top.CalcPercent(higher_width);
	m_margins.bottom	= m_css_margins.bottom.CalcPercent(higher_width);

	m_padding.top		= m_css_padding.top.CalcPercent(higher_width);
	m_padding.bottom	= m_css_padding.bottom.CalcPercent(higher_width);
}

void HtmlTag::CalcAutoMargins(int higher_width)
{
	if (GetElementPosition() != element_position_absolute && (m_display == display_block || m_display == display_table))
	{
		if (m_css_margins.left.IsPredefined() && m_css_margins.right.IsPredefined())
		{
			int el_width = m_pos.width + m_borders.left + m_borders.right + m_padding.left + m_padding.right;
			if (el_width <= higher_width)
			{
				m_margins.left = (higher_width - el_width) / 2;
				m_margins.right = (higher_width - el_width) - m_margins.left;
			}
			else
			{
				m_margins.left = 0;
				m_margins.right = 0;
			}
		}
		else if (m_css_margins.left.IsPredefined() && !m_css_margins.right.IsPredefined())
		{
			int el_width = m_pos.width + m_borders.left + m_borders.right + m_padding.left + m_padding.right + m_margins.right;
			m_margins.left = higher_width - el_width;
			if (m_margins.left < 0) m_margins.left = 0;
		}
		else if (!m_css_margins.left.IsPredefined() && m_css_margins.right.IsPredefined())
		{
			int el_width = m_pos.width + m_borders.left + m_borders.right + m_padding.left + m_padding.right + m_margins.left;
			m_margins.right = higher_width - el_width;
			if (m_margins.right < 0) m_margins.right = 0;
		}
	}
}

void HtmlTag::ParseAttributes()
{
	for(int i = 0; i < GetAttrCount(); i++) {
		LOG(GetAttrKey(i) << " = " << GetAttrValue(i) );
	}
	
	for(ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		(*i)->ParseAttributes();
	}
}

void HtmlTag::GetText( String& text )
{
	for(ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		(*i)->GetText(text);
	}
}

bool HtmlTag::IsBody()  const
{
	return false;
}

void HtmlTag::SetData( const String& data )
{

}

void HtmlTag::GetInLineBoxes( Position::vector& boxes )
{
	Box* old_box = 0;
	Position pos;
	for(ElementsVector::Iterator iter = m_children.Begin(); iter != m_children.End(); iter++)
	{
		Element* el = (*iter);
		if(!el->Skip())
		{
			if(el->m_box)
			{
				if(el->m_box != old_box)
				{
					if(old_box)
					{
						if(boxes.IsEmpty())
						{
							pos.x		-= m_padding.left + m_borders.left;
							pos.width	+= m_padding.left + m_borders.left;
						}
						boxes.Add(pos);
					}
					old_box		= el->m_box;
					pos.x		= el->Left() + el->MarginLeft();
					pos.y		= el->Top() - m_padding.top - m_borders.top;
					pos.width	= 0;
					pos.height	= 0;
				}
				pos.width	= el->Right() - pos.x - el->MarginRight() - el->MarginLeft();
				pos.height	= max(pos.height, el->Height() + m_padding.top + m_padding.bottom + m_borders.top + m_borders.bottom);
			} else if(el->GetDisplay() == display_inline)
			{
				Position::vector sub_boxes;
				el->GetInLineBoxes(sub_boxes);
				if(!sub_boxes.IsEmpty())
				{
					//sub_boxes.rbegin()->width += el->MarginRight();
					sub_boxes[sub_boxes.GetCount()-1].width += el->MarginRight();
					if(boxes.IsEmpty())
					{
						if(m_padding.left + m_borders.left > 0)
						{
							Position padding_box = (*sub_boxes.Begin());
							padding_box.x		-= m_padding.left + m_borders.left + el->MarginLeft();
							padding_box.width	= m_padding.left + m_borders.left + el->MarginLeft();
							boxes.Add(padding_box);
						}
					}

					sub_boxes[sub_boxes.GetCount()-1].width += el->MarginRight();
					
					boxes.Append(sub_boxes);
				}
			}
		}
	}
	if(pos.width || pos.height)
	{
		if(boxes.IsEmpty())
		{
			pos.x		-= m_padding.left + m_borders.left;
			pos.width	+= m_padding.left + m_borders.left;
		}
		boxes.Add(pos);
	}
	if(!boxes.IsEmpty())
	{
		if(m_padding.right + m_borders.right > 0)
		{
			//Position padding_box = (*boxes.rbegin());
			Position padding_box = (boxes[boxes.GetCount()-1]);
			padding_box.x		+= padding_box.width;
			padding_box.width	= m_padding.right + m_borders.right;
			boxes.Add(padding_box);
		}
	}
}

bool HtmlTag::OnMouseOver()
{
	bool ret = false;

	Element* el = this;
	while(el)
	{
		if(el->SetPseudoClass("hover", true))
		{
			ret = true;
		}
		el = el->GetParentElement();
	}

	return ret;
}

bool HtmlTag::FindStylesChanges( Position::vector& redraw_boxes, int x, int y )
{
	if(m_display == display_inline_text)
	{
		return false;
	}

	bool ret = false;
	bool apply = false;
	for (UsedSelector::vector::Iterator iter = m_used_styles.Begin(); iter != m_used_styles.End() && !apply; iter++)
	{
		if((*iter)->m_selector->IsMediaValid())
		{
			int res = Select(*((*iter)->m_selector), true);
			if( (res == select_no_match && (*iter)->m_used) || (res == select_match && !(*iter)->m_used) )
			{
				apply = true;
			}
		}
	}

	if(apply)
	{
		if(m_display == display_inline ||  m_display == display_TableRow)
		{
			Position::vector Boxes;
			GetInLineBoxes(Boxes);
			for(Position::vector::Iterator pos = Boxes.Begin(); pos != Boxes.End(); pos++)
			{
				pos->x	+= x;
				pos->y	+= y;
				redraw_boxes.Add(*pos);
			}
		} else
		{
			Position pos = m_pos;
			if(m_el_position != element_position_fixed)
			{
				pos.x += x;
				pos.y += y;
			}
			pos += m_padding;
			pos += m_borders;
			redraw_boxes.Add(pos);
		}

		ret = true;
		RefreshStyles();
		ParseStyles();
	}
	for(ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		if(!(*i)->Skip())
		{
			if(m_el_position != element_position_fixed)
			{
				if((*i)->FindStylesChanges(redraw_boxes, x + m_pos.x, y + m_pos.y))
				{
					ret = true;
				}
			} else
			{
				if((*i)->FindStylesChanges(redraw_boxes, m_pos.x, m_pos.y))
				{
					ret = true;
				}
			}
		}
	}
	return ret;
}

bool HtmlTag::OnMouseLeave()
{
	bool ret = false;

	Element* el = this;
	while(el)
	{
		if(el->SetPseudoClass("hover", false))
		{
			ret = true;
		}
		if(el->SetPseudoClass("active", false))
		{
			ret = true;
		}
		el = el->GetParentElement();
	}

	return ret;
}

bool HtmlTag::OnMouseLeftDown()
{
    bool ret = false;

    Element* el = this;
    while (el)
    {
        if (el->SetPseudoClass("active", true))
        {
            ret = true;
        }
        el = el->GetParentElement();
    }

    return ret;
}

bool HtmlTag::OnMouseLeftUp()
{
	bool ret = false;

    Element* el = this;
    while (el)
    {
        if (el->SetPseudoClass("active", false))
        {
            ret = true;
        }
        el = el->GetParentElement();
    }

    OnClick();

	return ret;
}

void HtmlTag::OnClick()
{
	if(GetParentElement())
	{
		GetParentElement()->OnClick();
	}
}

String HtmlTag::GetCursor()
{
	return GetStyleProperty("cursor", true, "");
}

static const int FontSize_table[8][7] =
{
	{ 9,    9,     9,     9,    11,    14,    18},
	{ 9,    9,     9,    10,    12,    15,    20},
	{ 9,    9,     9,    11,    13,    17,    22},
	{ 9,    9,    10,    12,    14,    18,    24},
	{ 9,    9,    10,    13,    16,    20,    26},
	{ 9,    9,    11,    14,    17,    21,    28},
	{ 9,   10,    12,    15,    17,    23,    30},
	{ 9,   10,    13,    16,    18,    24,    32}
};


void HtmlTag::InitFont()
{
	// Initialize font size
	String str = GetStyleProperty("font-size", false, "");

	int higher_sz = 0;
	Document* m_doc = GetDocument();
	Element* m_higher = GetParentElement();
	
	int doc_font_size = m_doc->GetHtmlFile()->GetDefaultFontSize();
	if(m_higher)
	{
		higher_sz = m_higher->GetFontSize();
	} else
	{
		higher_sz = doc_font_size;
	}


	if(!str.GetCount())
	{
		m_font_size = higher_sz;
	} else
	{
		m_font_size = higher_sz;

		CssLength sz;
		sz.FromString(str, FontSize_strings);
		if(sz.IsPredefined())
		{
			int idx_in_table = doc_font_size - 9;
			if(idx_in_table >= 0 && idx_in_table <= 7)
			{
				if(sz.PreDefine() >= fontSize_xx_small && sz.PreDefine() <= fontSize_xx_large)
				{
					m_font_size = FontSize_table[idx_in_table][sz.PreDefine()];
				} else
				{
					m_font_size = doc_font_size;
				}
			} else			
			{
				switch(sz.PreDefine())
				{
				case fontSize_xx_small:
					m_font_size = doc_font_size * 3 / 5;
					break;
				case fontSize_x_small:
					m_font_size = doc_font_size * 3 / 4;
					break;
				case fontSize_small:
					m_font_size = doc_font_size * 8 / 9;
					break;
				case fontSize_large:
					m_font_size = doc_font_size * 6 / 5;
					break;
				case fontSize_x_large:
					m_font_size = doc_font_size * 3 / 2;
					break;
				case fontSize_xx_large:
					m_font_size = doc_font_size * 2;
					break;
				default:
					m_font_size = doc_font_size;
					break;
				}
			}
		} else
		{
			if(sz.Units() == css_units_percentage)
			{
				m_font_size = sz.CalcPercent(higher_sz);
			} else if(sz.Units() == css_units_none)
			{
				m_font_size = higher_sz;
			} else
			{
				m_font_size = m_doc->CvtUnits(sz, higher_sz);
			}
		}
	}

	// Initialize font
	String name		= GetStyleProperty("font-family",		true,	"inherit");
	String weight		= GetStyleProperty("font-weight",		true,	"normal");
	String style		= GetStyleProperty("font-style",		true,	"normal");
	String decoration	= GetStyleProperty("text-decoration",	true,	"none");

	m_font = m_doc->GetFont(name, m_font_size, weight, style, decoration, &m_font_metrics);
	
	if (m_font > 1000){
		m_font = m_doc->GetFont(name, m_font_size, weight, style, decoration, &m_font_metrics);
		Panic("errorz here");
	}
}

bool HtmlTag::IsBreak() const
{
	return false;
}

void HtmlTag::SetTagName( const String& tag )
{
	m_tag.Clear();
	for(int i = 0; i < tag.GetCount(); i++) {
		m_tag.Cat( ToLower(tag[i]) );
	}
}

void HtmlTag::DrawBackground( Draw& hdc, int x, int y, const Position& clip )
{
	Position pos = m_pos;
	pos.x	+= x;
	pos.y	+= y;

	Position el_pos = pos;
	el_pos += m_padding;
	el_pos += m_borders;

	Document* m_doc = GetDocument();
	
	if(m_display != display_inline && m_display != display_TableRow)
	{
		if(el_pos.DoesIntersect(clip))
		{
			Background* bg = GetBackground();
			if(bg)
			{
				BackgroundPaint bg_paint;
				InitBackgroundPaint(pos, bg_paint, bg);

				m_doc->GetHtmlFile()->DrawBackground(hdc, bg_paint);
			}
			Position border_box = pos;
			border_box += m_padding;
			border_box += m_borders;

			Borders bdr = m_css_borders;
			bdr.radius = m_css_borders.radius.CalcPercents(border_box.width, border_box.height);

			m_doc->GetHtmlFile()->DrawBorders(hdc, bdr, border_box, GetParentElement() ? false : true);
		}
	} else
	{
		Background* bg = GetBackground();

		Position::vector Boxes;
		GetInLineBoxes(Boxes);

		BackgroundPaint bg_paint;
		Position content_box;

		for(Position::vector::Iterator Box = Boxes.Begin(); Box != Boxes.End(); Box++)
		{
			Box->x	+= x;
			Box->y	+= y;

			if(Box->DoesIntersect(clip))
			{
				content_box = *Box;
				content_box -= m_borders;
				content_box -= m_padding;

				if(bg)
				{
					InitBackgroundPaint(content_box, bg_paint, bg);
				}

				CssBorders bdr;

				// set left Borders radius for the first Box
				if(Box == Boxes.Begin())
				{
					bdr.radius.bottom_left_x	= m_css_borders.radius.bottom_left_x;
					bdr.radius.bottom_left_y	= m_css_borders.radius.bottom_left_y;
					bdr.radius.top_left_x		= m_css_borders.radius.top_left_x;
					bdr.radius.top_left_y		= m_css_borders.radius.top_left_y;
				}

				// set right Borders radius for the last Box
				if(Box == Boxes.End() -1)
				{
					bdr.radius.bottom_right_x	= m_css_borders.radius.bottom_right_x;
					bdr.radius.bottom_right_y	= m_css_borders.radius.bottom_right_y;
					bdr.radius.top_right_x		= m_css_borders.radius.top_right_x;
					bdr.radius.top_right_y		= m_css_borders.radius.top_right_y;
				}

				
				bdr.top		= m_css_borders.top;
				bdr.bottom	= m_css_borders.bottom;
				if(Box == Boxes.Begin())
				{
					bdr.left	= m_css_borders.left;
				}
				if(Box == Boxes.End()-1)
				{
					bdr.right	= m_css_borders.right;
				}


				if(bg)
				{
					bg_paint.border_radius = bdr.radius.CalcPercents(bg_paint.border_box.width, bg_paint.border_box.width);
					m_doc->GetHtmlFile()->DrawBackground(hdc, bg_paint);
				}
				Borders b = bdr;
				b.radius = bdr.radius.CalcPercents(Box->width, Box->height);
				m_doc->GetHtmlFile()->DrawBorders(hdc, b, *Box, false);
			}
		}
	}
}

int HtmlTag::RenderInline( Element* container, int max_width )
{
	int ret_width = 0;
	int rw = 0;
	for(ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		rw = container->PlaceElement( (*i), max_width );
		if(rw > ret_width)
		{
			ret_width = rw;
		}
	}
	return ret_width;
}

int HtmlTag::PlaceElement( Element* el, int max_width )
{
	if(el->GetDisplay() == display_none) return 0;

	if(el->GetDisplay() == display_inline)
	{
		return el->RenderInline(this, max_width);
	}

	ElementPosition el_position = el->GetElementPosition();

	if(el_position == element_position_absolute || el_position == element_position_fixed)
	{
		int line_top = 0;
		if(!m_boxes.IsEmpty())
		{
			if(LAST(m_boxes)->GetType() == Box_line)
			{
				line_top = LAST(m_boxes)->Top();
				if(!LAST(m_boxes)->IsEmpty())
				{
					line_top += LineHeight();
				}
			} else
			{
				line_top = LAST(m_boxes)->Bottom();
			}
		}

		el->Render(0, line_top, max_width);
		el->m_pos.x	+= el->ContentMarginsLeft();
		el->m_pos.y	+= el->ContentMarginsTop();

		return 0;
	}

	int ret_width = 0;

	switch(el->GetFloat())
	{
	case float_left:
		{
			int line_top = 0;
			if(!m_boxes.IsEmpty())
			{
				if(LAST(m_boxes)->GetType() == Box_line)
				{
					line_top = LAST(m_boxes)->Top();
				} else
				{
					line_top = LAST(m_boxes)->Bottom();
				}
			}
			line_top		= GetClearedTop(el, line_top);
			int line_left	= 0;
			int line_right	= max_width;
			GetLineLeftRight(line_top, max_width, line_left, line_right);

			el->Render(line_left, line_top, line_right);
			if(el->Right() > line_right)
			{
				int new_top = FindNextLineTop(el->Top(), el->Width(), max_width);
				el->m_pos.x = GetLineLeft(new_top) + el->ContentMarginsLeft();
				el->m_pos.y = new_top + el->ContentMarginsTop();
			}
			AddFloat(el, 0, 0);
			ret_width = FixLineWidth(max_width, float_left);
			if(!ret_width)
			{
				ret_width = el->Right();
			}
		}
		break;
	case float_right:
		{
			int line_top = 0;
			if(!m_boxes.IsEmpty())
			{
				if(LAST(m_boxes)->GetType() == Box_line)
				{
					line_top = LAST(m_boxes)->Top();
				} else
				{
					line_top = LAST(m_boxes)->Bottom();
				}
			}
			line_top		= GetClearedTop(el, line_top);
			int line_left	= 0;
			int line_right	= max_width;
			GetLineLeftRight(line_top, max_width, line_left, line_right);

			el->Render(0, line_top, line_right);

			if(line_left + el->Width() > line_right)
			{
				int new_top = FindNextLineTop(el->Top(), el->Width(), max_width);
				el->m_pos.x = GetLineRight(new_top, max_width) - el->Width() + el->ContentMarginsLeft();
				el->m_pos.y = new_top + el->ContentMarginsTop();
			} else
			{
				el->m_pos.x = line_right - el->Width() + el->ContentMarginsLeft();
			}
			AddFloat(el, 0, 0);
			ret_width = FixLineWidth(max_width, float_right);

			if(!ret_width)
			{
				line_left	= 0;
				line_right	= max_width;
				GetLineLeftRight(line_top, max_width, line_left, line_right);

				ret_width = ret_width + (max_width - line_right);
			}
		}
		break;
	default:
		{
			int line_top = 0;
			if(!m_boxes.IsEmpty())
			{
				line_top = LAST(m_boxes)->Top();
			}
			int line_left	= 0;
			int line_right	= max_width;
			GetLineLeftRight(line_top, max_width, line_left, line_right);

			switch(el->GetDisplay())
			{
			case display_inline_block:
				ret_width = el->Render(line_left, line_top, line_right);
				break;
			case display_block:		
				if(el->IsReplaced() || el->IsFloatsHolder())
				{
					el->m_pos.width		= el->GetCssWidth().CalcPercent(line_right - line_left);
					el->m_pos.height		= el->GetCssHeight().CalcPercent(el->GetParentElement() ?  el->GetParentElement()->m_pos.height : 0);
				}
				el->CalcOutlines(line_right - line_left);
				break;
			case display_inline_text:
				{
					Upp::Size sz;
					el->GetContentSize(sz, line_right);
					el->m_pos = sz;
				}
				break;
			default:
				ret_width = 0;
				break;
			}

			bool add_box = true;
			if(!m_boxes.IsEmpty())
			{
				if(LAST(m_boxes)->CanHold(el, m_white_space))
				{
					add_box = false;
				}
			}
			if(add_box)
			{
				line_top = NewBox(el, max_width);
			} else if(!m_boxes.IsEmpty())
			{
				line_top = LAST(m_boxes)->Top();
			}

			line_left	= 0;
			line_right	= max_width;
			GetLineLeftRight(line_top, max_width, line_left, line_right);

			if(!el->IsInLineBox())
			{
				if(m_boxes.GetCount() == 1)
				{
					if(CollapseTopMargin())
					{
						int shift = el->MarginTop();
						if(shift >= 0)
						{
							line_top -=shift;
							LAST(m_boxes)->SetYShift(-shift);
						}
					}
				} else
				{
					int shift = 0;
					int prev_margin = m_boxes[m_boxes.GetCount() - 2]->BottomMargin();

					if(prev_margin > el->MarginTop())
					{
						shift = el->MarginTop();
					} else
					{
						shift = prev_margin;
					}
					if(shift >= 0)
					{
						line_top -= shift;
						LAST(m_boxes)->SetYShift(-shift);
					}
				}
			}

			switch(el->GetDisplay())
			{
			case display_table:
			case display_list_item:
				ret_width = el->Render(line_left, line_top, line_right - line_left);
				break;
			case display_block:
			case display_TableCell:
			case display_table_caption:
			case display_TableRow:
				if(el->IsReplaced() || el->IsFloatsHolder())
				{
					ret_width = el->Render(line_left, line_top, line_right - line_left) + line_left + (max_width - line_right);
				} else
				{
					ret_width = el->Render(0, line_top, max_width);
				}
				break;
			default:
				ret_width = 0;
				break;
			}

			LAST(m_boxes)->AddElement(el);

			if(el->IsInLineBox() && !el->Skip())
			{
				ret_width = el->Right() + (max_width - line_right);
			}
		}
		break;
	}

	return ret_width;
}

bool HtmlTag::SetPseudoClass( const String& pclass, bool add )
{
	bool ret = false;
	if(add)
	{
		if(Find(m_pseudo_classes.Begin(), m_pseudo_classes.End(), pclass, StdEqual<String>()) == m_pseudo_classes.End())
		{
			m_pseudo_classes.Add(pclass);
			ret = true;
		}
	} else
	{
		int pi = -1;
		for(int i = 0; i < m_pseudo_classes.GetCount(); i++) {
			if (m_pseudo_classes[i] == pclass)
				pi = i;
		}
		if(pi != -1)
		{
			m_pseudo_classes.Remove(pi);
			ret = true;
		}
	}
	return ret;
}

int HtmlTag::LineHeight() const
{
	return m_line_height;
}

bool HtmlTag::IsReplaced() const
{
	return false;
}

int HtmlTag::FinishLastBox(bool end_of_render)
{
	int line_top = 0;

	if(!m_boxes.IsEmpty())
	{
		LAST(m_boxes)->Finish(end_of_render);

		if(LAST(m_boxes)->IsEmpty())
		{
			line_top = LAST(m_boxes)->Top();
			REMOVELAST(m_boxes);
		}

		if(!m_boxes.IsEmpty())
		{
			line_top = LAST(m_boxes)->Bottom();
		}
	}
	return line_top;
}

int HtmlTag::NewBox( Element* el, int max_width )
{
	int line_top	= GetClearedTop(el, FinishLastBox());

	int line_left	= 0;
	int line_right	= max_width;
	GetLineLeftRight(line_top, max_width, line_left, line_right);

	if(el->IsInLineBox() || el->IsFloatsHolder())
	{
		if(el->Width() > line_right - line_left)
		{
			line_top	= FindNextLineTop(line_top, el->Width(), max_width);
			line_left	= 0;
			line_right	= max_width;
			GetLineLeftRight(line_top, max_width, line_left, line_right);
		}
	}

	int first_line_margin = 0;
	if(m_boxes.IsEmpty() && m_list_style_type != list_style_type_none && m_list_style_position == ListStyleposition_inside)
	{
		int sz_font = GetFontSize();
		first_line_margin = sz_font;
	}

	if(el->IsInLineBox())
	{
		int text_indent = 0;
		if(m_css_text_indent.Value() != 0)
		{
			bool LineBox_found = false;
			for(Box::vector::Iterator iter = m_boxes.Begin(); iter != m_boxes.End(); iter++)
			{
				if((*iter)->GetType() == Box_line)
				{
					LineBox_found = true;
					break;
				}
			}
			if(!LineBox_found)
			{
				text_indent = m_css_text_indent.CalcPercent(max_width);
			}
		}

		FontMetrics fm;
		GetFont(&fm);
		LineBox* lb = new LineBox(GetObjectPool(), line_top, line_left + first_line_margin + text_indent, line_right, LineHeight(), fm, m_text_align);
		m_boxes.Add(lb);
	} else
	{
		BlockBox* bb = new BlockBox(GetObjectPool(), line_top, line_left, line_right);
		m_boxes.Add(bb);
	}

	return line_top;
}

int HtmlTag::GetClearedTop( Element* el, int line_top )
{
	switch(el->GetClear())
	{
	case clear_left:
		{
			int fh = GetLeftFloatsHeight();
			if(fh && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	case clear_right:
		{
			int fh = GetRightGloatsHeight();
			if(fh && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	case clear_both:
		{
			int fh = GetFloatsHeight();
			if(fh && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	default:
		if(el->GetFloat() != float_none)
		{
			int fh = GetFloatsHeight(el->GetFloat());
			if(fh && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	}
	return line_top;
}

StyleDisplay HtmlTag::GetDisplay() const
{
	return m_display;
}

ElementFloat HtmlTag::GetFloat() const
{
	return m_float;
}

bool HtmlTag::IsFloatsHolder() const
{
	if(	m_display == display_inline_block || 
		m_display == display_TableCell || 
		!GetParentElement() || 
		IsBody() || 
		m_float != float_none ||
		m_el_position == element_position_absolute ||
		m_el_position == element_position_fixed ||
		m_overflow > overflow_visible)
	{
		return true;
	}
	return false;
}

bool HtmlTag::IsFirstChildInline( const Element* el )
{
	if(!m_children.IsEmpty())
	{
		for(ElementsVector::Iterator iter = m_children.Begin(); iter != m_children.End(); iter++)
		{
			if(!(*iter)->IsWhiteSpace())
			{
				if(el == (*iter))
				{
					return true;
				}
				if((*iter)->GetDisplay() == display_inline)
				{
					if((*iter)->HaveInlineChild())
					{
						return false;
					}
				} else
				{
					return false;
				}
			}
		}
	}
	return false;
}

bool HtmlTag::IsLastChildInline( const Element* el )
{
	if(!m_children.IsEmpty())
	{
		//for(ElementsVector::reverse_iterator iter = m_children.rbegin(); iter != m_children.rend(); iter++)
		for (int i = m_children.GetCount() - 1; i >= 0; i--)
		{
			ObjectPtr<Element>& iter = m_children[i];
			if(!iter->IsWhiteSpace())
			{
				if(el == iter)
				{
					return true;
				}
				if(iter->GetDisplay() == display_inline)
				{
					if(iter->HaveInlineChild())
					{
						return false;
					}
				} else
				{
					return false;
				}
			}
		}
	}
	return false;
}

WhiteSpace HtmlTag::GetWhiteSpace() const
{
	return m_white_space;
}

VerticalAlign HtmlTag::GetVerticalAlign() const
{
	return m_vertical_align;
}

CssLength HtmlTag::GetCssLeft() const
{
	return m_css_offsets.left;
}

CssLength HtmlTag::GetCssRight() const
{
	return m_css_offsets.right;
}

CssLength HtmlTag::GetCssTop() const
{
	return m_css_offsets.top;
}

CssLength HtmlTag::GetCssBottom() const
{
	return m_css_offsets.bottom;
}


CssOffsets HtmlTag::GetCssOffsets() const
{
	return m_css_offsets;
}

ElementClear HtmlTag::GetClear() const
{
	return m_clear;
}

CssLength HtmlTag::GetCssWidth() const
{
	return m_css_width;
}

CssLength HtmlTag::GetCssHeight() const
{
	return m_css_height;
}

size_t HtmlTag::GetChildrenCount() const
{
	return m_children.GetCount();
}

Element::ptr HtmlTag::GetChild( int idx ) const
{
	return m_children[idx];
}

void HtmlTag::SetCssWidth( CssLength& w )
{
	m_css_width = w;
}

void HtmlTag::ApplyVerticalAlign()
{
	if(!m_boxes.IsEmpty())
	{
		int add = 0;
		int content_height	= LAST(m_boxes)->Bottom();

		if(m_pos.height > content_height)
		{
			switch(m_vertical_align)
			{
			case va_middle:
				add = (m_pos.height - content_height) / 2;
				break;
			case va_bottom:
				add = m_pos.height - content_height;
				break;
			default:
				add = 0;
				break;
			}
		}

		if(add)
		{
			for(size_t i = 0; i < m_boxes.GetCount(); i++)
			{
				m_boxes[i]->SetYShift(add);
			}
		}
	}
}

ElementPosition HtmlTag::GetElementPosition(CssOffsets* offsets) const
{
	if(offsets && m_el_position != element_position_static)
	{
		*offsets = m_css_offsets;
	}
	return m_el_position;
}

void HtmlTag::InitBackgroundPaint( Position pos, BackgroundPaint &bg_paint, Background* bg )
{
	if(!bg) return;

	bg_paint = *bg;
	Position content_box	= pos;
	Position padding_box	= pos;
	padding_box += m_padding;
	Position border_box		= padding_box;
	border_box += m_borders;

	switch(bg->m_clip)
	{
	case background_box_padding:
		bg_paint.clip_box = padding_box;
		break;
	case background_box_content:
		bg_paint.clip_box = content_box;
		break;
	default:
		bg_paint.clip_box = border_box;
		break;
	}

	switch(bg->m_origin)
	{
	case background_box_border:
		bg_paint.origin_box = border_box;
		break;
	case background_box_content:
		bg_paint.origin_box = content_box;
		break;
	default:
		bg_paint.origin_box = padding_box;
		break;
	}

	if(!bg_paint.image.IsEmpty())
	{
		GetDocument()->GetHtmlFile()->GetImageSize(bg_paint.image, bg_paint.baseurl, bg_paint.image_size);
		
		if(bg_paint.image_size.cx && bg_paint.image_size.cy)
		{
			Upp::Size img_new_sz = bg_paint.image_size;
			double img_ar_width		= (double) bg_paint.image_size.cx / (double) bg_paint.image_size.cy;
			double img_ar_height	= (double) bg_paint.image_size.cy / (double) bg_paint.image_size.cx;


			if(bg->m_position.width.IsPredefined())
			{
				switch(bg->m_position.width.PreDefine())
				{
				case background_size_contain:
					if( (int) ((double) bg_paint.origin_box.width * img_ar_height) <= bg_paint.origin_box.height )
					{
						img_new_sz.cx = bg_paint.origin_box.width;
						img_new_sz.cy	= (int) ((double) bg_paint.origin_box.width * img_ar_height);
					} else
					{
						img_new_sz.cy = bg_paint.origin_box.height;
						img_new_sz.cx	= (int) ((double) bg_paint.origin_box.height * img_ar_width);
					}
					break;
				case background_size_cover:
					if( (int) ((double) bg_paint.origin_box.width * img_ar_height) >= bg_paint.origin_box.height )
					{
						img_new_sz.cx = bg_paint.origin_box.width;
						img_new_sz.cy	= (int) ((double) bg_paint.origin_box.width * img_ar_height);
					} else
					{
						img_new_sz.cy = bg_paint.origin_box.height;
						img_new_sz.cx	= (int) ((double) bg_paint.origin_box.height * img_ar_width);
					}
					break;
					break;
				case background_size_auto:
					if(!bg->m_position.height.IsPredefined())
					{
						img_new_sz.cy	= bg->m_position.height.CalcPercent(bg_paint.origin_box.height);
						img_new_sz.cx	= (int) ((double) img_new_sz.cy * img_ar_width);
					}
					break;
				}
			} else
			{
				img_new_sz.cx = bg->m_position.width.CalcPercent(bg_paint.origin_box.width);
				if(bg->m_position.height.IsPredefined())
				{
					img_new_sz.cy = (int) ((double) img_new_sz.cx * img_ar_height);
				} else
				{
					img_new_sz.cy = bg->m_position.height.CalcPercent(bg_paint.origin_box.height);
				}
			}

			bg_paint.image_size = img_new_sz;
			bg_paint.position_x = bg_paint.origin_box.x + (int) bg->m_position.x.CalcPercent(bg_paint.origin_box.width - bg_paint.image_size.cx);
			bg_paint.position_y = bg_paint.origin_box.y + (int) bg->m_position.y.CalcPercent(bg_paint.origin_box.height - bg_paint.image_size.cy);
		}

	}
	bg_paint.border_radius	= m_css_borders.radius.CalcPercents(border_box.width, border_box.height);;
	bg_paint.border_box		= border_box;
	bg_paint.is_root		= GetParentElement() ? false : true;
}

Visibility HtmlTag::GetVisibility() const
{
	return m_visibility;
}

void HtmlTag::DrawListMarker( Draw& hdc, const Position &pos )
{
	ListMarker lm;

	String list_image = GetStyleProperty("list-style-image", true, "");
	Size img_size(0,0);
	//if(list_image)
	if(list_image.GetCount())
	{
		Css::ParseCssUrl(list_image, lm.image);
		lm.baseurl = GetStyleProperty("list-style-image-baseurl", true, "");
		GetDocument()->GetHtmlFile()->GetImageSize(lm.image, lm.baseurl, img_size);
	} else
	{
		lm.baseurl = 0;
	}

	int ln_height	= LineHeight();
	int sz_font		= GetFontSize();
	lm.pos.x		= pos.x;
	lm.pos.width	= sz_font	- sz_font * 2 / 3;
	lm.pos.height	= sz_font	- sz_font * 2 / 3;
	lm.pos.y		= pos.y		+ ln_height / 2 - lm.pos.height / 2;

	if(img_size.cx && img_size.cy)
	{
		if(lm.pos.y + img_size.cy > pos.y + pos.height)
		{
			lm.pos.y = pos.y + pos.height - img_size.cy;
		}
		if(img_size.cx > lm.pos.width)
		{
			lm.pos.x -= img_size.cx - lm.pos.width;
		}

		lm.pos.width	= img_size.cx;
		lm.pos.height	= img_size.cy;
	}
	if(m_list_style_position == ListStyleposition_outside)
	{
		lm.pos.x -= sz_font;
	}

	lm.color = GetColor("color", true, WebColor(0, 0, 0));
	lm.marker_type = m_list_style_type;
	GetDocument()->GetHtmlFile()->DrawListMarker(hdc, lm);
}

void HtmlTag::DrawChildren( Draw& hdc, int x, int y, const Position& clip, DrawFlag flag, int zindex )
{
	if (m_display == display_table || m_display == display_inline_table)
	{
		DrawChildrenTable(hdc, x, y, clip, flag, zindex);
	}
	else
	{
		DrawChildrenBox(hdc, x, y, clip, flag, zindex);
	}
}

bool HtmlTag::FetchPositioned()
{
	bool ret = false;

	m_positioned.Clear();

	for(ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		Element* el = (*i);
		if(el->GetElementPosition() != element_position_static)
		{
			AddPositioned(el);
		}
		if(!ret && (el->GetElementPosition() == element_position_absolute || el->GetElementPosition() == element_position_fixed))
		{
			ret = true;
		}
		if(el->FetchPositioned())
		{
			ret = true;
		}
	}
	return ret;
}

int HtmlTag::GetZIndex() const
{
	return m_z_index;
}

void HtmlTag::RenderPositioned(RenderType rt)
{
	Position wnd_position;
	GetDocument()->GetHtmlFile()->GetClientRect(wnd_position);

	ElementPosition el_position;
	Element* el;
	bool process;
	for(ElementsVector::Iterator abs_el = m_positioned.Begin(); abs_el != m_positioned.End(); abs_el++)
	{
		el = (*abs_el);
		el_position = el->GetElementPosition();

		process = false;
		if(el->GetDisplay() != display_none)
		{
			if(el_position == element_position_absolute)
			{
				if(rt != render_fixed_only)
				{
					process = true;
				}
			} else if(el_position == element_position_fixed)
			{
				if(rt != render_no_fixed)
				{
					process = true;
				}
			}
		}

		if(process)
		{
			int higher_height	= 0;
			int higher_width	= 0;
			int client_x		= 0;
			int client_y		= 0;
			if(el_position == element_position_fixed)
			{
				higher_height	= wnd_position.height;
				higher_width	= wnd_position.width;
				client_x		= wnd_position.Left();
				client_y		= wnd_position.Top();
			} else
			{
				if(el->GetParentElement())
				{
					higher_height	= el->GetParentElement()->Height();
					higher_width	= el->GetParentElement()->Width();
				}
			}

			CssLength	css_left	= el->GetCssLeft();
			CssLength	css_right	= el->GetCssRight();
			CssLength	css_top		= el->GetCssTop();
			CssLength	css_bottom	= el->GetCssBottom();

			bool need_render = false;

			CssLength el_w = el->GetCssWidth();
			CssLength el_h = el->GetCssHeight();

            int new_width = -1;
            int new_height = -1;
			if(el_w.Units() == css_units_percentage && higher_width)
			{
                new_width = el_w.CalcPercent(higher_width);
                if(el->m_pos.width != new_width)
				{
					need_render = true;
                    el->m_pos.width = new_width;
				}
			}

			if(el_h.Units() == css_units_percentage && higher_height)
			{
                new_height = el_h.CalcPercent(higher_height);
                if(el->m_pos.height != new_height)
				{
					need_render = true;
                    el->m_pos.height = new_height;
				}
			}

			bool cvt_x = false;
			bool cvt_y = false;

			if(el_position == element_position_fixed)
			{
				if(!css_left.IsPredefined() || !css_right.IsPredefined())
				{
					if(!css_left.IsPredefined() && css_right.IsPredefined())
					{
						el->m_pos.x = css_left.CalcPercent(higher_width) + el->ContentMarginsLeft();
					} else if(css_left.IsPredefined() && !css_right.IsPredefined())
					{
						el->m_pos.x = higher_width - css_right.CalcPercent(higher_width) - el->m_pos.width - el->ContentMarginsRight();
					} else
					{
						el->m_pos.x		= css_left.CalcPercent(higher_width) + el->ContentMarginsLeft();
						el->m_pos.width	= higher_width - css_left.CalcPercent(higher_width) - css_right.CalcPercent(higher_width) - (el->ContentMarginsLeft() + el->ContentMarginsRight());
						need_render = true;
					}
				}

				if(!css_top.IsPredefined() || !css_bottom.IsPredefined())
				{
					if(!css_top.IsPredefined() && css_bottom.IsPredefined())
					{
						el->m_pos.y = css_top.CalcPercent(higher_height) + el->ContentMarginsTop();
					} else if(css_top.IsPredefined() && !css_bottom.IsPredefined())
					{
						el->m_pos.y = higher_height - css_bottom.CalcPercent(higher_height) - el->m_pos.height - el->ContentMarginsBottom();
					} else
					{
						el->m_pos.y			= css_top.CalcPercent(higher_height) + el->ContentMarginsTop();
						el->m_pos.height	= higher_height - css_top.CalcPercent(higher_height) - css_bottom.CalcPercent(higher_height) - (el->ContentMarginsTop() + el->ContentMarginsBottom());
						need_render = true;
					}
				}
			} else 
			{
				if(!css_left.IsPredefined() || !css_right.IsPredefined())
				{
					if(!css_left.IsPredefined() && css_right.IsPredefined())
					{
						el->m_pos.x = css_left.CalcPercent(higher_width) + el->ContentMarginsLeft() - m_padding.left;
					} else if(css_left.IsPredefined() && !css_right.IsPredefined())
					{
						el->m_pos.x = m_pos.width + m_padding.right - css_right.CalcPercent(higher_width) - el->m_pos.width - el->ContentMarginsRight();
					} else
					{
						el->m_pos.x		= css_left.CalcPercent(higher_width) + el->ContentMarginsLeft() - m_padding.left;
						el->m_pos.width	= m_pos.width + m_padding.left + m_padding.right - css_left.CalcPercent(higher_width) - css_right.CalcPercent(higher_width) - (el->ContentMarginsLeft() + el->ContentMarginsRight());
                        if (new_width != -1)
                        {
                            el->m_pos.x += (el->m_pos.width - new_width) / 2;
                            el->m_pos.width = new_width;
                        }
                        need_render = true;
					}
					cvt_x = true;
				}

				if(!css_top.IsPredefined() || !css_bottom.IsPredefined())
				{
					if(!css_top.IsPredefined() && css_bottom.IsPredefined())
					{
						el->m_pos.y = css_top.CalcPercent(higher_height) + el->ContentMarginsTop() - m_padding.top;
					} else if(css_top.IsPredefined() && !css_bottom.IsPredefined())
					{
						el->m_pos.y = m_pos.height + m_padding.bottom - css_bottom.CalcPercent(higher_height) - el->m_pos.height - el->ContentMarginsBottom();
					} else
					{
						el->m_pos.y			= css_top.CalcPercent(higher_height) + el->ContentMarginsTop() - m_padding.top;
						el->m_pos.height	= m_pos.height + m_padding.top + m_padding.bottom - css_top.CalcPercent(higher_height) - css_bottom.CalcPercent(higher_height) - (el->ContentMarginsTop() + el->ContentMarginsBottom());
                        if (new_height != -1)
                        {
                            el->m_pos.y += (el->m_pos.height - new_height) / 2;
                            el->m_pos.height = new_height;
                        }
                        need_render = true;
					}
					cvt_y = true;
				}
			}

			if(cvt_x || cvt_y)
			{
				int offset_x = 0;
				int offset_y = 0;
				Element* cur_el = el->GetParentElement();
				while(cur_el && cur_el != this)
				{
					offset_x += cur_el->m_pos.x;
					offset_y += cur_el->m_pos.y;
					cur_el = cur_el->GetParentElement();
				}
				if(cvt_x)	el->m_pos.x -= offset_x;
				if(cvt_y)	el->m_pos.y -= offset_y;
			}

			if(need_render)
			{
				Position pos = el->m_pos;
				el->Render(el->Left(), el->Top(), el->Width(), true);
				el->m_pos = pos;
			}

			if(el_position == element_position_fixed)
			{
				Position fixed_pos;
				el->GetRedrawBox(fixed_pos);
				GetDocument()->AddFixedBox(fixed_pos);
			}
		}

		el->RenderPositioned();
	}

	if(!m_positioned.IsEmpty())
	{
		Sort(m_positioned, ElementZIndexSort());
	}
}

void HtmlTag::DrawStackingContext( Draw& hdc, int x, int y, const Position& clip, bool with_positioned )
{
	if(!IsVisible()) return;

	ArrayMap<int, bool> zindexes;
	if(with_positioned)
	{
		for(ElementsVector::Iterator i = m_positioned.Begin(); i != m_positioned.End(); i++)
		{
			zindexes.GetAdd((*i)->GetZIndex()) = false;
		}

		for (int i = 0; i < zindexes.GetCount(); i++)
		{
			int j = zindexes.GetKey(i);
			if(j < 0)
			{
				//DrawChildren(hdc, x, y, clip, draw_positioned, idx->first);
				DrawChildren(hdc, x, y, clip, draw_positioned, j);
			}
		}
	}
	DrawChildren(hdc, x, y, clip, draw_block, 0);
	DrawChildren(hdc, x, y, clip, draw_floats, 0);
	DrawChildren(hdc, x, y, clip, draw_inlines, 0);
	if(with_positioned)
	{
		for (int i = 0; i < zindexes.GetCount(); i++)
		{
			int j = zindexes.GetKey(i);
			if(j == 0)
			{
				DrawChildren(hdc, x, y, clip, draw_positioned, j);
			}
		}

		for (int i = 0; i < zindexes.GetCount(); i++)
		{
			int j = zindexes.GetKey(i);
			if(j > 0)
			{
				DrawChildren(hdc, x, y, clip, draw_positioned, j);
			}
		}
	}
}

Overflow HtmlTag::GetOverflow() const
{
	return m_overflow;
}

bool HtmlTag::IsNthChild( Element* el, int num, int off, bool of_type )
{
	int idx = 1;
	for(ElementsVector::Iterator child = m_children.Begin(); child != m_children.End(); child++)
	{
		if((*child)->GetDisplay() != display_inline_text)
		{
			if( (!of_type) || (of_type && !el->GetTagName().Compare((*child)->GetTagName())) )
			{
				if(el == (*child))
				{
					if(num != 0)
					{
						if((idx - off) >= 0 && (idx - off) % num == 0)
						{
							return true;
						}

					} else if(idx == off)
					{
						return true;
					}
					return false;
				}
				idx++;
			}
			if(el == (*child)) break;
		}
	}
	return false;
}

bool HtmlTag::IsNthLastChild( Element* el, int num, int off, bool of_type )
{
	int idx = 1;
	for (int i = m_children.GetCount() - 1; i >= 0; i--)
	{
		ObjectPtr<Element>& child = m_children[i];
		
		if(child->GetDisplay() != display_inline_text)
		{
			if( !of_type || (of_type && !el->GetTagName().Compare(child->GetTagName())) )
			{
				if(el == child)
				{
					if(num != 0)
					{
						if((idx - off) >= 0 && (idx - off) % num == 0)
						{
							return true;
						}

					} else if(idx == off)
					{
						return true;
					}
					return false;
				}
				idx++;
			}
			if(el == child) break;
		}
	}
	return false;
}

void HtmlTag::ParseNthChildParams( String param, int &num, int &off )
{
	if(param == "odd")
	{
		num = 2;
		off = 1;
	} else if(param == "even")
	{
		num = 2;
		off = 0;
	} else
	{
		StringVector tokens;
		SplitString(param, tokens, " n", "n");

		String s_num;
		String s_off;

		String s_int;
		for(StringVector::Iterator tok = tokens.Begin(); tok != tokens.End(); tok++)
		{
			if((*tok) == "n")
			{
				s_num = s_int;
				s_int.Clear();
			} else
			{
				s_int += (*tok);
			}
		}
		s_off = s_int;

		num = StrInt(s_num);
		off = StrInt(s_off);
	}
}

void HtmlTag::CalcDocumentSize( Upp::Size& sz, int x /*= 0*/, int y /*= 0*/ )
{
	if(IsVisible() && m_el_position != element_position_fixed)
	{
		Element::CalcDocumentSize(sz, x, y);

		if(m_overflow == overflow_visible)
		{
			for(ElementsVector::Iterator el = m_children.Begin(); el != m_children.End(); el++)
			{
				(*el)->CalcDocumentSize(sz, x + m_pos.x, y + m_pos.y);
			}
		}

		// root element (<html>) must to cover entire window
		if(!GetParentElement())
		{
			Position client_pos;
			GetDocument()->GetHtmlFile()->GetClientRect(client_pos);
			m_pos.height = max(sz.cy, client_pos.height) - ContentMarginsTop() - ContentMarginsBottom();
			m_pos.width	 = max(sz.cx, client_pos.width) - ContentMarginsLeft() - ContentMarginsRight();
		}
	}
}


void HtmlTag::GetRedrawBox(Position& pos, int x /*= 0*/, int y /*= 0*/)
{
	if(IsVisible())
	{
		Element::GetRedrawBox(pos, x, y);

		if(m_overflow == overflow_visible)
		{
			for(ElementsVector::Iterator el = m_children.Begin(); el != m_children.End(); el++)
			{
				if((*el)->GetElementPosition() != element_position_fixed)
				{
					(*el)->GetRedrawBox(pos, x + m_pos.x, y + m_pos.y);
				}
			}
		}
	}
}

Element* HtmlTag::FindAdjacentSibling( Element* el, const CssSelector& Selector, bool apply_pseudo /*= true*/, bool* is_pseudo /*= 0*/ )
{
	Element* ret = 0;
	for(ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		Element* e = (*i);
		if(e->GetDisplay() != display_inline_text)
		{
			if(e == el)
			{
				if(ret)
				{
					int res = ret->Select(Selector, apply_pseudo);
					if(res != select_no_match)
					{
						if(is_pseudo)
						{
							if(res & select_match_pseudo_class)
							{
								*is_pseudo = true;
							} else
							{
								*is_pseudo = false;
							}
						}
						return ret;
					}
				}
				return 0;
			} else
			{
				ret = e;
			}
		}
	}
	return 0;
}

Element* HtmlTag::FindSibling( Element* el, const CssSelector& Selector, bool apply_pseudo /*= true*/, bool* is_pseudo /*= 0*/ )
{
	Element* ret = 0;
	for(ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		Element* e = (*i);
		if(e->GetDisplay() != display_inline_text)
		{
			if(e == el)
			{
				return ret;
			} else if(!ret)
			{
				int res = e->Select(Selector, apply_pseudo);
				if(res != select_no_match)
				{
					if(is_pseudo)
					{
						if(res & select_match_pseudo_class)
						{
							*is_pseudo = true;
						} else
						{
							*is_pseudo = false;
						}
					}
					ret = e;
				}
			}
		}
	}
	return 0;
}

bool HtmlTag::IsOnlyChild( Element* el, bool of_type )
{
	int child_count = 0;
	for(ElementsVector::Iterator child = m_children.Begin(); child != m_children.End(); child++)
	{
		if((*child)->GetDisplay() != display_inline_text)
		{
			if( !of_type || (of_type && !el->GetTagName().Compare((*child)->GetTagName())) )
			{
				child_count++;
			}
			if(child_count > 1) break;
		}
	}
	if(child_count > 1)
	{
		return false;
	}
	return true;
}

void HtmlTag::UpdateFloats( int dy, Element* higher )
{
	if(IsFloatsHolder())
	{
		bool reset_cache = false;
		for (int i = m_floats_left.GetCount() - 1; i >= 0; i--)
		{
			FloatedBox& fb = m_floats_left[i];
			if(fb.el->IsAncestor(higher))
			{
				reset_cache	= true;
				fb.pos.y	+= dy;
			}
		}
		if(reset_cache)
		{
			m_cahe_line_left.invalidate();
		}
		reset_cache = false;
		for (int i = m_floats_right.GetCount() - 1; i >= 0; i--) {
			FloatedBox& fb = m_floats_right[i];
			if(fb.el->IsAncestor(higher))
			{
				reset_cache	= true;
				//fb->pos.y	+= dy;
				fb.pos.y	+= dy;
			}
		}
		if(reset_cache)
		{
			m_cahe_line_right.invalidate();
		}
	} else
	{
		GetParentElement()->UpdateFloats(dy, higher);
	}
}

void HtmlTag::RemoveBeforeAfter()
{
	if(!m_children.IsEmpty())
	{
		if( !m_children[0]->GetTagName().Compare("::before") )
		{
			m_children.Remove(0);
		}
	}
	if(!m_children.IsEmpty())
	{
		if( !LAST(m_children)->GetTagName().Compare("::after") )
		{
			m_children.Remove(m_children.GetCount() - 1);
		}
	}
}

Element* HtmlTag::GetElementBefore()
{
	if(!m_children.IsEmpty())
	{
		if( !m_children[0]->GetTagName().Compare("::before") )
		{
			return m_children[0];
		}
	}
	Element* el = new ElBefore(*GetDocument());
	el->MoveTo(*this);
	m_children.Insert(0, el);
	return el;
}

Element* HtmlTag::GetElementAfter()
{
	if(!m_children.IsEmpty())
	{
		if( !LAST(m_children)->GetTagName().Compare("::after") )
		{
			return LAST(m_children);
		}
	}
	Element* el = new ElAfter(*GetDocument());
	el->MoveTo(*this);
	m_children.Add(el);
	return el;
}

void HtmlTag::AddStyle( Style::ptr st )
{
	m_style.Combine(*st);
}

bool HtmlTag::HaveInlineChild()
{
	if(!m_children.IsEmpty())
	{
		for(ElementsVector::Iterator iter = m_children.Begin(); iter != m_children.End(); iter++)
		{
			if(!(*iter)->IsWhiteSpace())
			{
				return true;
			}
		}
	}
	return false;
}

void HtmlTag::RefreshStyles()
{
	RemoveBeforeAfter();

	for(ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		if((*i)->GetDisplay() != display_inline_text)
		{
			(*i)->RefreshStyles();
		}
	}

	m_style.Clear();

	for(UsedSelector::vector::Iterator sel = m_used_styles.Begin(); sel != m_used_styles.End(); sel++)
	{
		UsedSelector* usel = (*sel);
		usel->m_used = false;

		if(usel->m_selector->IsMediaValid())
		{
			int apply = Select(*usel->m_selector, false);

			if(apply != select_no_match)
			{
				if(apply & select_match_pseudo_class)
				{
					if(Select(*usel->m_selector, true))
					{
						AddStyle(usel->m_selector->m_style);
						usel->m_used = true;
					}
				} else if(apply & select_match_with_after)
				{
					Element* el = GetElementAfter();
					if(el)
					{
						el->AddStyle(usel->m_selector->m_style);
					}
				} else if(apply & select_match_with_before)
				{
					Element* el = GetElementBefore();
					if(el)
					{
						el->AddStyle(usel->m_selector->m_style);
					}
				} else
				{
					AddStyle(usel->m_selector->m_style);
					usel->m_used = true;
				}
			}
		}
	}
}

Element* HtmlTag::GetChildByPoint(int x, int y, int client_x, int client_y, DrawFlag flag, int zindex)
{
	Element* ret = 0;

	if(m_overflow > overflow_visible)
	{
		if(!m_pos.IsPointInside(x, y))
		{
			return ret;
		}
	}

	Position pos = m_pos;
	pos.x	= x - pos.x;
	pos.y	= y - pos.y;

	for (int i = m_children.GetCount() - 1; i >= 0; i--)
	{
		Element* el = m_children[i];
		Element* iter = el;

		if(el->IsVisible() && el->GetDisplay() != display_inline_text)
		{
			switch(flag)
			{
			case draw_positioned:
				if(el->IsPositioned() && el->GetZIndex() == zindex)
				{
					if(el->GetElementPosition() == element_position_fixed)
					{
						ret = el->GetElementByPoint(client_x, client_y, client_x, client_y);
						if(!ret && iter->IsPointInside(client_x, client_y))
						{
							ret = iter;
						}
					} else
					{
						ret = el->GetElementByPoint(pos.x, pos.y, client_x, client_y);
						if(!ret && iter->IsPointInside(pos.x, pos.y))
						{
							ret = iter;
						}
					}
					el = 0;
				}
				break;
			case draw_block:
				if(!el->IsInLineBox() && el->GetFloat() == float_none && !el->IsPositioned())
				{
					if(el->IsPointInside(pos.x, pos.y))
					{
						ret = el;
					}
				}
				break;
			case draw_floats:
				if(el->GetFloat() != float_none && !el->IsPositioned())
				{
					ret = el->GetElementByPoint(pos.x, pos.y, client_x, client_y);

					if(!ret && iter->IsPointInside(pos.x, pos.y))
					{
						ret = iter;
					}
					el = 0;
				}
				break;
			case draw_inlines:
				if(el->IsInLineBox() && el->GetFloat() == float_none && !el->IsPositioned())
				{
					if(el->GetDisplay() == display_inline_block)
					{
						ret = el->GetElementByPoint(pos.x, pos.y, client_x, client_y);
						el = 0;
					}
					if(!ret && iter->IsPointInside(pos.x, pos.y))
					{
						ret = iter;
					}
				}
				break;
			default:
				break;
			}

			if(el && !el->IsPositioned())
			{
				if(flag == draw_positioned)
				{
					Element* child = el->GetChildByPoint(pos.x, pos.y, client_x, client_y, flag, zindex);
					if(child)
					{
						ret = child;
					}
				} else
				{
					if(	el->GetFloat() == float_none &&
						el->GetDisplay() != display_inline_block)
					{
						Element* child = el->GetChildByPoint(pos.x, pos.y, client_x, client_y, flag, zindex);
						if(child)
						{
							ret = child;
						}
					}
				}
			}
		}
	}

	return ret;
}

Element* HtmlTag::GetElementByPoint( int x, int y, int client_x, int client_y )
{
	if(!IsVisible()) return 0;

	Element* ret = 0;

	ArrayMap<int, bool> zindexes;

	for(ElementsVector::Iterator i = m_positioned.Begin(); i != m_positioned.End(); i++)
	{
		zindexes[(*i)->GetZIndex()];
	}

	for (int i = 0; i < zindexes.GetCount(); i++)
	{
		int j = zindexes.GetKey(i);
		if (j > 0)
		{
			ret = GetChildByPoint(x, y, client_x, client_y, draw_positioned, j);
		}
	}
	if(ret) return ret;

	for (int i = 0; i < zindexes.GetCount(); i++)
	{
		int j = zindexes.GetKey(i);
		if (j > 0)
		{
			ret = GetChildByPoint(x, y, client_x, client_y, draw_positioned, j);
		}
	}
	if(ret) return ret;

	ret = GetChildByPoint(x, y, client_x, client_y, draw_inlines, 0);
	if(ret) return ret;

	ret = GetChildByPoint(x, y, client_x, client_y, draw_floats, 0);
	if(ret) return ret;

	ret = GetChildByPoint(x, y, client_x, client_y, draw_block, 0);
	if(ret) return ret;


	for (int i = 0; i < zindexes.GetCount() && !ret; i++)
	{
		int j = zindexes.GetKey(i);
		if (j < 0)
		{
			ret = GetChildByPoint(x, y, client_x, client_y, draw_positioned, j);
		}
	}
	if(ret) return ret;

	if(m_el_position == element_position_fixed)
	{
		if(IsPointInside(client_x, client_y))
		{
			ret = this;
		}
	} else
	{
		if(IsPointInside(x, y))
		{
			ret = this;
		}
	}

	return ret;
}

Background* HtmlTag::GetBackground(bool own_only)
{
	if(own_only)
	{
		// return own background with check for empty one
		if(m_bg.m_image.IsEmpty() && !m_bg.m_color.alpha)
		{
			return 0;
		}
		return &m_bg;
	}

	if(m_bg.m_image.IsEmpty() && !m_bg.m_color.alpha)
	{
		// if this is root element (<html>) try to get background from body
		if(!GetParentElement())
		{
			for(ElementsVector::Iterator iter = m_children.Begin(); iter != m_children.End(); iter++)
			{
				if( (*iter)->IsBody() )
				{
					// return own body background
					return (*iter)->GetBackground(true);
				}
			}
		}
		return 0;
	}
	
	if(IsBody())
	{
		if(!GetParentElement()->GetBackground(true))
		{
			// higher of body will draw background for body
			return 0;
		}
	}
	
	if(m_bg.cache_id == -1 && !m_bg.m_image.IsEmpty()) {
		String base_url;
		if (m_bg.m_baseurl.GetCount())
			base_url = m_bg.m_baseurl;
		else
			base_url = GetBaseUrl();
		
		m_bg.cache_id = GetCache().FindCacheObject(m_bg.m_image, base_url);
	}

	return &m_bg;
}

int HtmlTag::RenderBox(int x, int y, int max_width, bool second_pass /*= false*/)
{
	int higher_width = max_width;
	Element* m_higher = GetParentElement();
	
	CalcOutlines(higher_width);

	m_pos.Clear();
	m_pos.MoveTo(x, y);

	m_pos.x += ContentMarginsLeft();
	m_pos.y += ContentMarginsTop();

	int ret_width = 0;

	DefValue<int>	block_width(0);

	if (m_display != display_TableCell && !m_css_width.IsPredefined())
	{
		int w = CalcWidth(higher_width);
		if (m_box_sizing == box_sizing_border_box)
		{
			w -= m_padding.left + m_borders.left + m_padding.right + m_borders.right;
		}
		ret_width = max_width = block_width = w;
	}
	else
	{
		if (max_width)
		{
			max_width -= ContentMarginsLeft() + ContentMarginsRight();
		}
	}

	// check for max-width
	if (!m_css_max_width.IsPredefined())
	{
		int mw = GetDocument()->CvtUnits(m_css_max_width, m_font_size, higher_width);
		if (m_box_sizing == box_sizing_border_box)
		{
			mw -= m_padding.left + m_borders.left + m_padding.right + m_borders.right;
		}
		if (max_width > mw)
		{
			max_width = mw;
		}
	}

	m_floats_left.Clear();
	m_floats_right.Clear();
	m_boxes.Clear();
	m_cahe_line_left.invalidate();
	m_cahe_line_right.invalidate();

	Element* el;
	ElementPosition el_position;

	int block_height = 0;

	m_pos.height = 0;

	if (GetPredefinedHeight(block_height))
	{
		m_pos.height = block_height;
	}

	for (ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		el = (*i);
		el_position = el->GetElementPosition();
		if ((el_position == element_position_absolute || el_position == element_position_fixed) && second_pass) continue;

		int rw = PlaceElement(el, max_width);
		if (rw > ret_width)
		{
			ret_width = rw;
		}
	}

	FinishLastBox(true);

	if (block_width.is_default() && IsInLineBox())
	{
		m_pos.width = ret_width;
	}
	else
	{
		m_pos.width = max_width;
	}
	CalcAutoMargins(higher_width);

	if (!m_boxes.IsEmpty())
	{
		if (CollapseTopMargin())
		{
			int old_top = m_margins.top;
			m_margins.top = max(m_boxes[0]->TopMargin(), m_margins.top);
			if (m_margins.top != old_top)
			{
				UpdateFloats(m_margins.top - old_top, this);
			}
		}
		if (CollapseBottomMargin())
		{
			m_margins.bottom = max(LAST(m_boxes)->BottomMargin(), m_margins.bottom);
			m_pos.height = LAST(m_boxes)->Bottom() - LAST(m_boxes)->BottomMargin();
		}
		else
		{
			m_pos.height = LAST(m_boxes)->Bottom();
		}
	}

	// add the floats height to the block height
	if (IsFloatsHolder())
	{
		int floats_height = GetFloatsHeight();
		if (floats_height > m_pos.height)
		{
			m_pos.height = floats_height;
		}
	}

	// calculate the final Position

	m_pos.MoveTo(x, y);
	m_pos.x += ContentMarginsLeft();
	m_pos.y += ContentMarginsTop();

	if (GetPredefinedHeight(block_height))
	{
		m_pos.height = block_height;
	}

	int min_height = 0;
	if (!m_css_min_height.IsPredefined() && m_css_min_height.Units() == css_units_percentage)
	{
		if (m_higher)
		{
			if (m_higher->GetPredefinedHeight(block_height))
			{
				min_height = m_css_min_height.CalcPercent(block_height);
			}
		}
	}
	else
	{
		min_height = (int)m_css_min_height.Value();
	}
	if (min_height != 0 && m_box_sizing == box_sizing_border_box)
	{
		min_height -= m_padding.top + m_borders.top + m_padding.bottom + m_borders.bottom;
		if (min_height < 0) min_height = 0;
	}

	if (m_display == display_list_item)
	{
		String list_image = GetStyleProperty("list-style-image", true, "");
		//if (list_image)
		if (list_image.GetCount())
		{
			String url;
			Css::ParseCssUrl(list_image, url);

			Size sz;
			String list_image_baseurl = GetStyleProperty("list-style-image-baseurl", true, "");
			GetDocument()->GetHtmlFile()->GetImageSize(url, list_image_baseurl, sz);
			if (min_height < sz.cy)
			{
				min_height = sz.cy;
			}
		}

	}

	if (min_height > m_pos.height)
	{
		m_pos.height = min_height;
	}

	int min_width = m_css_min_width.CalcPercent(higher_width);

	if (min_width != 0 && m_box_sizing == box_sizing_border_box)
	{
		min_width -= m_padding.left + m_borders.left + m_padding.right + m_borders.right;
		if (min_width < 0) min_width = 0;
	}

	if (min_width != 0)
	{
		if (min_width > m_pos.width)
		{
			m_pos.width = min_width;
		}
		if (min_width > ret_width)
		{
			ret_width = min_width;
		}
	}

	ret_width += ContentMarginsLeft() + ContentMarginsRight();

	// re-render with new width
	if (ret_width < max_width && !second_pass && m_higher)
	{
		if (m_display == display_inline_block ||
			m_css_width.IsPredefined() &&
			(m_float != float_none ||
			m_display == display_table ||
			m_el_position == element_position_absolute ||
			m_el_position == element_position_fixed
			)
			)
		{
			Render(x, y, ret_width, true);
			m_pos.width = ret_width - (ContentMarginsLeft() + ContentMarginsRight());
		}
	}

	return ret_width;
}

int HtmlTag::RenderTable(int x, int y, int max_width, bool second_pass /*= false*/)
{
	int higher_width = max_width;
	Element* m_higher = GetParentElement();
	
	CalcOutlines(higher_width);

	m_pos.Clear();
	m_pos.MoveTo(x, y);

	m_pos.x += ContentMarginsLeft();
	m_pos.y += ContentMarginsTop();

	DefValue<int>	block_width(0);

	if (!m_css_width.IsPredefined())
	{
		max_width = block_width = CalcWidth(higher_width - (ContentMarginsLeft() + ContentMarginsRight()));
	}
	else
	{
		if (max_width)
		{
			max_width -= ContentMarginsLeft() + ContentMarginsRight();
		}
	}

	// Calculate table spacing
	int table_width_spacing = 0;
	if (m_borderCollapse == border_collapse_separate)
	{
		table_width_spacing = m_border_spacing_x * (m_grid.ColsCount() + 1);
	}
	else
	{
		table_width_spacing = 0;

		if (m_grid.ColsCount())
		{
			table_width_spacing -= min(BorderLeft(), m_grid.Column(0).border_left);
			table_width_spacing -= min(BorderRight(), m_grid.Column(m_grid.ColsCount() - 1).border_right);
		}

		for (int col = 1; col < m_grid.ColsCount(); col++)
		{
			table_width_spacing -= min(m_grid.Column(col).border_left, m_grid.Column(col - 1).border_right);
		}
	}


	// Calculate the minimum content width (MCW) of each cell: the formatted content may span any number of lines but may not Overflow the cell Box. 
	// If the specified 'width' (W) of the cell is greater than MCW, W is the minimum cell width. A value of 'auto' means that MCW is the minimum 
	// cell width.
	// 
	// Also, calculate the "maximum" cell width of each cell: formatting the content without breaking lines other than where explicit line breaks occur.

	if (m_grid.ColsCount() == 1 && !block_width.is_default())
	{
		for (int row = 0; row < m_grid.RowsCount(); row++)
		{
			TableCell* cell = m_grid.Cell(0, row);
			if (cell && cell->el)
			{
				cell->min_width = cell->max_width = cell->el->Render(0, 0, max_width - table_width_spacing);
				cell->el->m_pos.width = cell->min_width - cell->el->ContentMarginsLeft() - cell->el->ContentMarginsRight();
			}
		}
	}
	else
	{
		for (int row = 0; row < m_grid.RowsCount(); row++)
		{
			for (int col = 0; col < m_grid.ColsCount(); col++)
			{
				TableCell* cell = m_grid.Cell(col, row);
				if (cell && cell->el)
				{
					if (!m_grid.Column(col).css_width.IsPredefined() && m_grid.Column(col).css_width.Units() != css_units_percentage)
					{
						int css_w = m_grid.Column(col).css_width.CalcPercent(block_width);
						int el_w = cell->el->Render(0, 0, css_w);
						cell->min_width = cell->max_width = max(css_w, el_w);
						cell->el->m_pos.width = cell->min_width - cell->el->ContentMarginsLeft() - cell->el->ContentMarginsRight();
					}
					else
					{
						// calculate minimum content width
						cell->min_width = cell->el->Render(0, 0, 1);
						// calculate maximum content width
						cell->max_width = cell->el->Render(0, 0, max_width - table_width_spacing);
					}
				}
			}
		}
	}

	// For each column, determine a maximum and minimum column width from the cells that span only that column. 
	// The minimum is that required by the cell with the largest minimum cell width (or the column 'width', whichever is larger). 
	// The maximum is that required by the cell with the largest maximum cell width (or the column 'width', whichever is larger).

	for (int col = 0; col < m_grid.ColsCount(); col++)
	{
		m_grid.Column(col).max_width = 0;
		m_grid.Column(col).min_width = 0;
		for (int row = 0; row < m_grid.RowsCount(); row++)
		{
			if (m_grid.Cell(col, row)->colspan <= 1)
			{
				m_grid.Column(col).max_width = max(m_grid.Column(col).max_width, m_grid.Cell(col, row)->max_width);
				m_grid.Column(col).min_width = max(m_grid.Column(col).min_width, m_grid.Cell(col, row)->min_width);
			}
		}
	}

	// For each cell that spans more than one column, increase the minimum widths of the columns it spans so that together, 
	// they are at least as wide as the cell. Do the same for the maximum widths. 
	// If possible, widen all spanned columns by approximately the same amount.

	for (int col = 0; col < m_grid.ColsCount(); col++)
	{
		for (int row = 0; row < m_grid.RowsCount(); row++)
		{
			if (m_grid.Cell(col, row)->colspan > 1)
			{
				int max_total_width = m_grid.Column(col).max_width;
				int min_total_width = m_grid.Column(col).min_width;
				for (int col2 = col + 1; col2 < col + m_grid.Cell(col, row)->colspan; col2++)
				{
					max_total_width += m_grid.Column(col2).max_width;
					min_total_width += m_grid.Column(col2).min_width;
				}
				if (min_total_width < m_grid.Cell(col, row)->min_width)
				{
					m_grid.DistributeMinWidth(m_grid.Cell(col, row)->min_width - min_total_width, col, col + m_grid.Cell(col, row)->colspan - 1);
				}
				if (max_total_width < m_grid.Cell(col, row)->max_width)
				{
					m_grid.DistributeMaxWidth(m_grid.Cell(col, row)->max_width - max_total_width, col, col + m_grid.Cell(col, row)->colspan - 1);
				}
			}
		}
	}

	// If the 'table' or 'inline-table' element's 'width' property has a computed value (W) other than 'auto', the used width is the 
	// greater of W, CAPMIN, and the minimum width required by all the columns plus cell spacing or Borders (MIN). 
	// If the used width is greater than MIN, the extra width should be distributed over the columns.
	//
	// If the 'table' or 'inline-table' element has 'width: auto', the used width is the greater of the table's containing block width, 
	// CAPMIN, and MIN. However, if either CAPMIN or the maximum width required by the columns plus cell spacing or Borders (MAX) is 
	// less than that of the containing block, use max(MAX, CAPMIN).


	int table_width = 0;
	int min_table_width = 0;
	int max_table_width = 0;

	if (!block_width.is_default())
	{
		table_width = m_grid.CalcTableWidth(block_width - table_width_spacing, false, min_table_width, max_table_width);
	}
	else
	{
		table_width = m_grid.CalcTableWidth(max_width - table_width_spacing, true, min_table_width, max_table_width);
	}

	min_table_width += table_width_spacing;
	max_table_width += table_width_spacing;
	table_width += table_width_spacing;
	m_grid.CalcHorizPos(m_borders, m_borderCollapse, m_border_spacing_x);

	bool row_span_found = false;

	// render cells with computed width
	for (int row = 0; row < m_grid.RowsCount(); row++)
	{
		m_grid.Row(row).height = 0;
		for (int col = 0; col < m_grid.ColsCount(); col++)
		{
			TableCell* cell = m_grid.Cell(col, row);
			if (cell->el)
			{
				int span_col = col + cell->colspan - 1;
				if (span_col >= m_grid.ColsCount())
				{
					span_col = m_grid.ColsCount() - 1;
				}
				int cell_width = m_grid.Column(span_col).right - m_grid.Column(col).left;

				if (cell->el->m_pos.width != cell_width - cell->el->ContentMarginsLeft() - cell->el->ContentMarginsRight())
				{
					cell->el->Render(m_grid.Column(col).left, 0, cell_width);
					cell->el->m_pos.width = cell_width - cell->el->ContentMarginsLeft() - cell->el->ContentMarginsRight();
				}
				else
				{
					cell->el->m_pos.x = m_grid.Column(col).left + cell->el->ContentMarginsLeft();
				}

				if (cell->rowspan <= 1)
				{
					m_grid.Row(row).height = max(m_grid.Row(row).height, cell->el->Height());
				}
				else
				{
					row_span_found = true;
				}

			}
		}
	}

	if (row_span_found)
	{
		for (int col = 0; col < m_grid.ColsCount(); col++)
		{
			for (int row = 0; row < m_grid.RowsCount(); row++)
			{
				TableCell* cell = m_grid.Cell(col, row);
				if (cell->el)
				{
					int span_row = row + cell->rowspan - 1;
					if (span_row >= m_grid.RowsCount())
					{
						span_row = m_grid.RowsCount() - 1;
					}
					if (span_row != row)
					{
						int h = 0;
						for (int i = row; i <= span_row; i++)
						{
							h += m_grid.Row(i).height;
						}
						if (h < cell->el->Height())
						{
							m_grid.Row(span_row).height += cell->el->Height() - h;
						}
					}
				}
			}
		}
	}


	int unconstrained_table_height = 0;

	// compute vertical size inferred by cells
	for (int row = 0; row < m_grid.RowsCount(); row++)
	{
		unconstrained_table_height += m_grid.Row(row).height;
	}

	unconstrained_table_height += m_border_spacing_y * (m_grid.RowsCount() + 1);

	int extra_row_height = 0;
	int minimum_table_height = max(m_css_height.CalcPercent(m_higher ? m_higher->m_pos.height : 0), m_css_min_height.CalcPercent(m_higher ? m_higher->m_pos.height : 0));

	if(minimum_table_height > unconstrained_table_height)
	{
		extra_row_height = (minimum_table_height - unconstrained_table_height) / m_grid.RowsCount();

		for (int row = 0; row < m_grid.RowsCount(); row++)
		{
			m_grid.Row(row).height += extra_row_height;
		}
	}

	m_grid.CalcVertPos(m_borders, m_borderCollapse, m_border_spacing_y);

	int table_height = 0;

	// place cells vertically
	for (int col = 0; col < m_grid.ColsCount(); col++)
	{
		for (int row = 0; row < m_grid.RowsCount(); row++)
		{
			TableCell* cell = m_grid.Cell(col, row);
			if (cell->el)
			{
				int span_row = row + cell->rowspan - 1;
				if (span_row >= m_grid.RowsCount())
				{
					span_row = m_grid.RowsCount() - 1;
				}
				cell->el->m_pos.y = m_grid.Row(row).top + cell->el->ContentMarginsTop();
				cell->el->m_pos.height = m_grid.Row(span_row).bottom - m_grid.Row(row).top - cell->el->ContentMarginsTop() - cell->el->ContentMarginsBottom();
				table_height = max(table_height, m_grid.Row(span_row).bottom);
				cell->el->ApplyVerticalAlign();
			}
		}
	}

	if (m_borderCollapse == border_collapse_collapse)
	{
		if (m_grid.RowsCount())
		{
			table_height -= min(BorderBottom(), m_grid.Row(m_grid.RowsCount() - 1).border_bottom);
		}
	}
	else
	{
		table_height += m_border_spacing_y;
	}

	m_pos.width = table_width;

	CalcAutoMargins(higher_width);

	m_pos.MoveTo(x, y);
	m_pos.x += ContentMarginsLeft();
	m_pos.y += ContentMarginsTop();
	m_pos.width = table_width;
	m_pos.height = table_height;

	return max_table_width;
}

void HtmlTag::DrawChildrenBox(Draw& hdc, int x, int y, const Position& clip, DrawFlag flag, int zindex)
{
	Position pos = m_pos;
	pos.x += x;
	pos.y += y;

	if (m_overflow > overflow_visible)
	{
		Position border_box = pos;
		border_box += m_padding;
		border_box += m_borders;

		BorderRadiuses bdr_radius = m_css_borders.radius.CalcPercents(border_box.width, border_box.height);

		bdr_radius -= m_borders;
		bdr_radius -= m_padding;

		GetDocument()->GetHtmlFile()->SetClip(pos, bdr_radius, true, true);
	}

	Position browser_wnd;
	GetDocument()->GetHtmlFile()->GetClientRect(browser_wnd);

	for (ElementsVector::Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		Element* el = (*i);

		if (el->IsVisible())
		{
			switch (flag)
			{
			case draw_positioned:
				if (el->IsPositioned() && el->GetZIndex() == zindex)
				{
					if (el->GetElementPosition() == element_position_fixed)
					{
						el->Paint(hdc, browser_wnd.x, browser_wnd.y, clip);
						el->DrawStackingContext(hdc, browser_wnd.x, browser_wnd.y, clip, true);
					}
					else
					{
						el->Paint(hdc, pos.x, pos.y, clip);
						el->DrawStackingContext(hdc, pos.x, pos.y, clip, true);
					}
					el = 0;
				}
				break;
			case draw_block:
				if (!el->IsInLineBox() && el->GetFloat() == float_none && !el->IsPositioned())
				{
					el->Paint(hdc, pos.x, pos.y, clip);
				}
				break;
			case draw_floats:
				if (el->GetFloat() != float_none && !el->IsPositioned())
				{
					el->Paint(hdc, pos.x, pos.y, clip);
					el->DrawStackingContext(hdc, pos.x, pos.y, clip, false);
					el = 0;
				}
				break;
			case draw_inlines:
				if (el->IsInLineBox() && el->GetFloat() == float_none && !el->IsPositioned())
				{
					el->Paint(hdc, pos.x, pos.y, clip);
					if (el->GetDisplay() == display_inline_block)
					{
						el->DrawStackingContext(hdc, pos.x, pos.y, clip, false);
						el = 0;
					}
				}
				break;
			default:
				break;
			}

			if (el)
			{
				if (flag == draw_positioned)
				{
					if (!el->IsPositioned())
					{
						el->DrawChildren(hdc, pos.x, pos.y, clip, flag, zindex);
					}
				}
				else
				{
					if (el->GetFloat() == float_none &&
						el->GetDisplay() != display_inline_block &&
						!el->IsPositioned())
					{
						el->DrawChildren(hdc, pos.x, pos.y, clip, flag, zindex);
					}
				}
			}
		}
	}

	if (m_overflow > overflow_visible)
	{
		GetDocument()->GetHtmlFile()->DelClip();
	}
}

void HtmlTag::DrawChildrenTable(Draw& hdc, int x, int y, const Position& clip, DrawFlag flag, int zindex)
{
	Position pos = m_pos;
	pos.x += x;
	pos.y += y;
	for (int row = 0; row < m_grid.RowsCount(); row++)
	{
		if (flag == draw_block)
		{
			m_grid.Row(row).el_row->DrawBackground(hdc, pos.x, pos.y, clip);
		}
		for (int col = 0; col < m_grid.ColsCount(); col++)
		{
			TableCell* cell = m_grid.Cell(col, row);
			if (cell->el)
			{
				if (flag == draw_block)
				{
					cell->el->Paint(hdc, pos.x, pos.y, clip);
				}
				cell->el->DrawChildren(hdc, pos.x, pos.y, clip, flag, zindex);
			}
		}
	}
}

END_HTMLCTRL_NAMESPACE

