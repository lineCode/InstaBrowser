#include "Html.h"
#include "Style.h"

#include <Core/Core.h>
using namespace Upp;

NAMESPACE_HTMLCTRL

StrMap Style::m_valid_values;

/* =
{
	{ _t("white-space"), white_space_strings }
};*/

void Style::__CheckStyleDefaultValues() {
	if (m_valid_values.GetCount() == 0) {
		m_valid_values.Add("white-space", white_space_strings);
	}
}

Style::Style(ObjectPool& pool) : Object(pool) {
	__CheckStyleDefaultValues();
}

Style::Style( const Style& val ) : Object(val.GetObjectPool())
{
	__CheckStyleDefaultValues();
	
	m_properties <<= val.m_properties;
}

Style::~Style()
{

}

void Style::operator=(const Style& val) {
	//LOG("void operator=(const Style& val) {");
	m_properties <<= val.m_properties;
}


void Style::Parse( const String& txt, const String& baseurl )
{
	Vector<String> properties;
	SplitString(txt, properties, ";");

	for(Vector<String>::Iterator i = properties.Begin(); i != properties.End(); i++)
	{
		ParseProperty(*i, baseurl);
	}
}

void Style::ParseProperty( const String& txt, const String& baseurl )
{
	int pos = txt.FindFirstOf(":");
	if(pos != -1)
	{
		String name	= txt.Mid(0, pos);
		String val		= txt.Mid(pos + 1);

		Trim(name);
		Trim(val);

		LowerCase(name);

		if(!name.IsEmpty() && !val.IsEmpty())
		{
			StringVector vals;
			SplitString(val, vals, "!");
			if(vals.GetCount() == 1)
			{
				AddProperty(name, val, baseurl, false);
			} else if(vals.GetCount() > 1)
			{
				Trim(vals[0]);
				LowerCase(vals[1]);
				if(vals[1] == "important")
				{
					AddProperty(name, vals[0], baseurl, true);
				} else
				{
					AddProperty(name, vals[0], baseurl, false);
				}
			}
		}
	}
}

void Style::Combine( const Style& src )
{
	for (int i = 0; i < src.m_properties.GetCount(); i++)
	{
		String str = src.m_properties.GetKey(i);
		PropertyValue pv = src.m_properties[i];
		AddParsedProperty(str, pv.m_value, pv.m_important);
	}
}

void Style::AddProperty( const String& name, const String& val, const String& baseurl, bool important )
{
	//LOG("Style::AddProperty: " << name << " = " << val);
	if(!name.GetCount() || !val.GetCount())
	{
		return;
	}

	// Add baseurl for background image 
	if(	!name.Compare("background-image"))
	{
		AddParsedProperty(name, val, important);
		if(baseurl.GetCount())
		{
			AddParsedProperty("background-image-baseurl", baseurl, important);
		}
	} else

	// Parse Border spacing properties 
	if(	!name.Compare("border-spacing"))
	{
		StringVector tokens;
		SplitString(val, tokens, " ");
		if(tokens.GetCount() == 1)
		{
			AddProperty("-litehtml-border-spacing-x", tokens[0], baseurl, important);
			AddProperty("-litehtml-border-spacing-y", tokens[0], baseurl, important);
		} else if(tokens.GetCount() == 2)
		{
			AddProperty("-litehtml-border-spacing-x", tokens[0], baseurl, important);
			AddProperty("-litehtml-border-spacing-y", tokens[1], baseurl, important);
		}
	} else

	// Parse Borders shorthand properties 

	if(	!name.Compare("border"))
	{
		StringVector tokens;
		SplitString(val, tokens, " ", "", "(");
		int idx;
		String str;
		for(StringVector::Iterator tok = tokens.Begin(); tok != tokens.End(); tok++)
		{
			idx = ValueIndex(*tok, border_style_strings, -1);
			if(idx >= 0)
			{
				AddProperty("border-left-style", *tok, baseurl, important);
				AddProperty("border-right-style", *tok, baseurl, important);
				AddProperty("border-top-style", *tok, baseurl, important);
				AddProperty("border-bottom-style", *tok, baseurl, important);
			} else
			{
				if(WebColor::IsColor(*tok))
				{
					AddProperty("border-left-color", *tok, baseurl, important);
					AddProperty("border-right-color", *tok, baseurl, important);
					AddProperty("border-top-color", *tok, baseurl, important);
					AddProperty("border-bottom-color", *tok, baseurl, important);
				} else
				{
					AddProperty("border-left-width", *tok, baseurl, important);
					AddProperty("border-right-width", *tok, baseurl, important);
					AddProperty("border-top-width", *tok, baseurl, important);
					AddProperty("border-bottom-width", *tok, baseurl, important);
				}
			}
		}
	} else if(	!name.Compare("border-left")	||
		!name.Compare("border-right")	||
		!name.Compare("border-top")	||
		!name.Compare("border-bottom") )
	{
		StringVector tokens;
		SplitString(val, tokens, " ", "", "(");
		int idx;
		String str;
		for(StringVector::Iterator tok = tokens.Begin(); tok != tokens.End(); tok++)
		{
			idx = ValueIndex(*tok, border_style_strings, -1);
			if(idx >= 0)
			{
				str = name;
				str += "-style";
				AddProperty(str, *tok, baseurl, important);
			} else
			{
				if(WebColor::IsColor(*tok))
				{
					str = name;
					str += "-color";
					AddProperty(str, *tok, baseurl, important);
				} else
				{
					str = name;
					str += "-width";
					AddProperty(str, *tok, baseurl, important);
				}
			}
		}
	} else 

	// Parse Border radius shorthand properties 
	if(!name.Compare("border-bottom-left-radius"))
	{
		StringVector tokens;
		SplitString(val, tokens, " ");
		if(tokens.GetCount() >= 2)
		{
			AddProperty("border-bottom-left-radius-x", tokens[0], baseurl, important);
			AddProperty("border-bottom-left-radius-y", tokens[1], baseurl, important);
		} else if(tokens.GetCount() == 1)
		{
			AddProperty("border-bottom-left-radius-x", tokens[0], baseurl, important);
			AddProperty("border-bottom-left-radius-y", tokens[0], baseurl, important);
		}

	} else if(!name.Compare("border-bottom-right-radius"))
	{
		StringVector tokens;
		SplitString(val, tokens, " ");
		if(tokens.GetCount() >= 2)
		{
			AddProperty("border-bottom-right-radius-x", tokens[0], baseurl, important);
			AddProperty("border-bottom-right-radius-y", tokens[1], baseurl, important);
		} else if(tokens.GetCount() == 1)
		{
			AddProperty("border-bottom-right-radius-x", tokens[0], baseurl, important);
			AddProperty("border-bottom-right-radius-y", tokens[0], baseurl, important);
		}

	} else if(!name.Compare("border-top-right-radius"))
	{
		StringVector tokens;
		SplitString(val, tokens, " ");
		if(tokens.GetCount() >= 2)
		{
			AddProperty("border-top-right-radius-x", tokens[0], baseurl, important);
			AddProperty("border-top-right-radius-y", tokens[1], baseurl, important);
		} else if(tokens.GetCount() == 1)
		{
			AddProperty("border-top-right-radius-x", tokens[0], baseurl, important);
			AddProperty("border-top-right-radius-y", tokens[0], baseurl, important);
		}

	} else if(!name.Compare("border-top-left-radius"))
	{
		StringVector tokens;
		SplitString(val, tokens, " ");
		if(tokens.GetCount() >= 2)
		{
			AddProperty("border-top-left-radius-x", tokens[0], baseurl, important);
			AddProperty("border-top-left-radius-y", tokens[1], baseurl, important);
		} else if(tokens.GetCount() == 1)
		{
			AddProperty("border-top-left-radius-x", tokens[0], baseurl, important);
			AddProperty("border-top-left-radius-y", tokens[0], baseurl, important);
		}

	} else 

	// Parse border-radius shorthand properties 
	if(!name.Compare("border-radius"))
	{
		StringVector tokens;
		SplitString(val, tokens, "/");
		if(tokens.GetCount() == 1)
		{
			AddProperty("border-radius-x", tokens[0], baseurl, important);
			AddProperty("border-radius-y", tokens[0], baseurl, important);
		} else if(tokens.GetCount() >= 2)
		{
			AddProperty("border-radius-x", tokens[0], baseurl, important);
			AddProperty("border-radius-y", tokens[1], baseurl, important);
		}
	} else if(!name.Compare("border-radius-x"))
	{
		StringVector tokens;
		SplitString(val, tokens, " ");
		if(tokens.GetCount() == 1)
		{
			AddProperty("border-top-left-radius-x",		tokens[0], baseurl, important);
			AddProperty("border-top-right-radius-x",		tokens[0], baseurl, important);
			AddProperty("border-bottom-right-radius-x",	tokens[0], baseurl, important);
			AddProperty("border-bottom-left-radius-x",	tokens[0], baseurl, important);
		} else if(tokens.GetCount() == 2)
		{
			AddProperty("border-top-left-radius-x",		tokens[0], baseurl, important);
			AddProperty("border-top-right-radius-x",		tokens[1], baseurl, important);
			AddProperty("border-bottom-right-radius-x",	tokens[0], baseurl, important);
			AddProperty("border-bottom-left-radius-x",	tokens[1], baseurl, important);
		} else if(tokens.GetCount() == 3)
		{
			AddProperty("border-top-left-radius-x",		tokens[0], baseurl, important);
			AddProperty("border-top-right-radius-x",		tokens[1], baseurl, important);
			AddProperty("border-bottom-right-radius-x",	tokens[2], baseurl, important);
			AddProperty("border-bottom-left-radius-x",	tokens[1], baseurl, important);
		} else if(tokens.GetCount() == 4)
		{
			AddProperty("border-top-left-radius-x",		tokens[0], baseurl, important);
			AddProperty("border-top-right-radius-x",		tokens[1], baseurl, important);
			AddProperty("border-bottom-right-radius-x",	tokens[2], baseurl, important);
			AddProperty("border-bottom-left-radius-x",	tokens[3], baseurl, important);
		}
	} else if(!name.Compare("border-radius-y"))
	{
		StringVector tokens;
		SplitString(val, tokens, " ");
		if(tokens.GetCount() == 1)
		{
			AddProperty("border-top-left-radius-y",		tokens[0], baseurl, important);
			AddProperty("border-top-right-radius-y",		tokens[0], baseurl, important);
			AddProperty("border-bottom-right-radius-y",	tokens[0], baseurl, important);
			AddProperty("border-bottom-left-radius-y",	tokens[0], baseurl, important);
		} else if(tokens.GetCount() == 2)
		{
			AddProperty("border-top-left-radius-y",		tokens[0], baseurl, important);
			AddProperty("border-top-right-radius-y",		tokens[1], baseurl, important);
			AddProperty("border-bottom-right-radius-y",	tokens[0], baseurl, important);
			AddProperty("border-bottom-left-radius-y",	tokens[1], baseurl, important);
		} else if(tokens.GetCount() == 3)
		{
			AddProperty("border-top-left-radius-y",		tokens[0], baseurl, important);
			AddProperty("border-top-right-radius-y",		tokens[1], baseurl, important);
			AddProperty("border-bottom-right-radius-y",	tokens[2], baseurl, important);
			AddProperty("border-bottom-left-radius-y",	tokens[1], baseurl, important);
		} else if(tokens.GetCount() == 4)
		{
			AddProperty("border-top-left-radius-y",		tokens[0], baseurl, important);
			AddProperty("border-top-right-radius-y",		tokens[1], baseurl, important);
			AddProperty("border-bottom-right-radius-y",	tokens[2], baseurl, important);
			AddProperty("border-bottom-left-radius-y",	tokens[3], baseurl, important);
		}
	}
	

	// Parse list-style shorthand properties 
	if(!name.Compare("list-style"))
	{
		AddParsedProperty("list-style-type",			"disc",		important);
		AddParsedProperty("list-style-position",		"outside",	important);
		AddParsedProperty("list-style-image",			"",			important);
		AddParsedProperty("list-style-image-baseurl",	"",			important);

		StringVector tokens;
		SplitString(val, tokens, " ", "", "(");
		for(StringVector::Iterator tok = tokens.Begin(); tok != tokens.End(); tok++)
		{
			int idx = ValueIndex(*tok, list_style_type_strings, -1);
			if(idx >= 0)
			{
				AddParsedProperty("list-style-type", *tok, important);
			} else
			{
				idx = ValueIndex(*tok, ListStyleposition_strings, -1);
				if(idx >= 0)
				{
					AddParsedProperty("list-style-position", *tok, important);
				} else if(!val.Compare("url"))
				{
					AddParsedProperty("list-style-image", *tok, important);
					if(baseurl.GetCount())
					{
						AddParsedProperty("list-style-image-baseurl", baseurl, important);
					}
				}
			}
		}
	} else 

	// Add baseurl for background image 
	if(	!name.Compare("list-style-image"))
	{
		AddParsedProperty(name, val, important);
		if(baseurl.GetCount())
		{
			AddParsedProperty("list-style-image-baseurl", baseurl, important);
		}
	} else
		
	// Parse background shorthand properties 
	if(!name.Compare("background"))
	{
		ParseShortBackground(val, baseurl, important);

	} else 
		
	// Parse margin and padding shorthand properties 
	if(!name.Compare("margin") || !name.Compare("padding"))
	{
		StringVector tokens;
		SplitString(val, tokens, " ");
		if(tokens.GetCount() >= 4)
		{
			AddParsedProperty(name + "-top",		tokens[0], important);
			AddParsedProperty(name + "-right",		tokens[1], important);
			AddParsedProperty(name + "-bottom",	tokens[2], important);
			AddParsedProperty(name + "-left",		tokens[3], important);
		} else if(tokens.GetCount() == 3)
		{
			AddParsedProperty(name + "-top",		tokens[0], important);
			AddParsedProperty(name + "-right",		tokens[1], important);
			AddParsedProperty(name + "-left",		tokens[1], important);
			AddParsedProperty(name + "-bottom",	tokens[2], important);
		} else if(tokens.GetCount() == 2)
		{
			AddParsedProperty(name + "-top",		tokens[0], important);
			AddParsedProperty(name + "-bottom",	tokens[0], important);
			AddParsedProperty(name + "-right",		tokens[1], important);
			AddParsedProperty(name + "-left",		tokens[1], important);
		} else if(tokens.GetCount() == 1)
		{
			AddParsedProperty(name + "-top",		tokens[0], important);
			AddParsedProperty(name + "-bottom",	tokens[0], important);
			AddParsedProperty(name + "-right",		tokens[0], important);
			AddParsedProperty(name + "-left",		tokens[0], important);
		}
	} else 
		
		
	// Parse border-* shorthand properties 
	if(	!name.Compare("border-left") || 
		!name.Compare("border-right") ||
		!name.Compare("border-top")  || 
		!name.Compare("border-bottom"))
	{
		ParseShortBorder(name, val, important);
	} else 
		
	// Parse border-width/Style/color shorthand properties 
	if(	!name.Compare("border-width") ||
		!name.Compare("border-style")  ||
		!name.Compare("border-color"))
	{
		StringVector nametokens;
		SplitString(name, nametokens, "-");

		StringVector tokens;
		SplitString(val, tokens, " ");
		if(tokens.GetCount() >= 4)
		{
			AddParsedProperty(nametokens[0] + "-top-"		+ nametokens[1],	tokens[0], important);
			AddParsedProperty(nametokens[0] + "-right-"	+ nametokens[1],	tokens[1], important);
			AddParsedProperty(nametokens[0] + "-bottom-"	+ nametokens[1],	tokens[2], important);
			AddParsedProperty(nametokens[0] + "-left-"	+ nametokens[1],	tokens[3], important);
		} else if(tokens.GetCount() == 3)
		{
			AddParsedProperty(nametokens[0] + "-top-"		+ nametokens[1],	tokens[0], important);
			AddParsedProperty(nametokens[0] + "-right-"	+ nametokens[1],	tokens[1], important);
			AddParsedProperty(nametokens[0] + "-left-"	+ nametokens[1],	tokens[1], important);
			AddParsedProperty(nametokens[0] + "-bottom-"	+ nametokens[1],	tokens[2], important);
		} else if(tokens.GetCount() == 2)
		{
			AddParsedProperty(nametokens[0] + "-top-"		+ nametokens[1],	tokens[0], important);
			AddParsedProperty(nametokens[0] + "-bottom-"	+ nametokens[1],	tokens[0], important);
			AddParsedProperty(nametokens[0] + "-right-"	+ nametokens[1],	tokens[1], important);
			AddParsedProperty(nametokens[0] + "-left-"	+ nametokens[1],	tokens[1], important);
		} else if(tokens.GetCount() == 1)
		{
			AddParsedProperty(nametokens[0] + "-top-"		+ nametokens[1],	tokens[0], important);
			AddParsedProperty(nametokens[0] + "-bottom-"	+ nametokens[1],	tokens[0], important);
			AddParsedProperty(nametokens[0] + "-right-"	+ nametokens[1],	tokens[0], important);
			AddParsedProperty(nametokens[0] + "-left-"	+ nametokens[1],	tokens[0], important);
		}
	} else 
		
	// Parse font shorthand properties 
	if(!name.Compare("font"))
	{
		ParseShortFont(val, important);
	} else 
	{
		AddParsedProperty(name, val, important);
	}
}

void Style::ParseShortBorder( const String& prefix, const String& val, bool important )
{
	StringVector tokens;
	SplitString(val, tokens, " ", "", "(");
	if(tokens.GetCount() >= 3)
	{
		AddParsedProperty(prefix + "-width",	tokens[0], important);
		AddParsedProperty(prefix + "-style",	tokens[1], important);
		AddParsedProperty(prefix + "-color",	tokens[2], important);
	} else if(tokens.GetCount() == 2)
	{
		if(iswdigit(tokens[0][0]) || ValueIndex(val, border_width_strings) >= 0)
		{
			AddParsedProperty(prefix + "-width",	tokens[0], important);
			AddParsedProperty(prefix + "-style",	tokens[1], important);
		} else
		{
			AddParsedProperty(prefix + "-style",	tokens[0], important);
			AddParsedProperty(prefix + "-color",	tokens[1], important);
		}
	}
}

void Style::ParseShortBackground( const String& val, const String& baseurl, bool important )
{
	AddParsedProperty("background-color",			"transhigher",	important);
	AddParsedProperty("background-image",			"",				important);
	AddParsedProperty("background-image-baseurl", "",				important);
	AddParsedProperty("background-repeat",		"repeat",		important);
	AddParsedProperty("background-origin",		"padding-box",	important);
	AddParsedProperty("background-clip",			"border-box",	important);
	AddParsedProperty("background-attachment",	"scroll",		important);

	if(val == "none")
	{
		return;
	}

	StringVector tokens;
	SplitString(val, tokens, " ", "", "(");
	bool origin_found = false;
	for(StringVector::Iterator tok = tokens.Begin(); tok != tokens.End(); tok++)
	{
		if(WebColor::IsColor(*tok))
		{
			AddParsedProperty("background-color", *tok, important);
		} 
		else if(tok->Mid(0, 3) == "url")
		{
			AddParsedProperty("background-image", *tok, important);
			//if(baseurl)
			if(baseurl.GetCount())
			{
				AddParsedProperty("background-image-baseurl", baseurl, important);
			}

		} else if( ValueInList(*tok, background_repeat_strings) )
		{
			AddParsedProperty("background-repeat", *tok, important);
		} else if( ValueInList(*tok, BackgroundAttachment_strings) )
		{
			AddParsedProperty("background-attachment", *tok, important);
		} else if( ValueInList(*tok, background_box_strings) )
		{
			if(!origin_found)
			{
				AddParsedProperty("background-origin", *tok, important);
				origin_found = true;
			} else
			{
				AddParsedProperty("background-clip",*tok, important);
			}
		} else if(	ValueInList(*tok, "left;right;top;bottom;center") ||
					iswdigit((*tok)[0]) ||
					(*tok)[0] == '-'	||
					(*tok)[0] == '.'	||
					(*tok)[0] == '+')
		{
			if(m_properties.Find("background-position") != -1)
			{
				m_properties.Get("background-position").m_value = 
					m_properties.Get("background-position").m_value + " " + *tok;
			} else
			{
				AddParsedProperty("background-position", *tok, important);
			}
		}
	}
}

void Style::ParseShortFont( const String& val, bool important )
{
	AddParsedProperty("font-style",	"normal",	important);
	AddParsedProperty("font-variant",	"normal",	important);
	AddParsedProperty("font-weight",	"normal",	important);
	AddParsedProperty("font-size",		"medium",	important);
	AddParsedProperty("line-height",	"normal",	important);

	StringVector tokens;
	SplitString(val, tokens, " ", "", "\"");

	int idx = 0;
	bool was_normal = false;
	bool is_family = false;
	String font_family;
	for(StringVector::Iterator tok = tokens.Begin(); tok != tokens.End(); tok++)
	{
		idx = ValueIndex(*tok, Fontstyle_strings);
		if(!is_family)
		{
			if(idx >= 0)
			{
				if(idx == 0 && !was_normal)
				{
					AddParsedProperty("font-weight",		*tok, important);
					AddParsedProperty("font-variant",		*tok, important);
					AddParsedProperty("font-style",		*tok, important);
				} else
				{
					AddParsedProperty("font-style",		*tok, important);
				}
			} else
			{
				if(ValueInList(*tok, FontWeight_strings))
				{
					AddParsedProperty("font-weight",		*tok, important);
				} else
				{
					if(ValueInList(*tok, FontVariant_strings))
					{
						AddParsedProperty("font-variant",	*tok, important);
					} else if( iswdigit((*tok)[0]) )
					{
						StringVector szlh;
						SplitString(*tok, szlh, "/");

						if(szlh.GetCount() == 1)
						{
							AddParsedProperty("font-size",	szlh[0], important);
						} else	if(szlh.GetCount() >= 2)
						{
							AddParsedProperty("font-size",	szlh[0], important);
							AddParsedProperty("line-height",	szlh[1], important);
						}
					} else
					{
						is_family = true;
						font_family += *tok;
					}
				}
			}
		} else
		{
			font_family += *tok;
		}
	}
	AddParsedProperty("font-family", font_family, important);
}

void Style::AddParsedProperty( const String& name, const String& val, bool important )
{
	bool is_valid = true;
	int i = m_valid_values.Find(name);
	
	if (i != -1)
	{
		if (!ValueInList(val, m_valid_values[i]))
		{
			is_valid = false;
		}
	}

	if (is_valid)
	{
		i = m_properties.Find(name);
		if (i != -1)
		{
			PropertyValue& prop = m_properties[i];
			if (!prop.m_important || (important && prop.m_important))
			{
				prop.m_value = val;
				prop.m_important = important;
			}
		}
		else
		{
			m_properties.GetAdd(name) = PropertyValue(val, important);
		}
	}
}

void Style::RemoveProperty( const String& name, bool important )
{
	int i = m_properties.Find(name);
	if (i != -1)
	{
		PropertyValue& prop = m_properties[i];
		if (!prop.m_important || (important && prop.m_important))
		{
			m_properties.Remove(i);
		}
	}
}

END_HTMLCTRL_NAMESPACE
