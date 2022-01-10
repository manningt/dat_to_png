//dat2png.cpp
// reads a frame of binary data from a file and writes it to a PNG format file of the same name but with a PNG extention

#include <stdio.h>
#include <unistd.h> 
#include <getopt.h> //for optarg
#include <stdlib.h> //for exit
#include <stdint.h>
#include <string.h>
#include <libgen.h> //for basename?

#include <png.h>

#define WIDTH 1280
#define HEIGHT 800

int main(int argc, char *argv[]) {

	int option = 0;
   // tresholding is not provided in this version, but taking in a argument is left for the future
   int threshold = 160; //default; range is 0 to 255; 0 is black

	while ((option = getopt(argc, argv, "t:")) != -1) {
		switch (option) {
         case 't':
            threshold = atoi(optarg);
            // printf("Entered threshold: %d\n", threshold);
            if (threshold < 1 || threshold > 255) {
               printf("Error: threshold '%d' is out of range; range is 1..255\n", threshold);
               exit(1);
            }
            break;
			case '?':
            printf("unknown option: %c\n", optopt);
            break;
		}
	}
   //the file to read is the first positional argument
   FILE *fp;
   if ((fp = fopen(argv[optind], "rb")) == 0)
   {
      fprintf(stderr, "Error opening %s can't be open for reading.\n", argv[optind]);
      exit(1);
   }
   png_byte buf[(WIDTH*HEIGHT)];
   size_t buf_size = sizeof(*buf); //1
   size_t num_elements = sizeof(buf)/sizeof(*buf); //1024000
   size_t ret = fread(buf, 1, (WIDTH*HEIGHT), fp);
   if (ret*buf_size != num_elements )
   {
      fprintf(stderr, "Error on file '%s' read; only read %zu out of %zu bytes\n", argv[optind],ret*buf_size,num_elements );
      exit(1);
   }
   fclose(fp);

   char *bname= basename(argv[optind]);
   char out_file[64];
   strcpy(out_file, bname);
   int length = (int) strlen(out_file);
   for (int i = length-1; i>0; i--)
   {
      if (out_file[i] == '.') {
         out_file[i] = '\0';
         // printf("replaced char %d, string: %s\n", i, filename);
         break;
      }
   }
   strcat(out_file, ".png");
   if ((fp = fopen(out_file, "wb")) == 0)
   {
      fprintf(stderr, "Could not open file %s for writing\n", out_file);
      exit(1);
   }
 
   // Initialize write structure
   png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      fprintf(stderr, "Could not allocate write struct\n");
      exit(1);
   }

   // Initialize info structure
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      fprintf(stderr, "Could not allocate info struct\n");
      exit(1);
   }

   // Setup Exception handling
   if (setjmp(png_jmpbuf(png_ptr))) {
      fprintf(stderr, "Error during png creation\n");
      exit(1);
   }

   png_init_io(png_ptr, fp);

   // Write header
   png_byte bit_depth = 8;
   png_set_IHDR(png_ptr, info_ptr, WIDTH, HEIGHT,
         bit_depth, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

   // Set title & PNG info - doesn't really result in much info with the following code - no author...
   // refer to this to add more info: http://libpng.org/pub/png/book/chapter15.html
   png_text title_text;
   title_text.compression = PNG_TEXT_COMPRESSION_NONE;
   title_text.key = "Title";
   title_text.text = bname;
   png_set_text(png_ptr, info_ptr, &title_text, 1);
   png_write_info(png_ptr, info_ptr);

   //setup pointers to the frame of data that was read
   png_byte * row_pointers[HEIGHT];
   for (int i=0; i<HEIGHT; i++)
   row_pointers[i]= (png_byte *) &buf+(WIDTH*i);

   png_write_image(png_ptr, row_pointers);
   png_write_end(png_ptr, NULL);
   fclose(fp);
}
