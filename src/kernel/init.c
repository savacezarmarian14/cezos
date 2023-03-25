#include <console.h>
#include <string.h>
#include <stdio.h>

/* Init Window */
void say_hi(void)
{
    extern char *hello;
    cprintf("%s\n", hello);
}

/* 
    To complete the ELF loading process, the .bss section should be initialize
    with 0. To be able to do that at link time I provided two labels one at the start of
    the section and one at the end of it 
*/
void init_bss(void)
{
    extern char start_bss[], end_bss[];
    memset(start_bss, 0, end_bss - start_bss);
}

void init(void)
{
    /* Finalize the ELF Loading processc */
    init_bss();

    /* Activate and initialize console for printing */
    console_init();
    say_hi();




    while(1);
}