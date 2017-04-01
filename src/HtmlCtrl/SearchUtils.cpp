#include "SearchUtils.h"


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
int FindLastOf(const String& src, const String& match, int endpos) {
	if (endpos < 0) return -1;
	for(int i = src.GetCount() - 1; i >= endpos; i--) {
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
int FindFirstNotOf(const String& src, const String& match, int startpos) {
	if (startpos < 0) return -1;
	for(int i = startpos; i < src.GetCount(); i++) {
		int c = src[i];
		bool is_match = false;
		for(int j = 0; j < match.GetCount(); j++) {
			if (c == match[j]) {
				is_match = true; break;
			}
		}
		if (!is_match)
			return i;
	}
	return -1;
}

// equal to FindLastOf(), but returns the last position of any characters not specified
int FindLastNotOf(const String& src, const String& match, int endpos) {
	if (endpos < 0) return -1;
	for(int i = src.GetCount() - 1; i >= endpos; i--) {
		int c = src[i];
		bool is_match = false;
		for(int j = 0; j < match.GetCount(); j++) {
			if (c == match[j]) {
				is_match = true; break;
			}
		}
		if (!is_match)
			return i;
	}
	return -1;
}
