// Compiling: gcc pRegex.c -o pRegex.exe -lregex
#include "pRegex.h"

int matchPattern(char *iStr, char *iExp, char *oBefore, char *oMatch, char *oAfter)
{
    regex_t regex;
    int reti;
    reti = regcomp(&regex, iExp, 0);
    if(reti)
    {
        puts("Could not compile regex");
        regfree(&regex);
        return -1;
    }
    else
    {
        regmatch_t matches[1];
        reti = regexec(&regex, iStr, 1, matches, 0);
        if (!reti) {
            int strStartIndex = matches[0].rm_so;
            int resultLength = (matches[0].rm_eo - matches[0].rm_so);

            strncpy(oBefore, iStr, strStartIndex);

            strncpy(oMatch, iStr + strStartIndex, resultLength-1);

            strncpy(oAfter, iStr + strStartIndex + resultLength, strlen(iStr) - (strStartIndex + resultLength - 1));
        }
        
        else if (reti == REG_NOMATCH)
        {
            strcpy(oBefore, iStr);
            strcpy(oMatch, "");
            strcpy(oAfter, "");
            regfree(&regex);
            return -2;
        } 
        
        else
        {
            puts("Regex match failed");
            regfree(&regex);
            return -3;    
        }
    }
    regfree(&regex);

    return 0;
}


int findRegex(char *dest, char *src, char *exp)
{  
    regex_t regex;
    int reti;
    reti = regcomp(&regex, exp, 0);
    if (reti)
    {
        puts("Could not compile regex");
        regfree(&regex);
        return -1;
    }
    else
    {
        regmatch_t matches[1];
        reti = regexec(&regex, src, 1, matches, 0);
        if (!reti) {
            printf("Match found at position %d, length %d: %.*s\n",
                 matches[0].rm_so,  //First character
                 matches[0].rm_eo - matches[0].rm_so,   // Length of result
                 matches[0].rm_eo - matches[0].rm_so,   // Length of result
                 &src[matches[0].rm_so]);   //

        int strStartIndex = matches[0].rm_so;
        int resultLength = (matches[0].rm_eo - matches[0].rm_so);

        printf("strValue: %s\n", src);
        printf("strIndex: %d\n", strStartIndex);
        printf("strLength: %d\n", resultLength);

        memcpy(dest, src + strStartIndex, resultLength);    
        printf("destValue: %s\n", dest);

        }
        
        else if (reti == REG_NOMATCH)
        {
            puts("No match found");
            regfree(&regex);
            return -2;
        } 
        
        else
        {
            puts("Regex match failed");
            regfree(&regex);
            return -3;    
        }
    }
    regfree(&regex);

    return 0;
}