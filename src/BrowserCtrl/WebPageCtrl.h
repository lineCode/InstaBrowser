#pragma once
#include "WebPage.h"
#include "WebHistory.h"

NAMESPACE_HTMLCTRL


class SimpleImageCtrl : public Ctrl {
	Image data;
	int x_off, y_off;
public:
	SimpleImageCtrl();
	
	Image& GetImage() {return data;}
	
	virtual void Paint(Draw& d);
	
	virtual bool Key(dword key, int count) {return Ctrl::GetParent()->Key(key, count);}
	virtual void LeftDown(Point p, dword keyflags) {Ctrl::GetParent()->LeftDown(p, keyflags);}
	virtual void LeftUp(Point p, dword keyflags) {Ctrl::GetParent()->LeftUp(p, keyflags);}
	virtual void MouseMove(Point p, dword keyflags) {Ctrl::GetParent()->MouseMove(p, keyflags);}
	
	Callback2<int*, int*> WhenScroll;
	
};

class ScriptCanvas : public ParentCtrl {
	
public:
	ScriptCanvas() {}
	
	virtual bool Key(dword key, int count) {return Ctrl::GetParent()->Key(key, count);}
	virtual void LeftDown(Point p, dword keyflags) {Ctrl::GetParent()->LeftDown(p, keyflags);}
	virtual void LeftUp(Point p, dword keyflags) {Ctrl::GetParent()->LeftUp(p, keyflags);}
	virtual void MouseMove(Point p, dword keyflags) {Ctrl::GetParent()->MouseMove(p, keyflags);}
	
};

class BrowserCtrl;

class WebPageCtrl : public ParentCtrl, public HtmlNode
{
	int							m_hWnd;
	int							m_hInst;
	WebHistory					m_history;
	One<WebPage>				m_page;
	int							m_sync;
	
	VScrollBar					scroll_v;
	HScrollBar					scroll_h;
	
	Position					sel_highlight;
	bool						sel_hover;
	
	SimpleImageCtrl				img_ctrl;
	ScriptCanvas				gui_ctrl;
	
	bool posted_redraw;
	
public:
	typedef WebPageCtrl CLASSNAME;
	
	WebPageCtrl();
	WebPageCtrl(HtmlNode& dir);
	virtual ~WebPageCtrl();
	
	ParentCtrl&			GetGuiCanvas() {return gui_ctrl;}
	void				Create(int x, int y, int width, int height, int parent);
	void				Open(const String& url, bool reload = false);
	int					Wnd()	{ return m_hWnd;	}
	void				Back();
	void				Forward();
	
	void				SetCaption();
	void				Lock();
	void				Unlock();
	bool				IsValidPage(bool with_lock = true);
	WebPage&			GetPage(bool with_lock = true);

	//void				Render(bool calc_time = false, bool do_redraw = true);
	void				GetClientRect(Position& client);
	void				ShowHash(String& hash);
	void				UpdateHistory();
	void				RefreshBuffer();
	
	void InspectorSelection(Position pos);
	
	void				GetScroll(int* x, int* y);
	void				OnRedraw();

	Callback			WhenOpenPageChanged;
	Callback1<String> 	WhenTitleChange;
	
	virtual WebPageCtrl* 		GetAsWebPageCtrl() 			{return this;}
	
	
protected:
	WebPage& 			AddPage();
	
	virtual void 		Layout();
	
	virtual void		OnCreate();
	virtual void		OnDestroy();
	
	//virtual void		OnVScroll(int pos, int flags);
	//virtual void		OnHScroll(int pos, int flags);
	virtual void 		MouseWheel(Point, int zdelta, dword);
	
	virtual bool		Key(dword key, int count);
	virtual void		MouseMove(Point p, dword keyflags);
	virtual void		LeftDown(Point p, dword keyflags);
	virtual void		LeftUp(Point p, dword keyflags);
	virtual void		MouseLeave();
	
	virtual void		OnPageReady();
	
	void				OnDemandRedraw();
	void				OnDemandRender();
	
	void				Redraw(Rect rcDraw, bool update);
	//void				UpdateScroll();
	//void				UpdateCursor();
	void				Scroll();
	

};

END_HTMLCTRL_NAMESPACE

