#ifndef _Browser_NetworkCtrl_h_
#define _Browser_NetworkCtrl_h_


#include <CtrlLib/CtrlLib.h>
using namespace Upp;


#include <HtmlCtrl/Common.h>

NAMESPACE_HTMLCTRL

class NetworkCtrl : public ParentCtrl, public HtmlNode {
	ArrayCtrl list;
	
	bool last_idle;
	
public:
	NetworkCtrl(HtmlNode& dir);
	
	void RefreshData();
	
	
};


END_HTMLCTRL_NAMESPACE
#endif
