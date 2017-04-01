#pragma once
#include "Object.h"

NAMESPACE_HTMLCTRL

class HtmlTag;

enum Box_type
{
	Box_block,
	Box_line
};

class Box : public Object
{
public:
	typedef ObjectPtr<Box>					ptr;
	typedef Vector< ObjectPtr<Box> >	vector;
protected:
	int		m_box_top;
	int		m_box_left;
	int		m_box_right;
public:
	Box(ObjectPool& pool, int top, int left, int right) : Object(pool)
	{
		m_box_top	= top;
		m_box_left	= left;
		m_box_right	= right;
	}

	int		Bottom()	{ return m_box_top + Height();	}
	int		Top()		{ return m_box_top;				}
	int		Right()		{ return m_box_left + Width();	}
	int		Left()		{ return m_box_left;			}

	virtual Box_type	GetType() = 0;
	virtual int					Height() = 0;
	virtual int					Width() = 0;
	virtual void				AddElement(Element* el) = 0;
	virtual bool				CanHold(Element* el, WhiteSpace ws) = 0;
	virtual void				Finish(bool last_box = false) = 0;
	virtual bool				IsEmpty() = 0;
	virtual int					Baseline() = 0;
	virtual void				GetElements(ElementsVector& els) = 0;
	virtual int					TopMargin() = 0;
	virtual int					BottomMargin() = 0;
	virtual void				SetYShift(int shift) = 0;
	virtual void				NewWidth(int left, int right, ElementsVector& els) = 0;
};

//////////////////////////////////////////////////////////////////////////

class BlockBox : public Box
{
	Element*	m_element;
public:
	BlockBox(ObjectPool& pool, int top, int left, int right) : Box(pool, top, left, right)
	{
		m_element = 0;
	}

	virtual Box_type	GetType();
	virtual int					Height();
	virtual int					Width();
	virtual void				AddElement(Element* el);
	virtual bool				CanHold(Element* el, WhiteSpace ws);
	virtual void				Finish(bool last_box = false);
	virtual bool				IsEmpty();
	virtual int					Baseline();
	virtual void				GetElements(ElementsVector& els);
	virtual int					TopMargin();
	virtual int					BottomMargin();
	virtual void				SetYShift(int shift);
	virtual void				NewWidth(int left, int right, ElementsVector& els);
};

//////////////////////////////////////////////////////////////////////////

class LineBox : public Box
{
	Vector<Element*>		m_items;
	int						m_height;
	int						m_width;
	int						m_line_height;
	FontMetrics				m_font_metrics;
	int						m_baseline;
	text_align				m_text_align;
public:
	LineBox(ObjectPool& pool, int top, int left, int right, int line_height, FontMetrics& fm, text_align align) : Box(pool, top, left, right)
	{
		m_height		= 0;
		m_width			= 0;
		m_font_metrics	= fm;
		m_line_height	= line_height;
		m_baseline		= 0;
		m_text_align	= align;
	}

	virtual Box_type			GetType();
	virtual int					Height();
	virtual int					Width();
	virtual void				AddElement(Element* el);
	virtual bool				CanHold(Element* el, WhiteSpace ws);
	virtual void				Finish(bool last_box = false);
	virtual bool				IsEmpty();
	virtual int					Baseline();
	virtual void				GetElements(ElementsVector& els);
	virtual int					TopMargin();
	virtual int					BottomMargin();
	virtual void				SetYShift(int shift);
	virtual void				NewWidth(int left, int right, ElementsVector& els);

private:
	Element*					GetLastSpace();
	bool						IsBreakOnly();
};

END_HTMLCTRL_NAMESPACE
