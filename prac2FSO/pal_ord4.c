/***************************************************************************/
/*                                pal_ord3.c                                */
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "winsuport2.h"
#include "memoria.h" 
#include "semafor.h"
#include "missatge.h"
#include <pthread.h>

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
    int n_pal;
} mem_comu;

char mis[2], mis_aux[2];
int *id_busties;
int paleta_index;

int cua(int cua[], int pal, int max) 
{
  for(int i = 0; i < max;i++) 
  {
    if(cua[i] == pal) return 1;
  }
  return 0;
}

void* rebre(void * arg)
{
  do
  {
    receiveM(id_busties[paleta_index+1], mis);
    fprintf(stderr, "%d ",paleta_index+1);
    fprintf(stderr, "recibido %c/ ",mis[0]);
    return NULL;
  } while (1);
}

int main(int argc, char *argv[]) {
    // int ipo_pf, ipo_pc;
    // float v_pal, pal_ret;
    pthread_t rebre_mis;
    int semPantalla, semMoviments, desplasa_dir, j, pos, mou;
    paleta_index = atoi(argv[1]);

    int mem_id;
    mem_id = atoi(argv[2]);
    mem_comu *parametres_pal;
    parametres_pal = map_mem(mem_id);

    int tauler = atoi(argv[3]);
    int *p_tauler = map_mem(tauler);

    semPantalla = atoi(argv[4]);

    semMoviments = atoi(argv[5]);

    id_busties = map_mem(atoi(argv[6]));

    int cua_paletes[parametres_pal->l_pal];

    win_set(p_tauler, parametres_pal->n_fil, parametres_pal->n_col);

    pthread_create(&rebre_mis, NULL, rebre, NULL);

//fprintf(stderr, "%i", parametres_pal->l_pal);
  int f_h;
  do
  {

    if(id_busties[paleta_index] == -1) continue;

    if(atoi(mis) == 0) desplasa_dir = 0;
    else if(atoi(mis) == 1) desplasa_dir = 1;
    else if(atoi(mis) == 2) desplasa_dir = -1;
    
    if (desplasa_dir != 0) 
    {
      waitS(semPantalla);
      waitS(semMoviments);
      mou = 1;
      j = 0;
      for(int i = 0; i < parametres_pal->l_pal; i++) 
      {
        cua_paletes[i] = 0;
      }
      for (int i = 1; i <= parametres_pal->l_pal; i++) 
      {
        pos = win_quincar(  parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc + desplasa_dir);
        if(pos != ' ' && pos != '+') 
        {
          if(cua(cua_paletes, (int)pos-48, i) != 1)
          {
            cua_paletes[j++] = (int)pos-48;
          }
          if(mou == 1) mou = 0;
        }
      }
      if(mou == 1) 
      {
        if(parametres_pal->paletes_vect[paleta_index].ipo_pc + desplasa_dir == (parametres_pal->n_col-1)) 
        {
          for(int i = 1; i <= parametres_pal->l_pal; i++)
          {
            win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - i, parametres_pal->paletes_vect[paleta_index].ipo_pc - i, ' ', NO_INV); 
            id_busties[paleta_index] = -1;
          }
        }
        else
        {
          for(int i = 1; i <= parametres_pal->l_pal; i++)
          {
            win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - i, parametres_pal->paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); 
            win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - i, parametres_pal->paletes_vect[paleta_index].ipo_pc + desplasa_dir, paleta_index, INVERS);
          }
          parametres_pal->paletes_vect[paleta_index].ipo_pc += desplasa_dir;
        }
      }
      else
      {
        for(int i = 0; i < parametres_pal->n_pal; i++) 
        {
          if(id_busties[i] == -1) continue;
          if(cua(cua_paletes, i+1, j) == 1) 
          {
            sprintf(mis_aux, "%s", mis);
          }
          else
          {
            sprintf(mis_aux, "%i", 0);
          }
          sendM(id_busties[i], mis_aux, 2);
        }
      }
      signalS(semMoviments);
      signalS(semPantalla);
    }
    
    f_h = parametres_pal->paletes_vect[paleta_index].po_pf + parametres_pal->paletes_vect[paleta_index].v_pal; /* posicio hipotetica de la paleta */
    if (f_h != parametres_pal->paletes_vect[paleta_index].ipo_pf) /* si pos. hipotetica no coincideix amb pos. actual */
    {
      if (parametres_pal->paletes_vect[paleta_index].v_pal > 0.0) /* verificar moviment cap avall */
      {
        waitS(semPantalla);
        if (win_quincar(f_h + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc) == ' ') /* si no hi ha obstacle */
        {
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf, parametres_pal->paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); /* esborra primer bloc */
          parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal;
          parametres_pal->paletes_vect[paleta_index].ipo_pf = parametres_pal->paletes_vect[paleta_index].po_pf; /* actualitza posicio */
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc, '1'+paleta_index, INVERS); /* impr. ultim bloc */
          signalS(semPantalla);
          waitS(semMoviments);
          if (parametres_pal->moviments > 0) parametres_pal->moviments--;    /* he fet un moviment de la paleta */
          signalS(semMoviments);
        }
        else /* si hi ha obstacle, canvia el sentit del moviment */
        {
          signalS(semPantalla);
          parametres_pal->paletes_vect[paleta_index].v_pal = -parametres_pal->paletes_vect[paleta_index].v_pal;
        }
          
      }
      else /* verificar moviment cap amunt */
      {
        waitS(semPantalla);
        if (win_quincar(f_h, parametres_pal->paletes_vect[paleta_index].ipo_pc) == ' ') /* si no hi ha obstacle */
        {
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); /* esbo. ultim bloc */
          parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal;
          parametres_pal->paletes_vect[paleta_index].ipo_pf = parametres_pal->paletes_vect[paleta_index].po_pf; /* actualitza posicio */
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf, parametres_pal->paletes_vect[paleta_index].ipo_pc, '1' +paleta_index, INVERS); /* impr. primer bloc */
          signalS(semPantalla);
          waitS(semMoviments);
          if (parametres_pal->moviments > 0) parametres_pal->moviments--;    /* he fet un moviment de la paleta */
          signalS(semMoviments);
        }
        else /* si hi ha obstacle, canvia el sentit del moviment */
        {
            signalS(semPantalla);
            parametres_pal->paletes_vect[paleta_index].v_pal = -parametres_pal->paletes_vect[paleta_index].v_pal;
        }
          
      }
    }
    else{
      parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal; /* actualitza posicio vertical real de la paleta */

    }
    win_retard(parametres_pal->retard * parametres_pal->paletes_vect[paleta_index].pal_ret);

  } while (parametres_pal->finalJoc == 0);
  
  pthread_join(rebre_mis, NULL);
    return 0;
}
