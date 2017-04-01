#ifndef _Browser_PerformanceMeter_h_
#define _Browser_PerformanceMeter_h_


#include <Core/Core.h>
using namespace Upp;
using namespace Upp;

#include "Common.h"

NAMESPACE_HTMLCTRL


#define PERFMETER_BEGIN(x) 	PerformanceMeterBegin(x)
#define PERFMETER_END(x)	PerformanceMeterEnd(x)


enum {
	PERF_CREATEFONT, PERF_TEXTWIDTH, PERF_DRAWTEXT, PERF_DRAWLISTMARKER, PERF_IMAGESIZEGET,
	PERF_IMAGESIZEFIND, PERF_DRAWBG, PERF_DRAWBORDERS, PERF_LOADMASTERCSS, PERF_CSSELEMENTPARSE,
	PERF_CSSSELPARSE, PERF_CSSSELCALCSPEC, PERF_CSSPARSE, PERF_CREATEDOC, PERF_DOCINIT,
	PERF_DOCADDFONT, PERF_GETFONT, PERF_DOCRENDER, PERF_DOCPAINT, PERF_MOUSEOVER,
	PERF_MOUSELEAVE, PERF_MOUSELEFTDOWN, PERF_MOUSELEFTUP, PERF_DRAWBG0, PERF_DRAWBG1,
	PERF_DRAWBG2
};

	
class PerformanceMeter {
	TimeStop ts;
	
	uint64 total;
	uint64 count;
	String key;
public:
	PerformanceMeter();
	
	void Begin();
	void End();
	
	void SetTitle(const String& s) {key = s;}
	
	String GetTitle() {return key;}
	int Elapsed() {return total;}
	int Count() {return count;}
	int Average() {return total / count;}
};
	
void PerformanceMeterBegin(int id);
void PerformanceMeterEnd(int id);

int GetPerformanceMeterCount();
PerformanceMeter& GetPerformanceMeter(int i);

END_HTMLCTRL_NAMESPACE

#endif
