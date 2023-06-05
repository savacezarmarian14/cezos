#include <types.h>
#include <memlayout.h>
#include <x86.h>
#include <kbdreg.h>
#include <string.h>
#include <stdio.h>

#include <console.h>


static unsigned port; // port for operating console
static uint16_t *c_buffer;
static uint16_t c_pos;

static bool serial_exists;

static void console_interrupt(int (*proc)(void));
static void console_putc(int c);

// Stupid I/O delay routine necessitated by historical PC design flaws
static void
delay(void)
{
	inb(0x84);
	inb(0x84);
	inb(0x84);
	inb(0x84);
}

static int serial_proc_data(void)
{
    /* proccess data */
    /* Test Line Status register for data */
    if (!(inb(COM1+COM_LSR) & COM_LSR_DATA))
        return -1;
    return inb(COM1+COM_RX);
}

void serial_interrupt(void)
{
    if (serial_exists) {
        console_interrupt(serial_proc_data);
    }
}

/* Print to serial display */
static void serial_putc(int c)
{
    int i = 0;
    for (i = 0; !(inb(COM1 + COM_LSR) & COM_LSR_TXRDY) && (i < 12800); ++i) {
        delay();
    }
    outb(COM1 + COM_TX, c);

}

static void
lpt_putc(int c)
{
	int i;

	for (i = 0; !(inb(0x378+1) & 0x80) && i < 12800; i++)
		delay();
	outb(0x378+0, c);
	outb(0x378+2, 0x08|0x04|0x01);
	outb(0x378+2, 0x08);
}

static void init_cga(void)
{
    /*
     * The volatile keyword is intended to prevent the compiler from applying any optimizations
     * on objects that can change in ways that cannot be determined by the compiler.
     * Objects declared as volatile are omitted from optimization because their values
     * can be changed by code outside the scope of current code at any time.
     */
    volatile uint16_t *cp;
    uint16_t base;
    unsigned cursor_pos;

    cp = (uint16_t *) (KERNEL_BASE_ADDRESS + CGA_BUFF_ADDR); // [0xF000000 - 0xF00B8000]
    base = *cp;

    /* Test if zone pointed by cp is writable */
    *cp = 0xCE70; // It should work with any value
    if (*cp != 0xCE70) {
        cp = (uint16_t *) (KERNEL_BASE_ADDRESS + MONOCHROME_BUFF_ADDR);
        port = MONOCHROME_PORT;
    } else {
        *cp = base;
        port = CGA_PORT;
    }

    /* Operations to extract cursor location */
    outb(port, 14);
    cursor_pos = inb(port + 1) << 8;        // second 8 bits
    outb(port, 15);
    cursor_pos = cursor_pos | inb(port + 1); // first 8 bits

    c_buffer = (uint16_t *) cp;
    c_pos = cursor_pos;
}

static void cga_putc(int c)
{
	// if no attribute given, then use black on white
	if (!(c & ~0xFF))
		c |= 0x0700;

	switch (c & 0xff) {
	case '\b':
		if (c_pos > 0) {
			c_pos--;
			c_buffer[c_pos] = (c & ~0xff) | ' ';
		}
		break;
	case '\n':
		c_pos += CRT_COLS;
		/* fallthru */
	case '\r':
		c_pos -= (c_pos % CRT_COLS);
		break;
	case '\t':
		console_putc(' ');
		console_putc(' ');
		console_putc(' ');
		console_putc(' ');
		console_putc(' ');
		break;
	default:
		c_buffer[c_pos++] = c;		/* write the character */
		break;
	}

	// What is the purpose of this?
	if (c_pos >= CRT_SIZE) {
		int i;

		memmove(c_buffer, c_buffer + CRT_COLS, (CRT_SIZE - CRT_COLS) * sizeof(uint16_t));
		for (i = CRT_SIZE - CRT_COLS; i < CRT_SIZE; i++)
			c_buffer[i] = 0x0700 | ' ';
		c_pos -= CRT_COLS;
	}

	/* move that little blinky thing */
	outb(port, 14);
	outb(port + 1, c_pos >> 8);
	outb(port, 15);
	outb(port + 1, c_pos);
}


static void kbd_init(void)
{

}

static int kbd_proc_data(void)
{
	int c;
	uint8_t stat, data;
	static uint32_t shift;

	stat = inb(KBSTATP);
	if ((stat & KBS_DIB) == 0)
		return -1;
	// Ignore data from mouse.
	if (stat & KBS_TERR)
		return -1;

	data = inb(KBDATAP);

	if (data == 0xE0) {
		// E0 escape character
		shift |= E0ESC;
		return 0;
	} else if (data & 0x80) {
		// Key released
		data = (shift & E0ESC ? data : data & 0x7F);
		shift &= ~(shiftcode[data] | E0ESC);
		return 0;
	} else if (shift & E0ESC) {
		// Last character was an E0 escape; or with 0x80
		data |= 0x80;
		shift &= ~E0ESC;
	}

	shift |= shiftcode[data];
	shift ^= togglecode[data];

	c = charcode[shift & (CTL | SHIFT)][data];
	if (shift & CAPSLOCK) {
		if ('a' <= c && c <= 'z')
			c += 'A' - 'a';
		else if ('A' <= c && c <= 'Z')
			c += 'a' - 'A';
	}

	// Process special keys
	// Ctrl-Alt-Del: reboot
	if (!(~shift & (CTL | ALT)) && c == KEY_DEL) {
		cprintf("Rebooting!\n");
		outb(0x92, 0x3); // courtesy of Chris Frost
	}

	return c;
}

void keyboard_interrupt(void)
{
	console_interrupt(kbd_proc_data);
}

static void serial_init(void)
{
	// Turn off the FIFO
	outb(COM1+COM_FCR, 0);

	// Set speed; requires DLAB latch
	outb(COM1+COM_LCR, COM_LCR_DLAB);
	outb(COM1+COM_DLL, (uint8_t) (115200 / 9600));
	outb(COM1+COM_DLM, 0);

	// 8 data bits, 1 stop bit, parity off; turn off DLAB latch
	outb(COM1+COM_LCR, COM_LCR_WLEN8 & ~COM_LCR_DLAB);

	// No modem controls
	outb(COM1+COM_MCR, 0);
	// Enable rcv interrupts
	outb(COM1+COM_IER, COM_IER_RDI);

	// Clear any preexisting overrun indications and interrupts
	// Serial port doesn't exist if COM_LSR returns 0xFF
	serial_exists = (inb(COM1+COM_LSR) != 0xFF);
	(void) inb(COM1+COM_IIR);
	(void) inb(COM1+COM_RX);

}
/* Initialize console for printing if it can */
void console_init(void)
{
    /* Determine where is console buffer and currsor position */
    init_cga();
    kbd_init();
    /* Test Serial Communication */
    serial_init();
    if (!serial_exists) {
        /* TODO: Implement kernel prints */
		cprintf("No serial monitor\n");
    }

}



/***** General device-independent console code *****/
// Here we manage the console input buffer,
// where we stash characters received from the keyboard or serial port
// whenever the corresponding interrupt occurs.

#define CONSBUFSIZE 512

static struct {
	uint8_t buf[CONSBUFSIZE];
	uint32_t rpos;
	uint32_t wpos;
} cons;

static void console_interrupt(int (*proc)(void))
{
	int c;

	while ((c = (*proc)()) != -1) {
		if (c == 0)
			continue;
		cons.buf[cons.wpos++] = c;
		if (cons.wpos == CONSBUFSIZE)
			cons.wpos = 0;
	}
}

int
console_getc(void)
{
	int c;

	// poll for any pending input characters,
	// so that this function works even when interrupts are disabled
	// (e.g., when called from the kernel monitor).
	serial_interrupt();
	keyboard_interrupt();

	// grab the next character from the input buffer.
	if (cons.rpos != cons.wpos) {
		c = cons.buf[cons.rpos++];
		if (cons.rpos == CONSBUFSIZE)
			cons.rpos = 0;
		return c;
	}
	return 0;
}

static void
console_putc(int c)
{
	serial_putc(c);
	lpt_putc(c);
	cga_putc(c);
}

void
cputchar(int c)
{
	console_putc(c);
}

int
getchar(void)
{
	int c;

	while ((c = console_getc()) == 0)
		/* do nothing */;
	return c;
}

int
iscons(int fdnum)
{
	// used by readline
	return 1;
}

void clrscr() {
    // Dezactivează întreruperile
    asm("cli");

    // Obține adresa de început a memoriei video
    char* video_mem = (char*)CGA_BUFF_ADDR;

    // Setează valoarea pentru fiecare pixel pentru a fi un spațiu gol cu culoare neagră
    for (int i = 0; i < 80*25*2; i += 2) {
        video_mem[i] = ' ';
        video_mem[i+1] = 0;
    }

    // Curăță buffer-ul de intrare a tastaturii
    while(inb(0x64) & 0x1) {
        inb(0x60);
    }

    // Re-activează întreruperile
    asm("sti");
}
