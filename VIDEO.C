#include "VIDEO.H"

#include <dos.h>
// #include <conio.h>
#include <stdio.h>
#include <malloc.h>
#include <alloc.h>
// #include <stdlib.h>
#include <mem.h>

static byte old_mode;

#ifdef __DJGPP
  static byte  buffer [64000];
#else
  static byte far* buffer;
  static byte far* screen;
#endif


int video_buffers_init(void) {
	buffer = (byte far*)farmalloc(BUFFER_SIZE);
    if (!buffer) {
        /* video_leave_mode13h();*/
        perror("Out of mem!\n");
        return 1;
    }
	
	screen = (byte far*)MK_FP(0xa000, 0);
		
	/* video_enter_mode13h(); */
	_fmemset(buffer, 0, BUFFER_SIZE);
    return 0;
}

void video_buffers_free(void) {
    if (buffer) {
        farfree(buffer);
    }
}

void video_set_mode(byte mode) {
    union REGS regs;
    regs.h.ah = 0x00;
    regs.h.al = mode;
	int86(0x10, &regs, &regs);
}

void video_enter_mode13h(void) {
    union REGS in, out;

	// get old video mode
	in.h.ah = 0xf;
	int86(0x10, &in, &out);
	old_mode = out.h.al;

	// enter mode 13h
	in.h.ah = 0;
	in.h.al = 0x13;
	int86(0x10, &in, &out);
}

void video_leave_mode13h(void) {
	union REGS in, out;

	// change to the video mode we were in before we switched to mode 13h
	in.h.ah = 0;
	in.h.al = old_mode;
	int86(0x10, &in, &out);
}

// Sets a pixel in the buffer to a specific colour
void video_put_pixel(word x, word y, byte c) {
    if ((x < SCREEN_W) && (y < SCREEN_H)) {
        buffer[(y << 8) + (y << 6) + x] = c;
    }
}

// Reads the colour value of a pixel from the buffer
byte video_read_pixel(word x, word y) {
    if ((x < SCREEN_W) && (y < SCREEN_H)) {
        return (buffer[(y << 8) + (y << 6) + x]);
    } else {
        return 0;
  }
}

// Clears the buffer
void video_clear_buffer(void) {
  #ifdef __DJGPP
    memset(buffer, 0, BUFFER_SIZE);
  #else
    _fmemset(buffer, 0, BUFFER_SIZE);
  #endif
}

// Displays the buffer
void video_display_buffer(void) {
  #ifdef __DJGPP
    memcpy(screen, buffer, SCREEN_SIZE);
  #else
    _fmemcpy(screen, buffer, SCREEN_SIZE);
  #endif
}
