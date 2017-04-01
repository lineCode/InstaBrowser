#include "Html.h"
#include "ElScript.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

ElScript::ElScript( Document& doc ) : Element(doc)
{
}

ElScript::~ElScript()
{

}

void ElScript::SetAttr(const String& name, const String& val) {
	if (name == "type") {
		type = val;
	}
}

void ElScript::ParseAttributes()
{
	if (type == "text/be") {
		HtmlPageNode* page = GetPage();
		if (page) {
			page->Script(this);
		}
	}
}

bool ElScript::AppendChild( Element* el )
{
	el->GetText(m_text);
	return true;
}

String ElScript::GetTagName() const
{
	return "script";
}

END_HTMLCTRL_NAMESPACE
