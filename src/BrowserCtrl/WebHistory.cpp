#include "WebHistory.h"

NAMESPACE_HTMLCTRL

WebHistory::WebHistory()
{
	m_current_item = 0;
}

WebHistory::~WebHistory()
{

}

void WebHistory::UrlOpened( const String& url )
{
	if(!m_items.IsEmpty())
	{
		if(m_current_item != m_items.GetCount() - 1)
		{
			if(m_current_item > 0 && m_items[m_current_item - 1] == url)
			{
				m_current_item--;
			} else if(m_current_item < m_items.GetCount() - 1 && m_items[m_current_item + 1] == url)
			{
				m_current_item++;
			} else
			{
				//m_items.erase(m_items.Begin() + m_current_item + 1, m_items.End());
				m_items.Remove(m_current_item+1, m_items.GetCount() -1 -m_current_item);
				m_items.Add(url);
				m_current_item = m_items.GetCount() - 1;
			}
		} else
		{
			if(m_current_item > 0 && m_items[m_current_item - 1] == url)
			{
				m_current_item--;
			} else 
			{
				m_items.Add(url);
				m_current_item = m_items.GetCount() - 1;
			}
		}
	} else
	{
		m_items.Add(url);
		m_current_item = m_items.GetCount() - 1;
	}
}

bool WebHistory::Back( String& url )
{
	if(m_items.IsEmpty())	return false;
	
	if(m_current_item > 0)
	{
		url = m_items[m_current_item - 1];
		return true;
	}
	return false;
}

bool WebHistory::Forward( String& url )
{
	if(m_items.IsEmpty())	return false;

	if(m_current_item < m_items.GetCount() - 1)
	{
		url = m_items[m_current_item + 1];
		return true;
	}
	return false;
}

END_HTMLCTRL_NAMESPACE
