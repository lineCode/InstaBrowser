#ifndef _HtmlCtrl_Common_h_
#define _HtmlCtrl_Common_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include <CoreUtils/Node.h>
#include <BrowserCore/BrowserCore.h>

#define NAMESPACE_HTMLCTRL namespace HtmlCtrl {
#define END_HTMLCTRL_NAMESPACE }

#define LAST(x) (*(x.End()-1))
#define REMOVELAST(x) x.Remove(x.GetCount()-1)

int FindFirstOf(const String& src, const String& match, int startpos=0);
int FindFirstOf(const String& src, char match, int startpos=0);
int FindLastOf(const String& src, const String& match, int endpos=-1, int count=-1);
int FindFirstNotOf(const String& src, const String& match, int startpos=0, int count=-1);
int FindLastNotOf(const String& src, const String& match, int endpos=-1, int count=-1);

inline int HexInt(const String& s) {char* c; return strtol(s.Begin(), &c, 16);}



class CacheImage : public CacheObject {
	
protected:
	Image img;
	
public:
	CacheImage();
	
	
	operator const Image&();
	operator const Image*();
	const Image& GetImage();
	
	CacheImage& SetImage(const Image& img);
	bool SetData(Stream& s);
};

NAMESPACE_HTMLCTRL

class HtmlPageNode;

class HtmlNode {
	
protected:
	HtmlNode* parent;
	Vector<HtmlNode*> sub_nodes;
public:
	HtmlNode(const Nuller& n) {parent = 0;}
	HtmlNode(HtmlNode& parent) {this->parent = &parent; parent.sub_nodes.Add(this);}
	virtual ~HtmlNode() {}
	HtmlNode* GetParent() const {return parent;}
	void MoveTo(HtmlNode& new_parent) {
		for(int i = 0; i < parent->sub_nodes.GetCount(); i++) {
			if (parent->sub_nodes[i] == this) {
				parent->sub_nodes.Remove(i);
				i--;
			}
		}
		new_parent.sub_nodes.Add(this);
		parent = &new_parent;
	}
	//int GetCount() const {return sub_nodes.GetCount();}
	template <class T> T* Get() {return dynamic_cast<T*>(this);}
	template <class T> bool Is() {return dynamic_cast<T*>(this) != 0;}
	template <class T> T* FindNode() {
		for(int i = 0; i < sub_nodes.GetCount(); i++) {
			T* ptr = dynamic_cast<T*>(sub_nodes[i]);
			if (ptr) return ptr;
		}
		return 0;
	}
	
	template <class T> T* GetParentClass() {
		HtmlNode* p = this;
		while(1) {
			T* t = dynamic_cast<T*>(p);
			if (t) return t;
			p = p->parent;
		}
		return 0;
	}
	
	HtmlPageNode* GetPage();
	String GetBaseUrl();
	
};

class ElScript;

class HtmlPageNode : public HtmlNode {
	String url, base_url, protocol;
	
public:
	HtmlPageNode(HtmlNode& parent);
	
	virtual void Script(ElScript* ptr) {}
	
	String GetUrl();
	String GetBaseUrl();
	String GetProtocol();
	
	void SetUrl(const String& s);
	void SetBaseUrl(const String& s);
	
};

END_HTMLCTRL_NAMESPACE

#endif
