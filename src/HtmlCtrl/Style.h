#pragma once
#include "Attributes.h"
#include <Core/Core.h>
using namespace Upp;

NAMESPACE_HTMLCTRL

class PropertyValue
{
public:
	String	m_value;
	bool			m_important;

	PropertyValue()
	{
		m_important = false;
	}
	PropertyValue(const String& val, bool imp)
	{
		m_important = imp;
		m_value		= val;
	}
	PropertyValue(const PropertyValue& val)
	{
		m_value		= val.m_value;
		m_important	= val.m_important;
	}

	PropertyValue& operator=(const PropertyValue& val)
	{
		m_value		= val.m_value;
		m_important	= val.m_important;
		return *this;
	}
};

typedef ArrayMap<String, PropertyValue>	PropsMap;

class Style : public Object
{
public:
	typedef ObjectPtr<Style>			ptr;
	typedef Vector<Style::ptr>		vector;
private:
	PropsMap			m_properties;
	static StrMap	m_valid_values;
	
	void __CheckStyleDefaultValues();
public:
	Style(ObjectPool& pool);
	Style(const Style& val);
	virtual ~Style();

	void operator=(const Style& val);

	void add(const String& txt, const String& baseurl)
	{
		Parse(txt, baseurl);
	}

	void AddProperty(const String& name, const String& val, const String& baseurl, bool important);
	
	String GetProperty(const String& name) const
	{
		int f = m_properties.Find(name);
		if (f != -1) {
			return m_properties[f].m_value;
		}
		else return "";
	}

	void Combine(const Style& src);
	void Clear()
	{
		m_properties.Clear();
	}
	
	int GetCount() {return m_properties.GetCount();}
	String GetKey(int i) {return m_properties.GetKey(i);}
	String GetValue(int i) {return m_properties[i].m_value;}
	

private:
	void ParseProperty(const String& txt, const String& baseurl);
	void Parse(const String& txt, const String& baseurl);
	void ParseShortBorder(const String& prefix, const String& val, bool important);
	void ParseShortBackground(const String& val, const String& baseurl, bool important);
	void ParseShortFont(const String& val, bool important);
	void AddParsedProperty(const String& name, const String& val, bool important);
	void RemoveProperty(const String& name, bool important);
};

/*
	class used_styles
	{
	public:
		typedef Vector<used_styles>	vector;

		style_sheet::ptr	m_style_sheet;
		bool				m_used;

		used_styles()
		{
			m_used = false;
		}

		used_styles(style_sheet::ptr sh, bool used)
		{
			m_used			= used;
			m_style_sheet	= sh;
		}

		used_styles(const used_styles& val)
		{
			m_style_sheet	= val.m_style_sheet;
			m_used			= val.m_used;
		}
	};
*/

END_HTMLCTRL_NAMESPACE
