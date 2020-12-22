#ifndef __TGA_H__
#define __TGA_H__

#ifndef __BORLANDC__
  #define far 
#endif

typedef unsigned char  byte;
typedef unsigned short word;

#define DEBUG 1

#define TGA_HEADER_SIZE 18   /* 12h */

void debugf(const char* format, ...);

typedef enum TGA_ImageType {
    TGA_IMAGE_TYPE_NONE,            /* 0 - no image data included */
    TGA_IMAGE_TYPE_UNCOMPR_MAP,     /* 1 - uncompressed, color mapped image */
    TGA_IMAGE_TYPE_UNCOMPR_RGB,     /* 2 - uncompressed, RGB image */
    TGA_IMAGE_TYPE_UNCOMPR_BW,      /* 3 - uncompressed, black & white image */
    TGA_IMAGE_TYPE_RLE_MAP = 9,     /* 9 - run-length encoded(RLE-lossless compression),color mapped image */
    TGA_IMAGE_TYPE_RLE_RGB,         /*10 - RLE, RGB image*/
    TGA_IMAGE_TYPE_COMPR_BW         /*11 - compressed, black & white image*/
} TGA_ImageType;

typedef struct ImageDescriptorFields {
    byte pixel_attr_num :4;
    byte img_location_x :1;
    byte img_location_y :1;
    byte ignored :2;
} ImageDescriptorFields;

typedef union ImgDescr {
    byte                    descriptor;
    ImageDescriptorFields   descriptor_fields;
} ImgDescr;


/* Prevent structure packing */
/* For Open Watcom Compiler */
#if defined(__WATCOMC__)
    #pragma pack(push,1)
#endif

typedef struct TGA_Header {
    byte IDLength;        /* 00h  Size of Image ID field, starting at byte 12h */
    byte ColorMapType;    /* 01h  Color map type (0 or 1; no map or 256 entry palette)*/
    byte ImageType;       /* 02h  Image type code 
                               (0 - no image data included
                                1 - uncompressed, color mapped image
                                2 - uncompressed, RGB image
                                3 - uncompressed, black & white image
                                9 - run-length encoded(RLE-lossless compression),color mapped image
                               10 - RLE, RGB image
                               11 - compressed, black & white image )*/
    word CMapStart;       /* 03h  Color map origin (first entry index for color map) (offset from 12h?)*/
    word CMapLength;      /* 05h  Color map length (total number of entries in color map)*/
    byte CMapDepth;       /* 07h  Depth of color map entries (number of bits per entry 15, 16, 24, 32)*/
    word XOffset;         /* 08h  X origin of image */
    word YOffset;         /* 0Ah  Y origin of image */
    word Width;           /* 0Ch  Width of image */
    word Height;          /* 0Eh  Height of image */
    byte PixelDepth;      /* 10h  Image pixel size (8, 16, 24, 32)*/
    ImgDescr ImageDescriptor; /* 11h  Image descriptor byte */
} TGA_Header;

#if defined(__WATCOMC__)
    #pragma pack(pop)
#endif

typedef struct TGA_File {
    TGA_Header  header;
    byte*       color_map;
    byte far*   img_data;
    long        img_data_size;
    byte        color_chan_num;
} TGA_File;


void tga_file_free(TGA_File* tga_file);

int tga_load_file(const char* fname, TGA_File* tga_file);

void tga_print_header(TGA_File* tga_file);


#endif // __TGA_H__
