/***************************************************************************
Programa que realiza o agrupamento de partículas de acordo com o raio de 
percolação fornecido, usando o algoritmo Friends of Friends.  
Ultima atualização 12/01/2009
Autor: Renata S. Rocha Ruiz
******************************************************************************/

//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <omp.h>
#include "grupo.h"

using namespace std;
using namespace std::chrono;
//---------------------------------------------------------------------------

int  *igru, *iden, N, Nthreads,numGrupos;
float  *x, *y, *z, *v1, *v2, *v3;
float rperc;  
high_resolution_clock::time_point findFriends;
high_resolution_clock::time_point postProccessTime;

int countR = 0;
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

//bounds[0]=Xmax,bounds[1]=Xmin,[2]=Ymax,[3]=Ymin,[4]=Zmax,[5]=Zmin
//Testa se dois cubos A e B intersectam, contando o raio de percolação. 
bool CubesInstersect(float *bounds1, float *bounds2){
  //printf("CubsesStart\n");
  if(bounds1[1] > bounds2[0] + rperc)//A está a direita de B
    return false;
  if(bounds1[0] + rperc < bounds2[1])//A está a esquerda de B
    return false;
  if(bounds1[3] > bounds2[2] + rperc)//A está acima de B
    return false;
  if(bounds1[2] + rperc < bounds2[3])//A está abaixo de B
    return false;
   if(bounds1[5] > bounds2[4] + rperc)//A está a frente de B
    return false;
  if(bounds1[4] + rperc < bounds2[5])//A está atrás de B
    return false;
  //printf("Intersect: %f e %f\n",bounds1[0],bounds2[0]);
  //Se nenhuma das condições acima se aplica, existe intersecção.
  return true;
}

//Checa se existe uma ligação entre os dois grupos, concatenando-os caso positivo.
//Retorna true caso tenha reordenado os grupos.
bool ReorderGroup(grupo *g1, grupo *g2){
  
  float dist;
  int g1Size = g1->lista.size(); // Lista com os ids das partículas pertencentes a este grupo.
  int g2Size = g2->lista.size();
  int i,j,id1,id2;
  for(i=0; i < g1Size; i++){
    for(j=0; j < g2Size; j++){
      id1 = g1->lista[i];
      id2 = g2->lista[j];
      dist = sqrt(((x[id1] - x[id2])*(x[id1] - x[id2])) + ((y[id1] - y[id2])*(y[id1] - y[id2])) + ((z[id1] - z[id2])*(z[id1] - z[id2])) ); // Testo a distância entre j e l.
        if (dist <= rperc) //Se a distância é menor que o raio de percolação
        {
         // printf("REORDERED %d e %d \n",id1,id2);
          //Concateno a lista de ids do segundo grupo com a do primeiro
          g1->lista.insert(std::end(g1->lista), std::begin(g2->lista), std::end(g2->lista));
          //printf("INSERT\n");
          //Atualizo as bordas do primeiro grupo com as do segundo.
          g1->mergeBounds(g2->bounds);
          return true;
        }
    }
  }
  //printf("ENDREORDER");
  return false;
}


void quickDelete( int *vec, int n, vector<grupo*> *grps )
{
 // printf("Deleting %d groups : ",n);
  for(int i = n-1; i >= 0; i--){
   // printf("grupsSize = %d, %d ",grps->size(),vec[i]);
    grps->erase(grps->begin() + vec[i]);
    vec[i]=0;
  }
 // printf("ERASED\n");
}

//Esta função itera sobre os grupos encontrados pelas threads i e j, procurando intersecções.
//DeleteThese e NgruposDel são variáveis para controlar quais grupos devem ser excluídos após anexação.
int loopThroughGroups(vector<struct grupo*> *grupos, int *deleteThese,int NgruposDel, int i, int j){
  
  int it1,it2;
  for(it1 = 0; it1 < grupos[i].size(); it1++){
      for(it2 = 0 ; it2 < grupos[j].size(); it2++){
          //printf("post i: %d e j: %d \n", it1,it2);
          if(CubesInstersect (grupos[i][it1]->getBounds(),grupos[j][it2]->getBounds() ) ){
           //printf("GRPSsize: %d Intersecction\n.",grupos[j].size());
              if(ReorderGroup(grupos[j][it2],grupos[i][it1])){ //Concatena o grupo da thread I no grupo da thread J
                countR++;
                deleteThese[NgruposDel] = it1;
                NgruposDel++;
                break;
              }
          }
      }
  }
  return NgruposDel;

}

//Função usada para teste ente grupos de uma mesma thread. Só é necessário testar com os grupos além do iterador para evitar
//duplicação dos testes.
int loopThroughGroups2(vector<struct grupo*> *grupos, int *deleteThese,int NgruposDel, int i, int j){
  
  int it1,it2; 
  for(it1 = 0; it1 < grupos[i].size(); it1++){
      for(it2 = it1 + 1; it2 < grupos[j].size(); it2++){
          //printf("post i: %d e j: %d \n", it1,it2);
          if(CubesInstersect (grupos[i][it1]->getBounds(),grupos[j][it2]->getBounds() ) ){
           //printf("GRPSsize: %d Intersecction\n.",grupos[j].size());
              if(ReorderGroup(grupos[j][it2],grupos[i][it1])){ //Concatena o grupo da thread I no grupo da thread J
                countR++;
                deleteThese[NgruposDel] = it1;
                NgruposDel++;
                break;
              }
          }
      }
  }
  return NgruposDel;

}

//Tenho um vector (grupos) com NThreads posições, sendo cada uma delas um vector de ponteiros para grupos
     // Pós processamento que deve ser realizado:
    // Pega um cubo da 1ª thread e testa com os cubos das outras threads. Não precisa testar com cubos da sua própria execução.
    // Caso haja intersecção, testa as partículas dos grupos, tentando agrupar os dois.
    // Se agrupar dois cubos, altera os valores de mín ou máximo das coordenadas e passa p/ os cubos da próxima thread.
void postProccess(vector<struct grupo*> *grupos){
    int it1,it2,i,j; //Iteradores 
    int NgruposDel;
    int *deleteThese = new int [N/Nthreads];
    NgruposDel = 0;
    bool exists;

    //A primeira thread não precisa testar com os grupos dela mesma.
    i = 0;
    for(j = 1; j < Nthreads; j++){
           
      NgruposDel = loopThroughGroups(grupos,deleteThese,NgruposDel,i,j);
      quickDelete(deleteThese,NgruposDel, &grupos[i]);
      NgruposDel = 0;

    }  
    //Como os grupos das threads além da primeira podem ter sido alterados pelo laço anterior, é necessário
    // testar estes grupos com os grupos da sua própria thread, porém apenas aqueles após o grupo sendo testado.
    for(i = 1; i < Nthreads; i++){
        
        j = i;
        NgruposDel = loopThroughGroups2(grupos,deleteThese,NgruposDel,i,j);
        quickDelete(deleteThese,NgruposDel, &grupos[i]);
        NgruposDel = 0;
        
        //Testa com as demais threads
        for(j = i+1; j < Nthreads; j++){

          NgruposDel = loopThroughGroups(grupos,deleteThese,NgruposDel,i,j);
          quickDelete(deleteThese,NgruposDel, &grupos[i]);
          NgruposDel = 0;
        }    
    }

}

 //---------------------------------------------------------------------------
/**************************** Realiza o agrupamento ******************************/ 
//---------------------------------------------------------------------------
//void Friends(float *x, float *y, float *z, int *iden)
void Friends()
  {
   
  float dist;
  int i = 0;
  int k = 0;
  int j, l;
 
  printf ("Raio de Percolação (em Mpc): %f \n", rperc);
  

  vector<struct grupo*> grupos[Nthreads];
 
  //Sendo N= nº de partículas, t = nº de threads
  //Cada thread deve lidar com N/t partículas, dividindo o arquivo de entrada em t pedaços. Ou dividi-lo em X>t para mais flexibilidade.
  //Cada execução deve armazenar o id do primeiro e último elemento do vetor de entrada que analisou.
  //Para cada grupo, armazena os valores de mín e máx para cada coordenada (x,y,z), criando um cubo em torno do grupo.
  //Ao final, uma execução sequencial procura cubos que se intersectam e "cola" os grupos se necessário.
  
//grupo *g;
// g = new grupo(1);
  omp_set_num_threads(Nthreads);
  //printf("SIZE %d \n",grupos.size());
  
  float range = N / Nthreads;
  int *ranges = new int[Nthreads+1];
  ranges[0]=0;
  for(i=1;i<Nthreads;i++){
    ranges[i] = i*range;  
  }
  ranges[Nthreads]=N;
//PROCURAR SOBRE ESTRUTURAS NA BIBLIOTECA BOOST

  #pragma omp parallel shared(grupos,ranges) private(i,j,l,k) 
  {
    int Tid = omp_get_thread_num();  
  //  printf("TID %d RANGE %d \n",Tid,ranges[Tid+1]);
    /* grupo *g = new grupo(Tid,10*Tid,20*Tid,30*Tid);
    grupos[Tid].push_back(g);
    grupos[Tid][0]->printGrupo();*/
  //Tid = omp_get_thread_num();
  //#pragma omp parallel for schedule(static)
  for (i = ranges[Tid]; i < ranges[Tid+1]; i++)//Para cada linha do arquivo (Cada partícula/)
  {
    
    //printf("IDD %d \n",Tid);
    k++; //índice de um grupo que vai ser construído.
    //while (igru[i] != 0 )i++; // já tem designação, então passa para o próximo!
    if(igru[i]==0){  
      igru[i] = k;
      grupo *g = new grupo(k,x[i],y[i],z[i]);  
      //printf("Created group %d \n",Tid);
      g->addParticle(i);
      //  printf("Added particle %d \n",Tid);
      grupos[Tid].push_back(g);
      // printf("pushed group %d \n",Tid);

      for (j = i ; j < ranges[Tid+1] ; j++)//Para cada a linha do arquivo a partir da linha i.
      { 
        if(igru[j] == k) //Se este trocinho é do grupo k
        { 
          for (l = (i + 1) ; l < ranges[Tid+1] ; l++) //Para cada linha após a linha i. 
          {
            if (igru[l] == 0) //Se este trocinho não tem grupo(0)
            {
              dist = sqrt((x[j] - x[l])*(x[j] - x[l]) + (y[j] - y[l])*(y[j] - y[l]) + (z[j] - z[l])*(z[j] - z[l]) ); // Testo a distância entre j e l.
              //printf("Thread %d: Dist i%d l%d = %f \n",Tid,i,l,dist);
              if (dist <= rperc) //Se a distância é menor que o raio de percolação
              {
                igru[l] = k; //Associo l ao grupo de i.
                g->addParticle(l);
                //printf("ADD %d \n",Tid);
                g->changeBounds(x[l],y[l],z[l]);
                //printf("BOUNDS %d \n",Tid);
              }
            }
          }
        } 
      }
    }
      
  }

  }//PRAGMA PARALLEL
    
     
    findFriends = high_resolution_clock::now();
    
    postProccess(grupos);
      
    postProccessTime = high_resolution_clock::now();

//printf("POSTPROCCESS\n");
printf("TOTAL DE AGRUPAMENTOS: %d  \n",countR);

//********************escrevendo arquivo de saida ***********************
 
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
  k = 0;
  for(i=0;i<Nthreads;i++){
    k = k + grupos[i].size();
  }
   
  printf("%d \n", k);

  //********* Calculando a quantidade de particulas por grupo ***********
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
printf("Deleted \n");
  
  printf("C \n");

  }

//---------------------------------------------------------------------------
/**************************** Limpa a memória ******************************/
//---------------------------------------------------------------------------
void LimpaMemoria(void)
  {
  printf("Limpando memória \n");
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
  if(argc != 4 )
    {
    puts( "Por favor entre com o nome do arquivo de dados, Raio de percolação e Numero de threads ");
    getchar();
    exit(1);
    }
  puts ("Iniciando...");
  Arg1 = argv[1];
  
  
  rperc = atof(argv[2]);
  Nthreads = atoi(argv[3]);
 
///
  high_resolution_clock::time_point start = high_resolution_clock::now();
  
  LeDados(Arg1);
///
  high_resolution_clock::time_point dataRead = high_resolution_clock::now();

  Friends();
  //printf("Friendei já");
///
// 
  
  LimpaMemoria();
///
  high_resolution_clock::time_point clearMemory = high_resolution_clock::now();
   
  printf(" Terminou \n");

/// 
  auto leitura = duration_cast<microseconds>( dataRead - start ).count();
  auto friendsT = duration_cast<microseconds>( findFriends - dataRead ).count();
  auto postProccessing = duration_cast<microseconds>( postProccessTime - findFriends).count();
  auto cleanup = duration_cast<microseconds>( clearMemory - postProccessTime ).count();
  cout << "Tempos em microsegundos. Leitura: " << leitura  << " Friends: " << friendsT << " PosProcessamento" << postProccessing << " Limpeza: " << cleanup;

 
  return 0;
  }
