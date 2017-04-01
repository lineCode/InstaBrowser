#pragma once
#include "Object.h"
#include "Style.h"
#include "Types.h"
#include "Context.h"
#include "Element.h"

#ifndef flagWIN32
 #include <gumbo.h>
#else
 #include <gumbo/gumbo.h>
#endif

#include <HtmlCtrl/Common.h>


NAMESPACE_HTMLCTRL

struct CssText : public Moveable<CssText>
{
	typedef Vector<CssText>	vector;

	String	text;
	String	baseurl;
	String	media;
	
	CssText() {baseurl = ""; baseurl = ""; media = "";}
	CssText(const String& txt) : text(txt) {baseurl	= ""; media	= "";}
	CssText(const String& txt, const String& url) : text(txt), baseurl(url) {media	= "";}
	CssText(const String& txt, const String& url, const String& media_str)
		 : text(txt), baseurl(url), media(media_str) {}

	CssText(const CssText& val)
	{
		text	= val.text;
		baseurl	= val.baseurl;
		media	= val.media;
	}
};

struct StopTags
{
	String	tags;
	String	stop_higher;
};

struct OmmitedEndTags
{
	String	tag;
	String	followed_tags;
};

class HtmlTag;
class HtmlFile;

class Document : public Object, public HtmlNode
{
public:
	typedef ObjectPtr<Document>	ptr;
private:
	Element::ptr						m_root;
	FontsMap							m_fonts;
	CssText::vector						m_css;
	Css									m_styles;
	WebColor							m_DefColor;
	Upp::Size							m_size;
	Position::vector					m_fixed_boxes;
	MediaQueryList::vector				m_media_lists;
	Element::ptr						m_over_element;
	ElementsVector						m_tabular_elements;
	MediaFeatures						m_media;
public:
	Document(ObjectPool& pool, HtmlFile& knot);
	virtual ~Document();

	HtmlFile*						GetHtmlFile() const;
	uint32							GetFont(String name, int size, const String& weight, const String& Style, const String& decoration, FontMetrics* fm);
	int								Render(int max_width, RenderType rt = render_all);
	void							Paint(Draw& hdc, int x, int y, const Position& clip);
	WebColor						GetDefColor()	{ return m_DefColor; }
	int								CvtUnits(const String& str, int fontSize, bool* is_percent = 0) const;
	int								CvtUnits(CssLength& val, int fontSize, int size = 0) const;
	int								Width() const;
	int								Height() const;
	void							AddStylesheet(const String& str, const String& baseurl, const String& media);
	bool							OnMouseOver(int x, int y, int client_x, int client_y, Position::vector& redraw_boxes, Element** el_sel=0);
	bool							OnMouseLeftDown(int x, int y, int client_x, int client_y, Position::vector& redraw_boxes);
	bool							OnMouseLeftUp(int x, int y, int client_x, int client_y, Position::vector& redraw_boxes);
	bool							OnMouseLeave(Position::vector& redraw_boxes);
	Element::ptr					CreateElement(const String& tag_name, const StrMap& attributes);
	Element::ptr					Root();
	void							GetFixedBoxes(Position::vector& fixed_boxes);
	void							AddFixedBox(const Position& pos);
	void							AddMediaList(MediaQueryList::ptr list);
	bool							MediaChanged();
	void							AddTabular(Element::ptr el);

	void Init(GumboOutput* output, Context* ctx, Css* user_styles);
	
	
	virtual Document* GetAsDocument() {return this;}
	
private:
	uint32	AddFont(String name, int size, const String& weight, const String& Style, const String& decoration, FontMetrics* fm);

	void CreateNode(GumboNode* node, ElementsVector& elements);
	bool UpdateMediaLists(const MediaFeatures& features);
	void FixTablesLayout();
	void FixTableChildren(Element::ptr el_ptr, StyleDisplay disp, const String& disp_str);
	void FixTableParent(Element::ptr el_ptr, StyleDisplay disp, const String& disp_str);
};

inline Element::ptr Document::Root()
{
	return m_root;
}
inline void Document::AddTabular(Element::ptr el)
{
	m_tabular_elements.Add(el);
}

Document::ptr CreateFromString(const String& str, HtmlFile* objPainter, Context* ctx, Css* user_styles = 0);
Document::ptr CreateFromUTF8(const String& str, HtmlFile* objPainter, Context* ctx, Css* user_styles = 0);	


END_HTMLCTRL_NAMESPACE
