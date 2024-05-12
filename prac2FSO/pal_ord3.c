/***************************************************************************/
/*                                pal_ord3.c                                */
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "winsuport2.h"
#include "memoria.h" 

#define MIN_VEL -1.0
#define MAX_VEL 1.0
#define MIN_RET 0.0
#define MAX_RET 5.0

typedef struct {
    int ipo_pf, ipo_pc;
    float v_pal, pal_ret, po_pf;
} p_ordinador;

void mou_paleta_ordinador(int ipo_pf, int ipo_pc, float v_pal, float pal_ret) {
    int f_h;
    do {
        f_h = ipo_pf + v_pal;
        if (f_h != ipo_pf) {
            if (win_quincar(f_h, ipo_pc) == ' ') {
                win_escricar(ipo_pf, ipo_pc, ' ', NO_INV);
                ipo_pf += v_pal;
                win_escricar(ipo_pf, ipo_pc, '1', INVERS);
            } else {
                v_pal = -v_pal;
            }
        } else {
            ipo_pf += v_pal;
            win_retard(100);
        }
    } while (1);
}

int main(int argc, char *argv[]) {
    int ipo_pf, ipo_pc;
    float v_pal, pal_ret;

    if (argc != 6) {
        fprintf(stderr, "Ús: %s ipo_pf ipo_pc v_pal pal_ret\n", argv[0]);
        exit(1);
    }

    ipo_pf = atoi(argv[1]);
    ipo_pc = atoi(argv[2]);
    v_pal = atof(argv[3]);
    pal_ret = atof(argv[4]);

    mou_paleta_ordinador(ipo_pf, ipo_pc, v_pal, pal_ret);

    return 0;
}
