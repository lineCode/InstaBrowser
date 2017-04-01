#include "Html.h"
#include "Element.h"
#include "Document.h"

#define LITEHTML_EMPTY_FUNC			{}
#define LITEHTML_RETURN_FUNC(ret)	{return ret;}

NAMESPACE_HTMLCTRL

Element::Element(Document& doc) : Object(doc.GetObjectPool()), HtmlNode(doc) {
	m_box = 0;
	m_doc = &doc;
	m_skip = false;
}

Element::Element(Document& doc, Element& dir) : Object(doc.GetObjectPool()), HtmlNode(doc)
{
	m_box		= 0;
	m_doc		= &doc;
	m_skip		= false;
}

Element::~Element()
{

}

Element* Element::GetParentElement() const {
	if (!GetParent()) return 0;
	Element* e = GetParent()->Get<Element>();
	return e;
}

Document* Element::GetDocument() const {
	return m_doc;
}


bool Element::IsPointInside( int x, int y )
{
	if(GetDisplay() != display_inline && GetDisplay() != display_TableRow)
	{
		Position pos = m_pos;
		pos += m_padding;
		pos += m_borders;
		if(pos.IsPointInside(x, y))
		{
			return true;
		} else
		{
			return false;
		}
	} else
	{
		Position::vector Boxes;
		GetInLineBoxes(Boxes);
		for(Position::vector::Iterator Box = Boxes.Begin(); Box != Boxes.End(); Box++)
		{
			if(Box->IsPointInside(x, y))
			{
				return true;
			}
		}
	}
	return false;
}

WebColor Element::GetColor( const String& prop_name, bool inherited, const WebColor& DefColor )
{
	String clrstr = GetStyleProperty(prop_name, inherited, "");
	if(!clrstr.GetCount()) {
		return DefColor;
	}
	return WebColor::FromString(clrstr);
}

Position Element::GetPlacement() const
{
	Position pos = m_pos;
	Element* cur_el = GetParentElement();
	while(cur_el)
	{
		pos.x += cur_el->m_pos.x;
		pos.y += cur_el->m_pos.y;
		cur_el = cur_el->GetParentElement();
	}
	return pos;
}

bool Element::IsInLineBox() const
{
	StyleDisplay d = GetDisplay();
	if(	d == display_inline || 
		d == display_inline_block || 
		d == display_inline_text)
	{
		return true;
	}
	return false;
}

bool Element::CollapseTopMargin() const
{
	if(!m_borders.top && !m_padding.top && InNormalFlow() && GetFloat() == float_none && m_margins.top >= 0 && GetParent()->Is<Element>())
	{
		return true;
	}
	return false;
}

bool Element::CollapseBottomMargin() const
{
	if(!m_borders.bottom && !m_padding.bottom && InNormalFlow() && GetFloat() == float_none && m_margins.bottom >= 0 && GetParent()->Is<Element>())
	{
		return true;
	}
	return false;
}

bool Element::GetPredefinedHeight(int& p_height) const
{
	CssLength h = GetCssHeight();
	if(h.IsPredefined())
	{
		p_height = m_pos.height;
		return false;
	}
	if(h.Units() == css_units_percentage)
	{
		if(!GetParent()->Is<Element>())
		{
			Position client_pos;
			GetDocument()->GetHtmlFile()->GetClientRect(client_pos);
			p_height = h.CalcPercent(client_pos.height);
			return true;
		} else
		{
			int ph = 0;
			if(GetParentElement()->GetPredefinedHeight(ph))
			{
				p_height = h.CalcPercent(ph);
				p_height -= ContentMarginsTop() + ContentMarginsBottom();
				return true;
			} else
			{
				p_height = m_pos.height;
				return false;
			}
		}
	}
	p_height = GetDocument()->CvtUnits(h, GetFontSize());
	return true;
}

void Element::CalcDocumentSize( Upp::Size& sz, int x /*= 0*/, int y /*= 0*/ )
{
	if(IsVisible())
	{
		sz.cx	= max(sz.cx,	x + Right());
		sz.cy	= max(sz.cy,	y + Bottom());
	}
}

void Element::GetRedrawBox(Position& pos, int x /*= 0*/, int y /*= 0*/)
{
	if(IsVisible())
	{
		int p_left		= min(pos.Left(),	x + m_pos.Left() - m_padding.left - m_borders.left);
		int p_right		= max(pos.Right(), x + m_pos.Right() + m_padding.left + m_borders.left);
		int p_top		= min(pos.Top(), y + m_pos.Top() - m_padding.top - m_borders.top);
		int p_bottom	= max(pos.Bottom(), y + m_pos.Bottom() + m_padding.bottom + m_borders.bottom);

		pos.x = p_left;
		pos.y = p_top;
		pos.width	= p_right - p_left;
		pos.height	= p_bottom - p_top;
	}
}

int Element::CalcWidth(int defVal) const
{
	CssLength w = GetCssWidth();
	if(w.IsPredefined())
	{
		return defVal;
	}
	if(w.Units() == css_units_percentage)
	{
		if(!GetParent()->Is<Element>())
		{
			Position client_pos;
			GetDocument()->GetHtmlFile()->GetClientRect(client_pos);
			return w.CalcPercent(client_pos.width);
		} else
		{
			int pw = GetParentElement()->CalcWidth(defVal);
			pw = w.CalcPercent(pw);
			pw -= ContentMarginsRight() + ContentMarginsLeft();
			return pw;
		}
	}
	return 	GetDocument()->CvtUnits(w, GetFontSize());
}

bool Element::IsAncestor( Element* el )
{
	Element* el_higher = GetParentElement();
	while(el_higher && el_higher != el)
	{
		el_higher = el_higher->GetParentElement();
	}
	if(el_higher)
	{
		return true;
	}
	return false;
}

int Element::GetInlineShiftLeft()
{
	int ret = 0;

	if(GetParentElement()->GetDisplay() == display_inline)
	{
		StyleDisplay disp = GetDisplay();

		if(disp == display_inline_text || disp == display_inline_block)
		{
			Element* higher = GetParentElement();
			Element* el = this;
			while(higher && higher->GetDisplay() == display_inline)
			{
				if( higher->IsFirstChildInline(el) )
				{
					ret += higher->PaddingLeft() + higher->BorderLeft() + higher->MarginLeft();
				}
				el = higher;
				
				higher = higher->GetParentElement();
			}
		}
	}

	return ret;
}

int Element::GetInlineShiftRight()
{
	int ret = 0;

	if(GetParentElement()->GetDisplay() == display_inline)
	{
		StyleDisplay disp = GetDisplay();

		if(disp == display_inline_text || disp == display_inline_block)
		{
			Element* higher = GetParentElement();
			Element* el = this;
			while(higher && higher->GetDisplay() == display_inline)
			{
				if( higher->IsLastChildInline(el) )
				{
					ret += higher->PaddingRight() + higher->BorderRight() + higher->MarginRight();
				}
				el = higher;
				higher = higher->GetParentElement();
			}
		}
	}

	return ret;
}

void Element::CalcAutoMargins(int higher_width)							LITEHTML_EMPTY_FUNC
Background* Element::GetBackground(bool own_only)				LITEHTML_RETURN_FUNC(0)
Element* Element::GetElementByPoint( int x, int y, int client_x, int client_y )	LITEHTML_RETURN_FUNC(0)
Element* Element::GetChildByPoint( int x, int y, int client_x, int client_y, DrawFlag flag, int zindex ) LITEHTML_RETURN_FUNC(0)
void Element::GetLineLeftRight( int y, int def_right, int& ln_left, int& ln_right ) LITEHTML_EMPTY_FUNC
void Element::AddStyle( Style::ptr st )						LITEHTML_EMPTY_FUNC
Element::ptr Element::SelectOne( const CssSelector& Selector ) LITEHTML_RETURN_FUNC(0)
Element::ptr Element::SelectOne( const String& Selector )		LITEHTML_RETURN_FUNC(0)
Element* Element::FindAdjacentSibling( Element* el, const CssSelector& Selector, bool apply_pseudo /*= true*/, bool* is_pseudo /*= 0*/ ) LITEHTML_RETURN_FUNC(0)
Element* Element::FindSibling( Element* el, const CssSelector& Selector, bool apply_pseudo /*= true*/, bool* is_pseudo /*= 0*/ ) LITEHTML_RETURN_FUNC(0)
bool Element::IsNthLastChild( Element* el, int num, int off, bool of_type )			LITEHTML_RETURN_FUNC(false)
bool Element::IsNthChild( Element* el, int num, int off, bool of_type )				LITEHTML_RETURN_FUNC(false)
bool Element::IsOnlyChild(Element* el, bool of_type)					LITEHTML_RETURN_FUNC(false)
Overflow Element::GetOverflow() const							LITEHTML_RETURN_FUNC(overflow_visible)
void Element::DrawChildren( Draw& hdc, int x, int y, const Position& clip, DrawFlag flag, int zindex ) LITEHTML_EMPTY_FUNC
void Element::DrawStackingContext( Draw& hdc, int x, int y, const Position& clip, bool with_positioned ) LITEHTML_EMPTY_FUNC
void Element::RenderPositioned(RenderType rt)							LITEHTML_EMPTY_FUNC
int Element::GetZIndex() const											LITEHTML_RETURN_FUNC(0)
bool Element::FetchPositioned()											LITEHTML_RETURN_FUNC(false)
Visibility Element::GetVisibility() const						LITEHTML_RETURN_FUNC(visibility_visible)
void Element::ApplyVerticalAlign()										LITEHTML_EMPTY_FUNC
void Element::SetCssWidth( CssLength& w )								LITEHTML_EMPTY_FUNC
Element::ptr Element::GetChild( int idx ) const				LITEHTML_RETURN_FUNC(0)
size_t Element::GetChildrenCount() const								LITEHTML_RETURN_FUNC(0)
void Element::CalcOutlines( int higher_width )							LITEHTML_EMPTY_FUNC
CssLength Element::GetCssWidth() const						LITEHTML_RETURN_FUNC(CssLength())
CssLength Element::GetCssHeight() const						LITEHTML_RETURN_FUNC(CssLength())
ElementClear Element::GetClear() const						LITEHTML_RETURN_FUNC(clear_none)
CssLength Element::GetCssLeft() const						LITEHTML_RETURN_FUNC(CssLength())
CssLength Element::GetCssRight() const						LITEHTML_RETURN_FUNC(CssLength())
CssLength Element::GetCssTop() const							LITEHTML_RETURN_FUNC(CssLength())
CssLength Element::GetCssBottom() const						LITEHTML_RETURN_FUNC(CssLength())
CssOffsets Element::GetCssOffsets() const					LITEHTML_RETURN_FUNC(CssOffsets())
VerticalAlign Element::GetVerticalAlign() const				LITEHTML_RETURN_FUNC(va_baseline)
int Element::PlaceElement( Element* el, int max_width )					LITEHTML_RETURN_FUNC(0)
int Element::RenderInline( Element* container, int max_width )			LITEHTML_RETURN_FUNC(0)
void Element::AddPositioned( Element* el )									LITEHTML_EMPTY_FUNC
int Element::FindNextLineTop( int top, int width, int def_right )		LITEHTML_RETURN_FUNC(0)
ElementFloat Element::GetFloat() const						LITEHTML_RETURN_FUNC(float_none)
void Element::AddFloat( Element* el, int x, int y )						LITEHTML_EMPTY_FUNC
void Element::UpdateFloats( int dy, Element* higher )					LITEHTML_EMPTY_FUNC
int Element::GetLineLeft( int y )										LITEHTML_RETURN_FUNC(0)
int Element::GetLineRight( int y, int def_right )						LITEHTML_RETURN_FUNC(def_right)
int Element::GetLeftFloatsHeight() const								LITEHTML_RETURN_FUNC(0)
int Element::GetRightGloatsHeight() const								LITEHTML_RETURN_FUNC(0)
int Element::GetFloatsHeight(ElementFloat el_float) const				LITEHTML_RETURN_FUNC(0)
bool Element::IsFloatsHolder() const									LITEHTML_RETURN_FUNC(false)
void Element::GetContentSize( Size& sz, int max_width )					LITEHTML_EMPTY_FUNC
void Element::Init()														LITEHTML_EMPTY_FUNC
int Element::Render( int x, int y, int max_width, bool second_pass )		LITEHTML_RETURN_FUNC(0)
bool Element::AppendChild( Element* el )						LITEHTML_RETURN_FUNC(false)
String Element::GetTagName() const						LITEHTML_RETURN_FUNC("")
void Element::SetTagName( const String& tag )							LITEHTML_EMPTY_FUNC
void Element::SetData( const String& data )								LITEHTML_EMPTY_FUNC
void Element::SetAttr( const String& name, const String& val )			LITEHTML_EMPTY_FUNC
void Element::ApplyStylesheet( const Css& Stylesheet )			LITEHTML_EMPTY_FUNC
void Element::RefreshStyles()											LITEHTML_EMPTY_FUNC
void Element::OnClick()													LITEHTML_EMPTY_FUNC
void Element::InitFont()													LITEHTML_EMPTY_FUNC
void Element::GetInLineBoxes( Position::vector& Boxes )					LITEHTML_EMPTY_FUNC
void Element::ParseStyles( bool is_reparse /*= false*/ )					LITEHTML_EMPTY_FUNC
String Element::GetAttr( const String& name, const String& def /*= 0*/ )	LITEHTML_RETURN_FUNC(def)
bool Element::IsWhiteSpace()											LITEHTML_RETURN_FUNC(false)
bool Element::IsBody() const												LITEHTML_RETURN_FUNC(false)
bool Element::IsBreak() const											LITEHTML_RETURN_FUNC(false)
int Element::GetBaseLine()												LITEHTML_RETURN_FUNC(0)
bool Element::OnMouseOver()												LITEHTML_RETURN_FUNC(false)
bool Element::OnMouseLeave()											LITEHTML_RETURN_FUNC(false)
bool Element::OnMouseLeftDown()											LITEHTML_RETURN_FUNC(false)
bool Element::OnMouseLeftUp()												LITEHTML_RETURN_FUNC(false)
bool Element::FindStylesChanges( Position::vector& redraw_boxes, int x, int y )	LITEHTML_RETURN_FUNC(false)
String Element::GetCursor()							LITEHTML_RETURN_FUNC("")
WhiteSpace Element::GetWhiteSpace() const					LITEHTML_RETURN_FUNC(white_space_normal)
StyleDisplay Element::GetDisplay() const						LITEHTML_RETURN_FUNC(display_none)
bool Element::SetPseudoClass( const String& pclass, bool add )			LITEHTML_RETURN_FUNC(false)
ElementPosition Element::GetElementPosition(CssOffsets* offsets) const			LITEHTML_RETURN_FUNC(element_position_static)
bool Element::IsReplaced() const											LITEHTML_RETURN_FUNC(false)
int Element::LineHeight() const											LITEHTML_RETURN_FUNC(0)
void Element::Paint( Draw& hdc, int x, int y, const Position& clip )	LITEHTML_EMPTY_FUNC
void Element::DrawBackground( Draw& hdc, int x, int y, const Position& clip )	LITEHTML_EMPTY_FUNC
String Element::GetStyleProperty( const String& name, bool inherited, const String& def /*= 0*/ )	LITEHTML_RETURN_FUNC("")
uint32 Element::GetFont( FontMetrics* fm /*= 0*/ )			LITEHTML_RETURN_FUNC(0)
int Element::GetFontSize()	const										LITEHTML_RETURN_FUNC(0)
void Element::GetText( String& text )									LITEHTML_EMPTY_FUNC
void Element::ParseAttributes()											LITEHTML_EMPTY_FUNC
int Element::Select( const CssSelector& Selector, bool apply_pseudo)		LITEHTML_RETURN_FUNC(select_no_match)
int Element::Select( const CssElementSelector& Selector, bool apply_pseudo /*= true*/ )	LITEHTML_RETURN_FUNC(select_no_match)
Element* Element::FindAncestor( const CssSelector& Selector, bool apply_pseudo, bool* is_pseudo)	LITEHTML_RETURN_FUNC(0)
bool Element::IsFirstChildInline( const Element* el )					LITEHTML_RETURN_FUNC(false)
bool Element::IsLastChildInline( const Element* el )					LITEHTML_RETURN_FUNC(false)
bool Element::HaveInlineChild()											LITEHTML_RETURN_FUNC(false)

END_HTMLCTRL_NAMESPACE
