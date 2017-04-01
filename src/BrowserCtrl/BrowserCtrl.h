#pragma once

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include "WebPageCtrl.h"
#include "WebTools.h"

void BrowserCtrl_UnpackContent();

NAMESPACE_HTMLCTRL

class BrowserCtrl : public ParentCtrl, public HtmlNode
{
	Context			m_browser_context;
	
	Splitter		webview;
	WebPageCtrl		m_view;

	
	WebTools		wtools;
	Splitter		split;
	
	
public:
	typedef BrowserCtrl CLASSNAME;
	
	BrowserCtrl(HtmlNode& f);
	virtual ~BrowserCtrl();

	void Open(const String& path);

	void Back();
	void Forward();
	void Reload();
	//void CalcTime();
	
	void PageLoaded(String& url);
	
	
	
	virtual BrowserCtrl* GetAsBrowserCtrl() {return this;}
	
	Callback WhenOpenPageChanged;
	
	WebPageCtrl& GetWebPageCtrl() {return m_view;}
	
	
	
protected:
	
	void OpenPageChanged();


};

END_HTMLCTRL_NAMESPACE
