#ifndef _Browser_PerformanceMonitor_h_
#define _Browser_PerformanceMonitor_h_


#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include <HtmlCtrl/PerformanceMeter.h>
#include <HtmlCtrl/Common.h>

NAMESPACE_HTMLCTRL


class PerformanceMonitor : public ParentCtrl, public HtmlNode {
	
	ArrayCtrl list;
	
public:
	
	PerformanceMonitor(HtmlNode& parent);
	
	void RefreshData();
	
	
};


END_HTMLCTRL_NAMESPACE


#endif
