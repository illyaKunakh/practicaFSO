/***************************************************************************/
/*                                pal_ord4.c                                */
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "winsuport2.h"
#include "memoria.h" 
#include "semafor.h"
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
    int n_pal;
} mem_comu;

int cua(int cua[], int pal, int max) 
{
  for(int i = 0; i < max;i++) 
  {
    if(cua[i] == pal) return 1;
  }
  return 0;
}


int main(int argc, char *argv[]) 
{
    
    int pos, j, mou, id_semafor_pantalla, id_semafor_moviments, desplasa_dir, *id_busties;
    char mis[4], mis_aux[4];
    int mem_id;

    int paleta_index = atoi(argv[1]);

    mem_id = atoi(argv[2]);
    mem_comu *parametres_pal;
    parametres_pal = map_mem(mem_id);

    int tauler = atoi(argv[3]);
    int *p_tauler = map_mem(tauler);

    id_semafor_pantalla = atoi(argv[4]);

    id_semafor_moviments = atoi(argv[5]);

    id_busties = map_mem(atoi(argv[6]));

    int cua_paletes[parametres_pal->l_pal];
    win_set(p_tauler, parametres_pal->n_fil, parametres_pal->n_col);

//fprintf(stderr, "%i", mem_id);
fprintf(stderr, "%i", parametres_pal->l_pal);
  int f_h;
  do
  {
    if(id_busties[paleta_index] == -1) continue;
    receiveM(id_busties[paleta_index], mis);

    if(atoi(mis) == 0) desplasa_dir = 0;
    else if(atoi(mis) == 1) desplasa_dir = 1;
    else if(atoi(mis) == 2) desplasa_dir = -1;
    
    if (desplasa_dir != 0) 
    {
      waitS(id_semafor_pantalla);
      j = 0;
      for(int i = 0; i < parametres_pal->l_pal; i++) 
      {
        cua_paletes[i] = 0;
      }
      mou = 1;
      for (int i = 1; i <= parametres_pal->l_pal; i++) 
      {
        pos = win_quincar(  parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc + desplasa_dir);
        if(pos != ' ' && pos != '+') 
        {
          if(!cua(cua_paletes, (int)pos-48, i))
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
          if(id_busties[i] == +1) continue;
          if(cua(cua_paletes, i+1, j)) 
          {
            sprintf(mis_aux, "%s", mis);
          }
          else
          {
            sprintf(mis_aux, "%i", 0);
          }
          sendM(id_busties[i], mis_aux, 4);
        }
      }
      signalS(id_semafor_pantalla);
    }





    f_h = parametres_pal->paletes_vect[paleta_index].po_pf + parametres_pal->paletes_vect[paleta_index].v_pal; /* posicio hipotetica de la paleta */
    if (f_h != parametres_pal->paletes_vect[paleta_index].ipo_pf) /* si pos. hipotetica no coincideix amb pos. actual */
    {
      if (parametres_pal->paletes_vect[paleta_index].v_pal > 0.0) /* verificar moviment cap avall */
      {
        waitS(id_semafor_pantalla);
        if (win_quincar(f_h + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc) == ' ') /* si no hi ha obstacle */
        {
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf, parametres_pal->paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); /* esborra primer bloc */
          parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal;
          parametres_pal->paletes_vect[paleta_index].ipo_pf = parametres_pal->paletes_vect[paleta_index].po_pf; /* actualitza posicio */
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc, '1'+paleta_index, INVERS); /* impr. ultim bloc */
          signalS(id_semafor_pantalla);
          if (parametres_pal->moviments > 0) parametres_pal->moviments--;    /* he fet un moviment de la paleta */
        }
        else 
        {
          signalS(id_semafor_pantalla);
          parametres_pal->paletes_vect[paleta_index].v_pal = -parametres_pal->paletes_vect[paleta_index].v_pal; /* si hi ha obstacle, canvia el sentit del moviment */
        } 
          
      }
      else /* verificar moviment cap amunt */
      {
        waitS(id_semafor_pantalla);
        if (win_quincar(f_h, parametres_pal->paletes_vect[paleta_index].ipo_pc) == ' ') /* si no hi ha obstacle */
        {
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf + parametres_pal->l_pal - 1, parametres_pal->paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); /* esbo. ultim bloc */
          parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal;
          parametres_pal->paletes_vect[paleta_index].ipo_pf = parametres_pal->paletes_vect[paleta_index].po_pf; /* actualitza posicio */
          win_escricar(parametres_pal->paletes_vect[paleta_index].ipo_pf, parametres_pal->paletes_vect[paleta_index].ipo_pc, '1' +paleta_index, INVERS); /* impr. primer bloc */
          signalS(id_semafor_pantalla);
          if (parametres_pal->moviments > 0)
          {
           signalS(id_semafor_pantalla);
           waitS(id_semafor_moviments);
           parametres_pal->moviments--;    /* he fet un moviment de la paleta */
           signalS(id_semafor_moviments);
          } 
        }
        else /* si hi ha obstacle, canvia el sentit del moviment */
        {
          signalS(id_semafor_pantalla);
          parametres_pal->paletes_vect[paleta_index].v_pal = -parametres_pal->paletes_vect[paleta_index].v_pal;
        } 
          
      }
    }
    else{
      parametres_pal->paletes_vect[paleta_index].po_pf += parametres_pal->paletes_vect[paleta_index].v_pal; /* actualitza posicio vertical real de la paleta */
    }
    win_retard(parametres_pal->retard * parametres_pal->paletes_vect[paleta_index].pal_ret);

  } while (parametres_pal->finalJoc == 0);
  
    return 0;
}
