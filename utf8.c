#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* ASCII byte:        0000 0000 - 0111 1111 is   0 - 127 */
/* Continuation byte: 1000 0000 - 1011 1111 is 128 - 191 */
/* Two-byte:          1100 0000 - 1101 1111 is 192 - 223 */
/* Three-byte:        1110 0000 - 1110 1111 is 224 - 239 */
/* Four-byte:         1111 0000 - 1111 0111 is 240 - 247 */

/* ASCII byte       : Type 0:  00 - 7F */
/* Continuation byte: Type 1:  80 - 8F */
/* Continuation byte: Type 2:  90 - 9F */
/* Continuation byte: Type 3:  A0 - BF */
/* Error byte       : Type 4:  C0 - C1 */
/* Two-byte         : Type 5:  C2 - DF */
/* Three-byte       : Type 6:  E0      */
/* Three-byte       : Type 7:  E1 - EC */
/* Three-byte       : Type 8:  ED      */
/* Three-byte       : Type 7:  EE - EF */
/* Four-byte        : Type 9:  F0      */
/* Four-byte        : Type 10: F1 - F3 */
/* Four-byte        : Type 11: F4      */
/* Four-byte        : Type 12: F5 - FF */

uint8_t bytetype[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 00 - 0F
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 10 - 1F
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 20 - 2F
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 30 - 3F
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 40 - 4F
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 50 - 5F
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 60 - 6F
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 70 - 7F
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 80 - 8F
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 90 - 9F
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // A0 - AF
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // B0 - BF
  4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, // C0 - CF
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, // D0 - DF
  6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 7, 7, // E0 - EF
  9, 10, 10, 10, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, // F0 - FF
};

//   0   1   2   3   4   5   6   7   8   9  10  11  12
int8_t states[][13] = {
  {  0, -1, -1, -1, -1,  1,  2,  3,  7,  4,  5,  6, -1 }, // 0
  { -1,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 1
  { -1, -1, -1,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 2
  { -1,  1,  1,  1, -1, -1, -1, -1, -1  -1, -1, -1, -1 }, // 3
  { -1, -1,  3,  3, -1, -1, -1 ,-1, -1, -1, -1, -1, -1 }, // 4
  { -1,  3,  3,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 5
  { -1,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 6
  { -1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 7
};

void decode(int8_t* state, uint32_t* cp, unsigned char b) {
  *state = states[*state][bytetype[b]];

  int andnums[13] = { 0xFF, 0x7F, 0x7F, 0x7F, -1, 0x3F, 0x1F, 0x1F, 0x1F, 0x0F, 0x0F, 0x0F, -1 };

  if (bytetype[b] > 0 && bytetype[b] < 4) {
    *cp = (*cp << 6) | (b & 0x7F);
  } else {
    *cp = b & andnums[bytetype[b]];
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Incorrect number of arguments\n");
    exit(1);
  }

  size_t i;
  int8_t state = 0;
  char* string = argv[1];
  uint32_t cp = 0;
  int length = 0;

  for (i = 0; i < strlen(string); i++) {
    decode(&state, &cp, string[i]);
    if (state == 0) {
      printf("Code point: U+%X\n", cp);
    } else if (state < 0) {
      printf("Failed at index %zu with state %i\n", i, state);
      exit(1);
    }
  }

  return 0;
}
