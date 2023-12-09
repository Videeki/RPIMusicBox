#ifndef PREGEX_H
#define PREGEX_H

#define PREGEX_VERSION_MAJOR    0
#define PREGEX_VERSION_MINOR    2
#define PREGEX_VERSION_PATCH    1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define GETLINE "\r\n"
//#define GETSECTION "^[\t ]*\[.+\]"
//#define GETSECTIONNOT "^[\t ]*\[[^\]]+\]"
#define KEYVALUEPAIR "^[\t ]*.+"
#define KEYVALUEPAIRNOT "^[\t ]*[^\t ;=\n\r][^=\n\r]*[ \t]*=.*"
#define EMPTYLINE "[\t ]*"

int matchPattern(char *iStr, char *iExp, char *oBefore, char *oMatch, char *oAfter);
int findRegex(char *dest, char *src, char *exp);

#endif  /* PREGEX_H */