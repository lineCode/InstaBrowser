#include "PerformanceMonitor.h"


NAMESPACE_HTMLCTRL


PerformanceMonitor::PerformanceMonitor(HtmlNode& parent) : HtmlNode(parent) {
	Ctrl::Add(list.VSizePos().HSizePos());
	
	list.AddColumn("Key");
	list.AddColumn("Elapsed");
	list.AddColumn("Count");
	list.AddColumn("Average");
}

void PerformanceMonitor::RefreshData() {
	int c1 = GetPerformanceMeterCount();
	
	list.Clear();
	for(int i = 0; i < c1; i++) {
		PerformanceMeter& pm = GetPerformanceMeter(i);
		
		list.Add(pm.GetTitle(), pm.Elapsed(), pm.Count(), pm.Average());
	}
}

END_HTMLCTRL_NAMESPACE
