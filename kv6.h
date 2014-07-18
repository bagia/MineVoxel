#ifndef KV6_H
#define KV6_H

#define MAXXSIZ 256
#define MAXYSIZ 256
#define BUFZSIZ 256
#define LIMZSIZ 255
#define MAXVOXS 1048576

typedef struct
{
    char z, col, vis, dir;
} voxtype;

int kv6_load (const char *filnam);
long kv6_xsiz();
long kv6_ysiz();
long kv6_zsiz();
unsigned long kv6_total_visible_blocks();
unsigned short ***kv6_points();
void kv6_free();

#endif // KV6_H
