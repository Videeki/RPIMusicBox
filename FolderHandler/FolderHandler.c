#include "FolderHandler.h"
#include "debugmalloc.h"

//gcc -c .\FolderHandler.c -o .\bin\FolderHandler.o -IFolderHandler -\FolderHandler.h "-IC:/msys64/mingw64/include/glib-2.0" "-IC:/msys64/mingw64/lib/glib-2.0/include" "-lglib-2.0"
GString* path_Join(GString* string, ...)
{
    va_list join;
    va_start(join, string);
    char separator;
    #ifdef _WIN32
        separator = '\\';
    #else
        separator = '/';
    #endif

    const gchar* temp;
    while(temp = va_arg(join, char*))
    {
        if(string->str[string->len-1] != separator)
        {
            string = g_string_append_c_inline(string, separator);
        }
        string = g_string_append(string, g_strdup(temp));
    }
    va_end(join);

    return string;
}


GList* listFolderElements(GList* listElements, const char* path, const char* exp)
{
    GError *error = NULL;
    GDir *dir = g_dir_open(path, 0, &error);
    if(error != NULL)
    {
        g_printerr("Problem with openning the %s folder. Error:%d -> Message:%s\n", path, error->code, error->message);
        g_error_free(error);
        return NULL;
    }

    const gchar *filename;
    while(filename = g_dir_read_name(dir))
    {
        if(endsWith(filename, exp))
        {
            listElements = g_list_append(listElements, g_strdup(filename));
            //printf("%s\n", filename);
        }
    }

    g_dir_close(dir);
    
    return listElements;
}


int printFolderElements(const gchar* path, const gchar* exp)
{
    GError *error = NULL;
    GDir *dir = g_dir_open(path, 0, &error);
    if(error != NULL)
    {
        g_printerr("Problem with openning the %s folder. Error:%d -> Message:%s\n", path, error->code, error->message);
        g_error_free(error);
        return -1;
    }

    const gchar *filename;
    while(filename = g_dir_read_name(dir))
    {
        if(endsWith(filename, exp))
        {
            printf("%s\n", filename);
        }
    }

    g_dir_close(dir);
    return 0;
}


gboolean endsWith(const gchar* str, const gchar* exp)
{
    if(exp == NULL) return TRUE;

    int strlength = strlen(str);
    int explength = strlen(exp);
    for(int i = 0; exp[i] != '\0'; i++)
    {
        if(str[strlength - explength + i] != exp[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**********************************************     Main     **********************************************

typedef struct
{
    GList* musicFolder;
    GList* folderElements;
} Data;


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        g_printerr("There are not enough parameters\n");
        return -1;
    }
    printf("FolderHandling is running...\n");

    Data foo; 

    foo.folderElements = NULL;
    //GList* folderElements = NULL;
    GString* path = g_string_new(g_strdup(argv[1]));
    path = path_Join(path, "Music", "Vegyes", NULL);

    printf("Path: %s\n", path->str);
    foo.folderElements = listFolderElements(foo.folderElements, path->str, NULL);
    
    g_string_free(path, TRUE);
    int i = 0;
    for(const GList* element = foo.folderElements; element != NULL; element = element->next)
    {
        i++;
        printf("%d. song: %s\n", i, (char*)element->data);
    }
    
    g_list_free(foo.folderElements);

    return 0;
}

**********************************************************************************************************/