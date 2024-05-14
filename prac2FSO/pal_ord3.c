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
#define MAX_PALETES 9

typedef struct {
    int ipo_pf, ipo_pc;
    float v_pal, pal_ret, po_pf;
} p_ordinador;

typedef struct {
    p_ordinador paletes_vect[MAX_PALETES];
    int tecla;
    int finalJoc;
    int moviments;
    int retard;
    int n_fil, n_col;
    int l_pal;
    int movimentsIni;
} mem_comu;


int main(int argc, char *argv[]) {
    // int ipo_pf, ipo_pc;
    // float v_pal, pal_ret;
    int paleta_index = atoi(argv[1]);

    int mem_id;
    mem_id = atoi(argv[2]);
    mem_comu *parametres_pal;
    parametres_pal = map_mem(mem_id);

    int tauler = atoi(argv[3]);
    int *p_tauler = map_mem(tauler);


    win_set(p_tauler, parametres_pal->n_fil, parametres_pal->n_col);

//fprintf(stderr, "%i", mem_id);
fprintf(stderr, "%i", parametres_pal->l_pal);
  int f_h;
  do
  {
    
    f_h = parametres_pal->paletes_vect[paleta_index].po_pf + parametres_pal->paletes_vect[paleta_index].v_pal; /* posicio hipotetica de la paleta */
    if (f_h != parametres_pal->paletes_vect[paleta_index].ipo_pf) /* si pos. hipotetica no coincideix amb pos. actual */
    {
      if (parametres_pal->paletes_vect[paleta_index].v_pal > 0.0) /* verificar moviment cap avall */
      {
        if (win_quincar(f_h + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc) == ' ') /* si no hi ha obstacle */
        {
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf, parametres_pal->paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); /* esborra primer bloc */
          parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal;
          parametres_pal->paletes_vect[paleta_index].ipo_pf = parametres_pal->paletes_vect[paleta_index].po_pf; /* actualitza posicio */
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc, '1'+paleta_index, INVERS); /* impr. ultim bloc */
          if (parametres_pal->moviments > 0) parametres_pal->moviments--;    /* he fet un moviment de la paleta */
        }
        else /* si hi ha obstacle, canvia el sentit del moviment */
          parametres_pal->paletes_vect[paleta_index].v_pal = -parametres_pal->paletes_vect[paleta_index].v_pal;
      }
      else /* verificar moviment cap amunt */
      {
        if (win_quincar(f_h, parametres_pal->paletes_vect[paleta_index].ipo_pc) == ' ') /* si no hi ha obstacle */
        {
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); /* esbo. ultim bloc */
          parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal;
          parametres_pal->paletes_vect[paleta_index].ipo_pf = parametres_pal->paletes_vect[paleta_index].po_pf; /* actualitza posicio */
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf, parametres_pal->paletes_vect[paleta_index].ipo_pc, '1' +paleta_index, INVERS); /* impr. primer bloc */
          if (parametres_pal->moviments > 0) parametres_pal->moviments--;    /* he fet un moviment de la paleta */
        }
        else /* si hi ha obstacle, canvia el sentit del moviment */
          parametres_pal->paletes_vect[paleta_index].v_pal = -parametres_pal->paletes_vect[paleta_index].v_pal;
      }
    }
    else{
      parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal; /* actualitza posicio vertical real de la paleta */
      
    }
    win_retard(parametres_pal->retard * parametres_pal->paletes_vect[paleta_index].pal_ret);

  } while (parametres_pal->finalJoc == 0);
  
    return 0;
}
