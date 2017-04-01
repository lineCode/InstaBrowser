#include "WebTools.h"

NAMESPACE_HTMLCTRL

WebTools::WebTools(HtmlNode& dir) : WebPageCtrl(dir), insp(*this), net(*this), cons(*this), perfmon(*this) {
	Ctrl::Add(tabs.VSizePos().HSizePos());
	
	tabs.Add(insp);
	tabs.Add(insp, "Inspector");
	tabs.Add(cons);
	tabs.Add(cons, "Console");
	tabs.Add(net);
	tabs.Add(net, "Network");
	tabs.Add(perfmon);
	tabs.Add(perfmon, "Performance Monitor");
	
	tabs.Set(1);
	
	Start();
}

WebTools::~WebTools() {
	Stop();
	
}


void WebTools::OpenPageChanged() {
	insp.RefreshTree();
	
}

void WebTools::Start() {
	running = 1;
	thrd.Run(THISBACK(Execute));
}

void WebTools::Stop() {
	running = 0;
	thrd.Wait();
}

void WebTools::Execute() {
	while (running) {
		int tab = tabs.Get();
		
		// TODO: Refresh on-demand only
		switch (tab) {
			case 0: insp.RefreshData(); break; // Inspector
			case 1: break; // Console
			case 2: net.RefreshData(); break; // Network
			//case 3: perfmon.RefreshData(); break;
			case 3: PostCallback(callback(&perfmon, &PerformanceMonitor::RefreshData)); break;
		}
		Sleep(200);
	}
}


END_HTMLCTRL_NAMESPACE
