#include "WebPageCtrl.h"
#include "BrowserCtrl.h"


NAMESPACE_HTMLCTRL



SimpleImageCtrl::SimpleImageCtrl() {
	
}

void SimpleImageCtrl::Paint(Draw& d) {
	Size sz = GetSize();
	d.DrawRect(sz, White());
	
	int x = 0, y = 0;
	WhenScroll(&x, &y);
	
	//d.DrawImage(x, y, data);
	d.DrawImage(0, 0, data, RectC(x, y, sz.cx, sz.cy));
}










WebPageCtrl::WebPageCtrl(HtmlNode& dir) : HtmlNode(dir)
{
	posted_redraw = false;
	
	sel_highlight.Clear();
	sel_hover = 1;
	
	scroll_v.SetLine(16);
	scroll_h.SetLine(16);
	
	AddFrame(scroll_v);
	AddFrame(scroll_h);
	
	scroll_h.Horz();

	scroll_v.WhenScroll = THISBACK(Scroll);
	scroll_h.WhenScroll = THISBACK(Scroll);
	
	Add(img_ctrl.SizePos());
	Add(gui_ctrl.SizePos());
	
	img_ctrl.WhenScroll = THISBACK(GetScroll);
	img_ctrl.Ctrl::NoWantFocus();
	img_ctrl.Ctrl::IgnoreMouse();
	gui_ctrl.Transparent(true);
	
}

WebPageCtrl::~WebPageCtrl(void)
{
	if (m_page) {
		m_page->Clear();
		m_page.Clear();
	}
}



void WebPageCtrl::OnCreate()
{

}

void WebPageCtrl::Layout()
{
	Size sz(GetSize());
	
	scroll_v.SetPage(sz.cy);
	
	scroll_h.SetPage(sz.cx);
	
	RefreshBuffer();
}

void WebPageCtrl::GetScroll(int* x, int* y) {
	*x = scroll_h;
	*y = scroll_v;
}


void WebPageCtrl::RefreshBuffer() {
	WebPage& page = GetPage();
	
	Size sz(GetSize());
	
	if (!page.m_doc) return;
	
	page.m_doc->Render(sz.cx);
	
	int w = page.m_doc->Width();
	int h = page.m_doc->Height();
	ImageDraw id(w, h);
	id.DrawRect(Size(w,h), White());
	Position clip(0, 0, w, h);
	page.m_doc->Paint(id, 0, 0, clip);
	
	img_ctrl.GetImage() = id;
	
	scroll_v.SetTotal(h);
	scroll_h.SetTotal(w);
	
	/*if (sel_highlight.width) {
		Size sz(sel_highlight.GetRect().GetSize());
		ImageDraw id(sz);
		id.Alpha().DrawRect(sz, GrayColor(128));
		id.DrawRect(sz, Cyan());
		id.DrawLine(0, 0, 0, sz.cy-1, 1, Blue());
		id.DrawLine(sz.cx-1, 0, sz.cx-1, sz.cy-1, 1, Blue());
		id.DrawLine(0, 0, sz.cx-1, 0, 1, Blue());
		id.DrawLine(0, sz.cy-1, sz.cx-1, sz.cy-1, 1, Blue());
		d.DrawImage(-scroll_h + sel_highlight.x, -scroll_v + sel_highlight.y, id);
		//d.DrawRect(-scroll_h + sel_highlight.x, -scroll_v + sel_highlight.y, sel_highlight.width, sel_highlight.height, Blue());
	}*/
	
	page.Release();
}


void WebPageCtrl::OnDestroy()
{

}

void WebPageCtrl::Create( int x, int y, int width, int height, int parent ) {
	
}

void WebPageCtrl::Open( const String& url, bool reload ) {
	
	WebPage& wp = AddPage();
	wp.WhenPageReady = THISBACK(OnPageReady);
	wp.WhenRedraw = THISBACK(OnRedraw);
	wp.Load(url);
	
}

WebPage& WebPageCtrl::AddPage() {
	//WebPage& wp = *webpages.Add(new WebPage(*this));
	if (m_page) {
		m_page.Clear();
	}
	m_page = new WebPage(*this);
	m_page->WhenTitleChange = Proxy(WhenTitleChange);
	return *m_page;
}

// Callback target to redraw
void WebPageCtrl::OnRedraw() {
	if (posted_redraw)
		return;
	posted_redraw = true;
	PostCallback(THISBACK(OnDemandRedraw));
}

/*void WebPageCtrl::Render(bool calc_time, bool do_redraw)
{
	
	WebPage& page = GetPage();
	if (!page.m_doc) return;
	
	Size sz = GetSize();
	
	if (sz.cx == 0)
		return;
	
	page.m_doc->Render(sz.cx);
	
	if(do_redraw)
	{
		OnDemandRedraw();
	}
	
	
}*/

void WebPageCtrl::OnDemandRedraw() {
	posted_redraw = false;
	Rect nullrect;
	Redraw(nullrect, false);
	//UpdateScroll();
	WhenOpenPageChanged();
}

void WebPageCtrl::OnDemandRender() {
	scroll_v.Set(0);
	scroll_h.Set(0);
	
	//Render(false, false);
	//RefreshBuffer();
	
	//UpdateScroll();
	
	OnDemandRedraw();
}
	
void WebPageCtrl::Redraw(Rect rcDraw, bool update)
{
	RefreshBuffer();
	Refresh();
}

/*void WebPageCtrl::UpdateScroll()
{
	if(!IsValidPage())
	{
		scroll_v.Show(false);
		scroll_h.Show(false);
		return;
	}
	
	Size sz(GetSize());
	WebPage& page = GetPage();
	scroll_v.SetTotal( page.m_doc->Height() - sz.cy );
	scroll_h.SetTotal( page.m_doc->Width() - sz.cx );
	
	if(scroll_v.GetTotal() > 0)
	{
		scroll_v.Show(true);
	} else
	{
		scroll_v.Show(false);
	}

	if(scroll_h.GetTotal() > 0)
	{
		scroll_h.Show(true);
	} else
	{
		scroll_h.Show(false);
	}
}*/

/*void WebPageCtrl::OnVScroll( int pos, int flags )
{

}

void WebPageCtrl::OnHScroll( int pos, int flags )
{
	
}
*/
void WebPageCtrl::MouseWheel(Point, int zdelta, dword)
{
	//int lineHeight	= 16;
	
	/*int WHEEL_DELTA = 3;
	
	int new_top  = scroll_v - zdelta / WHEEL_DELTA * scroll_v.GetLine() * 3;

	if(new_top < 0)
	{
		new_top = 0;
	}
	if(new_top > scroll_h.GetTotal())
	{
		new_top = scroll_h.GetTotal();
	}

	if(new_top != scroll_v)
	{
		scroll_v.Set(new_top);
		Render();
	}*/
	
	scroll_v.Wheel(zdelta);
	img_ctrl.Refresh();
}

bool WebPageCtrl::Key(dword key, int count)
{
	/*switch(vKey)
	{
	case VK_F5:
		refresh();
		break;
	case VK_NEXT:
		OnVScroll(0, SB_PAGEDOWN);
		break;
	case VK_PRIOR:
		OnVScroll(0, SB_PAGEUP);
		break;
	case VK_DOWN:
		OnVScroll(0, SB_LINEDOWN);
		break;
	case VK_UP:
		OnVScroll(0, SB_LINEUP);
		break;
	case VK_HOME:
		ScrollTo(scroll_h, 0);
		break;
	case VK_END:
		ScrollTo(scroll_h, m_max_top);
		break;
	case VK_LEFT:
		OnHScroll(0, SB_LINELEFT);
		break;
	case VK_RIGHT:
		OnHScroll(0, SB_LINERIGHT);
		break;
	}*/
	return false;
}


void WebPageCtrl::SetCaption() {

}

void WebPageCtrl::MouseMove(Point p, dword keyflags)
{
	WebPage& page = GetPage();
	if (!page.m_doc) return;
	Position::vector redraw_boxes;
	int x = p.x, y = p.y;
	Element* el = 0;
	if(page.m_doc->OnMouseOver(x + scroll_h, y + scroll_v, x, y, redraw_boxes, &el))
	{
		if (el && sel_hover)
			sel_highlight = el->GetPlacement();
		
		for(Position::vector::iterator box = redraw_boxes.Begin(); box != redraw_boxes.End(); box++)
		{
			box->x -= scroll_h;
			box->y -= scroll_v;
			Rect rcRedraw;
			rcRedraw.left	= box->Left();
			rcRedraw.right	= box->Right();
			rcRedraw.top	= box->Top();
			rcRedraw.bottom	= box->Bottom();
			Redraw(rcRedraw, false);
		}
		//UpdateCursor();
	}
	else {
		if (el && sel_hover) {
			sel_highlight = el->GetPlacement();
			OnDemandRedraw();
		}
	}
	
	page.Release();
}

void WebPageCtrl::MouseLeave()
{
	WebPage& page = GetPage();
	if (!page.m_doc) return;
	
	Position::vector redraw_boxes;
	if(page.m_doc->OnMouseLeave(redraw_boxes))
	{
		for(Position::vector::iterator box = redraw_boxes.Begin(); box != redraw_boxes.End(); box++)
		{
			box->x -= scroll_h;
			box->y -= scroll_v;
			Rect rcRedraw;
			rcRedraw.left	= box->Left();
			rcRedraw.right	= box->Right();
			rcRedraw.top	= box->Top();
			rcRedraw.bottom	= box->Bottom();
			Redraw(rcRedraw, false);
		}
	}

	page.Release();
}

void WebPageCtrl::LeftDown(Point p, dword keyflags)
{
	WebPage& page = GetPage();
	if (!page.m_doc) return;
	
	Position::vector redraw_boxes;
	int x = p.x, y = p.y;
	if(page.m_doc->OnMouseLeftDown(x + scroll_h, y + scroll_v, x, y, redraw_boxes))
	{
		for(Position::vector::iterator box = redraw_boxes.Begin(); box != redraw_boxes.End(); box++)
		{
			box->x -= scroll_h;
			box->y -= scroll_v;
			Rect rcRedraw;
			rcRedraw.left	= box->Left();
			rcRedraw.right	= box->Right();
			rcRedraw.top	= box->Top();
			rcRedraw.bottom	= box->Bottom();
			Redraw(rcRedraw, false);
		}
	}

	page.Release();
}

void WebPageCtrl::LeftUp(Point p, dword keyflags)
{
	WebPage& page = GetPage();
	if (!page.m_doc) return;
	
	//if(page) {
	Position::vector redraw_boxes;
	int x = p.x, y = p.y;
	if(page.m_doc->OnMouseLeftUp(x + scroll_h, y + scroll_v, x, y, redraw_boxes))
	{
		for(Position::vector::iterator box = redraw_boxes.Begin(); box != redraw_boxes.End(); box++)
		{
			box->x -= scroll_h;
			box->y -= scroll_v;
			Rect rcRedraw;
			rcRedraw.left	= box->Left();
			rcRedraw.right	= box->Right();
			rcRedraw.top	= box->Top();
			rcRedraw.bottom	= box->Bottom();
			Redraw(rcRedraw, false);
		}
	}

	page.Release();
}

void WebPageCtrl::Back()
{
	String url;
	if(m_history.Back(url))
	{
		Open(url, false);
	}
}

void WebPageCtrl::Forward()
{
	String url;
	if(m_history.Forward(url))
	{
		Open(url, false);
	}
}

/*void WebPageCtrl::UpdateCursor()
{
	
}
*/
void WebPageCtrl::GetClientRect( Position& client )
{
	Rect rcClient;
	rcClient		= Rect(GetSize());
	client.x		= rcClient.left;
	client.y		= rcClient.top;
	client.width	= rcClient.right - rcClient.left;
	client.height	= rcClient.bottom - rcClient.top;
}

bool WebPageCtrl::IsValidPage(bool with_lock)
{
	bool ret_val = true;

	WebPage& page = GetPage();
	if(!page.m_doc)
	{
		ret_val = false;
	}

	return ret_val;
}

WebPage& WebPageCtrl::GetPage(bool with_lock)
{
	if (m_page.IsEmpty())
		AddPage();
	return *m_page;
}

void WebPageCtrl::OnPageReady()
{
	PostCallback(THISBACK(OnDemandRender));
}

void WebPageCtrl::ShowHash(String& hash)
{
	
}

void WebPageCtrl::UpdateHistory()
{
	
}


void WebPageCtrl::Scroll() {
	//Render();
	img_ctrl.Refresh();
}

void WebPageCtrl::InspectorSelection(Position pos) {
	sel_highlight = pos;
	PostCallback(THISBACK(OnDemandRedraw));
}

END_HTMLCTRL_NAMESPACE
