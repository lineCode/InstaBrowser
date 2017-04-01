#include "Console.h"

NAMESPACE_HTMLCTRL

Console::Console(HtmlNode& parent) : HtmlNode(parent) {
	
	Add(log.SizePos());
	
	
}

void Console::Print(const String& line) {
	log.Insert(log.GetLength(), line + "\n");
}

END_HTMLCTRL_NAMESPACE
