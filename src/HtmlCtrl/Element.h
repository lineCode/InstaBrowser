#pragma once
#include "Object.h"
#include "Stylesheet.h"
#include "CssOffsets.h"
#include <HtmlCtrl/Common.h>

#include "WebColor.h"

NAMESPACE_HTMLCTRL

class Box;
class Background;

class Element : public Object, public HtmlNode
{
	friend class BlockBox;
	friend class LineBox;
	friend class HtmlTag;
	friend class ElTable;
	friend class Document;
public:
	typedef ObjectPtr<Element>		ptr;
protected:

	ElementsVector			m_children;
	
	Document*				m_doc;
	Box*					m_box;
	Position					m_pos;
	Margins						m_margins;
	Margins						m_padding;
	Margins						m_borders;
	bool						m_skip;
	
	Element(Document& doc);
	
public:
	Element(Document& doc, Element& higher);
	
	Element* GetParentElement() const;
	Document* GetDocument() const;
	
	virtual ~Element();

	int							Left()						const;
	int							Right()						const;
	int							Top()						const;
	int							Bottom()					const;
	int							Height()					const;
	int							Width()						const;

	int							ContentMarginsTop()			const;
	int							ContentMarginsBottom()		const;
	int							ContentMarginsLeft()		const;
	int							ContentMarginsRight()		const;

	int							MarginTop()					const;
	int							MarginBottom()				const;
	int							MarginLeft()				const;
	int							MarginRight()				const;
	Margins						GetMargins()				const;

	int							PaddingTop()				const;
	int							PaddingBottom()				const;
	int							PaddingLeft()				const;
	int							PaddingRight()				const;
	Margins						GetPaddings()				const;

	int							BorderTop()					const;
	int							BorderBottom()				const;
	int							BorderLeft()				const;
	int							BorderRight()				const;
	Margins						GetBorders()				const;

	bool						InNormalFlow()				const;
	WebColor				GetColor(const String& prop_name, bool inherited, const WebColor& DefColor = WebColor());
	bool						IsInLineBox()				const;
	Position					GetPlacement()				const;
	bool						CollapseTopMargin()			const;
	bool						CollapseBottomMargin()		const;
	bool						IsPositioned()				const;

	bool						Skip();
	void						Skip(bool val);
	bool						IsVisible() const;
	int							CalcWidth(int defVal) const;
	int							GetInlineShiftLeft();
	int							GetInlineShiftRight();

	virtual Element::ptr		SelectOne(const String& Selector);
	virtual Element::ptr		SelectOne(const CssSelector& Selector);

	virtual int					Render(int x, int y, int max_width, bool second_pass = false);
	virtual int					RenderInline(Element* container, int max_width);
	virtual int					PlaceElement(Element* el, int max_width);
	virtual void				CalcOutlines( int higher_width );
	virtual void				CalcAutoMargins(int higher_width);
	virtual void				ApplyVerticalAlign();
	virtual bool				FetchPositioned();
	virtual void				RenderPositioned(RenderType rt = render_all);

	virtual bool				AppendChild(Element* el);

	virtual String				GetTagName() const;
	virtual void				SetTagName(const String& tag);
	virtual void				SetData(const String& data);
	virtual ElementFloat		GetFloat() const;
	virtual VerticalAlign		GetVerticalAlign() const;
	virtual ElementClear		GetClear() const;
	virtual size_t				GetChildrenCount() const;
	virtual Element::ptr		GetChild(int idx) const;
	virtual Overflow			GetOverflow() const;

	virtual CssLength			GetCssLeft() const;
	virtual CssLength			GetCssRight() const;
	virtual CssLength			GetCssTop() const;
	virtual CssLength			GetCssBottom() const;
	virtual CssOffsets			GetCssOffsets() const;
	virtual CssLength			GetCssWidth() const;
	virtual void				SetCssWidth(CssLength& w);
	virtual CssLength			GetCssHeight() const;

	virtual void				SetAttr(const String& name, const String& val);
	virtual String				GetAttr(const String& name, const String& def = "");
	virtual int 				GetAttrCount() {return 0;}
	virtual String				GetAttrKey(int i) {return "";}
	virtual String				GetAttrValue(int i) {return "";}
	
	
	virtual void				ApplyStylesheet(const Css& Stylesheet);
	virtual void				RefreshStyles();
	virtual bool				IsWhiteSpace();
	virtual bool				IsBody() const;
	virtual bool				IsBreak() const;
	virtual int					GetBaseLine();
	virtual bool				OnMouseOver();
	virtual bool				OnMouseLeave();
	virtual bool				OnMouseLeftDown();
	virtual bool				OnMouseLeftUp();
	virtual void				OnClick();
	virtual bool				FindStylesChanges(Position::vector& redraw_boxes, int x, int y);
	virtual String				GetCursor();
	virtual void				InitFont();
	virtual bool				IsPointInside(int x, int y);
	virtual bool				SetPseudoClass(const String& pclass, bool add);
	virtual bool				IsReplaced() const;
	virtual int					LineHeight() const;
	virtual WhiteSpace			GetWhiteSpace() const;
	virtual StyleDisplay		GetDisplay() const;
	virtual Visibility			GetVisibility() const;
	virtual ElementPosition	GetElementPosition(CssOffsets* offsets = 0) const;
	virtual void				GetInLineBoxes(Position::vector& Boxes);
	virtual void				ParseStyles(bool is_reparse = false);
	virtual void				Paint(Draw& hdc, int x, int y, const Position& clip);
	virtual void				DrawBackground( Draw& hdc, int x, int y, const Position& clip );
	virtual String				GetStyleProperty(const String& name, bool inherited, const String& def = "");
	virtual uint32				GetFont(FontMetrics* fm = 0);
	virtual int					GetFontSize() const;
	virtual void				GetText(String& text);
	virtual void				ParseAttributes();
	virtual int					Select(const CssSelector& selector, bool apply_pseudo = true);
	virtual int					Select(const CssElementSelector& selector, bool apply_pseudo = true);
	virtual Element*			FindAncestor(const CssSelector& selector, bool apply_pseudo = true, bool* is_pseudo = 0);
	virtual bool				IsAncestor(Element* el);
	virtual Element*			FindAdjacentSibling(Element* el, const CssSelector& selector, bool apply_pseudo = true, bool* is_pseudo = 0);
	virtual Element*			FindSibling(Element* el, const CssSelector& selector, bool apply_pseudo = true, bool* is_pseudo = 0);
	virtual bool				IsFirstChildInline(const Element* el);
	virtual bool				IsLastChildInline(const Element* el);
	virtual bool				HaveInlineChild();
	virtual void				GetContentSize(Size& sz, int max_width);
	virtual void				Init();
	virtual bool				IsFloatsHolder() const;
	virtual int					GetFloatsHeight(ElementFloat el_float = float_none) const;
	virtual int					GetLeftFloatsHeight() const;
	virtual int					GetRightGloatsHeight() const;
	virtual int					GetLineLeft(int y);
	virtual int					GetLineRight(int y, int def_right);
	virtual void				GetLineLeftRight(int y, int def_right, int& ln_left, int& ln_right);
	virtual void				AddFloat(Element* el, int x, int y);
	virtual void				UpdateFloats(int dy, Element* higher);
	virtual void				AddPositioned(Element* el);
	virtual int					FindNextLineTop(int top, int width, int def_right);
	virtual int					GetZIndex() const;
	virtual void				DrawStackingContext(Draw& hdc, int x, int y, const Position& clip, bool with_positioned);
	virtual void				DrawChildren( Draw& hdc, int x, int y, const Position& clip, DrawFlag flag, int zindex );
	virtual bool				IsNthChild(Element* el, int num, int off, bool of_type);
	virtual bool				IsNthLastChild(Element* el, int num, int off, bool of_type);
	virtual bool				IsOnlyChild(Element* el, bool of_type);
	virtual bool				GetPredefinedHeight(int& p_height) const;
	virtual void				CalcDocumentSize(Upp::Size& sz, int x = 0, int y = 0);
	virtual void				GetRedrawBox(Position& pos, int x = 0, int y = 0);
	virtual void				AddStyle(Style::ptr st);
	virtual Element*			GetElementByPoint(int x, int y, int client_x, int client_y);
	virtual Element*			GetChildByPoint(int x, int y, int client_x, int client_y, DrawFlag flag, int zindex);
	virtual Background*			GetBackground(bool own_only = false);
	
	
	virtual int					GetStylePropertyCount() {return 0;}
	virtual String				GetStylePropertyKey(int i) {return "";}
	virtual String				GetStylePropertyValue(int i) {return "";}
	
	
	
	
	virtual Element* 			GetAsElement()				{return this;}
};

//////////////////////////////////////////////////////////////////////////
//							INLINE FUNCTIONS							//
//////////////////////////////////////////////////////////////////////////

inline int Element::Right() const
{
	return Left() + Width();
}

inline int Element::Left() const
{
	return m_pos.Left() - MarginLeft() - m_padding.left - m_borders.left;
}

inline int Element::Top() const
{
	return m_pos.Top() - MarginTop() - m_padding.top - m_borders.top;
}

inline int Element::Bottom() const
{
	return Top() + Height();
}

inline int Element::Height() const
{
	return m_pos.height + MarginTop() + MarginBottom() + m_padding.Height() + m_borders.Height();
}

inline int Element::Width() const
{
	return m_pos.width + MarginLeft() + MarginRight() + m_padding.Width() + m_borders.Width();
}

inline int Element::ContentMarginsTop() const
{
	return MarginTop() + m_padding.top + m_borders.top;
}

inline int Element::ContentMarginsBottom() const
{
	return MarginBottom() + m_padding.bottom + m_borders.bottom;
}

inline int Element::ContentMarginsLeft() const
{
	return MarginLeft() + m_padding.left + m_borders.left;
}

inline int Element::ContentMarginsRight() const
{
	return MarginRight() + m_padding.right + m_borders.right;
}

inline Margins Element::GetPaddings()	const
{
	return m_padding;
}

inline Margins Element::GetBorders()	const
{
	return m_borders;
}

inline int Element::PaddingTop() const
{
	return m_padding.top;
}

inline int Element::PaddingBottom() const
{
	return m_padding.bottom;
}

inline int Element::PaddingLeft() const
{
	return m_padding.left;
}

inline int Element::PaddingRight() const
{
	return m_padding.right;
}

inline bool Element::InNormalFlow() const
{
	if(GetElementPosition() != element_position_absolute && GetDisplay() != display_none)
	{
		return true;
	}
	return false;
}

inline int Element::BorderTop() const
{
	return m_borders.top;
}

inline int Element::BorderBottom() const
{
	return m_borders.bottom;
}

inline int Element::BorderLeft() const
{
	return m_borders.left;
}

inline int Element::BorderRight() const
{
	return m_borders.right;
}

inline bool Element::Skip()
{
	return m_skip;
}

inline void Element::Skip(bool val)
{
	m_skip = val;
}

inline int Element::MarginTop() const
{
	return m_margins.top;
}

inline int Element::MarginBottom() const
{
	return m_margins.bottom;
}

inline int Element::MarginLeft() const
{
	return m_margins.left;
}

inline int Element::MarginRight() const
{
	return m_margins.right;
}

inline Margins Element::GetMargins() const
{
	Margins ret;
	ret.left	= MarginLeft();
	ret.right	= MarginRight();
	ret.top		= MarginTop();
	ret.bottom	= MarginBottom();

	return ret;
}

inline bool Element::IsPositioned()	const
{
	return (GetElementPosition() > element_position_static);
}

inline bool Element::IsVisible() const
{
	return !(m_skip || GetDisplay() == display_none || GetVisibility() != visibility_visible);
}


END_HTMLCTRL_NAMESPACE
