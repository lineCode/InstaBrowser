#include "Html.h"
#include "CssLength.h"

NAMESPACE_HTMLCTRL

void CssLength::FromString( const String& str, const String& predefs, int defValue )
{
	
	// TODO: Make support for calc
	if(str.Mid(0, 4) == "calc")
	{
		m_IsPredefined = true;
		m_predef		= 0;
		return;
	}

	int predef = ValueIndex(str, predefs, -1);
	if(predef >= 0)
	{
		m_IsPredefined = true;
		m_predef		= predef;
	} else
	{
		m_IsPredefined = false;

		String num;
		String un;
		bool is_unit = false;
		//for(String::Iterator chr = str.Begin(); chr != str.End(); chr++) {
		for (int i = 0; i < str.GetCount(); i++) {
			int chr = str[i];
			if(!is_unit)
			{
				if(IsDigit(chr) || chr == '.' || chr == '+' || chr == '-')
				{
					num += chr;
				} else
				{
					is_unit = true;
				}
			}
			if(is_unit)
			{
				un += chr;
			}
		}
		if(!num.IsEmpty())
		{
			m_value = StrDbl(num);
			m_units	= (CssUnits) ValueIndex(un, css_units_strings, css_units_none);
		} else
		{
			// not a number so it is predefined
			m_IsPredefined = true;
			m_predef = defValue;
		}
	}
}

END_HTMLCTRL_NAMESPACE
