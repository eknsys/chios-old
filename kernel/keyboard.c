#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

char keyboard_read() {
    uint8_t status = inb(0x64);
    if (!(status & 1)) return 0;
    if (status & 0x20) return 0;

    uint8_t sc = inb(0x60);

    if (sc > 127) return 0;

    static char map[128] = {
        0,27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
        'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
        'z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',0
    };

    return map[sc];
}
