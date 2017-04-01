#include "Html.h"
#include "ElCdata.h"

NAMESPACE_HTMLCTRL

ElCdata::ElCdata(Document& doc ) : Element(doc)
{
	m_skip = true;
}

ElCdata::~ElCdata()
{

}

void ElCdata::GetText( String& text )
{
	text += m_text;
}

void ElCdata::SetData( const String& data )
{
	//if(data) {
	if(data.GetCount()) {
		m_text += data;
	}
}

END_HTMLCTRL_NAMESPACE
