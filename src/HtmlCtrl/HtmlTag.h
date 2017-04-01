#pragma once

#include "Element.h"
#include "Object.h"
#include "Style.h"
#include "Background.h"
#include "CssMargins.h"
#include "Borders.h"
#include "CssSelector.h"
#include "Stylesheet.h"
#include "Box.h"
#include "Table.h"

NAMESPACE_HTMLCTRL

class HtmlTag : public Element
{
	friend class ElementsIterator;
	friend class ElTable;
	friend class TableGrid;
	friend class BlockBox;
	friend class LineBox;
public:
	typedef ObjectPtr<HtmlTag>	ptr;
protected:
	Box::vector				m_boxes;
	String					m_id;
	String					m_class;
	String					m_tag;
	Style				m_style;
	StrMap					m_attrs;
	VerticalAlign			m_vertical_align;
	text_align				m_text_align;
	StyleDisplay			m_display;
	ListStyleType			m_list_style_type;
	ListStylePosition		m_list_style_position;
	WhiteSpace				m_white_space;
	ElementFloat			m_float;
	ElementClear			m_clear;
	FloatedBox::vector		m_floats_left;
	FloatedBox::vector		m_floats_right;
	ElementsVector			m_positioned;
	Background				m_bg;
	ElementPosition			m_el_position;
	int						m_line_height;
	bool					m_lh_predefined;
	StringVector			m_pseudo_classes;
	UsedSelector::vector	m_used_styles;		
	
	uint32					m_font;
	int						m_font_size;
	FontMetrics				m_font_metrics;

	CssMargins				m_css_margins;
	CssMargins				m_css_padding;
	CssBorders				m_css_borders;
	CssLength				m_css_width;
	CssLength				m_css_height;
	CssLength				m_css_min_width;
	CssLength				m_css_min_height;
	CssLength				m_css_max_width;
	CssLength				m_css_max_height;
	CssOffsets				m_css_offsets;
	CssLength				m_css_text_indent;

	Overflow				m_overflow;
	Visibility				m_visibility;
	int						m_z_index;
	BoxSizing				m_box_sizing;

	IntIntCache				m_cahe_line_left;
	IntIntCache				m_cahe_line_right;

	// data for table rendering
	TableGrid				m_grid;
	CssLength				m_css_border_spacing_x;
	CssLength				m_css_border_spacing_y;
	int						m_border_spacing_x;
	int						m_border_spacing_y;
	BorderCollapse			m_borderCollapse;

	//HtmlTag();
	
	void Reset();
public:
	HtmlTag(Document& doc);
	virtual ~HtmlTag();

	/* render functions */

	virtual int					Render(int x, int y, int max_width, bool second_pass = false);

	virtual int					RenderInline(Element* container, int max_width);
	virtual int					PlaceElement(Element* el, int max_width);
	virtual bool				FetchPositioned();
	virtual void				RenderPositioned(RenderType rt = render_all);

	int							NewBox( Element* el, int max_width );

	int							GetClearedTop( Element* el, int line_top );
	int							FinishLastBox(bool end_of_render = false);

	virtual bool				AppendChild(Element* el);
	virtual String				GetTagName() const;
	virtual void				SetTagName(const String& tag);
	virtual void				SetData(const String& data);
	virtual ElementFloat		GetFloat() const;
	virtual VerticalAlign		GetVerticalAlign() const;
	virtual CssLength			GetCssLeft() const;
	virtual CssLength			GetCssRight() const;
	virtual CssLength			GetCssTop() const;
	virtual CssLength			GetCssBottom() const;
	virtual CssLength			GetCssWidth() const;
	virtual CssOffsets			GetCssOffsets() const;
	virtual void				SetCssWidth(CssLength& w);
	virtual CssLength			GetCssHeight() const;
	virtual ElementClear		GetClear() const;
	virtual size_t				GetChildrenCount() const;
	virtual Element::ptr		GetChild(int idx) const;
	virtual ElementPosition		GetElementPosition(CssOffsets* offsets = 0) const;
	virtual Overflow			GetOverflow() const;

	virtual void				SetAttr(const String& name, const String& val);
	virtual String				GetAttr(const String& name, const String& def = "");
	
	virtual int 				GetAttrCount() {return m_attrs.GetCount();}
	virtual String				GetAttrKey(int i) {return m_attrs.GetKey(i);}
	virtual String				GetAttrValue(int i) {return m_attrs[i];}
	
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
	virtual bool				SetPseudoClass(const String& pclass, bool add);
	virtual bool				IsReplaced() const;
	virtual int					LineHeight() const;
	virtual WhiteSpace			GetWhiteSpace() const;
	virtual StyleDisplay		GetDisplay() const;
	virtual Visibility			GetVisibility() const;
	virtual void				ParseStyles(bool is_reparse = false);
	virtual void				Paint(Draw& hdc, int x, int y, const Position& clip);
	virtual void				DrawBackground( Draw& hdc, int x, int y, const Position& clip );

	virtual String				GetStyleProperty(const String& name, bool inherited, const String& def = "");
	virtual uint32				GetFont(FontMetrics* fm = 0);
	virtual int					GetFontSize() const;

	ElementsVector&				Children();
	virtual void				CalcOutlines( int higher_width );
	virtual void				CalcAutoMargins(int higher_width);

	virtual int					Select(const CssSelector& Selector, bool apply_pseudo = true);
	virtual int					Select(const CssElementSelector& Selector, bool apply_pseudo = true);

	virtual Element::ptr		SelectOne(const String& Selector);
	virtual Element::ptr		SelectOne(const CssSelector& Selector);

	virtual Element*			FindAncestor(const CssSelector& Selector, bool apply_pseudo = true, bool* is_pseudo = 0);
	virtual Element*			FindAdjacentSibling(Element* el, const CssSelector& Selector, bool apply_pseudo = true, bool* is_pseudo = 0);
	virtual Element*			FindSibling(Element* el, const CssSelector& Selector, bool apply_pseudo = true, bool* is_pseudo = 0);
	virtual void				GetText(String& text);
	virtual void				ParseAttributes();

	virtual bool				IsFirstChildInline(const Element* el);
	virtual bool				IsLastChildInline(const Element* el);
	virtual bool				HaveInlineChild();
	virtual void				GetContentSize(Size& sz, int max_width);
	virtual void				Init();
	virtual void				GetInLineBoxes(Position::vector& Boxes);
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
	virtual void				ApplyVerticalAlign();
	virtual void				DrawChildren( Draw& hdc, int x, int y, const Position& clip, DrawFlag flag, int zindex );
	virtual int					GetZIndex() const;
	virtual void				DrawStackingContext(Draw& hdc, int x, int y, const Position& clip, bool with_positioned);
	virtual void				CalcDocumentSize(Upp::Size& sz, int x = 0, int y = 0);
	virtual void				GetRedrawBox(Position& pos, int x = 0, int y = 0);
	virtual void				AddStyle(Style::ptr st);
	virtual Element*			GetElementByPoint(int x, int y, int client_x, int client_y);
	virtual Element*			GetChildByPoint(int x, int y, int client_x, int client_y, DrawFlag flag, int zindex);

	virtual bool				IsNthChild(Element* el, int num, int off, bool of_type);
	virtual bool				IsNthLastChild(Element* el, int num, int off, bool of_type);
	virtual bool				IsOnlyChild(Element* el, bool of_type);
	virtual Background*			GetBackground(bool own_only = false);
	
	virtual int					GetStylePropertyCount();
	virtual String				GetStylePropertyKey(int i);
	virtual String				GetStylePropertyValue(int i);
	
	
protected:
	void						DrawChildrenBox(Draw& hdc, int x, int y, const Position& clip, DrawFlag flag, int zindex);
	void						DrawChildrenTable(Draw& hdc, int x, int y, const Position& clip, DrawFlag flag, int zindex);
	int							RenderBox(int x, int y, int max_width, bool second_pass = false);
	int							RenderTable(int x, int y, int max_width, bool second_pass = false);
	int							FixLineWidth(int max_width, ElementFloat flt);
	void						ParseBackground();
	void						InitBackgroundPaint( Position pos, BackgroundPaint &bg_paint, Background* bg );
	void						DrawListMarker( Draw& hdc, const Position &pos );
	void						ParseNthChildParams( String param, int &num, int &off );
	void						RemoveBeforeAfter();
	Element*				GetElementBefore();
	Element*				GetElementAfter();
};

/************************************************************************/
/*                        Inline Functions                              */
/************************************************************************/


class ElementZIndexSort
{
public:
	bool operator()(const Element* _Left, const Element* _Right) const
	{
		return (_Left->GetZIndex() < _Right->GetZIndex());
	}
};

END_HTMLCTRL_NAMESPACE


