#include "Html.h"
#include "ElFont.h"

NAMESPACE_HTMLCTRL

ElFont::ElFont( Document& doc ) : HtmlTag(doc)
{

}

ElFont::~ElFont()
{

}

void ElFont::ParseAttributes()
{
	String str = GetAttr("color");
	if(str.GetCount()) {
		m_style.AddProperty("color", str, "", false);
	}

	str = GetAttr("face");
	if(str.GetCount()){
		m_style.AddProperty("font-face", str, "", false);
	}

	str = GetAttr("color");
	if(str.GetCount()){
		
		int sz = StrInt(str);
		if(sz <= 1)
		{
			m_style.AddProperty("font-size", "x-small", "", false);
		} else if(sz >= 6)
		{
			m_style.AddProperty("font-size", "xx-large", "", false);
		} else
		{
			switch(sz)
			{
			case 2:
				m_style.AddProperty("font-size", "small", "", false);
				break;
			case 3:
				m_style.AddProperty("font-size", "medium", "", false);
				break;
			case 4:
				m_style.AddProperty("font-size", "large", "", false);
				break;
			case 5:
				m_style.AddProperty("font-size", "x-large", "", false);
				break;
			}
		}
	}

	HtmlTag::ParseAttributes();
}

END_HTMLCTRL_NAMESPACE
