**Diogo João Cardozo, Eric Tomás Zancanaro**
**Programação de Alto Desempenho - PPGI UFSM**
**Profª Andrea Schwertner Charão**




##Sobre o programa##


O programa utilizado realiza o agrupamento de partículas com base em um raio de percolação, utilizando a técnica Friends of Friends para concretizar o agrupamento.


Como entrada, o programa recebe um arquivo com coordenadas de partículas e requer do usuário um raio de percolação, utilizado como base para a categorização das partículas em grupos.


O algoritmo foi implementado na linguagem C++ e utiliza vetores simples para o armazenamento dos dados das partículas. Mais especificamente, o algoritmo possui oito vetores de tamanho N, para cada coordenada das partículas no espaço 3D, um vetor para cada coordenada da velocidade das partículas e dois vetores de classificação: identificador da partícula e índice do grupo.


Através do arquivo de entrada, o algoritmo popula os vetores de coordenadas das partículas seguindo a ordem pela qual as partículas são descritas linha a linha no arquivo, adicionando o valor 0 ao índice de grupo da partícula, representando que ela ainda não foi agrupada.


O algoritmo inicia um laço com uma partícula na posição i seguindo a ordem do vetor de partículas. Caso esta partícula já tenha sido designada a um grupo, passa para a próxima partícula do vetor. Caso contrário, designa esta partícula ao grupo k e faz os seguintes passos para todas as partículas do vetor, iniciando pela partícula na posição i. Estas partículas são referidas através da letra j. 


1. Actual numbers don't matter, just that it's a number
⋅⋅1. Ordered sub-list




 1. Se a partícula tiver designação do grupo k, continua para o próximo ponto, caso contrário repete este teste com a próxima partícula do vetor.
 2. Repete os testes seguintes para toda partícula da lista após a partícula i (i + 1), nomeando-a l.
..1.  Se a partícula ainda não tem nenhum grupo assinalado, segue ao próximo item, caso contrário testa a próxima partícula.
 ..2. Se a distância entre as partículas i e j for menor ou igual ao raio de percolação, esta partícula é assinalada ao grupo k.


Desta forma, o algoritmo faz o agrupamento através da propagação, agrupando primeiramente todas as partículas próximas (distância <= raio de percolação) da partícula inicial i e em seguida todas as partículas próximas daquelas que foram agrupadas pelo primeiro passo.


##Sobre os testes##                


Os testes foram executados em uma máquina com as seguintes configurações:
Windows 10x64.
Intel Core i5-4210U CPU @1.70GHz 2.40GHz
Memória RAM: 8GB


Foram executados testes com o mesmo arquivo de entrada, que apresenta 318133 partículas, utilizando como raio de percolação os valores: 2,4,16,32.


Para a medição dos tempos de execução, foi utilizada a biblioteca *chronus* da linguagem C++, utilizando uma escala de microssegundos. Os resultados obtidos são mostrados na tabela abaixo.


| Percolação | Nº de grupos | Tempo de leitura(μs)  | Tempo de agrupamento(μs) |
| -------------- |:-----------------:| -----------------------:|-------------------------------:|
| 2                 | 1029              |  1237543              | 1105287358 |
| 4                 | 4                    |  1433968              |   830558760 |
| 16               | 3                    |  1127073              |   339744321 |
| 32               |  2                   |  1131312              |   282424131 |