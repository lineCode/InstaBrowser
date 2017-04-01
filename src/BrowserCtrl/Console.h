#ifndef _Browser_Console_h_
#define _Browser_Console_h_


#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include <HtmlCtrl/Common.h>
#include <CodeEditor/CodeEditor.h>

NAMESPACE_HTMLCTRL


class Console : public ParentCtrl, public HtmlNode {
	CodeEditor log;
	
public:
	Console(HtmlNode& parent);
	
	void Print(const String& line);
	
};

END_HTMLCTRL_NAMESPACE
#endif
