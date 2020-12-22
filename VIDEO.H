#ifndef __VIDEO_H__
#define __VIDEO_H__

#ifndef __BORLANDC__
  #define far 
#endif

typedef unsigned char   byte;
typedef unsigned short  word;

#define MODE_640x480_16   0x12
#define MODE_320x200_256  0x13
#define MODE_TEXT         0x03

#define SCREEN_W        320
#define SCREEN_H        200
#define SCREEN_SIZE     64000u
#define BUFFER_SIZE     64000u

#define PALETTE_INDEX   0x03c8
#define PALETTE_DATA    0x03c9
#define INPUT_STATUS    0x03da


int  video_buffers_init(void);
void video_buffers_free(void);

void video_set_mode(byte mode);

void video_put_pixel(word x, word y, byte c);

byte video_read_pixel(word x, word y);

void video_clear_buffer(void);

void video_display_buffer(void);


#endif // __VIDEO_H__