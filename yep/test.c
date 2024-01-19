#include <stdio.h>

#include "yep.h"

int main(int argc, char *argv[])
{
    yep_initialize();
    yep_pack_directory("/home/ryan/repos/yoyoengine/yep/resources", "resources.yep");
    
    char * test = (char*)yep_extract_data("resources.yep", "test.txt");
    printf("%s\n",test);
    free(test);
    
    char * anotha = (char*)yep_extract_data("resources.yep", "anotha.txt");
    printf("%s\n",anotha);
    free(anotha);

    char * rep = (char*)yep_extract_data("resources.yep", "repetitive.data");
    printf("%s\n",rep);
    free(rep);

    char * entry = (char*)yep_extract_data("resources.yep", "entry.yoyo");
    printf("%s\n",entry);
    free(entry);

    yep_shutdown();
    return 0;
}