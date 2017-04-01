#ifndef _HtmlCtrl_HtmlCtrl_h_
#define _HtmlCtrl_HtmlCtrl_h_

/*
	Forked from LiteHtml 5.6.2015 (https://github.com/litehtml/litehtml)
	License: New BSD
	LiteHtml copyright: Yuri Kobets (tordex)
	
*/



#include <CtrlLib/CtrlLib.h>


#include "Common.h"
#include "Html.h"
#include "Document.h"
#include "HtmlTag.h"
#include "Stylesheet.h"
#include "Stylesheet.h"
#include "Element.h"
#include "HtmlTag.h"


NAMESPACE_HTMLCTRL



class HtmlCtrl : public Ctrl, public HtmlFile {
	
	
public:
	HtmlCtrl(HtmlNode& dir);
	
	virtual ~HtmlCtrl();

	virtual uint32						CreateFont(const String& faceName, int size, int weight, FontStyle italic, unsigned int decoration, FontMetrics* fm);
	virtual void						DeleteFont(uint32 hFont);
	virtual int							TextWidth(const String& text, uint32 hFont);
	virtual void						DrawText(Draw& hdc, const String& text, uint32 hFont, WebColor color, const Position& pos);
	virtual void						FillRect(Draw& hdc, const Position& pos, const WebColor color, const CssBorderRadius& radius);
	
	virtual int							PtPx(int pt);
	virtual int							GetDefaultFontSize() const;
	virtual String						GetDefaultFontName() const;
	virtual void						DrawListMarker(Draw& hdc, const ListMarker& marker);
	virtual void						LoadImage(const String& src, const String& baseurl, bool redraw_on_ready);
	virtual void						GetImageSize(const String& src, const String& baseurl, Upp::Size& sz);
	virtual void						GetImageSize(int cache_id, Size& sz);
	virtual void						DrawImage(Draw& hdc, const String& src, const String& baseurl, const Position& pos);
	virtual void						DrawBackground(Draw& hdc, const BackgroundPaint& bg);
	virtual void						DrawBorders(Draw& hdc, const Borders& Borders, const Position& draw_pos, bool root);

	virtual	void						TransformText(String& text, TextTransform tt);
	virtual void						SetClip(const Position& pos, const BorderRadiuses& bdr_radius, bool valid_x, bool valid_y);
	virtual void						DelClip();
	virtual Element*					CreateElement(const String& tag_name, const StrMap& attributes, Document* doc);
	virtual void						GetMediaFeatures(MediaFeatures& media);

	//virtual void						MakeUrl( LPCWSTR url, LPCWSTR basepath, String& out ) = 0;
	//virtual Image*						GetImage(const String& url, bool redraw_on_ready) = 0;
	//virtual	Image*						GetCacheObject(int cache_id) = 0;
	//virtual void						GetClientRect(Position& client) = 0;
	
	
	virtual	void						SetCaption(const String& caption);
	virtual	void						SetBaseUrl(const String& base_url);
	virtual void						Link(Document* doc, Element::ptr el);
	virtual void						OnAnchorClick(const String& url, Element::ptr el);
	virtual	void						SetCursor(const String& cursor);
	virtual void						ImportCss(String& text, const String& url, String& baseurl);
	virtual void						GetClientRect(Position& client);
		
	
protected:
	
	virtual void						DrawEllipse(Draw& hdc, int x, int y, int width, int height, const WebColor& color, double line_width);
	virtual void						FillEllipse(Draw& hdcr, int x, int y, int width, int height, const WebColor& color);
	virtual void						RoundedRectangle( Draw& hdc, const Position &pos, const BorderRadiuses &radius );
	
	
		
};




END_HTMLCTRL_NAMESPACE

#endif
