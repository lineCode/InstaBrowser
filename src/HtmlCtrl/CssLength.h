#pragma once
#include "Types.h"

NAMESPACE_HTMLCTRL

class CssLength
{
	union
	{
		float	m_value;
		int		m_predef;
	};
	CssUnits	m_units;
	bool		m_IsPredefined;
public:
	CssLength();
	CssLength(const CssLength& val);

	CssLength&	operator=(const CssLength& val);
	CssLength&	operator=(float val);
	bool		IsPredefined() const;
	void		PreDefine(int val);
	int			PreDefine() const;
	void		SetValue(float val, CssUnits units);
	float		Value() const;
	CssUnits	Units() const;
	int			CalcPercent(int width) const;
	void		FromString(const String& str, const String& predefs = "", int defValue = 0);
};

// CssLength inlines

inline CssLength::CssLength()
{
	m_value			= 0;
	m_predef		= 0;
	m_units			= css_units_none;
	m_IsPredefined	= false;
}

inline CssLength::CssLength(const CssLength& val)
{
	if(val.IsPredefined())
	{
		m_predef	= val.m_predef;
	} else
	{
		m_value		= val.m_value;
	}
	m_units			= val.m_units;
	m_IsPredefined	= val.m_IsPredefined;
}

inline CssLength&	CssLength::operator=(const CssLength& val)
{
	if(val.IsPredefined())
	{
		m_predef	= val.m_predef;
	} else
	{
		m_value		= val.m_value;
	}
	m_units			= val.m_units;
	m_IsPredefined	= val.m_IsPredefined;
	return *this;
}

inline CssLength&	CssLength::operator=(float val)
{
	m_value = val;
	m_units = css_units_px;
	m_IsPredefined = false;
	return *this;
}

inline bool CssLength::IsPredefined() const
{ 
	return m_IsPredefined;					
}

inline void CssLength::PreDefine(int val)		
{ 
	m_predef		= val; 
	m_IsPredefined = true;	
}

inline int CssLength::PreDefine() const
{ 
	if(m_IsPredefined)
	{
		return m_predef; 
	}
	return 0;
}

inline void CssLength::SetValue(float val, CssUnits units)		
{ 
	m_value			= val; 
	m_IsPredefined = false;	
	m_units			= units;
}

inline float CssLength::Value() const
{
	if(!m_IsPredefined)
	{
		return m_value;
	}
	return 0;
}

inline CssUnits CssLength::Units() const
{
	return m_units;
}

inline int CssLength::CalcPercent(int width) const
{
	if(!IsPredefined())
	{
		if(Units() == css_units_percentage)
		{
			return (int) ((double) width * (double) m_value / 100.0);
		} else
		{
			return (int) Value();
		}
	}
	return 0;
}

END_HTMLCTRL_NAMESPACE
