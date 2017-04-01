#include "Inspector.h"
#include "WebPageCtrl.h"
#include "Image.h"

NAMESPACE_HTMLCTRL

Inspector::Inspector(HtmlNode& dir) : HtmlNode(dir) {
	
	sel_addr = 0;
	
	Ctrl::Add(split.VSizePos().HSizePos());
	
	split.Set(tree, tabs);
	split.Horz();
	
	tree.NoRoot();
	
	tree.WhenCursor << THISBACK(SelectElement);
	
	tabs.Add(style, "Style");
	tabs.Add(style);
	
	tabs.Add("Engine");
	tabs.Add("Fonts");
	tabs.Add("Box Model");
	tabs.Add("Animations");
	
	
	style.AddColumn("Css-Key");
	style.AddColumn("Key");
	style.AddColumn("Value");
	
}

// Refresh html elements
void Inspector::RefreshTree() {
	/*WebPageCtrl* wpc; //GlobalFS().FindAll<WebPageCtrl>();  = GetWebPageCtrl();
	
	ASSERT(wpc);
	
	
	WebPage& page = wpc->GetPage();
	
	if (!page.m_doc) 
		return;
	
	Element* e = page.m_doc->Root();
	
	int cur = tree.GetCursor();
	Point scroll = tree.GetScroll();
	
	tree.Clear();
	tree_el.Clear();
	
	AddElement(0, e);
	
	tree.OpenDeep(0);
	
	//tree.SetCursor(cur);
	tree.ScrollTo(scroll);*/
}

void Inspector::AddElement(int higher, Element* e) {
	int c1 = e->GetChildrenCount();
	String tree_txt;
	bool print_txt_sum = 0;
	bool skip_txt_sum = 0;
	for(int i = 0; i < c1; i++) {
		Element* e2 = e->GetChild(i);
		ASSERT(e2); 
		
		
		String tag(e2->GetTagName());
		if (tag.GetCount()) {
			tree_txt = "";
			tree_txt << "<" << tag;
			int c2 = e2->GetAttrCount();
			for(int j = 0; j < c2; j++) {
				String key(e2->GetAttrKey(j));
				String value(e2->GetAttrValue(j));
				tree_txt << " " << key << "=\"" << value << "\"";
			}
			tree_txt << ">";
			
			tree_txt.Replace("\n", "");
			tree_txt.Replace("\t", "");
			tree_txt = TrimBoth(tree_txt);
			int id = tree.Add(higher, BrowserCtrlImages::Element, tree_txt);
			
			tree_el.Add(id, e2);
			
			AddElement(id, e2);
			
			skip_txt_sum = 1;
		} else {
			String s;
			e2->GetText(s);
			tree_txt << s;
			
			print_txt_sum = 1;
		}
	}
	if (print_txt_sum && !skip_txt_sum) {
		int id = tree.Add(higher, BrowserCtrlImages::Text, tree_txt);
		
		// Higher element
		tree_el.Add(id, e);
	}
}

void Inspector::RefreshData() {
	//RefreshTree();
	PostCallback(THISBACK(RefreshTree));
}


void Inspector::SelectElement() {
	/*
	int cur = tree.GetCursor();
	int i = tree_el.Find(cur);
	
	if (i == -1) return;
	
	// Get coordinates
	Element* e = tree_el.Get(i);
	
	// Get styles
	style.Clear();
	int c1 = e->GetStylePropertyCount();
	for(int i = 0; i < c1; i++) {
		String key(e->GetStylePropertyKey(i));
		String value(e->GetStylePropertyValue(i));
		style.Add("", key, value);
	}
	
	
	uint32 a = e->GetAddr();
	if (a == sel_addr) return;
	
	sel_addr = a;
	
	Position pos = e->GetPlacement();
	
	LOG("Inspector::SelectElement " << pos.x << ":" << pos.y << ":" << pos.width << ":" << pos.height);
	
	WhenSelection(pos);*/
}


END_HTMLCTRL_NAMESPACE

