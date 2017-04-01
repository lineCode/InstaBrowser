#include "HtmlCtrl.h"


// equal to String::Find(), but finds the first position of any character specified
int FindFirstOf(const String& src, const String& match, int startpos) {
    if (startpos < 0) return -1;
    for(int i = startpos; i < src.GetCount(); i++) {
        int c = src[i];
        for(int j = 0; j < match.GetCount(); j++) {
            if (c == match[j]) {
                return i;
            }
        }
    }
    return -1;
}

int FindFirstOf(const String& src, char match, int startpos) {
    if (startpos < 0) return -1;
    for(int i = startpos; i < src.GetCount(); i++) {
        int c = src[i];
        if (c == match) {
            return i;
        }
    }
    return -1;
}

// equal to FindFirstOf(), but finds the last position of any character specified
int FindLastOf(const String& src, const String& match, int endpos, int count) {
    if (endpos == -1) endpos = src.GetCount() - 1;
    int begin = (count == -1 ? 0 : endpos - count);
    for(int i = endpos; i >= begin; i--) {
        int c = src[i];
        for(int j = 0; j < match.GetCount(); j++) {
            if (c == match[j]) {
                return i;
            }
        }
    }
    return -1;
}

// equal to FindFirstOf(), but returns the position of the first character not of those specified
int FindFirstNotOf(const String& src, const String& match, int startpos, int count) {
    if (startpos < 0) return -1;
    int end = (count == -1 ? src.GetCount() : startpos + count);
    for(int i = startpos; i < end; i++) {
        int c = src[i];
        bool is_match = false;
        for(int j = 0; j < match.GetCount(); j++) {
            if (c == match[j]) {
                is_match = true;
                break;
            }
        }
        if (!is_match)
            return i;
    }
    return -1;
}

// equal to FindLastOf(), but returns the last position of any characters not specified
int FindLastNotOf(const String& src, const String& match, int endpos, int count) {
    if (endpos == -1) endpos = src.GetCount() - 1;
    int begin = (count == -1 ? 0 : max(endpos - count, 0));
    for(int i = endpos; i >= begin; i--) {
        int c = src[i];
        bool is_match = false;
        for(int j = 0; j < match.GetCount(); j++) {
            if (c == match[j]) {
                is_match = true;
                break;
            }
        }
        if (!is_match)
            return i;
    }
    return -1;
}



CacheImage::CacheImage() {
	
}

CacheImage::operator const Image&() {
	return img;
}

CacheImage::operator const Image*() {
	return &img;
}

const Image& CacheImage::GetImage() {
	return img;
}

CacheImage& CacheImage::SetImage(const Image& img) {
	ASSERT(this->img.GetSize().IsEmpty());
	this->img = img;
	return *this;
}

bool CacheImage::SetData(Stream& s) {
	if (!s.GetSize())
		return false;
	s.Seek(0);
	s.SetLoading();
	One<StreamRaster> r = StreamRaster::OpenAny(s);
	if(!r)
		return false;
	img = r->GetImage();
	return *this;
}


NAMESPACE_HTMLCTRL


HtmlPageNode* HtmlNode::GetPage() {
	return GetParentClass<HtmlPageNode>();
}

String HtmlNode::GetBaseUrl() {
	HtmlPageNode* page = GetPage();
	if (!page) return "";
	return page->GetBaseUrl();
}








HtmlPageNode::HtmlPageNode(HtmlNode& parent) : HtmlNode(parent) {
	protocol = "http";
}
	
String HtmlPageNode::GetUrl() {return url;}
String HtmlPageNode::GetBaseUrl() {return base_url;}
String HtmlPageNode::GetProtocol() {return protocol;}

void HtmlPageNode::SetUrl(const String& s) {
	protocol	= GetUrlProtocol(s);
	if (!protocol.GetCount()) protocol = "file";
	url			= s;
	base_url	= protocol + "://" + GetUrlBasePath(s);
}
void HtmlPageNode::SetBaseUrl(const String& s) {
	ASSERT(base_url.GetCount());
	ASSERT(GetUrlProtocol(base_url).GetCount());
	
	protocol = GetUrlProtocol(s);
	
	base_url = protocol + "://" + GetUrlBasePath( s );
	
	LOG("SetBaseUrl " << base_url);
}

END_HTMLCTRL_NAMESPACE
