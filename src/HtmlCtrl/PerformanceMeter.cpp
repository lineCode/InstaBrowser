#include "PerformanceMeter.h"


NAMESPACE_HTMLCTRL

ArrayMap<int, PerformanceMeter> meters;

int GetPerformanceMeterCount() {return meters.GetCount();}
PerformanceMeter& GetPerformanceMeter(int i) {return meters[i];}


PerformanceMeter::PerformanceMeter() {
	total = 0;
	count = 0;
}

void PerformanceMeter::Begin() {
	ts.Reset();
	count++;
}


void PerformanceMeter::End() {
	uint64 el = ts.Elapsed();
	total += el;
}

void PerformanceMeterBegin(int id) {
	int i = meters.Find(id);
	if (i == -1) {
		i = meters.GetCount(); 
		meters.Add(id);
		PerformanceMeter& pm = meters[i];
		switch (id) {
			case PERF_CREATEFONT:		pm.SetTitle("Create font"); break;
			case PERF_TEXTWIDTH:		pm.SetTitle("Text width"); break;
			case PERF_DRAWTEXT:			pm.SetTitle("Draw text"); break;
			case PERF_DRAWLISTMARKER:	pm.SetTitle("Draw Listmarker"); break;
			case PERF_IMAGESIZEGET:		pm.SetTitle("Image Size Get"); break;
			case PERF_IMAGESIZEFIND:	pm.SetTitle("Image Size Find"); break;
			case PERF_DRAWBG:			pm.SetTitle("Draw Background"); break;
			case PERF_DRAWBORDERS:		pm.SetTitle("Draw Borders"); break;
			case PERF_LOADMASTERCSS:	pm.SetTitle("Load Master Css"); break;
			case PERF_CSSELEMENTPARSE:	pm.SetTitle("Css Element Parse"); break;
			case PERF_CSSSELPARSE:		pm.SetTitle("Css Selection Parse"); break;
			case PERF_CSSSELCALCSPEC:	pm.SetTitle("Css Selection Calc Spec."); break;
			case PERF_CSSPARSE:			pm.SetTitle("Css Parse"); break;
			case PERF_CREATEDOC:		pm.SetTitle("Create document"); break;
			case PERF_DOCINIT :			pm.SetTitle("Document Init"); break;
			case PERF_DOCADDFONT:		pm.SetTitle("Document Add Font"); break;
			case PERF_GETFONT:			pm.SetTitle("Get Font"); break;
			case PERF_DOCRENDER:		pm.SetTitle("Document Render"); break;
			case PERF_DOCPAINT:			pm.SetTitle("Document Paint"); break;
			case PERF_MOUSEOVER :		pm.SetTitle("Document Mouse Over"); break;
			case PERF_MOUSELEAVE:		pm.SetTitle("Document Mouse Leave"); break;
			case PERF_MOUSELEFTDOWN:	pm.SetTitle("Document Mouse Left Down"); break;
			case PERF_MOUSELEFTUP:		pm.SetTitle("Document Mouse Left Up"); break;
			case PERF_DRAWBG0:			pm.SetTitle("Draw Background 0"); break;
			case PERF_DRAWBG1:			pm.SetTitle("Draw Background 1"); break;
			case PERF_DRAWBG2:			pm.SetTitle("Draw Background 2"); break;
			
			
			default: Panic("Unknown performance-meter id: " + IntStr(id));
		}
		pm.Begin();
	} else {
		PerformanceMeter& pm = meters[i];
		pm.Begin();
	}
}

void PerformanceMeterEnd(int id) {
	int i = meters.Find(id);
	ASSERT(i != -1);
	PerformanceMeter& pm = meters[i];
	
	pm.End();
}

END_HTMLCTRL_NAMESPACE
