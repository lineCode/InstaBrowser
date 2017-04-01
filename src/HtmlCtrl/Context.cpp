#include "Html.h"
#include "Context.h"
#include "Stylesheet.h"
#include "PerformanceMeter.h"

NAMESPACE_HTMLCTRL

Context::Context(HtmlNode& head) : m_master_css(*this), HtmlNode(head) {
	
}

void Context::LoadMasterStylesheet( const String& str )
{
	PERFMETER_BEGIN(PERF_LOADMASTERCSS);
	
	MediaQueryList::ptr media;

	m_master_css.ParseStylesheet(str, "", 0, media);
	m_master_css.SortSelectors();
	
	PERFMETER_END(PERF_LOADMASTERCSS);
}

END_HTMLCTRL_NAMESPACE
