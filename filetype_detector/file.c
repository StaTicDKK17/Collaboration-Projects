#include <stdio.h>  // fprintf, stdout, stderr.
#include <stdlib.h> // exit, EXIT_FAILURE, EXIT_SUCCESS.
#include <string.h> // strerror.
#include <errno.h>  // errno.
#include <stdbool.h>
#include <stdbool.h>

#define VALID_ONE_BYTE_UTF_8 0b10000000
#define VALID_TWO_BYTE_UTF_8 0b00100000
#define VALID_THREE_BYTE_UTF_8 0b00010000 
#define VALID_FOUR_BYTE_UTF_8 0b00001000

#define VALID_UTF_8_FOLLOW_BYTE 0b01000000

#define print_bits(x)                                            \
  do {                                                           \
    unsigned long long a__ = (x);                                \
    size_t bits__ = sizeof(x) * 8;                               \
    printf(#x ": ");                                             \
    while (bits__--) putchar(a__ &(1ULL << bits__) ? '1' : '0'); \
    putchar('\n');                                               \
  } while (0)

enum file_type {
  EMPTY,
  ASCII,
  ISO8859,
  UTF8,
  PNG,
  JPEG,
  PRIVATE_SSH_KEY,
  JAVA_CLASS_FILE,
  DATA
};

const char* FILE_TYPE_STRINGS[] = {
  "empty",
  "ASCII text",
  "ISO-8859 text",
  "UTF-8 text",
  "PNG",
  "JPEG",
  "Private SSH key",
  "Java Class",
  "data",
};

bool is_file_empty(const int length);
bool is_png_file(const unsigned char* bytes, int length);
bool is_jpg_file(const unsigned char* bytes, int length);
bool is_java_class_file(const unsigned char* bytes, int length);
enum file_type get_file_type(const unsigned char* bytes, int length);
unsigned char* read_file(const char* path, int* length);
bool is_valid_ascii_char(unsigned char c);
bool is_iso_char(unsigned char c);
bool is_ascii_file(const unsigned char* bytes, int length);
bool is_iso_file(const unsigned char* bytes, int length);
bool is_pcapng_file(const unsigned char* bytes, int length);
bool is_valid_utf8_one_byte_char(unsigned char c);
bool is_valid_utf8_two_byte_char(unsigned char c1, unsigned char c2);
bool is_valid_utf8_three_byte_char(unsigned char c1, unsigned char c2, unsigned char c3);
bool is_valid_utf8_four_byte_char(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4);
bool is_utf_file(const unsigned char* bytes, int length);
bool is_private_ssh_key_file(const unsigned char* bytes, int length);

int main(int argc, char* argv[]) {

  if (argc < 2) {
    // print help section
  }

  const char* path = argv[1];

  int length;
  unsigned char* bytes = read_file(path, &length);

  enum file_type result = get_file_type(bytes, length);

  printf("%s: %s\n", path, FILE_TYPE_STRINGS[result]);

  return EXIT_SUCCESS;
}

bool is_file_empty(const int length) {
  return length == 0; 
}

enum file_type get_file_type(const unsigned char* bytes, int length) {
  if (is_file_empty(length)) return EMPTY;
  if (is_ascii_file(bytes, length)) return ASCII;
  if (is_utf_file(bytes, length)) return UTF8;
  if (is_iso_file(bytes, length)) return ISO8859;
  if (is_png_file(bytes, length)) return PNG;
  if (is_jpg_file(bytes, length)) return JPEG;
  if (is_private_ssh_key_file(bytes, length)) return PRIVATE_SSH_KEY;
  if (is_java_class_file(bytes, length)) return JAVA_CLASS_FILE;
  return DATA;
}

unsigned char* read_file(const char* path, int* length) {
  unsigned char* file_bytes;
  FILE* fd = fopen(path, "rb");

  fseek(fd, 0, SEEK_END);
  *length = ftell(fd);
  rewind(fd);

  file_bytes = (unsigned char*)malloc(*length * sizeof(unsigned char));
  fread(file_bytes, 1, *length, fd);
  fclose(fd);

  return file_bytes;
}

bool is_png_file(const unsigned char* bytes, int length) {
    if (length < 8) return false;

    return 
      bytes[0] == 0x89 && 
      bytes[1] == 0x50 && 
      bytes[2] == 0x4E && 
      bytes[3] == 0x47 && 
      bytes[4] == 0x0D && 
      bytes[5] == 0x0A && 
      bytes[6] == 0x1A && 
      bytes[7] == 0x0A;
}

bool is_jpg_file(const unsigned char* bytes, int length) {  
  if (length < 4) return false; 

  return
      bytes[0] == 0xFF && 
      bytes[1] == 0xD8 &&
      bytes[2] == 0xFF;
}

bool is_java_class_file(const unsigned char* bytes, int length) {
  if (length < 4) return false;

  return 
    bytes[0] == 0xCA &&
    bytes[1] == 0xFE &&
    bytes[2] == 0xBA &&
    bytes[3] == 0xBE;
}

bool is_iso_char(unsigned char c) {
  return is_valid_ascii_char(c) || (c >= 160 && c <= 255);
}

bool is_valid_utf8_one_byte_char(unsigned char c) {
  return (VALID_ONE_BYTE_UTF_8 ^ c) >= 128;
}

bool is_valid_utf8_two_byte_char(unsigned char c1, unsigned char c2) {
  return 
    (VALID_TWO_BYTE_UTF_8 ^ c1)    >= 224 && 
    (VALID_UTF_8_FOLLOW_BYTE ^ c2) >= 192;
}

bool is_valid_utf8_three_byte_char(unsigned char c1, unsigned char c2, unsigned char c3) {
  return 
    (VALID_THREE_BYTE_UTF_8 ^ c1)  >= 240 && 
    (VALID_UTF_8_FOLLOW_BYTE ^ c2) >= 192 && 
    (VALID_UTF_8_FOLLOW_BYTE ^ c3) >= 192;
}

bool is_valid_ascii_char(unsigned char c) {
  return (c >= 0x07 && c <= 0x0D) || 
         (c == 0x1B) || 
         (c >= 0x20 && c <= 0x7E);
}

bool is_valid_utf8_four_byte_char(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4) {
  return 
  (VALID_FOUR_BYTE_UTF_8 ^ c1)   >= 248 && 
  (VALID_UTF_8_FOLLOW_BYTE ^ c2) >= 192 &&
  (VALID_UTF_8_FOLLOW_BYTE ^ c3) >= 192 &&
  (VALID_UTF_8_FOLLOW_BYTE ^ c4) >= 192;
}

bool is_ascii_file(const unsigned char* bytes, int length) {
  
  for(int i = 0; i < length; i++) {
    if (!is_valid_ascii_char(bytes[i])) 
      return false;
  }

  return true;
}

bool is_iso_file(const unsigned char* bytes, int length) {
  for (int i = 0; i < length; i++)
    if (!is_iso_char(bytes[i]))
      return false; 

  return true;
}

bool is_utf_file(const unsigned char* bytes, int length) {
  int idx = 0; 

  while (idx < length) {
    //printf("%d\n", bytes[idx]);

    if (length - idx >= 4) {
      if (bytes[idx] == 0x00 || bytes[idx+1] == 0x00 || bytes[idx+2] == 0x00 || bytes[idx+3] == 0x00) return false;
      if (is_valid_utf8_four_byte_char(bytes[idx], bytes[idx+1], bytes[idx+2], bytes[idx+3])) {
        idx += 4;
        continue;
      }
    }

    if (length - idx >= 3) {
      if (bytes[idx] == 0x00 || bytes[idx+1] == 0x00 || bytes[idx+2] == 0x00) return false;
      if (is_valid_utf8_three_byte_char(bytes[idx], bytes[idx+1], bytes[idx+2])) {
        idx += 3;
        continue;
      }
    }

    if (length - idx >= 2) {
      if (bytes[idx] == 0x00 || bytes[idx+1] == 0x00) return false;
      if (is_valid_utf8_two_byte_char(bytes[idx], bytes[idx+1])) {
        idx += 2;
        continue;
      }
    }

    if (bytes[idx] == 0x00) return false;

    if (is_valid_utf8_one_byte_char(bytes[idx])) {
      idx++;
      continue;
    }

    return false;
  }

  return true;

}

bool is_private_ssh_key_file(const unsigned char* bytes, int length) {
  if (length < 35) return false;

  unsigned char known_signature[] = "-----BEGIN OPENSSH PRIVATE KEY-----";

  for (int i = 0; i < 35; i++) {
    if ((char)bytes[i] != known_signature[i]) return false;
  }

  return true;
}

bool is_pcapng_file(const unsigned char* bytes, int length) {
  if (length < 4) return false;

  return (bytes[0] == 0x0A && bytes[1] == 0x0D && bytes[2] == 0x0D && bytes[3] == 0x0A)
}
