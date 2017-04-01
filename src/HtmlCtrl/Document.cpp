#include "Html.h"
#include "Document.h"
#include "Stylesheet.h"
#include "HtmlTag.h"
#include "ElText.h"
#include "ElPara.h"
#include "ElSpace.h"
#include "ElBody.h"
#include "ElImage.h"
#include "ElTable.h"
#include "ElTd.h"
#include "ElLink.h"
#include "ElTitle.h"
#include "ElStyle.h"
#include "ElScript.h"
#include "ElComment.h"
#include "ElCdata.h"
#include "ElBase.h"
#include "ElAnchor.h"
#include "ElBreak.h"
#include "ElDiv.h"
#include "ElFont.h"
#include "ElTr.h"

#include <Core/Core.h>
using namespace Upp;


#include "Utf8Strings.h"

#include "PerformanceMeter.h"

NAMESPACE_HTMLCTRL

Document::Document(ObjectPool& pool, HtmlFile& file) : m_styles(pool), Object(pool), HtmlNode(file)
{
	
}

Document::~Document()
{
	m_over_element = 0;
	HtmlFile* m_container = GetHtmlFile();
	if(m_container)
	{
		for (int i = 0; i < m_fonts.GetCount(); i++) {
			m_container->DeleteFont(m_fonts[i].font);
		}
		m_container = 0;
	}
	
	
}

HtmlFile* Document::GetHtmlFile() const {
	if (!GetParent()) return 0;
	return dynamic_cast<HtmlFile*>(GetParent());
}

Document::ptr CreateFromString( const String& str, HtmlFile* objPainter, Context* ctx, Css* user_styles) {
	return CreateFromUTF8(str, objPainter, ctx, user_styles);
}

Document::ptr CreateFromUTF8(const String& str, HtmlFile* objPainter, Context* ctx, Css* user_styles) {
	PERFMETER_BEGIN(PERF_CREATEDOC);
	
	// parse Document into GumboOutput
	String utf8_str = str.ToString();
	GumboOutput* output = gumbo_parse(utf8_str.Begin());

	// Create Document
	Document::ptr doc = new Document(*ctx, *objPainter);

	doc->Init(output, ctx, user_styles);

	PERFMETER_END(PERF_CREATEDOC);
	return doc;
}

void Document::Init(GumboOutput* output, Context* ctx, Css* user_styles) {
	ASSERT(ctx);
	PERFMETER_BEGIN(PERF_DOCINIT);
	
	// Create Elements.
	ElementsVector root_elements;
	CreateNode(output->root, root_elements);
	if (!root_elements.IsEmpty()) {
		m_root = LAST(root_elements);
	}
	// Destroy GumboOutput
	gumbo_destroy_output(&kGumboDefaultOptions, output);

	// Let's process created elements tree
	if (m_root)
	{
		GetHtmlFile()->GetMediaFeatures(m_media);
		
		// apply master CSS
		m_root->ApplyStylesheet(ctx->MasterCss());
		
		// parse elements attributes
		m_root->ParseAttributes();
		
		// parse Style sheets linked in Document
		MediaQueryList::ptr media;
		for (CssText::vector::Iterator css = m_css.Begin(); css != m_css.End(); css++)
		{
			if (!css->media.IsEmpty())
			{
				media = MediaQueryList::CreateFromString(css->media, this);
			}
			else
			{
				media = 0;
			}
			m_styles.ParseStylesheet(css->text, css->baseurl, this, media);
		}
		// Sort css Selectors using CSS rules.
		m_styles.SortSelectors();

		// get current media features
		if (!m_media_lists.IsEmpty()) {
			UpdateMediaLists(m_media);
		}

		// Apply parsed Styles.
		m_root->ApplyStylesheet(m_styles);

		// Apply user Styles if any
		if (user_styles) {
			m_root->ApplyStylesheet(*user_styles);
		}

		// Parse applied Styles in the elements
		m_root->ParseStyles();

		// Now the m_tabular_elements is filled with tabular elements.
		// We have to check the tabular elements for missing table elements 
		// and create the anonymous Boxes in visual table layout
		FixTablesLayout();

		// Fanaly Initialize elements
		m_root->Init();
	}
	
	PERFMETER_END(PERF_DOCINIT);
}

uint32 Document::AddFont( String name, int size, const String& weight, const String& Style, const String& decoration, FontMetrics* fm )
{
	PERFMETER_BEGIN(PERF_DOCADDFONT);
	
	uint32 ret = 0;

	HtmlFile* m_container = GetHtmlFile();
	
	if( !name.GetCount() || (name.GetCount() && !CompareNoCase(name, "inherit")) )
	{
		name = m_container->GetDefaultFontName();
	}

	if(!size)
	{
		size = GetHtmlFile()->GetDefaultFontSize();
	}

	String key = name;
	key += ":";
	key += IntStr(size);
	key += ":";
	key += weight;
	key += ":";
	key += Style;
	key += ":";
	key += decoration;

	if(m_fonts.Find(key) == -1)
	{
		FontStyle fs = (FontStyle) ValueIndex(Style, Fontstyle_strings, fontStyleNormal);
		int	fw = ValueIndex(weight, FontWeight_strings, -1);
		if(fw >= 0)
		{
			switch(fw)
			{
			case fontWeightBold:
				fw = 700;
				break;
			case fontWeightBolder:
				fw = 600;
				break;
			case fontWeightLighter:
				fw = 300;
				break;
			default:
				fw = 400;
				break;
			}
		} else
		{
			fw = StrInt(weight);
			if(fw < 100)
			{
				fw = 400;
			}
		}

		unsigned int decor = 0;

		if(decoration.GetCount())
		{
			Vector<String> tokens;
			SplitString(decoration, tokens, " ");
			for(Vector<String>::Iterator i = tokens.Begin(); i != tokens.End(); i++)
			{
				if(!CompareNoCase(*i, "underline"))
				{
					decor |= font_decoration_underline;
				} else if(!CompareNoCase(*i, "line-through"))
				{
					decor |= font_decoration_linethrough;
				} else if(!CompareNoCase(*i,"overline"))
				{
					decor |= font_decoration_overline;
				}
			}
		}

		FontItem fi= {0};
		fi.font = m_container->CreateFont(name, size, fw, fs, decor, &fi.metrics);
		m_fonts.GetAdd(key) = fi;
		ret = fi.font;
		if(fm)
		{
			*fm = fi.metrics;
		}
	}
	
	PERFMETER_END(PERF_DOCADDFONT);
	return ret;
}

uint32 Document::GetFont( String name, int size, const String& weight, const String& Style, const String& decoration, FontMetrics* fm )
{
	PERFMETER_BEGIN(PERF_GETFONT);
	
	if( !name.GetCount() || (name.GetCount() && !CompareNoCase(name, "inherit"))) {
		name = GetHtmlFile()->GetDefaultFontName();
	}

	if(!size)
	{
		size = GetHtmlFile()->GetDefaultFontSize();
	}

	String key = name;
	key += ":";
	key += IntStr(size);
	key += ":";
	key += weight;
	key += ":";
	key += Style;
	key += ":";
	key += decoration;
	
	LOG("Document::GetFont " << key);
	
	int i = m_fonts.Find(key);

	if(i != -1)
	{
		FontItem* el = &m_fonts[i];
		if (fm) {
			*fm = el->metrics;
		}
		return el->font;
	}
	uint32 f = AddFont(name, size, weight, Style, decoration, fm);
	
	PERFMETER_END(PERF_GETFONT);
	return f;
}

int Document::Render( int max_width, RenderType rt )
{
	PERFMETER_BEGIN(PERF_DOCRENDER);
	int ret = 0;
	if(m_root)
	{
		if(rt == render_fixed_only)
		{
			m_fixed_boxes.Clear();
			m_root->RenderPositioned(rt);
		} else
		{
			ret = m_root->Render(0, 0, max_width);
			if(m_root->FetchPositioned())
			{
				m_fixed_boxes.Clear();
				m_root->RenderPositioned(rt);
			}
			m_size.cx	= 0;
			m_size.cy	= 0;
			m_root->CalcDocumentSize(m_size);
		}
	}
	
	PERFMETER_END(PERF_DOCRENDER);
	return ret;
}

void Document::Paint( Draw& hdc, int x, int y, const Position& clip )
{
	PERFMETER_BEGIN(PERF_DOCPAINT);
	if(m_root)
	{
		m_root->Paint(hdc, x, y, clip);
		m_root->DrawStackingContext(hdc, x, y, clip, true);
	}
	PERFMETER_END(PERF_DOCPAINT);
}

int Document::CvtUnits( const String& str, int fontSize, bool* is_percent/*= 0*/ ) const
{
	if(!str.GetCount())	return 0;
	
	CssLength val;
	val.FromString(str);
	if(is_percent && val.Units() == css_units_percentage && !val.IsPredefined())
	{
		*is_percent = true;
	}
	return CvtUnits(val, fontSize);
}

int Document::CvtUnits( CssLength& val, int fontSize, int size ) const
{
	if(val.IsPredefined())
	{
		return 0;
	}
	int ret = 0;
	switch(val.Units())
	{
	case css_units_percentage:
		ret = val.CalcPercent(size);
		break;
	case css_units_em:
		ret = RoundFlt(val.Value() * fontSize);
		val.SetValue((float) ret, css_units_px);
		break;
	case css_units_pt:
		ret = GetHtmlFile()->PtPx((int) val.Value());
		val.SetValue((float) ret, css_units_px);
		break;
	case css_units_in:
		ret = GetHtmlFile()->PtPx((int) (val.Value() * 72));
		val.SetValue((float) ret, css_units_px);
		break;
	case css_units_cm:
		ret = GetHtmlFile()->PtPx((int) (val.Value() * 0.3937 * 72));
		val.SetValue((float) ret, css_units_px);
		break;
	case css_units_mm:
		ret = GetHtmlFile()->PtPx((int) (val.Value() * 0.3937 * 72) / 10);
		val.SetValue((float) ret, css_units_px);
		break;
	case css_units_vw:
		ret = (int)((double)m_media.width * (double)val.Value() / 100.0);
		break;
	case css_units_vh:
		ret = (int)((double)m_media.height * (double)val.Value() / 100.0);
		break;
	case css_units_vmin:
		ret = (int)((double)min(m_media.height, m_media.width) * (double)val.Value() / 100.0);
		break;
	case css_units_vmax:
		ret = (int)((double)max(m_media.height, m_media.width) * (double)val.Value() / 100.0);
		break;
	default:
		ret = (int) val.Value();
		break;
	}
	return ret;
}

int Document::Width() const
{
	return m_size.cx;
}

int Document::Height() const
{
	return m_size.cy;
}

void Document::AddStylesheet( const String& str, const String& baseurl, const String& media )
{
	if(str.GetCount())
	{
		m_css.Add(CssText(str, baseurl, media));
	}
}

bool Document::OnMouseOver( int x, int y, int client_x, int client_y, Position::vector& redraw_boxes, Element** el_sel )
{
	
	if(!m_root)
	{
		return false;
	}
	
	PERFMETER_BEGIN(PERF_MOUSEOVER);
	
	bool ret;
	Element::ptr over_el = m_root->GetElementByPoint(x, y, client_x, client_y);
	if (el_sel)
		*el_sel = over_el;
	
	bool state_was_changed = false;

	if(over_el != m_over_element)
	{
		if(m_over_element)
		{
			if(m_over_element->OnMouseLeave())
			{
				state_was_changed = true;
			}
		}
		m_over_element = over_el;
	}

	String cursor;

	if(m_over_element)
	{
		if(m_over_element->OnMouseOver())
		{
			state_was_changed = true;
		}
		cursor = m_over_element->GetCursor();
	}
	
	GetHtmlFile()->SetCursor(cursor.GetCount() ? cursor : "auto");
	
	if(state_was_changed)
	{
		ret = m_root->FindStylesChanges(redraw_boxes, 0, 0);
	}
	else ret = false;
	
	PERFMETER_END(PERF_MOUSEOVER);
	return ret;
}

bool Document::OnMouseLeave( Position::vector& redraw_boxes )
{
	
	if(!m_root)
	{
		return false;
	}
	
	PERFMETER_BEGIN(PERF_MOUSELEAVE);
	bool ret;
	if(m_over_element &&m_over_element->OnMouseLeave()) {
		ret = m_root->FindStylesChanges(redraw_boxes, 0, 0);
	}
	else ret = false;
	
	PERFMETER_END(PERF_MOUSELEAVE);
	return ret;
}

bool Document::OnMouseLeftDown( int x, int y, int client_x, int client_y, Position::vector& redraw_boxes )
{
	
	if(!m_root)
	{
		return false;
	}

	PERFMETER_BEGIN(PERF_MOUSELEFTDOWN);
	bool ret;
	Element::ptr over_el = m_root->GetElementByPoint(x, y, client_x, client_y);

	bool state_was_changed = false;

	if(over_el != m_over_element)
	{
		if(m_over_element)
		{
			if(m_over_element->OnMouseLeave())
			{
				state_was_changed = true;
			}
		}
		m_over_element = over_el;
		if(m_over_element)
		{
			if(m_over_element->OnMouseOver())
			{
				state_was_changed = true;
			}
		}
	}

	String cursor;

	if(m_over_element)
	{
		if(m_over_element->OnMouseLeftDown())
		{
			state_was_changed = true;
		}
		cursor = m_over_element->GetCursor();
	}

	GetHtmlFile()->SetCursor(cursor.GetCount() ? cursor : "auto");

	if(state_was_changed)
	{
		ret = m_root->FindStylesChanges(redraw_boxes, 0, 0);
	}
	else ret = false;
	
	PERFMETER_END(PERF_MOUSELEFTDOWN);
	return ret;
}

bool Document::OnMouseLeftUp( int x, int y, int client_x, int client_y, Position::vector& redraw_boxes )
{
	if(!m_root)
	{
		return false;
	}
	
	PERFMETER_BEGIN(PERF_MOUSELEFTUP);
	bool ret;
	if(m_over_element&& m_over_element->OnMouseLeftUp()) {
		ret = m_root->FindStylesChanges(redraw_boxes, 0, 0);
	}
	else ret = false;
	PERFMETER_BEGIN(PERF_MOUSELEFTUP);
	return ret;
}

Element::ptr Document::CreateElement(const String& tag_name, const StrMap& attributes)
{
	Element::ptr newTag = 0;
	HtmlFile* m_container = GetHtmlFile();
	if(m_container)
	{
		newTag = m_container->CreateElement(tag_name, attributes, this);
	}
	if(!newTag)
	{
		if(!tag_name.Compare("br"))
		{
			newTag = new ElBreak(*this);
		} else if(!tag_name.Compare("p"))
		{
			newTag = new ElPara(*this);
		} else if(!tag_name.Compare("img"))
		{
			newTag = new ElImage(*this);
		} else if(!tag_name.Compare("table"))
		{
			newTag = new ElTable(*this);
		} else if(!tag_name.Compare("td") || !tag_name.Compare("th"))
		{
			newTag = new ElTd(*this);
		} else if(!tag_name.Compare("link"))
		{
			newTag = new ElLink(*this);
		} else if(!tag_name.Compare("title"))
		{
			newTag = new ElTitle(*this);
		} else if(!tag_name.Compare("a"))
		{
			newTag = new ElAnchor(*this);
		} else if(!tag_name.Compare("tr"))
		{
			newTag = new ElTr(*this);
		} else if(!tag_name.Compare("style"))
		{
			newTag = new ElStyle(*this);
		} else if(!tag_name.Compare("base"))
		{
			newTag = new ElBase(*this);
		} else if(!tag_name.Compare("body"))
		{
			newTag = new ElBody(*this);
		} else if(!tag_name.Compare("div"))
		{
			newTag = new ElDiv(*this);
		} else if(!tag_name.Compare("script"))
		{
			newTag = new ElScript(*this);
		} else if(!tag_name.Compare("font"))
		{
			newTag = new ElFont(*this);
		} else
		{
			newTag = new HtmlTag(*this);
		}
	}

	if(newTag)
	{
		newTag->SetTagName(tag_name);
		for (int i = 0; i < attributes.GetCount(); i++) {
			newTag->SetAttr(attributes.GetKey(i), attributes[i]);
		}
	}

	return newTag;
}

void Document::GetFixedBoxes( Position::vector& fixed_boxes ) {
	fixed_boxes <<= m_fixed_boxes;
}

void Document::AddFixedBox( const Position& pos )
{
	m_fixed_boxes.Add(pos);
}

bool Document::MediaChanged()
{
	if(!m_media_lists.IsEmpty())
	{
		GetHtmlFile()->GetMediaFeatures(m_media);
		if (UpdateMediaLists(m_media))
		{
			m_root->RefreshStyles();
			m_root->ParseStyles();
			return true;
		}
	}
	return false;
}

bool Document::UpdateMediaLists(const MediaFeatures& features)
{
	bool update_styles = false;
	for(MediaQueryList::vector::Iterator iter = m_media_lists.Begin(); iter != m_media_lists.End(); iter++)
	{
		if((*iter)->ApplyMediaFeatures(features))
		{
			update_styles = true;
		}
	}
	return update_styles;
}

void Document::AddMediaList( MediaQueryList::ptr list )
{
	if(list)
	{
		if(Find(m_media_lists.Begin(), m_media_lists.End(), list, StdEqual<MediaQueryList::ptr>()) == m_media_lists.End())
		{
			m_media_lists.Add(list);
		}
	}
}

void Document::CreateNode(GumboNode* node, ElementsVector& elements)
{
	switch (node->type)
	{
	case GUMBO_NODE_ELEMENT:
		{
			StrMap attrs;
			GumboAttribute* attr;
			for (unsigned int i = 0; i < node->v.element.attributes.length; i++)
			{
				attr = (GumboAttribute*)node->v.element.attributes.data[i];
				attrs.GetAdd(litehtml_from_utf8(attr->name)) = litehtml_from_utf8(attr->value);
			}


			Element::ptr ret;
			const char* tag = gumbo_normalized_tagname(node->v.element.tag);
			if (tag[0])
			{
				ret = CreateElement(litehtml_from_utf8(tag), attrs);
			}
			else
			{
				if (node->v.element.original_tag.data && node->v.element.original_tag.length)
				{
					// TODO: remove gumbo, and convert this
					std::string strA;
					strA.append(node->v.element.original_tag.data, node->v.element.original_tag.length);
					ret = CreateElement(litehtml_from_utf8(strA.c_str()), attrs);
				}
			}
			if (ret)
			{
				ElementsVector child;
				for (unsigned int i = 0; i < node->v.element.children.length; i++)
				{
					child.Clear();
					CreateNode(static_cast<GumboNode*> (node->v.element.children.data[i]), child);
					ElementsVector::Iterator it  = child.Begin();
					ElementsVector::Iterator end = child.End();
					for (; it != end; it++) {
						ret->AppendChild(*it);
					}
				}
				child.Clear();
				elements.Add(ret);
			}
		}
		break;
	case GUMBO_NODE_TEXT:
		{
			String str;
			String str_in(node->v.text.text);
			
			ucode_t c;
			for (size_t i = 0; i < str_in.GetCount(); i++)
			{
				c = (ucode_t) str_in[i];
				if (c <= ' ' && (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f'))
				{
					if (!str.IsEmpty())
					{
						elements.Add(new ElText(litehtml_from_wchar(str), *this));
						str.Clear();
					}
					str += c;
					elements.Add(new ElSpace(litehtml_from_wchar(str), *this));
					str.Clear();
				}
				// CJK character range
				else if (c >= 0x4E00 && c <= 0x9FCC)
				{
					if (!str.IsEmpty())
					{
						elements.Add(new ElText(litehtml_from_wchar(str), *this));
						str.Clear();
					}
					str += c;
					elements.Add(new ElText(litehtml_from_wchar(str), *this));
					str.Clear();
				}
				else
				{
					str += c;
				}
			}
			if (!str.IsEmpty()) {
				elements.Add(new ElText(litehtml_from_wchar(str), *this));
			}
		}
		break;
	case GUMBO_NODE_CDATA:
		{
			Element::ptr ret = new ElCdata(*this);
			ret->SetData(litehtml_from_utf8(node->v.text.text));
			elements.Add(ret);
		}
		break;
	case GUMBO_NODE_COMMENT:
		{
			Element::ptr ret = new ElComment(*this);
			ret->SetData(litehtml_from_utf8(node->v.text.text));
			elements.Add(ret);
		}
		break;
	case GUMBO_NODE_WHITESPACE:
		{
			String str = litehtml_from_utf8(node->v.text.text);
			for (size_t i = 0; i < str.GetCount(); i++)
			{
				elements.Add(new ElSpace(str.Mid(i, 1), *this));
			}
		}
		break;
	default:
		break;
	}
}

void Document::FixTablesLayout()
{
	size_t i = 0;
	while (i < m_tabular_elements.GetCount())
	{
		Element::ptr el_ptr = m_tabular_elements[i];

		switch (el_ptr->GetDisplay())
		{
		case display_inline_table:
		case display_table:
			FixTableChildren(el_ptr, display_TableRow_group, "table-row-group");
			break;
		case display_table_footer_group:
		case display_TableRow_group:
		case display_table_header_group:
			FixTableParent(el_ptr, display_table, "table");
			FixTableChildren(el_ptr, display_TableRow, "table-row");
			break;
		case display_TableRow:
			FixTableParent(el_ptr, display_TableRow_group, "table-row-group");
			FixTableChildren(el_ptr, display_TableCell, "table-cell");
			break;
		case display_TableCell:
			FixTableParent(el_ptr, display_TableRow, "table-row");
			break;
		// TODO: make table layout fix for table-caption, table-column etc. elements
		case display_table_caption:
		case display_TableColumn:
		case display_TableColumn_group:
		default:
			break;
		}
		i++;
	}
}

void Document::FixTableChildren(Element::ptr el_ptr, StyleDisplay disp, const String& disp_str)
{
	ElementsVector tmp;
	int first = 0, cur = 0;

	auto flush_elements = [&]()
	{
		Element::ptr annon_tag = new HtmlTag(*this);
		Style::ptr st = new Style(GetObjectPool());
		st->AddProperty("display", disp_str, "", false);
		annon_tag->AddStyle(st);
		annon_tag->MoveTo(*el_ptr);
		annon_tag->ParseStyles();
		
		ElementsVector::Iterator it  = tmp.Begin();
		ElementsVector::Iterator end = tmp.End();
		for (; it != end; it++) {
			annon_tag->AppendChild(*it);
		}
		el_ptr->m_children.Insert(first, annon_tag);
		cur = first + 1;
		ObjectPtr<Element>* cur_iter = &el_ptr->m_children[cur];
		while (cur != el_ptr->m_children.GetCount())
		{
			if ((*cur_iter)->GetParentElement() == el_ptr) break;
			el_ptr->m_children.Remove(cur);
			cur_iter = &el_ptr->m_children[cur];
		}
		first = cur;
		tmp.Clear();
	};

	while (cur != el_ptr->m_children.GetCount())
	{
		ObjectPtr<Element>* cur_iter = &el_ptr->m_children[cur];
		
		if ((*cur_iter)->GetDisplay() != disp) {
			if (!(*cur_iter)->IsWhiteSpace() || ((*cur_iter)->IsWhiteSpace() && !tmp.IsEmpty()))
			{
				if (tmp.IsEmpty())
				{
					first = cur;
				}
				tmp.Add((*cur_iter));
			}
			cur++;
		}
		else if (!tmp.IsEmpty()) {
			flush_elements();
		}
		else {
			cur++;
		}
	}
	if (!tmp.IsEmpty()) {
		flush_elements();
	}
}

void Document::FixTableParent(Element::ptr el_ptr, StyleDisplay disp, const String& disp_str)
{
	Element::ptr higher = el_ptr->GetParentElement();

	if (higher->GetDisplay() != disp) {
		int pos = FindPos(higher->m_children, el_ptr);
		if (pos != -1) {
			StyleDisplay el_disp = el_ptr->GetDisplay();
			
			int first = pos;
			int last = pos;
			int cur = pos;

			// find first element with same display
			while (true) {
				if (cur == 0) break;
				cur--;
				
				ObjectPtr<Element>& el = higher->m_children[cur];
				if (el->IsWhiteSpace() || el->GetDisplay() == el_disp) {
					first = cur;
				}
				else {
					break;
				}
			}

			// find last element with same display
			cur = pos;
			while (true)
			{
				cur++;
				if (cur == higher->m_children.GetCount()) break;

				ObjectPtr<Element>& el = higher->m_children[cur];
				if (el->IsWhiteSpace() || el->GetDisplay() == el_disp) {
					last = cur;
				}
				else {
					break;
				}
			}

			// extract elements with the same display and wrap them with anonymous Object
			Element::ptr annon_tag = new HtmlTag(*this);
			Style::ptr st = new Style(GetObjectPool());
			st->AddProperty("display", disp_str, "", false);
			annon_tag->AddStyle(st);
			annon_tag->MoveTo(*higher);
			annon_tag->ParseStyles();
			for (int j = first; j < last + 1; j++) {
				annon_tag->AppendChild(higher->m_children[j]);
			}
			higher->m_children.Remove(first, last+1-first);
			higher->m_children.Insert(first, annon_tag);
		}
	}
}

END_HTMLCTRL_NAMESPACE
