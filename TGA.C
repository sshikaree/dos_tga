#include "TGA.H"

#include <stdio.h>
#include <malloc.h>
#include <alloc.h>
#include <mem.h>
// #include <alloc.h>



#if DEBUG
    #include <stdarg.h>

    void debugf(const char* format, ...) {
        va_list args;
        if (!DEBUG) {
            return;
        }
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
#else
    void debugf(const char* format, ...){}
#endif


// OS specific functions
// #if defined __MSDOS__ || defined MSDOS
//     #include <io.h>
    
//     static long get_file_size(FILE* f) {
//         debugf("MSDOS detected!\n");
//         return filelength(fileno(f));
//     }
// #else
//     static long get_file_size(FILE* f) {
//         long file_size = 0;
//         fseek(f, 0L, SEEK_END);
//         file_size = ftell(f);
//         rewind(f);
//         return file_size;
//     }
// #endif


void tga_file_free(TGA_File* tga_file) {
    if (tga_file->img_data) {
        farfree(tga_file->img_data);
    }
    if (tga_file->color_map) {
        free(tga_file->color_map);
    }
}


/* Read image file and fill up struct.
 * Maybe should rewrite with reading each field separately?
 * */
int tga_load_file(const char* fname, TGA_File* tga_file) {
    FILE*   f = NULL;
    long    bytes_num  = 0;
	long    file_size  = 0;
    long    i;

	tga_file->img_data_size = 0;
	f = fopen(fname, "rb");
	if (!f) {
		perror("Error opening image file");
		return -1;
	}

	/* Get file size */
	fseek(f, 0L, SEEK_END);
	file_size = ftell(f);
	rewind(f);
	// file_size = get_file_size(f);
	debugf("File %s size: %ld bytes.\n", fname, file_size);

    /* Read header */    
    bytes_num = fread(&tga_file->header, 1, sizeof(tga_file->header), f);
    if (bytes_num != sizeof(tga_file->header)) {
        perror("Error reading image header");
        return -1;
    }
        
    /* Read color map */
    /* Should we take care of IDLength or CMapStart or both ?? */
    if (tga_file->header.ColorMapType > 0 && tga_file->header.CMapLength > 0) {
        fseek(f, tga_file->header.IDLength, SEEK_CUR);
        tga_file->color_map = (byte*)malloc(tga_file->header.CMapLength * tga_file->header.CMapDepth / 8);
        if (!tga_file->color_map) {
            perror("Memory allocation error");
            fclose(f);
            return -1;
        }
        bytes_num = fread(tga_file->color_map, 1, tga_file->header.CMapLength * tga_file->header.CMapDepth / 8, f);
        if (bytes_num != tga_file->header.CMapLength * tga_file->header.CMapDepth / 8) {
            perror("Error reading color map");
            tga_file_free(tga_file);
            fclose(f);
            return -1;
        }
    }   
        
    /* Read image data */
    // tga_file->img_data_size = file_size - ftell(f);
    tga_file->img_data_size = (long)tga_file->header.Width * (long)tga_file->header.Height * (long)tga_file->header.PixelDepth / 8L;
    debugf("Data section start byte: %ld\n", ftell(f));
    if (!tga_file->img_data_size) {
        perror("Empty data section");
        fclose(f);
        return -1;
    }
    // debugf("Data section size: %ld - %ld = %ld bytes\n", file_size, ftell(f), tga_file->img_data_size);
    debugf("Data section size: %ld bytes\n", tga_file->img_data_size);
    tga_file->img_data = (byte far*)farmalloc(tga_file->img_data_size);
    if (!tga_file->img_data) {
        perror("Memory allocation error!");
        fclose(f);
        return -1;
    }
    // FIXME: check tga_file->img_data_size  size !!!
    // read chunks must not exceed sizeof(size_t)
    // bytes_num = fread(tga_file->img_data, 1, tga_file->img_data_size, f);
    // if (bytes_num != tga_file->img_data_size) {
    //     printf("Error reading image data: wanted %lu got %lu bytes\n", tga_file->img_data_size, bytes_num);
    //     fclose(f);
    //     return -1;
    // }

    // Reading by one byte to override fread() size_t limitation
    // TODO: takes too long, must be optimized!
    for (i = 0; i < tga_file->img_data_size; ++i) {
        int c = getc(f);
        if (c == EOF) {
            printf("Error reading image data: unexpected EOF. %ld of %ld bytes was read.\n", i, tga_file->img_data_size);
            fclose(f);
            farfree(tga_file->img_data);
            return -1;
        }
        tga_file->img_data[i] = (byte)c;
    }
        
    fclose(f);
    
    /* Get number of color channels */
    switch (tga_file->header.ImageType) {
        case TGA_IMAGE_TYPE_UNCOMPR_MAP:
        case TGA_IMAGE_TYPE_RLE_MAP:
            tga_file->color_chan_num = tga_file->header.CMapDepth / 8;
            break;
        default:
            tga_file->color_chan_num = tga_file->header.PixelDepth / 8; /* 1, 3, 4*/
            if (tga_file->color_chan_num == 2) {
                tga_file->color_chan_num = 3; // 16-bit means R5G6B5.
            }
            
    }
    printf("Number of color channels: %u\n", tga_file->color_chan_num);
    
    
    return 0;
}

void tga_print_header(TGA_File* tga_file) {
    printf("Header size: %u\n", sizeof(tga_file->header));
    printf("Image ID length: %u\n", tga_file->header.IDLength);
    printf("Image color map type: %u\n", tga_file->header.ColorMapType);
    printf("Image type: %u\n", tga_file->header.ImageType);
    printf("Color map start offset: %u\n", tga_file->header.CMapStart);
    printf("Color map length: %u\n", tga_file->header.CMapLength);
    printf("Color map depth: %u\n", tga_file->header.CMapDepth);
    
    printf("X offcet: %u\n", tga_file->header.XOffset);
    printf("Y offcet: %u\n", tga_file->header.YOffset);
    printf("Image size: %ux%u\n", tga_file->header.Width, tga_file->header.Height);
    printf("Image bits per pixel: %u\n", tga_file->header.PixelDepth);
    printf(
        "Number of attributes per pixel: %u\n",
        tga_file->header.ImageDescriptor.descriptor_fields.pixel_attr_num
    );
    printf(
        "Image origin location: %u, %u\n",
        tga_file->header.ImageDescriptor.descriptor_fields.img_location_x,
        tga_file->header.ImageDescriptor.descriptor_fields.img_location_y
    );
    
}

