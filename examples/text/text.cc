#include <libgba/types.h>
#include <libgba/gba.h>
#include <libgba/txt.h>

int main() {
    txt_init();

    write("Hello World\n");
    write("This is a test\n");

    while (1);
}
