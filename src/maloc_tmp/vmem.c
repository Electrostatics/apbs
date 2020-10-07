/*
 * ***************************************************************************
 * MALOC = < Minimal Abstraction Layer for Object-oriented C >
 * Copyright (C) 1994-- Michael Holst
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * rcsid="$Id: vmem.c,v 1.16 2010/08/12 05:40:31 fetk Exp $"
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * File:     vmem.c
 *
 * Purpose:  Class Vmem: methods.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */

#include "vmem.h"

/* total and misc (default) malloc/free tracking */
VPRIVATE Vmem vmemTotal, vmemMisc;
VPRIVATE size_t vmemInit=0;

/*
 * ***************************************************************************
 * Class Vmem: Inlineable methods
 * ***************************************************************************
 */
#if !defined(VINLINE_MALOC)

#endif /* if !defined(VINLINE_MALOC) */
/*
 * ***************************************************************************
 * Class Vmem: Non-inlineable methods
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * Routine:  Vmem_init
 *
 * Purpose:  Initialize the total log and the catch-all misc log.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void Vmem_init(void)
{
    if (!vmemInit) {

        strncpy(vmemTotal.name, "TOTAL", VMAX_ARGLEN);
        vmemTotal.mallocBytes = 0;
        vmemTotal.freeBytes = 0;
        vmemTotal.highWater = 0;
        vmemTotal.mallocAreas = 0;

        strncpy(vmemMisc.name, "MISC", VMAX_ARGLEN);
        vmemMisc.mallocBytes = 0;
        vmemMisc.freeBytes = 0;
        vmemMisc.highWater = 0;
        vmemMisc.mallocAreas = 0;

        vmemInit = 1;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vmem_bytesTotal
 *
 * Purpose:  Return total size of ALL currently ACTIVE malloc areas that
 *           went through Vmem_malloc.  This is the current memory footprint.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC size_t Vmem_bytesTotal(void)
{
    Vmem_init();
    return (vmemTotal.mallocBytes - vmemTotal.freeBytes);
}

/*
 * ***************************************************************************
 * Routine:  Vmem_mallocBytesTotal
 *
 * Purpose:  Return total size of ALL malloc areas that went through
 *           Vmem_malloc (even if they have been subsequently freed).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC size_t Vmem_mallocBytesTotal(void)
{
    Vmem_init();
    return vmemTotal.mallocBytes;
}

/*
 * ***************************************************************************
 * Routine:  Vmem_freeBytesTotal
 *
 * Purpose:  Return total size of ALL freed malloc areas that
 *           went through Vmem_free.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC size_t Vmem_freeBytesTotal(void)
{
    Vmem_init();
    return vmemTotal.freeBytes;
}

/*
 * ***************************************************************************
 * Routine:  Vmem_highWaterTotal
 *
 * Purpose:  Return the high-water malloc bytemark hit by ALL active
 *           malloc areas; this is the largest active malloc total hit at
 *           one time by areas obtained through Vmem_malloc.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC size_t Vmem_highWaterTotal(void)
{
    Vmem_init();
    return vmemTotal.highWater;
}

/*
 * ***************************************************************************
 * Routine:  Vmem_mallocAreasTotal
 *
 * Purpose:  Return the total number of ALL individual active malloc areas.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC size_t Vmem_mallocAreasTotal(void)
{
    Vmem_init();
    return vmemTotal.mallocAreas;
}

/*
 * ***************************************************************************
 * Routine:  Vmem_printTotal
 *
 * Purpose:  Print current memory statistics for all malloc areas that have
 *           gone through Vmem_malloc and Vmem_free.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vmem_printTotal(void)
{
    Vmem_init();
    fprintf(stderr,"%12ld %12ld %12ld %12ld %12ld %% %s\n",
        (vmemTotal.mallocBytes-vmemTotal.freeBytes),
        vmemTotal.mallocAreas, vmemTotal.mallocBytes,
        vmemTotal.freeBytes, vmemTotal.highWater,
        vmemTotal.name);
}

/*
 * ***************************************************************************
 * Routine:  Vmem_ctor
 *
 * Purpose:  Construct the dynamic memory allocation logging object.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC Vmem *Vmem_ctor(char *name)
{
    Vmem *thee;

    thee = Vmem_malloc( VNULL, 1, sizeof(Vmem) );
    VASSERT( thee != VNULL );

    strncpy( thee->name, name, VMAX_ARGLEN );
    thee->mallocBytes = 0;
    thee->freeBytes = 0;
    thee->highWater = 0;
    thee->mallocAreas = 0;

    return thee;
}

/*
 * ***************************************************************************
 * Routine:  Vmem_dtor
 *
 * Purpose:  Destruct the dynamic memory allocation logging object.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vmem_dtor(Vmem **thee)
{
    Vmem_free( VNULL, 1, sizeof(Vmem), (void**)thee );
}

/*
 * ***************************************************************************
 * Routine:  Vmem_malloc
 *
 * Purpose:  A logged version of malloc.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void *Vmem_malloc(Vmem *thee, size_t num, size_t size)
{
    size_t btmp;
    void *ram = VNULL;

    Vmem_init();

    /* VWARN( (num > 0) && (size > 0) ); */
    VASSERT( (num > 0) && (size > 0) );
    if ( (num > 0) && (size > 0) ) {

        ram = (void*)calloc((size_t)num, (size_t)size);
        if (ram == VNULL) {
            fprintf(stderr, "Unable to allocate memory!\n");
            fprintf(stderr, "(This often means you don't have enough memory available for this calculation.)\n");
        }
        VASSERT( ram != VNULL );

        vmemTotal.mallocBytes += (num * size);
        btmp = (vmemTotal.mallocBytes - vmemTotal.freeBytes);
        if ( vmemTotal.highWater < btmp ) vmemTotal.highWater = btmp;
        vmemTotal.mallocAreas++;

        if (thee != VNULL) {
            thee->mallocBytes += (num * size);
            btmp = (thee->mallocBytes - thee->freeBytes);
            if ( thee->highWater < btmp ) thee->highWater = btmp;
            thee->mallocAreas++;
        } else {
            vmemMisc.mallocBytes += (num * size);
            btmp = (vmemMisc.mallocBytes - vmemMisc.freeBytes);
            if ( vmemMisc.highWater < btmp ) vmemMisc.highWater = btmp;
            vmemMisc.mallocAreas++;
        }

    }

    return ram;
}

/*
 * ***************************************************************************
 * Routine:  Vmem_free
 *
 * Purpose:  A logged version of free.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vmem_free(Vmem *thee, size_t num, size_t size, void **ram)
{
    Vmem_init();

    /* VWARN( (*ram) != VNULL ); */
    VASSERT( (*ram) != VNULL );
    if ((*ram) != VNULL) {

        free(*ram);
        (*ram) = VNULL;

        vmemTotal.freeBytes += (num * size);
        vmemTotal.mallocAreas--;

        if (thee != VNULL) {
            thee->freeBytes += (num * size);
            thee->mallocAreas--;
        } else {
            vmemMisc.freeBytes += (num * size);
            vmemMisc.mallocAreas--;
        }
    }
}

/*
 * ***************************************************************************
 * Routine:  Vmem_realloc
 *
 * Purpose:  A logged version of realloc (using this is usually a bad idea).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void *Vmem_realloc(Vmem *thee, size_t num, size_t size, void **ram,
    size_t newNum)
{
    void *tee = Vmem_malloc(thee, newNum, size);
    memcpy(tee, (*ram), size*VMIN2(num,newNum));
    Vmem_free(thee, num, size, ram);
    return tee;
}

/*
 * ***************************************************************************
 * Routine:  Vmem_bytes
 *
 * Purpose:  Return total of ACTIVE malloc areas used by the Vmem object.
 *           (If Vmem is VNULL, return the misc catch-all malloc total).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC size_t Vmem_bytes(Vmem *thee)
{
    Vmem_init();

    if (thee != VNULL) {
        return (thee->mallocBytes - thee->freeBytes);
    } else {
        return (vmemMisc.mallocBytes - vmemMisc.freeBytes);
    }
}

/*
 * ***************************************************************************
 * Routine:  Vmem_mallocBytes
 *
 * Purpose:  Return total of all mallocs performed by the Vmem object.
 *           (If Vmem is VNULL, return the misc catch-all malloc total).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC size_t Vmem_mallocBytes(Vmem *thee)
{
    Vmem_init();

    if (thee != VNULL) {
        return thee->mallocBytes;
    } else {
        return vmemMisc.mallocBytes;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vmem_freeBytes
 *
 * Purpose:  Return total of the frees performed by the Vmem object.
 *           (If Vmem is VNULL, return misc catch-all free total).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC size_t Vmem_freeBytes(Vmem *thee)
{
    Vmem_init();

    if (thee != VNULL) {
        return thee->freeBytes;
    } else {
        return vmemMisc.freeBytes;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vmem_highWater
 *
 * Purpose:  Return the high-water malloc bytemark hit by the Vmem object.
 *           (If Vmem is VNULL, return misc catch-all malloc highwater).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC size_t Vmem_highWater(Vmem *thee)
{
    Vmem_init();

    if (thee != VNULL) {
        return thee->highWater;
    } else {
        return vmemMisc.highWater;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vmem_mallocAreas
 *
 * Purpose:  Return the total number of individual active malloc areas.
 *           (If Vmem is VNULL, return misc catch-all malloc areas).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC size_t Vmem_mallocAreas(Vmem *thee)
{
    Vmem_init();

    if (thee != VNULL) {
        return thee->mallocAreas;
    } else {
        return vmemMisc.mallocAreas;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vmem_print
 *
 * Purpose:  Print current memory statistics associated with this Vmem object.
 *           (If Vmem is VNULL, print info for the catch-all ovject).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vmem_print(Vmem *thee)
{
    Vmem_init();

    if (thee != VNULL) {
        fprintf(stderr,"%12ld %12ld %12ld %12ld %12ld %% %s\n",
            (thee->mallocBytes-thee->freeBytes),
            thee->mallocAreas, thee->mallocBytes,
            thee->freeBytes, thee->highWater,
            thee->name);
    } else {
        fprintf(stderr,"%12ld %12ld %12ld %12ld %12ld %% %s\n",
            (vmemMisc.mallocBytes-vmemMisc.freeBytes),
            vmemMisc.mallocAreas, vmemMisc.mallocBytes,
            vmemMisc.freeBytes, vmemMisc.highWater,
            vmemMisc.name);
    }
}

