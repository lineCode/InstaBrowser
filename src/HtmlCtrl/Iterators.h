#pragma once

#include "Types.h"

NAMESPACE_HTMLCTRL

class Element;

class IteratorSelector
{
public:
	virtual bool Select(Element* el) = 0;
};

class ElementsIterator
{
private:
	struct stack_item : public Moveable<stack_item>
	{
		int			idx;
		Element*	el;
	};

	Vector<stack_item>		m_stack;
	Element*					m_el;
	int							m_idx;
	IteratorSelector*			m_go_inside;
	IteratorSelector*			m_Select;
public:

	ElementsIterator(Element* el, IteratorSelector* go_inside, IteratorSelector* Select)
	{ 
		m_el			= el;
		m_idx			= -1; 
		m_go_inside		= go_inside;
		m_Select		= Select;
	}

	~ElementsIterator()
	{

	}

	Element* Next(bool ret_higher = true);

private:
	void NextIdx();
};

class go_inside_inline : public IteratorSelector
{
public:
	virtual bool Select(Element* el);
};

class go_inside_table : public IteratorSelector
{
public:
	virtual bool Select(Element* el);
};

class TableRowsSelector : public IteratorSelector
{
public:
	virtual bool Select(Element* el);
};

class TableCellsSelector : public IteratorSelector
{
public:
	virtual bool Select(Element* el);
};

END_HTMLCTRL_NAMESPACE

