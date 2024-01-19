#include <stdio.h>

#include "yep.h"

int main(int argc, char *argv[])
{
    yep_initialize();
    yep_pack_directory("/home/ryan/repos/yoyoengine/yep/resources", "resources.yep");
    char * test = (char*)yep_extract_data("resources.yep", "test.txt");
    printf("%s\n",test);
    free(test);
    yep_shutdown();
    return 0;
}