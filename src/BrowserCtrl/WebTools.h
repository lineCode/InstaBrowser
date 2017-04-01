#ifndef _Browser_WebTools_h_
#define _Browser_WebTools_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include "Inspector.h"
#include "Console.h"
#include "NetworkCtrl.h"
#include "PerformanceMonitor.h"
#include "WebPageCtrl.h"

NAMESPACE_HTMLCTRL


class WebTools : public WebPageCtrl {
	TabCtrl tabs;
	Inspector insp;
	Console cons;
	NetworkCtrl net;
	PerformanceMonitor perfmon;
	
	Thread thrd;
	bool running;
	
public:
	typedef WebTools CLASSNAME;
	WebTools();
	WebTools(HtmlNode& dir);
	~WebTools();
	
	
	void OpenPageChanged();
	
	
	void Stop();
	
	
	Inspector& GetInspector() {return insp;}
	
protected:
	void Start();
	void Execute();
};

END_HTMLCTRL_NAMESPACE

#endif
