/***************************************************************************/
/*                                pal_ord4.c                                */
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "winsuport2.h"
#include "memoria.h" 
#include "semafor.c"
#include "semafor.h"
#include "missatge.c"
#include "missatge.h"

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

int id_sem_pantalla, id_sem_global, n_paletes, retard;
int *p_busties;

int main(int argc, char *argv[]) {
    int f_h;
    int id_semafor, id_bustia;
    char mis[2];
    int mem_id;
    int index, desplacament;
    char pos, mis[4], mis_aux[4];
    
    int paleta_index = atoi(argv[1]);

    mem_id = atoi(argv[2]);
    mem_comu *parametres_pal;
    parametres_pal = map_mem(mem_id);

    int tauler = atoi(argv[3]);
    int *p_tauler = map_mem(tauler);

    //id_semafor = atoi(argv[4]);

    //id_bustia = atoi(argv[5]);

    fprintf(stderr, "%i", parametres_pal->l_pal);
   
    //index = (atoi(ll_args[12]))-1;

    id_sem_pantalla = atoi(argv[4]);

    id_sem_global = atoi(argv[5]);

    p_busties = map_mem(atoi(ll_args[16]));

    n_paletes = atoi(ll_args[17]);

    win_set(p_tauler, parametres_pal->n_fil, parametres_pal->n_col);

    do
    {
      
      f_h = parametres_pal->paletes_vect[paleta_index].po_pf + parametres_pal->paletes_vect[paleta_index].v_pal; /* posicio hipotetica de la paleta */
      if (f_h != parametres_pal->paletes_vect[paleta_index].ipo_pf) /* si pos. hipotetica no coincideix amb pos. actual */
      {
        if (parametres_pal->paletes_vect[paleta_index].v_pal > 0.0) /* verificar moviment cap avall */
        {
          waitS(id_semafor);
          if (win_quincar(f_h + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc) == ' ') /* si no hi ha obstacle */
          {
            win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf, parametres_pal->paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); /* esborra primer bloc */
            parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal;
            parametres_pal->paletes_vect[paleta_index].ipo_pf = parametres_pal->paletes_vect[paleta_index].po_pf; /* actualitza posicio */
            win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc, '1'+paleta_index, INVERS); /* impr. ultim bloc */
            sendM(id_bustia, mis, 2);
            if (parametres_pal->moviments > 0) parametres_pal->moviments--;    /* he fet un moviment de la paleta */
            signalS(id_semafor);
          }
          else /* si hi ha obstacle, canvia el sentit del moviment */
            waitS(id_sem_pantalla);
            parametres_pal->paletes_vect[paleta_index].v_pal = -parametres_pal->paletes_vect[paleta_index].v_pal;
            signalS(id_semafor);
        }
        else /* verificar moviment cap amunt */
        {
          waitS(id_sem_pantalla);
          if (win_quincar(f_h, parametres_pal->paletes_vect[paleta_index].ipo_pc) == ' ') /* si no hi ha obstacle */
          {
            win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); /* esbo. ultim bloc */
            parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal;
            parametres_pal->paletes_vect[paleta_index].ipo_pf = parametres_pal->paletes_vect[paleta_index].po_pf; /* actualitza posicio */
            win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf, parametres_pal->paletes_vect[paleta_index].ipo_pc, '1' +paleta_index, INVERS); /* impr. primer bloc */
            if (parametres_pal->moviments > 0) parametres_pal->moviments--;    /* he fet un moviment de la paleta */
            signalS(id_sem_pantalla);
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
//}