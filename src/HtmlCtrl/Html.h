#pragma once

#include <CtrlLib/CtrlLib.h>


#include "Types.h"
#include "Object.h"
#include "Background.h"
#include "Borders.h"
#include "HtmlTag.h"
#include "WebColor.h"
#include "MediaQuery.h"

NAMESPACE_HTMLCTRL

struct ListMarker
{
	String			image;
	String			baseurl;
	ListStyleType	marker_type;
	WebColor		color;
	Position		pos;
};

// call back interface to draw text, images and other elements
class HtmlFile : public HtmlPageNode
{
public:
	HtmlFile(HtmlNode& dir);
	
	
	virtual uint32				CreateFont(const String& faceName, int size, int weight, FontStyle italic, unsigned int decoration, FontMetrics* fm) = 0;
	virtual void				DeleteFont(uint32 hFont) = 0;
	virtual int					TextWidth(const String& text, uint32 hFont) = 0;
	virtual void				DrawText(Draw& hdc, const String& text, uint32 hFont, WebColor color, const Position& pos) = 0;
	virtual int					PtPx(int pt) = 0;
	virtual int					GetDefaultFontSize() const = 0;
	virtual String				GetDefaultFontName() const = 0;
	virtual void				DrawListMarker(Draw& hdc, const ListMarker& marker) = 0;
	virtual void				LoadImage(const String& src, const String& baseurl, bool redraw_on_ready) = 0;
	virtual void				GetImageSize(const String& src, const String& baseurl, Upp::Size& sz) = 0;
	virtual void				GetImageSize(int cache_id, Size& sz) = 0;
	virtual void				DrawBackground(Draw& hdc, const BackgroundPaint& bg) = 0;
	virtual void				DrawBorders(Draw& hdc, const Borders& Borders, const Position& draw_pos, bool root) = 0;
	
	virtual	void				SetCaption(const String& caption)		= 0;
	virtual	void				SetBaseUrl(const String& base_url)	= 0;
	virtual void				Link(Document* doc, Element::ptr el) = 0;
	virtual void				OnAnchorClick(const String& url, Element::ptr el) = 0;
	virtual	void				SetCursor(const String& cursor)	= 0;
	virtual	void				TransformText(String& text, TextTransform tt) = 0;
	virtual void				ImportCss(String& text, const String& url, String& baseurl) = 0;
	virtual void				SetClip(const Position& pos, const BorderRadiuses& bdr_radius, bool valid_x, bool valid_y) = 0;
	virtual void				DelClip() = 0;
	virtual void				GetClientRect(Position& client) = 0;
	virtual Element*		CreateElement(const String& tag_name, const StrMap& attributes, Document* doc) = 0;
	virtual void				GetMediaFeatures(MediaFeatures& media) = 0;
	
	
	virtual HtmlNode* 	GetAsFile() 	{return this;}
	
};

class HtmlFileDummy : public HtmlFile {
	
public:
	HtmlFileDummy(HtmlNode& dir) : HtmlFile(dir) {}
	
	virtual uint32				CreateFont(const String& faceName, int size, int weight, FontStyle italic, unsigned int decoration, FontMetrics* fm) {return 0;}
	virtual void				DeleteFont(uint32 hFont) {}
	virtual int					TextWidth(const String& text, uint32 hFont) {return 0;}
	virtual void				DrawText(Draw& hdc, const String& text, uint32 hFont, WebColor color, const Position& pos) {}
	virtual int					PtPx(int pt) {return 0;}
	virtual int					GetDefaultFontSize() const {return 0;}
	virtual String				GetDefaultFontName() const {return "";}
	virtual void				DrawListMarker(Draw& hdc, const ListMarker& marker) {}
	virtual void				LoadImage(const String& src, const String& baseurl, bool redraw_on_ready) {}
	virtual void				GetImageSize(const String& src, const String& baseurl, Upp::Size& sz) {}
	virtual void				GetImageSize(int cache_id, Size& sz) {}
	virtual void				DrawBackground(Draw& hdc, const BackgroundPaint& bg) {}
	virtual void				DrawBorders(Draw& hdc, const Borders& Borders, const Position& draw_pos, bool root) {}
	
	virtual	void				SetCaption(const String& caption) {}
	virtual	void				SetBaseUrl(const String& base_url) {}
	virtual void				Link(Document* doc, Element::ptr el) {}
	virtual void				OnAnchorClick(const String& url, Element::ptr el) {}
	virtual	void				SetCursor(const String& cursor) {}
	virtual	void				TransformText(String& text, TextTransform tt) {}
	virtual void				ImportCss(String& text, const String& url, String& baseurl) {}
	virtual void				SetClip(const Position& pos, const BorderRadiuses& bdr_radius, bool valid_x, bool valid_y) {}
	virtual void				DelClip() {}
	virtual void				GetClientRect(Position& client) {}
	virtual Element*		CreateElement(const String& tag_name, const StrMap& attributes, Document* doc) {return 0;}
	virtual void				GetMediaFeatures(MediaFeatures& media) {}
	
};

void Trim(String &s);
void LowerCase(String &s);
int	 ValueIndex(const String& val, const String& strings, int defValue = -1, wchar delim = ';');
bool ValueInList(const String& val, const String& strings, wchar delim = ';');
int FindCloseBracket(const String &s, int off, wchar open_b = '(', wchar close_b = ')');
void SplitString(const String& str, StringVector& tokens, const String& delims, const String& delims_preserve = "", const String& quote = "\"");

inline int RoundFlt(float val)
{
	int int_val = (int) val;
	if(val - int_val >= 0.5)
	{
		int_val++;
	}
	return int_val;
}

inline int RoundDbl(double val)
{
	int int_val = (int) val;
	if(val - int_val >= 0.5)
	{
		int_val++;
	}
	return int_val;
}

END_HTMLCTRL_NAMESPACE
