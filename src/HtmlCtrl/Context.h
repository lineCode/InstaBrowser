#pragma once
#include "Stylesheet.h"
#include <HtmlCtrl/Common.h>

/*
	Context contains master css style for every element created.
	In future it may be a signal bus between elements.
*/

NAMESPACE_HTMLCTRL

class Context : public ObjectPool, public HtmlNode
{
	Css	m_master_css;
public:
	
	Context(HtmlNode& head);
	
	void		LoadMasterStylesheet(const String& str);
	Css&	MasterCss()
	{
		return m_master_css;
	}
};

END_HTMLCTRL_NAMESPACE
