#ifndef _Browser_Inspector_h_
#define _Browser_Inspector_h_


#include <CtrlLib/CtrlLib.h>


using namespace Upp;

#include <HtmlCtrl/Common.h>

#include <HtmlCtrl/Types.h>

NAMESPACE_HTMLCTRL

class Inspector : public ParentCtrl, public HtmlNode {
	
	Splitter split;
	TreeCtrl tree;
	
	TabCtrl tabs;
	ArrayCtrl style;
	
	void AddElement(int higher, Element* e);
	
	void SelectElement();
	
	ArrayMap<int, Element*> tree_el;
	
	uint32 sel_addr;
public:
	typedef Inspector CLASSNAME;
	
	Inspector(HtmlNode& dir);
	
	void RefreshTree();
	void RefreshData();
	
	
	Callback1<Position> WhenSelection;
	
};

END_HTMLCTRL_NAMESPACE
#endif
