#include "Html.h"
#include "CssSelector.h"
#include "Document.h"
#include "ElBody.h"
#include "PerformanceMeter.h"

NAMESPACE_HTMLCTRL

CssElementSelector::CssElementSelector(const CssElementSelector& val)
{
	m_tag	= val.m_tag;
	m_attrs	<<= val.m_attrs;
}


CssElementSelector& CssElementSelector::operator = (const CssElementSelector& es) {
	m_tag = es.m_tag; 
	m_attrs <<= es.m_attrs; 
	return *this;
}
	
void CssElementSelector::Parse( const String& txt )
{
	PERFMETER_BEGIN(PERF_CSSELEMENTPARSE);
	
	int el_end = FindFirstOf(txt, ".#[:");
	if (el_end >= 0)
		m_tag = txt.Mid(0, el_end);
	else
		m_tag = txt;
	LowerCase(m_tag);
	while(el_end != -1)
	{
		if(txt[el_end] == '.')
		{
			CssAttributeSelector attribute;

			int pos = FindFirstOf(txt, ".#[:", el_end + 1);
			if (pos != -1)
				attribute.val		= txt.Mid(el_end + 1, pos - el_end - 1);
			else
				attribute.val		= txt.Mid(el_end + 1);
			attribute.condition	= select_equal;
			attribute.attribute	= "class";
			m_attrs.Add(attribute);
			el_end = pos;
		} else if(txt[el_end] == ':')
		{
			CssAttributeSelector attribute;

			if(txt[el_end + 1] == ':')
			{
				int pos = FindFirstOf(txt, ".#[:", el_end + 2);
				if (pos != -1)
					attribute.val		= txt.Mid(el_end + 2, pos - el_end - 2);
				else
					attribute.val		= txt.Mid(el_end + 2);
				
				attribute.condition	= select_pseudo_element;
				LowerCase(attribute.val);
				attribute.attribute	= "pseudo-el";
				m_attrs.Add(attribute);
				el_end = pos;
			} else
			{
				int pos = FindFirstOf(txt, ".#[:(", el_end + 1);
				if(pos != -1 && txt[pos] == '(')
				{
					pos = FindCloseBracket(txt, pos);
					if(pos != -1)
					{
						pos++;
					} else
					{
						int iii = 0;
						iii++;
					}
				}
				if(pos != -1)
				{
					attribute.val		= txt.Mid(el_end + 1, pos - el_end - 1);
				} else
				{
					attribute.val		= txt.Mid(el_end + 1);
				}
				LowerCase(attribute.val);
				if(attribute.val == "after" || attribute.val == "before")
				{
					attribute.condition	= select_pseudo_element;
				} else
				{
					attribute.condition	= select_pseudo_class;
				}
				attribute.attribute	= "pseudo";
				m_attrs.Add(attribute);
				el_end = pos;
			}
		} else if(txt[el_end] == '#')
		{
			CssAttributeSelector attribute;

			int pos = FindFirstOf(txt, ".#[:", el_end + 1);
			if (pos == -1)		attribute.val 		= txt.Mid(el_end + 1);
			else				attribute.val		= txt.Mid(el_end + 1, pos - el_end - 1);
			attribute.condition	= select_equal;
			attribute.attribute	= "id";
			m_attrs.Add(attribute);
			el_end = pos;
		} else if(txt[el_end] == '[')
		{
			CssAttributeSelector attribute;

			int pos = FindFirstOf(txt, "]~=|$*^", el_end + 1);
			String attr;
			if (pos == -1)		attr = txt.Mid(el_end + 1);
			else				attr = txt.Mid(el_end + 1, pos - el_end - 1);
			Trim(attr);
			LowerCase(attr);
			if(pos != -1)
			{
				if(txt[pos] == ']')
				{
					attribute.condition = select_exists;
				} else if(txt[pos] == '=')
				{
					attribute.condition = select_equal;
					pos++;
				} else if(txt.Mid(pos, 2) == "~=")
				{
					attribute.condition = select_contain_str;
					pos += 2;
				} else if(txt.Mid(pos, 2) == "|=")
				{
					attribute.condition = select_start_str;
					pos += 2;
				} else if(txt.Mid(pos, 2) == "^=")
				{
					attribute.condition = select_start_str;
					pos += 2;
				} else if(txt.Mid(pos, 2) == "$=")
				{
					attribute.condition = select_end_str;
					pos += 2;
				} else if(txt.Mid(pos, 2) == "*=")
				{
					attribute.condition = select_contain_str;
					pos += 2;
				} else
				{
					attribute.condition = select_exists;
					pos += 1;
				}
				//pos = txt.find_first_not_of(" \t"), pos);
				pos = FindFirstNotOf(txt, " \t", pos);
				if(pos != -1)
				{
					if(txt[pos] == '"')
					{
						int pos2 = FindFirstOf(txt, '\"', pos + 1);
						attribute.val = txt.Mid(pos + 1, pos2 == -1 ? pos2 : (pos2 - pos - 1));
						pos = pos2 == -1 ? pos2 : (pos2 + 1);
					} else if(txt[pos] == ']')
					{
						pos ++;
					} else
					{
						int pos2 = FindFirstOf(txt, "]", pos + 1);
						attribute.val = txt.Mid(pos, pos2 == -1 ? pos2 : (pos2 - pos));
						Trim(attribute.val);
						pos = pos2 == -1 ? pos2 : (pos2 + 1);
					}
				}
			} else
			{
				attribute.condition = select_exists;
			}
			attribute.attribute	= attr;
			m_attrs.Add(attribute);
			el_end = pos;
		} else
		{
			el_end++;
		}
		el_end = FindFirstOf(txt, ".#[:", el_end);
	}
	
	PERFMETER_END(PERF_CSSELEMENTPARSE);
}


bool CssSelector::Parse( const String& text )
{
	PERFMETER_BEGIN(PERF_CSSSELPARSE);
	
	if(text.IsEmpty())
	{
		PERFMETER_END(PERF_CSSSELPARSE);
		return false;
	}
	StringVector tokens;
	SplitString(text, tokens, "", " \t>+~", "([");

	if(tokens.IsEmpty())
	{
		PERFMETER_END(PERF_CSSSELPARSE);
		return false;
	}

	String left;
	String right = LAST(tokens);
	wchar combinator = 0;

	REMOVELAST(tokens);
	while(!tokens.IsEmpty() && (LAST(tokens) == " " || LAST(tokens) == "\t" || LAST(tokens) == "+" || LAST(tokens) == "~" || LAST(tokens) == ">"))
	{
		if(combinator == ' ' || combinator == 0)
		{
			combinator = LAST(tokens)[0];
		}
		REMOVELAST(tokens);
	}

	for(StringVector::Iterator i = tokens.Begin(); i != tokens.End(); i++)
	{
		left += *i;
	}

	Trim(left);
	Trim(right);

	if(right.IsEmpty())
	{
		PERFMETER_END(PERF_CSSSELPARSE);
		return false;
	}

	m_right.Parse(right);

	switch(combinator)
	{
	case '>':
		m_combinator	= combinator_child;
		break;
	case '+':
		m_combinator	= combinator_adjacent_sibling;
		break;
	case '~':
		m_combinator	= combinator_general_sibling;
		break;
	default:
		m_combinator	= combinator_descendant;
		break;
	}

	m_left = 0;

	if(!left.IsEmpty())
	{
		m_left = new CssSelector(GetObjectPool(), MediaQueryList::ptr(0));
		if(!m_left->Parse(left))
		{
			PERFMETER_END(PERF_CSSSELPARSE);
			return false;
		}
	}
	PERFMETER_END(PERF_CSSSELPARSE);
	return true;
}

void CssSelector::CalcSpecificity()
{
	PERFMETER_BEGIN(PERF_CSSSELCALCSPEC);
	
	if(!m_right.m_tag.IsEmpty() && m_right.m_tag != "*")
	{
		m_specificity.d = 1;
	}
	
	for(CssAttributeSelector::vector::Iterator i = m_right.m_attrs.Begin(); i != m_right.m_attrs.End(); i++)
	{
		if(i->attribute == "id")
		{
			m_specificity.b++;
		} else
		{
			if(i->attribute == "class")
			{
				StringVector tokens;
				SplitString(i->val, tokens, " ");
				m_specificity.c += (int) tokens.GetCount();
			} else
			{
				m_specificity.c++;
			}
		}	
	}
	if(m_left)
	{
		m_left->CalcSpecificity();
		m_specificity += m_left->m_specificity;
	}
	
	PERFMETER_END(PERF_CSSSELCALCSPEC);
}

void CssSelector::AddMediaToDoc( Document* doc ) const
{
	if(m_media_query && doc)
	{
		doc->AddMediaList(m_media_query);
	}
}

END_HTMLCTRL_NAMESPACE
