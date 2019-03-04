#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 1920
#define HEIGHT 1080
#define RED_MULTIPLIER 0.09    // Multiply the number of hits in a pixel by these values
#define GREEN_MULTIPLIER 0.11  // (needs to be lower the greater the number of source
#define BLUE_MULTIPLIER 0.18   // values and iterations, else image will be too bright)
#define COLOUR_OFFSET -230     // This value is added to the number of hits before a pixel is drawn.
#define reduce(foo) (foo)      // Macro to reduce colours, can use sqrt(n), log(n), etc, or just (n)

int iterations;
long long redcount[WIDTH][HEIGHT];
long long greencount[WIDTH][HEIGHT];
long long bluecount[WIDTH][HEIGHT];
double red_multiplier = RED_MULTIPLIER;
double green_multiplier = GREEN_MULTIPLIER;
double blue_multiplier = BLUE_MULTIPLIER;

void drawbmp(char *filename)
{

	unsigned int headers[13];
	FILE *outfile;
	int extrabytes;
	int paddedsize;
	int x; int y; int n;
	int red, green, blue;

	extrabytes = 4 - ((WIDTH * 3) % 4);   // How many bytes of padding to add to each
	// horizontal line - the size of which must be a multiple of 4 bytes.
	if (extrabytes == 4)
		extrabytes = 0;

	paddedsize = ((WIDTH * 3) + extrabytes) * HEIGHT;

	// Headers...
	// Note that the "BM" identifier in bytes 0 and 1 is NOT included in these "headers".
	headers[0]  = paddedsize + 54;      // bfSize (whole file size)
	headers[1]  = 0;                    // bfReserved (both)
	headers[2]  = 54;                   // bfOffbits
	headers[3]  = 40;                   // biSize
	headers[4]  = WIDTH;                // biWidth
	headers[5]  = HEIGHT;               // biHeight

	// Would have biPlanes and biBitCount in position 6, but they're shorts.
	// It's easier to write them out separately (see below) than pretend
	// they're a single int, especially with endian issues...

	headers[7]  = 0;                    // biCompression
	headers[8]  = paddedsize;           // biSizeImage
	headers[9]  = 0;                    // biXPelsPerMeter
	headers[10] = 0;                    // biYPelsPerMeter
	headers[11] = 0;                    // biClrUsed
	headers[12] = 0;                    // biClrImportant

	outfile = fopen(filename, "wb");

	//
	// Headers begin...
	// When printing ints and shorts, we write out 1 character at a time to avoid endian issues.
	//

	fprintf(outfile, "BM");
	for (n = 0; n <= 5; n++) {
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
	}

	// These next 4 characters are for the biPlanes and biBitCount fields.
	fprintf(outfile, "%c", 1);
	fprintf(outfile, "%c", 0);
	fprintf(outfile, "%c", 24);
	fprintf(outfile, "%c", 0);

	for (n = 7; n <= 12; n++) {
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
	}

	//
	// Headers done, now write the data...
	//

	// BMP image format is written from bottom to top...
	for (y = HEIGHT - 1; y >= 0; y--) {
		for (x = 0; x <= WIDTH - 1; x++) {
			red = reduce(redcount[x][y] + COLOUR_OFFSET) * red_multiplier;
			green = reduce(greencount[x][y] + COLOUR_OFFSET) * green_multiplier;
			blue = reduce(bluecount[x][y] + COLOUR_OFFSET) * blue_multiplier;
			// Also, it's written in (b,g,r) format...
			fprintf(outfile, "%c", blue);
			fprintf(outfile, "%c", green);
			fprintf(outfile, "%c", red);
		}
		// See above - BMP lines must be of lengths divisible by 4.
		if (extrabytes)
			for (n = 1; n <= extrabytes; n++)
				fprintf(outfile, "%c", 0);
	}

	fclose(outfile);
	return;
}

int main(int argc, char *argv[])
{
	if (argv[1])
		drawbmp(argv[1]);
	return 0;
}
