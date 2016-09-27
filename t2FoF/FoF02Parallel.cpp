/***************************************************************************
Programa que realiza o agrupamento de partículas de acordo com o raio de 
percolação fornecido, usando o algoritmo Friends of Friends.  
Ultima atualização 12/01/2009
Autor: Renata S. Rocha Ruiz
******************************************************************************/

//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <chrono>
#include <iostream>
#include <omp.h>

using namespace std;
using namespace std::chrono;
//---------------------------------------------------------------------------

int  *igru, *iden, N;
float  *x, *y, *z, *v1, *v2, *v3;
  

//---------------------------------------------------------------------------
/********************* Lendo o arquivo de dados de entrada *****************/
//---------------------------------------------------------------------------
bool LeDados(char *fn)
  {
  int m;
  float vx, vy, vz;
  FILE  *fp;
  fp = fopen(fn,"rt");
 
  fscanf (fp, "%d", &N); ;//N = 318133 = nº de linhas no arquivo de entrada

//********* alocando memória************//
  iden  = new int [N+1];  // indentificador da partícula
  igru  = new int [N+1];  // índice do grupo
  x  = new float [N+1];  // posição x da partícula;
  y  = new float [N+1];  // posição y da partícula;
  z  = new float [N+1];  // posição z da partícula
  v1 = new float [N+1];  // coordenada x da velocidade
  v2 = new float [N+1];  // coordenada y da velocidade
  v3 = new float [N+1];  // coordenada z da velocidade

  
    //Lê cada linha do arquivo de entrada e assinala índice do grupo 0. (Não está agrupado)
  for (int i = 0 ; i < N ; i++)
    {
    //fscanf (fp, "%f  %f %f %f  %f %f %f ",&x[i], &y[i],&z[i],&v1[i],&v2[i],&v3[i],&iden[i]);
    fscanf (fp, "%d %f %f %f %f %f %f %d ", &m , &x[i], &y[i],&z[i],&v1[i],&v2[i],&v3[i],&iden[i]);    
    igru[i] = 0;
    }
  
 
  fclose (fp);
  return false;
  }



  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  void setThreadNumber(int threads){
      if(threads != 0){
          omp_set_num_threads(threads);
          return;
        }
      omp_set_num_threads(omp_get_num_procs());
      return;
  }


  //---------------------------------------------------------------------------
/**************************** Realiza o agrupamento ******************************/ 
//---------------------------------------------------------------------------

//void Friends(float *x, float *y, float *z, int *iden)
void Friends(float rperc)
{
   
  float dist;
  int i = 0;
  int k = 0;
  int j, l;
 
  printf ("Raio de Percolação (em Mpc): %f", rperc);
  
 // #pragma omp parallel 
 // {
  setThreadNumber(0);
  
  for (i = 0; i < N; i++)//Para cada linha do arquivo (Cada partícula/astro/coisinha)
  {
    k++; //índice de um grupo que vai ser construído.
    
    while (igru[i] != 0 )i++; // já tem designação, então passa para o próximo!
            
    igru[i] = k;
       
        for (j = i ; j < N ; j++)//Para cada a linha do arquivo a partir da linha i.
        { 
            if(igru[j] == k) //Se este trocinho é do grupo k
            { 
                #pragma omp parallel for schedule(guided) private(dist)
                for (l = (i + 1) ; l < N ; l++) //Para cada linha após a linha i. 
                {
                    if (igru[l] == 0) //Se este trocinho não tem grupo(0)
                    {
                        dist = sqrt((x[j] - x[l])*(x[j] - x[l]) + (y[j] - y[l])*(y[j] - y[l]) + (z[j] - z[l])*(z[j] - z[l]) ); // Testo a distância entre j e l.
                        if (dist <= rperc) //Se a distância é menor que o raio de percolação
                        {
                            igru[l] = k; //Associo l ao grupo de i.
                        }
                    }
                }
                
                
            } 
        }
    
    //}
  }
       


/********************escrevendo arquivo de saida ************************/
 
  char str1[10],str2[10];

  FILE *fp;
  
  int num;
  char resultado;
  num = sprintf(str2, "%.2lf", rperc);

  strcpy(str1, "Grupos_RP");
  strcat(str1,str2);
  
  fp = fopen(str1,"w");

  fprintf(fp, "%d %d \n", N, k); //Nº de coisinhas, maior índice de grupo?(k)
  
  for (i = 0 ; i < N ; i++)
  fprintf(fp,"%4d % 10d %4d % 10.6e % 10.6e % 10.6e % 10.6e % 10.6e % 10.6e \n", i,iden[i],igru[i],x[i], y[i],z[i],
  v1[i], v2[i],v3[i]);
    //Printa: nº da coisinha, identificador, índice do grupo, posição x y z , velocidade Vx Vy Vz
  fclose (fp);
  
  printf("Numero total de grupos: ");
  printf("%d \n", k);

  /********* Calculando a quantidade de particulas por grupo ************/
  int nn, si, mult;
  int *Ngr; 
  
  Ngr  = new int [k+1];
    for ( nn = 1; nn <= k; nn++ )
      {
      mult = 0;
      for (si = 0; si < N; si++)
        if(igru[si] == nn) mult =  mult + 1;
        Ngr[nn] = mult;
       }
        
       
  int cont1 = 0;
  for (nn = 1 ; nn <= k ; nn++)
    if (Ngr[nn] > 1) cont1++; 
      
    printf("Grupos com massa maior que 1: %d \n", cont1);
  
  delete Ngr;
  
  }

  
  
//---------------------------------------------------------------------------
/**************************** Limpa a memória ******************************/
//---------------------------------------------------------------------------
void LimpaMemoria(void)
  {
  delete iden;
  delete igru;
  delete x; 
  delete y;
  delete z;
  delete v1;
  delete v2;
  delete v3;
  }
//---------------------------------------------------------------------------
/***************************************************************************/
//---------------------------------------------------------------------------

main(int argc, char **argv)
  {
  float  local_v[100] ;
  char *Arg1;
  if(argc != 3 )
    {
    puts( "Por favor entre com o nome do arquivo de dados! ");
    getchar();
    exit(1);
    }
  puts ("Iniciando...");
  Arg1 = argv[1];
  
  
  float rperc = atof(argv[2]);
///
  high_resolution_clock::time_point start = high_resolution_clock::now();
  
  LeDados(Arg1);
///
  high_resolution_clock::time_point dataRead = high_resolution_clock::now();

  Friends(rperc);
///
   high_resolution_clock::time_point findFriends = high_resolution_clock::now();
  
  LimpaMemoria();
///
   high_resolution_clock::time_point clearMemory = high_resolution_clock::now();
   
  printf(" Terminou \n");

/// 
  auto leitura = duration_cast<microseconds>( dataRead - start ).count();
  auto friendsT = duration_cast<microseconds>( findFriends - dataRead ).count();
  auto cleanup = duration_cast<microseconds>( clearMemory - findFriends ).count();
  cout << "Tempos em microsegundos. Leitura: " << leitura  << " Friends: " << friendsT << " Limpeza: " << cleanup;


   /* #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        printf(" hello(%d) ", ID);
        printf(" world(%d) \n", ID);
    }*/
  return 0;
  }
