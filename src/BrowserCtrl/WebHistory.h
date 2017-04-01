#pragma once

#include <Core/Core.h>
using namespace Upp;

#include <HtmlCtrl/Common.h>

NAMESPACE_HTMLCTRL

class WebHistory
{
	Vector<String>		m_items;
	int					m_current_item;
public:
	WebHistory();
	virtual ~WebHistory();

	void UrlOpened(const String& url);
	bool Back(String& url);
	bool Forward(String& url);
};

END_HTMLCTRL_NAMESPACE
