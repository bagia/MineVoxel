/*
    The KV6 loading code is an extracted and modified version
    of the loadkv6() function of Ken Silverman's SLAB6 tool.
    http://advsys.net/ken/download.htm
*/
#include <stdio.h>
#include <stdlib.h>

#include "kv6.h"

static long xsiz, ysiz, zsiz;
static unsigned short ***points;
static unsigned long total = 0;

unsigned short ***kv6_points()
{
    return points;
}

long kv6_xsiz()
{
    return xsiz;
}

long kv6_ysiz()
{
    return ysiz;
}

long kv6_zsiz()
{
    return zsiz;
}

unsigned long kv6_total_visible_blocks()
{
    return total;
}

int kv6_load (const char *filnam)
{
    int retval = 1;
    float xpiv, ypiv, zpiv;
    long numvoxs; //sum of xlens and number of surface voxels
    unsigned short *xlen;
    unsigned short **ylen;
    voxtype *voxdata;
    char *fipalette;
    long i, j, x, y, z;
    unsigned short u;
    voxtype *vptr, *vend;
    FILE *fil;

    xlen = malloc(MAXXSIZ * sizeof(unsigned short));
    ylen = malloc(MAXXSIZ * sizeof(unsigned short*));
    for(i=0;i<MAXYSIZ;i++) ylen[i] = malloc(MAXYSIZ * sizeof(unsigned short));
    voxdata = malloc(MAXVOXS * sizeof(voxtype));
    fipalette = malloc(768 * sizeof(char));

    if (!(fil = fopen(filnam,"rb"))) goto kv6_load_exit_fail;
    fread(&i,4,1,fil);
    if (i != 0x6c78764b) goto kv6_load_exit_fail; //Kvxl
    fread(&xsiz,4,1,fil);
    fread(&ysiz,4,1,fil);
    fread(&zsiz,4,1,fil);
    if ((xsiz > MAXXSIZ) || (ysiz > MAXYSIZ) || (zsiz > LIMZSIZ))
    {
        fclose(fil);
        goto kv6_load_exit_fail;
    }
    fread(&xpiv,4,1,fil);
    fread(&ypiv,4,1,fil);
    fread(&zpiv,4,1,fil);
    fread(&numvoxs,4,1,fil);

    //Added 06/30/2007: suggested palette (optional)
    fseek(fil,32+numvoxs*8+(xsiz<<2)+((xsiz*ysiz)<<1),SEEK_SET);
    fread(&i,4,1,fil);
    if (i == 0x6c615053) //SPal
    {
        fread(fipalette,768,1,fil);
        fseek(fil,32,SEEK_SET);
    }
    else fseek(fil,32,SEEK_SET);

    for(vptr=voxdata,vend=&voxdata[numvoxs]; vptr<vend; vptr++)
    {
        fread(&i,4,1,fil);
        // We do not care about the color
        vptr->col = 255;
        vptr->z = fgetc(fil);
        fgetc(fil);
        vptr->vis = fgetc(fil);
        vptr->dir = fgetc(fil);
    }

    for(x=0; x<xsiz; x++)
    {
        fread(&i,4,1,fil);
        xlen[x] = i;
    }
    for(x=0; x<xsiz; x++)
        for(y=0; y<ysiz; y++)
        {
            fread(&u,2,1,fil);
            ylen[x][y] = u;
        }
    fclose(fil);

    vptr = voxdata;

    // Initialize the points tri-dimensional array:
    points = malloc(xsiz * sizeof(unsigned short**));
    for(x=0; x<xsiz; x++) points[x] = malloc(ysiz * sizeof(unsigned short*));
    for(x=0; x<xsiz; x++)
        for(y=0; y<ysiz; y++)
        {
            points[x][y] = malloc(zsiz *sizeof(unsigned short));
            for (z=0; z<zsiz; z++)
                points[x][y][z] = 0;
        }

    for(x=0; x<xsiz; x++)
    {
        for(y=0; y<ysiz; y++)
        {
            j = ((x*MAXYSIZ)+y)*BUFZSIZ;
            vend = &vptr[ylen[x][y]];
            for(; vptr<vend; vptr++)
            {
                // Whichever side of the cube is visible, I need the entire cube in Minecraft.
                if (vptr->vis)
                {
                    unsigned long index_z = vptr->z;
                    points[x][y][index_z] = 1;
                    total++;
                }
            } //z
        } //y
    } //x

kv6_load_exit:
    free(xlen);
    for(i=0;i<MAXYSIZ;i++) free(ylen[i]);
    free(ylen);
    free(voxdata);
    free(fipalette);
    return retval;

kv6_load_exit_fail:
    retval = 0;
    goto kv6_load_exit;
}

void kv6_free()
{
    long x,y;
    for(x=0; x<xsiz; x++)
        for(y=0; y<ysiz; y++)
        {
            free(points[x][y]);
        }
    for(x=0; x<xsiz; x++) free(points[x]);
    free(points);
}
