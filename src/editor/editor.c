 
/*
    Use Nuklear to add some editor ui as well as a smaller viewport synced to the current scene event

    Goals:
    - allow easily creating new render objects and events as well as dragging them around to resize and reorient
    - we need a way to put the viewport in a corner or even a seperate window?

    do we want this to live in this folder that its in rn? how to seperate the engine from the core? it needs to ship with the core

    Constraints:
    - editor only supported on linux
*/

// main function accepting one string argument for the path to the project folder
int main(int argc, char **argv) {
    // get our path from the command line
    char *path = argv[1];

    // print the path
    printf("path: %s\n", path);

    // exit
    return 0;
}
