
#ifndef IO_H
#define IO_H

// the key bits
#define IO_KEY_BIT_LEFT   (1 <<  0)
#define IO_KEY_BIT_RIGHT  (1 <<  1)
#define IO_KEY_BIT_UP     (1 <<  2)
#define IO_KEY_BIT_DOWN   (1 <<  3)
#define IO_KEY_BIT_A      (1 <<  4)
#define IO_KEY_BIT_B      (1 <<  5)
#define IO_KEY_BIT_X      (1 <<  6)
#define IO_KEY_BIT_Y      (1 <<  7)
#define IO_KEY_BIT_L      (1 <<  8)
#define IO_KEY_BIT_R      (1 <<  9)
#define IO_KEY_BIT_SELECT (1 << 10)
#define IO_KEY_BIT_START  (1 << 11)

// the current key bits
extern int ioKeyBits;

void ioInit(void);
void ioUpdate(void);

#endif
