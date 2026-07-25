// ATTENTION: CE CODE EST UN MAUVAIS EXEMPLE. IL NE S'AGIT *PAS* DE LA CORRECTION.
// L'OBJECTIF EST DE TROUVER LES PROBLEMES QU'IL CONTIENT ET DE LES CORRIGER !

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>


typedef struct {
  unsigned int width;
  unsigned int height;
  char pixel_type;
} header_t;

typedef struct {
  header_t header;
  char* data;
} image;


void fatal_error (const char* str) {
  fprintf (stderr, "Error: %s\n", str);
  exit (1);
}

void check_magic_number (FILE* f) {
  char magic_number[8];
  size_t n = fread (magic_number, 1, 8, f);  
  if ((n < 8) || (memcmp (magic_number, "Mini-PNG", 8) != 0))
    fatal_error ("Invalid magic number");
}

unsigned int read_uint32 (FILE* f) {
  int res;
  size_t n = fread (&res, 1, 4, f);
  if (n < 4)
    fatal_error ("Unexpected end of file while reading a 4-byte integer");
  return ntohl (res);
}

char* read_block (FILE* f, char* block_type, unsigned int* block_len) {
  size_t n = fread (block_type, 1, 1, f);
  if (n < 1) {
    if (feof (f))
      return NULL;
    else
      fatal_error ("Unexpected error while reading a block type");
  }
  *block_len = read_uint32(f);
  char* res = (char*) malloc (*block_len);
  if (res == NULL)
    fatal_error ("Unable to allocate a block");
  n = fread (res, 1, *block_len, f);
  if (n != *block_len)
    fatal_error ("Unexpected end of file while retrieving the block content");
  return res;
}

int bits_per_pixel (char pixel_type) {
  switch (pixel_type) {
  case 0: return 1;
  case 1:
  case 2: return 8;
  case 3: return 24;
  default: fatal_error ("Invalid pixel type"); return 0;
  }
}


header_t read_header (char* block_content) {
  header_t res;
  memcpy (&res.width, block_content, 4);
  res.width = ntohl (res.width);
  memcpy (&res.height, block_content + 4, 4);
  res.height = ntohl (res.height);
  res.pixel_type = block_content[8];
  return res;
}

void handle_file (FILE* f) {
  image i;
  unsigned int data_size;
  unsigned int block_len;
  char* data_ptr = NULL;
  check_magic_number (f);
  while (1) {
    char block_type;
    char* block_content = read_block (f, &block_type, &block_len);
    if (block_content == NULL) break;
    switch (block_type) {
    case 'H':
      i.header = read_header (block_content);
      data_size = i.header.width * i.header.height * bits_per_pixel (i.header.pixel_type);
      data_size = (data_size + 7) / 8;
      i.data = (char*) malloc (data_size);
      if (i.data == NULL)
        fatal_error ("Unable to allocate data buffer");
      data_ptr = i.data;
      break;
    case 'C':
      printf ("Comment: %s\n", block_content);
      break;
    case 'D':
      memcpy (data_ptr, block_content, block_len);
      data_ptr += block_len;
      break;
    default:
      fatal_error ("Unexpected block type");
    }
    free(block_content);
  }

  printf ("Width: %u\nHeight: %u\nPixel Type: %d\n\n", i.header.width, i.header.height, i.header.pixel_type);

  if (i.header.pixel_type == 0) {
    data_ptr = i.data;
    int mask = 0x80;
    unsigned int x, y;
    for (y = 0; y < i.header.height; y++) {
      for (x = 0; x < i.header.width; x++) {
        printf ("%c", ((*data_ptr & mask) == 0) ? ' ' : 'X');
        mask >>= 1;
        if (mask == 0) {
          mask = 0x80;
          data_ptr++;
        }
      }
      printf ("\n");
    }
    printf("\n");
  }
}


int main (int argc, char* argv[]) {
  if (argc != 2)
    fatal_error ("Exactly one argument is expected");
  FILE* f = fopen (argv[1], "r");
  if (f == NULL)
    fatal_error ("Unable to open the file");
  handle_file (f);
  return 0;
}
