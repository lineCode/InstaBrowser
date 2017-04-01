#include "Html.h"
#include "ElComment.h"

NAMESPACE_HTMLCTRL

ElComment::ElComment(Document& doc ) : Element(doc)
{
	m_skip = true;
}

ElComment::~ElComment()
{

}

void ElComment::GetText( String& text )
{
	text += m_text;
}

void ElComment::SetData( const String& data )
{
	//if(data){
	if(data.GetCount()){
		m_text += data;
	}
}

END_HTMLCTRL_NAMESPACE
