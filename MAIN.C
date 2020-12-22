#ifndef __BORLANDC__
  #define far 
#endif

#include <dos.h>
#include <conio.h>
#include <stdlib.h>

#include "VIDEO.H"
#include "TGA.H"



int main(void) {
    word x, y;
    byte*       color_map;
	byte i;
    TGA_File tga_file = {0};


    if (tga_load_file("data\\bird.tga", &tga_file)) {
        exit(EXIT_FAILURE);
    } 
    tga_print_header(&tga_file);
    getch();

    video_set_mode(MODE_320x200_256);
    video_buffers_init();
    
     
    switch (tga_file.header.ImageType) {
        case TGA_IMAGE_TYPE_UNCOMPR_MAP:
            /* Load palette from color map */
            color_map = tga_file.color_map;
            for (i = 0; i < 255; ++i) {
                outportb(PALETTE_INDEX, i);
                outportb(PALETTE_DATA, color_map[2] >> 2);
                outportb(PALETTE_DATA, color_map[1] >> 2);
                outportb(PALETTE_DATA, color_map[0] >> 2);
                color_map += tga_file.color_chan_num;
            }

            /* Copy pixels to buffer */
            for (y = 0; y < tga_file.header.Height; ++y) {
                for(x = 0; x < tga_file.header.Width; ++x) {
                    video_put_pixel(x, y, tga_file.img_data[y*tga_file.header.Width + x]);
                }
            }
			break;
        case TGA_IMAGE_TYPE_UNCOMPR_RGB:
            /* Load palette from img_data */
			/* TBD */


             /* Copy pixels to buffer */
            for (y = 0; y < tga_file.header.Height; ++y) {
                for(x = 0; x < tga_file.header.Width; ++x) {
                    video_put_pixel(x, y, tga_file.img_data[(y*tga_file.header.Width + x) * tga_file.color_chan_num]);
                }
            }
            break;
		default:
            /* FIXME: cprintf() does not work in graphics mode */
			cprintf("Unimplemented TGA image type: %i\n", tga_file.header.ImageType);
			goto  exit;
	}
	video_display_buffer();

exit:
	getch();
	video_set_mode(MODE_TEXT);
	video_buffers_free();

	tga_file_free(&tga_file);

	return 0;
}
