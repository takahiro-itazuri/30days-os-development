#include "mystring.h"

int mystrcmp(const char *s1, const char *s2)
{
	int i = 0, ans = 0;
	while (s1[i] != 0 && s2[i] != 0) {
		if (s1[i] != s2[i]) {
			if (s1[i] > s2[i]) { ans = 1; }
			else if (s1[i] < s2[i]) { ans = -1; }
			break;
		}
		++i;
	}
	if (s1[i] != 0 && s2[i] == 0) { ans = 1; }
	if (s1[i] == 0 && s2[i] != 0) { ans = -1; }
	return ans;
}
