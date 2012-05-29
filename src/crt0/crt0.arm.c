#include <unistd.h>

extern int main(int argc, char **argv);

void _start()
{
    main(0, 0);
    while(1);
}
