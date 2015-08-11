/**
 *  @file    dx-math.c
 *  @author  Nathan Baker
 *  @brief   Arithmetic with DX files
 *  @version $Id$
 */

#include "apbs.h"

#define IJK(i,j,k)  (((k)*(nx)*(ny))+((j)*(nx))+(i))
#define ERRRC 13
#define DXM_MAXOP 20
#define DXM_ISGRID 0
#define DXM_ISSCALAR 1

VEMBED(rcsid="$Id$")

typedef enum Dxmath_Opcode {
    DXM_ADD,  /**< + */
    DXM_SUB,  /**< - */
    DXM_DIV,  /**< / */
    DXM_MUL,  /**< * */
    DXM_EQU,  /**< = */
    DXM_EXP   /**< ^ */
} Dxmath_Opcode;

int main(int argc, char **argv) {

    /* *************** VARIABLES ******************* */
    char *input_path;
    char *MCwhiteChars = " \t\n";
    char *MCcommChars  = "#%";
    char tok[VMAX_BUFSIZE];
    char gridPath[DXM_MAXOP+1][VMAX_BUFSIZE];
    double scalar[DXM_MAXOP+1];
    int obType[DXM_MAXOP+1];
    int iop, numop;
    int i, nx, ny, nz, len;
    Dxmath_Opcode op[DXM_MAXOP];
    Vio *sock = VNULL;
    Vgrid *grid1 = VNULL;
    Vgrid *grid2 = VNULL;

    char *header = "\n\n\
    ----------------------------------------------------------------------\n\
    ----------------------------------------------------------------------\n\
    \n\n";

    char *usage = "\n\n\
    ----------------------------------------------------------------------\n\
    This driver program (like its UHBD counterpart) does simple arithmetic\n\
    with Cartesian grid data.  It is invoked as:\n\n\
      dx-math <path>\n\n\
    where <path> is the path to a file with operations specified\n\
    in a stack-based (RPN) manner.  For example, a command file which adds\n\
    grid1 and grid2, multiplies the result by 5.3, adds grid4, subtracts\n\
    99.3 from the whole thing, and writes the result on grid5 would have the\n\
    form:\n\n\
      grid1\n\
      grid2 +\n\
      5.3 *\n\
      grid4 +\n\
      99.3 -\n\
      grid5 =\n\n\
    where the file names, scalar values, and operations must be separated by\n\
    tabs, line breaks, or white space.  Comments can be included between the\n\
    character # and a new line (in the usual shell script fashion).\n\
    ----------------------------------------------------------------------\n\n";


    /* *************** CHECK INVOCATION ******************* */
    Vio_start();
    Vnm_print(1, "%s", header);
    if (argc != 2) {
        Vnm_print(2,"\n*** Syntax error: got %d arguments, expected 2.\n\n",
          argc);
        Vnm_print(2,"%s\n", usage);
        return ERRRC;
    }
    input_path = argv[1];

    /* *************** OPEN INPUT FILE ******************* */
    sock = Vio_ctor("FILE", "ASC", VNULL, input_path, "r");
    if (sock == VNULL) {
        Vnm_print(2, "main:  Null socket!\n");
        return ERRRC;
    }
    if (Vio_accept(sock, 0) < 0) {
        Vnm_print(2, "main:  Problem reading from socket!\n");
        return 0;
    }
    Vio_setWhiteChars(sock, MCwhiteChars);
    Vio_setCommChars(sock, MCcommChars);

    /* *************** PARSE INPUT FILE ******************* */
    /* After reading in the first arg, we should alternate between objects and
     * operations, starting with the objects.  For each opject, we assign a
     * type (0 = grid path, 1 = scalar) */
    if (Vio_scanf(sock, "%s", tok) != 1) {
        Vnm_print(2, "main:  Ran out of tokens when parsing initial input!\n");
        return ERRRC;
    }
    strncpy(gridPath[0], tok, VMAX_BUFSIZE);
    obType[0] = DXM_ISGRID;
    numop = 0;
    while (Vio_scanf(sock, "%s", tok) == 1) {
        if (sscanf(tok, "%lg", &scalar[numop+1]) == 1) {
            obType[numop+1] = DXM_ISSCALAR;
        } else {
            strncpy(gridPath[numop+1], tok, VMAX_BUFSIZE);
            obType[numop+1] = DXM_ISGRID;
        }
        if (Vio_scanf(sock, "%s", tok) != 1) {
            Vnm_print(2, "main:  Ran out of tokens when parsing input!\n");
            Vnm_print(2, "main:  Last token = %s.\n", tok);
            return ERRRC;
        }
        if (strcmp(tok, "*") == 0) op[numop] = DXM_MUL;
        else if (strcmp(tok, "+") == 0) op[numop] = DXM_ADD;
        else if (strcmp(tok, "-") == 0) op[numop] = DXM_SUB;
        else if (strcmp(tok, "/") == 0) op[numop] = DXM_DIV;
        else if (strcmp(tok, "^") == 0) op[numop] = DXM_EXP;
        else if (strcmp(tok, "=") == 0) {
            op[numop] = DXM_EQU;
            numop++;
            break;
        } else {
            Vnm_print(2, "main:  Undefined operation '%s'!\n", tok);
            return ERRRC;
        }
        numop++;
        if (numop == DXM_MAXOP) {
            Vnm_print(2, "main:  Exceed maximum number of operations (%d)!\n",
              DXM_MAXOP);
            return ERRRC;
        }
    }
    Vio_acceptFree(sock);

    /* Spit out what we parsed: */
    Vnm_print(1, "main:  Performing following operations:\n");
    Vnm_print(1, "main:    %s \n", gridPath[0]);
    for (iop=0; iop<numop; iop++) {
        if (obType[iop+1] == DXM_ISGRID)
          Vnm_print(1, "main:    %s (grid) ", gridPath[iop+1]);
        else
          Vnm_print(1, "main:    %g (scalar) ", scalar[iop+1]);
        switch (op[iop]) {
            case DXM_MUL:
                Vnm_print(1, "*\n");
                break;
            case DXM_ADD:
                Vnm_print(1, "+\n");
                break;
            case DXM_SUB:
                Vnm_print(1, "-\n");
                break;
            case DXM_DIV:
                Vnm_print(1, "/\n");
                break;
            case DXM_EQU:
                Vnm_print(1, "=\n");
                break;
            case DXM_EXP:
                Vnm_print(1, "^\n");
                break;
            default:
                Vnm_print(2, "\nmain:  Unknown operation (%d)!", op[iop]);
                return ERRRC;
        }
    }

    /* *************** PARSE INPUT FILE ******************* */
    Vnm_print(1, "main:  Reading grid from %s...\n", gridPath[0]);
    grid1 = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, VNULL);
    if (!Vgrid_readDX(grid1, "FILE", "ASC", VNULL, gridPath[0])) {
        Vnm_print(2, "main:  Problem reading OpenDX-format grid from %s\n",
          gridPath[0]);
        return ERRRC;
    }

    nx = grid1->nx;
    ny = grid1->ny;
    nz = grid1->nz;
    len = nx * ny * nz;

    for (iop=0; iop<numop-1; iop++) {
        if (obType[iop+1] == DXM_ISGRID) {
            Vnm_print(1, "main:  Reading grid from %s...\n", gridPath[iop+1]);
            grid2 = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, VNULL);
            if (!Vgrid_readDX(grid2, "FILE", "ASC", VNULL, gridPath[iop+1])) {
        Vnm_print(2, "main:  Problem reading OpenDX-format grid from \
%s\n", gridPath[0]);
                return ERRRC;
            }
            if ((grid2->nx != nx) || (grid2->ny != ny) || (grid2->nz != nz)) {
                Vnm_print(2, "main:  Grid dimension mis-match!\n");
                Vnm_print(2, "main:  Grid 1 is %d x %d x %d\n", nx, ny, nz);
                Vnm_print(2, "main:  Grid 2 is %d x %d x %d\n",
                  grid2->nx, grid2->ny, grid2->nz);
                return ERRRC;
            }

            switch (op[iop]) {
                case DXM_ADD:
                    Vnm_print(1, "main:  Adding...\n");
                    for (i=0; i<len; i++)
                      grid1->data[i] = grid1->data[i] + grid2->data[i];
                    break;
                case DXM_MUL:
                    Vnm_print(1, "main:  Multiplying...\n");
                    for (i=0; i<len; i++)
                      grid1->data[i] = grid1->data[i] * grid2->data[i];
                    break;
                case DXM_SUB:
                    Vnm_print(1, "main:  Subtracting...\n");
                    for (i=0; i<len; i++)
                      grid1->data[i] = grid1->data[i] - grid2->data[i];
                    break;
                case DXM_DIV:
                    Vnm_print(1, "main:  Dividing...\n");
                    for (i=0; i<len; i++)
                      grid1->data[i] = grid1->data[i] / grid2->data[i];
                    break;
                case DXM_EXP:
                    Vnm_print(1, "main:  Applying exponents...\n");
                    for(i = 0; i < grid2->data[i]; i++) {
                        grid1->data[i] *= grid1->data[i];
                    }
                    break;
                default:
                    Vnm_print(2, "main:  Unexpected operation (%d)!\n",
                      op[iop]);
                    break;
            }
            Vgrid_dtor(&grid2);
        } else { /* if (obType[iop+1] == DXM_ISGRID) */
            Vnm_print(1, "main:  Loading scalar %g...\n", scalar[iop+1]);
            switch (op[iop]) {
                case DXM_ADD:
                    Vnm_print(1, "main:  Adding...\n");
                    for (i=0; i<len; i++)
                      grid1->data[i] = grid1->data[i] + scalar[iop+1];
                    break;
                case DXM_MUL:
                    Vnm_print(1, "main:  Multiplying...\n");
                    for (i=0; i<len; i++)
                      grid1->data[i] = grid1->data[i] * scalar[iop+1];
                    break;
                case DXM_SUB:
                    Vnm_print(1, "main:  Subtracting...\n");
                    for (i=0; i<len; i++)
                      grid1->data[i] = grid1->data[i] - scalar[iop+1];
                    break;
                case DXM_DIV:
                    Vnm_print(1, "main:  Dividing...\n");
                    for (i=0; i<len; i++)
                      grid1->data[i] = grid1->data[i] / scalar[iop+1];
                    break;
                case DXM_EXP:
                    Vnm_print(1, "main:  Applying exponents...\n");
                    for(i = 0; i < scalar[iop+1]; i++) {
                        grid1->data[i] *= grid1->data[i];
                    }
                    break;
                default:
                    Vnm_print(2, "main:  Unexpected operation (%d)!\n",
                      op[iop]);
                    break;
            }
        } /* if (obType[iop+1] == DXM_ISGRID) */
    } /* for (iop=0; iop<numop-1; iop++) */

    /* The last operation is the = sign, implying that we write out the grid */
    Vnm_print(1, "main:  Writing results to %s...\n", gridPath[numop]);
    Vgrid_writeDX(grid1, "FILE", "ASC", VNULL, gridPath[numop],
      "DXMATH RESULTS", VNULL);

    Vnm_print(1, "main:  All done -- exiting.\n");
    return 0;
}
