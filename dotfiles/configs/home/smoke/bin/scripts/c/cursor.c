// cursor usage

// the main thing to take away is to never send newlines
// to the screen when manipulating the cursor using escape codes

#include <stdio.h>

#define esc           27
#define cls           printf("%c[2J", esc)
#define pos(row, col) printf("%c[%d;%dH", esc, row, col)

main() {
    // clear the screen
    cls;

    // position cursor at row 9, column 28
    pos(9, 28);

    // note there is no \n
    printf("**************");

    // go down to next line
    pos(10, 28);

    // aligns under *’s
    printf("* Hi There *");

    // next line will be bold, blinking
    pos(11, 28);

    // turn on ... turn off
    printf("* %c[1;5mHi There%c[0m *", esc, esc);

    // next two lines double high, but also double width - 14 double widths = 28 single
    pos(12, 14);

    // top half
    printf("%c#3*HELLO*", esc);

    // must follow up with 2nd identical line
    pos(13, 14);

    // top half
    printf("%c#4*HELLO*", esc);

    // close off the box - no need to reset height
    pos(14, 28);

    // still no \n
    printf("**************");

    // "park" the cursor after execution, the terminal displays the prompt wherever you left off
    pos(20, 1);

    return 0;
}
