#include "Html.h"
#include "ElBeforeAfter.h"
#include "ElText.h"
#include "ElSpace.h"
#include "ElImage.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElBeforeAfterBase::ElBeforeAfterBase( Document& doc, bool before ) : HtmlTag(doc)
{
	if(before)
	{
		SetTagName("::before");
	} else
	{
		SetTagName("::after");
	}
}

ElBeforeAfterBase::~ElBeforeAfterBase()
{

}

void ElBeforeAfterBase::AddStyle( Style::ptr st )
{
	HtmlTag::AddStyle(st);

	String content = GetStyleProperty("content", false, "");
	if(!content.IsEmpty())
	{
		int idx = ValueIndex(content, ContentProperty_string);
		if(idx < 0)
		{
			String fnc;
			int i = 0;
			while(i < content.GetCount() && i != -1)
			{
				if(content[i] == '"')
				{
					fnc.Clear();
					i++;
					int pos = content.Find('"', i);
					String txt;
					if(pos == -1)
					{
						txt = content.Mid(i);
						i = -1;
					} else
					{
						txt = content.Mid(i, pos - i);
						i = pos + 1;
					}
					AddText(txt);
				} else if(content[i] == '(')
				{
					i++;
					Trim(fnc);
					LowerCase(fnc);
					int pos = content.Find(')', i);
					String params;
					if(pos == -1)
					{
						params = content.Mid(i);
						i = -1;
					} else
					{
						params = content.Mid(i, pos - i);
						i = pos + 1;
					}
					AddFunction(fnc, params);
					fnc.Clear();
				} else
				{
					fnc += content[i];
					i++;
				}
			}
		}
	}
}

void ElBeforeAfterBase::AddText( const String& txt )
{
	String word;
	String esc;
	for(int i = 0; i < txt.GetCount(); i++)
	{
		if( (txt[i] == ' ') || (txt[i] == '\t') || (txt[i] == '\\' && !esc.IsEmpty()) )
		{
			if(esc.IsEmpty())
			{
				if(!word.IsEmpty())
				{
					Element* el = new ElText(word, *GetDocument());
					AppendChild(el);
					word.Clear();
				}

				Element* el = new ElSpace(txt.Mid(i, 1), *GetDocument());
				AppendChild(el);
			} else
			{
				word += ConvertEscape(esc.Mid(1));
				esc.Clear();
				if(txt[i] == '\\')
				{
					esc += txt[i];
				}
			}
		} else
		{
			if(!esc.IsEmpty() || txt[i] == '\\')
			{
				esc += txt[i];
			} else
			{
				word += txt[i];
			}
		}
	}

	if(!esc.IsEmpty())
	{
		word += ConvertEscape(esc.Mid(1));
	}
	if(!word.IsEmpty())
	{
		Element* el = new ElText(word, *GetDocument());
		AppendChild(el);
		word.Clear();
	}
}

void ElBeforeAfterBase::AddFunction( const String& fnc, const String& params )
{
	int idx = ValueIndex(fnc, "attr;counter;url");
	switch(idx)
	{
	// attr
	case 0:
		{
			String p_name = params;
			Trim(p_name);
			LowerCase(p_name);
			String attr_value = GetAttr(p_name);
			if (attr_value.GetCount())
				AddText(attr_value);
			
		}
		break;
	// counter
	case 1:
		break;
	// url
	case 2:
		{
			String p_url = params;
			Trim(p_url);
			if(!p_url.IsEmpty())
			{
				if(p_url[0] == '\'' || p_url[0] == '\"')
				{
					p_url.Remove(0, 1);
				}
			}
			if(!p_url.IsEmpty())
			{
				if(p_url[p_url.GetCount() - 1] == '\'' || p_url[p_url.GetCount() - 1] == '\"')
				{
					p_url.Remove(p_url.GetCount() - 1, 1);
				}
			}
			if(!p_url.IsEmpty())
			{
				Element* el = new ElImage(*GetDocument());
				el->SetAttr("src", p_url);
				el->SetAttr("style", "display:inline-block");
				el->SetTagName("img");
				AppendChild(el);
				el->ParseAttributes();
			}
		}
		break;
	}
}

wchar ElBeforeAfterBase::ConvertEscape( const String& txt )
{
	return HexInt(txt);
}

void ElBeforeAfterBase::ApplyStylesheet( const Css& Stylesheet )
{

}

END_HTMLCTRL_NAMESPACE
