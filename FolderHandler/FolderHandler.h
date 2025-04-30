#ifndef FOLDERHANDLER_H
#define FOLDERHANDLER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <stdarg.h>


GString* path_Join(GString* string, ...);
GList* listFolderElements(GList* listElements, const char* path, const char* exp);
int printFolderElements(const char* path, const char* exp);
gboolean endsWith(const gchar* str, const gchar* exp);


#endif  /* FOLDERHANDLER_H */