#include "FolderHandler.h"


//gcc -c .\FolderHandler.c -o .\bin\FolderHandler.o -IFolderHandler -\FolderHandler.h "-IC:/msys64/mingw64/include/glib-2.0" "-IC:/msys64/mingw64/lib/glib-2.0/include" "-lglib-2.0"


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

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        g_printerr("There are not enough parameters\n");
        return -1;
    }
    printf("FolderHandling is running...\n");

    //int ret = listFolderItems(argv[1], "mp3");
    //printf("FolderHandling is DONE: %d\n", ret);

    //printf("End of %s is exe: %s\n", argv[0], (endsWith(argv[0], ".exe") ? "TRUE" : "FALSE"));

    GList* folderElements = NULL;
    
    folderElements = listFolderElements(folderElements, argv[1], NULL);
    
    int i = 0;
    for(const GList* element = folderElements; element != NULL; element = element->next)
    {
        i++;
        printf("%d. song: %s\n", i, element->data);
    }
    
    g_list_free(folderElements);

    return 0;
}

**********************************************************************************************************/