#include "Html.h"
#include "Document.h"
#include "ElSpace.h"

NAMESPACE_HTMLCTRL

ElSpace::ElSpace( const String& text, Document& doc ) : ElText(text, doc)
{
}

ElSpace::~ElSpace()
{

}

bool ElSpace::IsWhiteSpace()
{
	WhiteSpace ws = GetWhiteSpace();
	if(	ws == white_space_normal || 
		ws == white_space_nowrap ||
		ws == white_space_pre_line )
	{
		return true;
	}
	return false;
}

bool ElSpace::IsBreak() const
{
	WhiteSpace ws = GetWhiteSpace();
	if(	ws == white_space_pre ||
		ws == white_space_pre_line ||
		ws == white_space_pre_wrap)
	{
		if(m_text == "\n")
		{
			return true;
		}
	}
	return false;
}

END_HTMLCTRL_NAMESPACE
