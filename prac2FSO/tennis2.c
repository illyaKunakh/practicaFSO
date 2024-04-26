/**************************/
/*                                Tennis1.c                                 */
/**************************/



#include <stdio.h>      /* incloure definicions de funcions estandard */
#include <stdint.h>
#include <stdlib.h>
#include "winsuport.h"      /* incloure definicions de funcions propies */
#include <pthread.h>

#define MIN_FIL 7
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80
#define MIN_PAL 3
#define MIN_VEL -1.0
#define MAX_VEL 1.0
#define MIN_RET 0.0
#define MAX_RET 5.0
#define MAX_PALETES 9

pthread_mutex_t pantallaMutex= PTHREAD_MUTEX_INITIALIZER; //Quan s'imprimeix per pantalla
pthread_mutex_t movimentsMutex= PTHREAD_MUTEX_INITIALIZER; //Decrementa moviments 

int n_fil, n_col, m_por;
int l_pal;
int ipu_pf, ipu_pc;
int ipil_pf, ipil_pc;
float pil_pf, pil_pc;
float pil_vf, pil_vc;
float pil_ret;

int retard;
int moviments;
int movimentsIni;
int finalJoc = 0;
int n_paletes = 0;
int cont = 0;
unsigned int temps_transcorregut = 0;
int pausa;

typedef struct {
    pthread_t thread_pc;
    int ipo_pf, ipo_pc;
    float v_pal, pal_ret, po_pf;
} p_ordinador;

p_ordinador paletes_vect[MAX_PALETES];

void carrega_parametres(const char *nom_fit) {
    FILE *fit;
    fit = fopen(nom_fit, "rt");
    if (fit == NULL) {
        fprintf(stderr, "No s'ha pogut obrir el fitxer \'%s\'\n", nom_fit);
        exit(2);
    }

    if (!feof(fit))
        fscanf(fit, "%d %d %d %d\n", &n_fil, &n_col, &m_por, &l_pal);
    if ((n_fil < MIN_FIL) || (n_fil > MAX_FIL) || (n_col < MIN_COL) || (n_col > MAX_COL) || (m_por < 0) ||
        (m_por > n_fil - 3) || (l_pal < MIN_PAL) || (l_pal > n_fil - 3)) {
        fprintf(stderr, "Error: dimensions del camp de joc incorrectes:\n");
        fprintf(stderr, "\t%d =< n_fil (%d) =< %d\n", MIN_FIL, n_fil, MAX_FIL);
        fprintf(stderr, "\t%d =< n_col (%d) =< %d\n", MIN_COL, n_col, MAX_COL);
        fprintf(stderr, "\t0 =< m_por (%d) =< n_fil-3 (%d)\n", m_por, (n_fil - 3));
        fprintf(stderr, "\t%d =< l_pal (%d) =< n_fil-3 (%d)\n", MIN_PAL, l_pal, (n_fil - 3));
        fclose(fit);
        exit(3);
    }

  if (!feof(fit)) fscanf(fit,"%d %d %f %f %f\n",&ipil_pf,&ipil_pc,&pil_vf,&pil_vc,&pil_ret);
  if ((ipil_pf < 1) || (ipil_pf > n_fil-3) ||
    (ipil_pc < 1) || (ipil_pc > n_col-2) ||
    (pil_vf < MIN_VEL) || (pil_vf > MAX_VEL) ||
    (pil_vc < MIN_VEL) || (pil_vc > MAX_VEL) ||
    (pil_ret < MIN_RET) || (pil_ret > MAX_RET))
  {
    fprintf(stderr,"Error: parametre pilota incorrectes:\n");
    fprintf(stderr,"\t1 =< ipil_pf (%d) =< n_fil-3 (%d)\n",ipil_pf,(n_fil-3));
    fprintf(stderr,"\t1 =< ipil_pc (%d) =< n_col-2 (%d)\n",ipil_pc,(n_col-2));
    fprintf(stderr,"\t%.1f =< pil_vf (%.1f) =< %.1f\n",MIN_VEL,pil_vf,MAX_VEL);
    fprintf(stderr,"\t%.1f =< pil_vc (%.1f) =< %.1f\n",MIN_VEL,pil_vc,MAX_VEL);
    fprintf(stderr,"\t%.1f =< pil_ret (%.1f) =< %.1f\n",MIN_RET,pil_ret,MAX_RET);
    fclose(fit);
    exit(4);
  }

  while ((n_paletes < MAX_PALETES)&&!feof(fit)) {
    fscanf(fit,"%d %d %f %f\n",&paletes_vect[n_paletes].ipo_pf,&paletes_vect[n_paletes].ipo_pc,&paletes_vect[n_paletes].v_pal,&paletes_vect[n_paletes].pal_ret);
    if ((paletes_vect[n_paletes].ipo_pf < 1) || (paletes_vect[n_paletes].ipo_pf+l_pal > n_fil-2) ||
    (paletes_vect[n_paletes].ipo_pc < 5) || (paletes_vect[n_paletes].ipo_pc > n_col-2) ||
    (paletes_vect[n_paletes].v_pal < MIN_VEL) || (paletes_vect[n_paletes].v_pal > MAX_VEL) ||
    (paletes_vect[n_paletes].pal_ret < MIN_RET) || (paletes_vect[n_paletes].pal_ret > MAX_RET))
    {
    fprintf(stderr,"Error: parametres paleta ordinador incorrectes:\n");
    fprintf(stderr,"\t1 =< ipo_pf (%d) =< n_fil-l_pal-3 (%d)\n",paletes_vect[n_paletes].ipo_pf,(n_fil-l_pal-3));
    fprintf(stderr,"\t5 =< ipo_pc (%d) =< n_col-2 (%d)\n",paletes_vect[n_paletes].ipo_pc,(n_col-2));
    fprintf(stderr,"\t%.1f =< v_pal (%.1f) =< %.1f\n",MIN_VEL,paletes_vect[n_paletes].v_pal,MAX_VEL);
    fprintf(stderr,"\t%.1f =< pal_ret (%.1f) =< %.1f\n",MIN_RET,paletes_vect[n_paletes].pal_ret,MAX_RET);
    fclose(fit);
    exit(5);
    }    
  n_paletes++;
  }
  fclose(fit); /* fitxer carregat: tot OK! */
}



/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
int inicialitza_joc(void) {
  int i, i_port, f_port, retwin;
  char strin[54];
  

  retwin = win_ini(&n_fil,&n_col,'+',INVERS);   /* intenta crear taulell */

  if (retwin < 0)       /* si no pot crear l'entorn de joc amb les curses */
  { fprintf(stderr,"Error en la creacio del taulell de joc:\t");
    switch (retwin)
    {   case -1: fprintf(stderr,"camp de joc ja creat!\n");
                 break;
        case -2: fprintf(stderr,"no s'ha pogut inicialitzar l'entorn de curses!\n");
         break;
        case -3: fprintf(stderr,"les mides del camp demanades son massa grans!\n");
                 break;
        case -4: fprintf(stderr,"no s'ha pogut crear la finestra!\n");
                 break;
     }
     return(retwin);
  }

  i_port = n_fil/2 - m_por/2;       /* crea els forats de la porteria */
  if (n_fil%2 == 0) i_port--;
  if (i_port == 0) i_port=1;
  f_port = i_port + m_por -1;
  for (i = i_port; i <= f_port; i++)
  { win_escricar(i,0,' ',NO_INV);
    win_escricar(i,n_col-1,' ',NO_INV);
  }


  ipu_pf = n_fil/2; ipu_pc = 3;     /* inicialitzar pos. paletes */
  if (ipu_pf+l_pal >= n_fil-3) ipu_pf = 1;
  for (i=0; i< l_pal; i++)      /* dibuixar paleta inicialment */
  { win_escricar(ipu_pf +i, ipu_pc, '0',INVERS);
  }



  paletes_vect[i].po_pf = paletes_vect[i].ipo_pf;       /* fixar valor real paleta ordinador */
  for(int j=0; j<n_paletes; j++){
    for(int i=0; i<l_pal; i++){
      win_escricar(paletes_vect[j].ipo_pf + i, paletes_vect[j].ipo_pc,('1' + j), INVERS);
    }
    paletes_vect[j].po_pf = paletes_vect[j].ipo_pf;
  }

  pil_pf = ipil_pf; pil_pc = ipil_pc;   /* fixar valor real posicio pilota */
  win_escricar(ipil_pf, ipil_pc, '.',INVERS);   /* dibuix inicial pilota */

  



sprintf(strin,"Tecles: \'%c\'-> amunt, \'%c\'-> avall, RETURN-> sortir",TEC_AMUNT,TEC_AVALL);
win_escristr(strin);
return(0);
}

void display_time() {
  char text[64];
  int time = temps_transcorregut; int mov = moviments; int mov1 = movimentsIni;

  snprintf(text, sizeof(text), "%02d:%02d", time/60, time%60);
  if(movimentsIni > 0) {
    snprintf(text, sizeof(text), "%02d:%02d Moviments: %d Moviments inicials: %d", time/60, time%60, mov, mov1);
  }
  else {
    snprintf(text, sizeof(text), "%02d:%02d", time/60, time%60);
  }

  pthread_mutex_lock(&pantallaMutex);
  win_escristr(text);
  pthread_mutex_unlock(&pantallaMutex);
}


/* funcio per moure la pilota; retorna un valor amb alguna d'aquestes   */
/* possibilitats:                           */
/*  -1 ==> la pilota no ha sortit del taulell           */
/*   0 ==> la pilota ha sortit per la porteria esquerra     */
/*  >0 ==> la pilota ha sortit per la porteria dreta        */
void* moure_pilota(void * arg) {
  do
  {
    int f_h, c_h;
    char rh,rv,rd,pd;
    //int *result = (int *)arg;

    if(!pausa) {
      win_retard(retard);
      f_h = pil_pf + pil_vf;        /* posicio hipotetica de la pilota */
      c_h = pil_pc + pil_vc;       
      rh = rv = rd = pd = ' ';

      if ((f_h != ipil_pf) || (c_h != ipil_pc))
      {     /* si posicio hipotetica no coincideix amb la pos. actual */
        pthread_mutex_lock(&pantallaMutex);
        if (f_h != ipil_pf)     /* provar rebot vertical */
        {   rv = win_quincar(f_h,ipil_pc);  /* veure si hi ha algun obstacle */
        if (rv != ' ')          /* si no hi ha res */
        {   pil_vf = -pil_vf;       /* canvia velocitat vertical */
            f_h = pil_pf+pil_vf;    /* actualitza posicio hipotetica */
        }
        }
        if (c_h != ipil_pc)     /* provar rebot horitzontal */
        {   rh = win_quincar(ipil_pf,c_h);  /* veure si hi ha algun obstacle */
        if (rh != ' ')          /* si no hi ha res */
        {    pil_vc = -pil_vc;      /* canvia velocitat horitzontal */
            c_h = pil_pc+pil_vc;   /* actualitza posicio hipotetica */
        }
        } 
        if ((f_h != ipil_pf) && (c_h != ipil_pc))   /* provar rebot diagonal */
        {   rd = win_quincar(f_h,c_h);
        if (rd != ' ')              /* si no hi ha obstacle */
        {    pil_vf = -pil_vf; pil_vc = -pil_vc;    /* canvia velocitats */
            f_h = pil_pf+pil_vf;
            c_h = pil_pc+pil_vc;       /* actualitza posicio entera */
        }
        }
        if (win_quincar(f_h,c_h) == ' ')    /* verificar posicio definitiva */
        {                       /* si no hi ha obstacle */
        win_escricar(ipil_pf,ipil_pc,' ',NO_INV);   /* esborra pilota */
        pil_pf += pil_vf; pil_pc += pil_vc;
        ipil_pf = f_h; ipil_pc = c_h;       /* actualitza posicio actual */
        if ((ipil_pc > 0) && (ipil_pc <= n_col)){    /* si no surt */
            win_escricar(ipil_pf,ipil_pc,'.',INVERS); /* imprimeix pilota */
            pthread_mutex_unlock(&pantallaMutex);
        }else{
             pthread_mutex_unlock(&pantallaMutex);
            cont = ipil_pc;    /* codi de finalitzacio de partida */
            
        }
        }
        else {
        pthread_mutex_unlock(&pantallaMutex);
        } 
      }
      else { pil_pf += pil_vf; pil_pc += pil_vc; }

    }

    win_retard(retard * pil_ret);

  } while (finalJoc == 0);

  pthread_exit(NULL);
}


/* funcio per moure la paleta de l'usuari en funcio de la tecla premuda */
void* mou_paleta_usuari(void * arg) {
    int tecla, mogut;  
    do
    { 
      pthread_mutex_lock(&pantallaMutex);
      tecla = win_gettec();
      pthread_mutex_unlock(&pantallaMutex);
      mogut = 0;
      if(!pausa){
        pthread_mutex_lock(&pantallaMutex);
        if (((tecla) == TEC_AVALL) && (win_quincar(ipu_pf+l_pal,ipu_pc) == ' '))
        {
          
          win_escricar(ipu_pf,ipu_pc,' ',NO_INV);    /* esborra primer bloc */
          ipu_pf++;                      /* actualitza posicio */
          win_escricar(ipu_pf+l_pal-1,ipu_pc,'0',INVERS); /* impri. ultim bloc */
          mogut = 1;
        }
        if (((tecla) == TEC_AMUNT) && (win_quincar(ipu_pf-1,ipu_pc) == ' '))
        {
          win_escricar(ipu_pf+l_pal-1,ipu_pc,' ',NO_INV); /* esborra ultim bloc */
          ipu_pf--;                       /* actualitza posicio */
          win_escricar(ipu_pf,ipu_pc,'0',INVERS);     /* imprimeix primer bloc */
          mogut = 1;
        } 
        pthread_mutex_unlock(&pantallaMutex);
        if(mogut == 1) {
          pthread_mutex_lock(&movimentsMutex);
          moviments--;
          pthread_mutex_unlock(&movimentsMutex);
        }
      }
      if (tecla == TEC_ESPAI) {
        pausa = !pausa;
      }

    } while ((tecla != TEC_RETURN) && (moviments > 0 || moviments == -1) && (cont != -1) && (finalJoc != 1) && (cont != n_col+1)); // Modificación aquí

    finalJoc = 1;

    pthread_exit(NULL);
}



/* funcio per moure la paleta de l'ordinador autonomament, en funcio de la */
/* velocitat de la paleta (variable global v_pal) */
void* mou_paleta_ordinador(void* arg)
{
  intptr_t paleta_index = (intptr_t) arg;
  int f_h;
  do
  {
    if(!pausa) {
      f_h = paletes_vect[paleta_index].po_pf + paletes_vect[paleta_index].v_pal; /* posicio hipotetica de la paleta */
      
      if (f_h != paletes_vect[paleta_index].ipo_pf) /* si pos. hipotetica no coincideix amb pos. actual */
      {
        
        if (paletes_vect[paleta_index].v_pal > 0.0) /* verificar moviment cap avall */
        {
          pthread_mutex_lock(&pantallaMutex);
          if (win_quincar(f_h + l_pal - 1, paletes_vect[paleta_index].ipo_pc) == ' ') /* si no hi ha obstacle */
          {
            
            win_escricar(paletes_vect[paleta_index].ipo_pf, paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); /* esborra primer bloc */
            paletes_vect[paleta_index].po_pf += paletes_vect[paleta_index].v_pal;
            paletes_vect[paleta_index].ipo_pf = paletes_vect[paleta_index].po_pf; /* actualitza posicio */
            win_escricar(paletes_vect[paleta_index].ipo_pf + l_pal - 1, paletes_vect[paleta_index].ipo_pc, '1'+paleta_index, INVERS); /* impr. ultim bloc */
            pthread_mutex_unlock(&pantallaMutex);
            pthread_mutex_lock(&movimentsMutex);
            moviments--;
            pthread_mutex_unlock(&movimentsMutex);
          }
          else {/* si hi ha obstacle, canvia el sentit del moviment */
            pthread_mutex_unlock(&pantallaMutex);
            paletes_vect[paleta_index].v_pal = -paletes_vect[paleta_index].v_pal;
          }
        }
        else /* verificar moviment cap amunt */
        {
          pthread_mutex_lock(&pantallaMutex);
          if (win_quincar(f_h, paletes_vect[paleta_index].ipo_pc) == ' ') /* si no hi ha obstacle */
          {
            
            win_escricar(paletes_vect[paleta_index].ipo_pf + l_pal - 1, paletes_vect[paleta_index].ipo_pc, ' ', NO_INV); /* esbo. ultim bloc */
            paletes_vect[paleta_index].po_pf += paletes_vect[paleta_index].v_pal;
            paletes_vect[paleta_index].ipo_pf = paletes_vect[paleta_index].po_pf; /* actualitza posicio */
            win_escricar(paletes_vect[paleta_index].ipo_pf, paletes_vect[paleta_index].ipo_pc, '1'+paleta_index, INVERS); /* impr. primer bloc */
            pthread_mutex_unlock(&pantallaMutex);
            pthread_mutex_lock(&movimentsMutex);
            moviments--;
            pthread_mutex_unlock(&movimentsMutex);
          }
          else {/* si hi ha obstacle, canvia el sentit del moviment */
            pthread_mutex_unlock(&pantallaMutex);
            paletes_vect[paleta_index].v_pal = -paletes_vect[paleta_index].v_pal;
          }
        }
        
      }
      
      else { 
        paletes_vect[paleta_index].po_pf += paletes_vect[paleta_index].v_pal; /* actualitza posicio vertical real de la paleta */
      }
    }
    win_retard(retard) * (paletes_vect[paleta_index].pal_ret);

  } while (finalJoc == 0);

  pthread_exit(NULL);
}







int main(int n_args, const char *ll_args[]) {
  
  int tec = 0;

  pthread_t pilota, paleta_u, paleta_o[n_paletes];

  if ((n_args != 3) && (n_args !=4))
  {
    fprintf(stderr,"Comanda: tennis0 fit_param moviments [retard]\n");
    exit(1);
  }
  carrega_parametres(ll_args[1]);
  moviments=atoi(ll_args[2]);
  movimentsIni=atoi(ll_args[2]);

  if (n_args == 4) retard = atoi(ll_args[3]);
  else retard = 100;

  if (inicialitza_joc() !=0)    /* attempt to create the game board */
     exit(4);   /* abort if there is any problem with the board */


  pthread_mutex_init(&movimentsMutex, NULL);
  pthread_mutex_init(&pantallaMutex, NULL);

  
  pthread_create(&paleta_u, NULL, mou_paleta_usuari, NULL);   
  pthread_create(&pilota, NULL, moure_pilota, NULL);
  
  for (intptr_t i = 0; i < n_paletes; i++) { 
    pthread_create(&paleta_o[i], NULL, mou_paleta_ordinador, (void *)i);
  }

  display_time();
  int freq = 100;
  do {
      if(!--freq) 
      {
        temps_transcorregut++;
        display_time();
        freq = 100;
      }
      win_retard(1000/100);
  } while (finalJoc == 0);

  pthread_join(paleta_u, NULL);
  pthread_join(pilota, NULL);
  for (int i = 0; i < n_paletes; i++) {
    pthread_join(paleta_o[i], NULL);
  }

  win_fi();
  pthread_mutex_destroy(&movimentsMutex);
  pthread_mutex_destroy(&pantallaMutex);
  
  

    if (cont == 0 || moviments == 0) printf("Ha guanyat l'ordinador!\n");
    else printf("Ha guanyat l'usuari!\n");


  

  

  
  return(0);
}
