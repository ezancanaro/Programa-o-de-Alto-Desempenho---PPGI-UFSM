**Diogo João Cardozo, Eric Tomás Zancanaro**


**Programação de Alto Desempenho - PPGI UFSM**


**Profª Andrea Schwertner Charão**




##Sobre o programa

Este programa trabalha de forma similar ao algoritmo analisado anteriormente, realizando o agrupamento de partículas com base em um raio de percolação, utilizando a técnica Friends of Friends para concretizar o agrupamento. 

Como entrada, o programa recebe um arquivo com coordenadas de partículas e um raio de percolação, utilizado como base para a categorização das partículas em grupos.


O algoritmo foi implementado na linguagem C++ e utiliza vetores simples para o armazenamento dos dados das partículas. Mais especificamente, o algoritmo possui oito vetores de tamanho **N**, para cada coordenada das partículas no espaço 3D, um vetor para cada coordenada da velocidade das partículas e dois vetores de classificação: identificador da partícula e índice do grupo.

Através do arquivo de entrada, o algoritmo popula os vetores de coordenadas das partículas seguindo a ordem pela qual as partículas são descritas linha a linha no arquivo, adicionando o valor 0 ao índice de grupo da partícula, representando que ela ainda não foi agrupada.

Este é o ponto em que o algoritmo difere largamente do funcionamento simplificado do programa anterior. Neste caso, foram utilizadas árvores para armazenar as partículas quando do seu agrupamento, bem como classes específicas implementadas para representar cada uma das partículas e seu agrupamento.

Em seguida serão descritas as classes presentes neste programa.

### Grupo.cpp
	
	Classe que representa os grupos formados durante o algoritmo. Armazena um valor *int* representativo do identificador do grupo e um vetor de ponteiros para os corpos membros deste grupo.

	Além do construtor, esta classe implementa também um método para adicionar novos corpos no grupo.

### Tupla.cpp

	Classe simples com o propósito de implementar a estrutura de tuplas na linguagem c++. Armazena uma combinação de *int* com Segmento, representando o número de grupos presente em cada segmento do espaço tridimensional.

	O único método presente nesta classe é um construtor.

### Segmento.cpp

	A classe segmento representa uma fração do espaço tridimensional que engloba os corpos analisados. Armazena uma árvore que representaria os corpos presentes nesta fração do espaço, bem como um vetor de grupos contendo os grupos daquele segmento.

	Esta classe não foi implementada nem utilizada no programa analisado.

### Corpo.cpp

	A classe corpo representa um dos objetos sendo agrupados pelo programa. Esta classe armazena valores *float* representativos das coordenadas **x**,**y** e **z** do objeto, bem como dois valores *int* representativos dos identificadores do objeto e do grupo ao qual ele foi alocado.

	Apenas o construtor foi implementado nesta classe.

### No.cpp

	Esta classe é a mais complexa do programa, tanto que é aqui que o algoritmo realiza a primeira passagem de testes para o agrupamento Friends of Friends. A classe representa um nó da árvore mencionada na classe Segmento e armazena um ponteiro para um Corpo, valores *float* representativos de coordenadas máximas e mínimas, valores *float* representando valores máximos e mínimos para que um corpo encontre-se na **fronteira do corpo apontado por este nó. Armazena ainda valores *bool* que indicam se o nó é ou não populado e se possui o menor raio. Possui ainda um vetor de Nós com 8 posições denominado filhos. Por fim armazena dois valores *int* que representam o identificador do nó e o número de grupos.

	Os métodos implementados nesta classe são:
		#### ver
			Verifica a distância entre dois corpos através da fórmula distância = sqrt((c1.x-c2.x)² + (c1.y-c2.y)² + (c1.z-c2.z)²)

		#### add
			Adiciona um novo nó na subárvore. O processo de inclusão de um novo nó na árvore dá início aos testes de verificação de agrupamento dos corpos, logo seu funcionamento será discutido em detalhes em uma seção subsequente quando discutimos o funcionamento concreto deste programa.

		#### relabel
			Faz a troca do identificar de grupo de um determinado nó caso o algoritmo determine que o mesmo tenha sido alocado a um grupo que, na realidade, é subgrupo de um agrupamento maior.	


## Funcionamento do Algoritmo

	A proposta básica do algoritmo é subdividir o espaço tridimensional que engloba os corpos e realizar o agrupamento em cada um destes segmentos de forma individual, realizando ao final a comparação dos corpos nas extremidades destes segmentos para verificar se existem grupos com corpos em mais de um segmento.

	Para fazer a divisão, o algoritmo calcula as coordenadas máximas e mínimas de todos os corpos da entrada, circundando os objetos com um cubo virtual. Sabendo os valores da extremidade, o algoritmo calcula também valores medianos para cada coordenada da forma [min + ((max - min) / 2)]. Com estes valores medianos, é feita uma divisão em 8 semi-cubos, denominados de segmentos.

	Com estes segmentos determinados, a entrada é então dividida em função das coordenadas dos corpos ali representados, de forma que cada um seja alocado a um destes segmentos. Com a entrada dividida, inica-se a criação das árvores que armazenarão os corpos e realizarão o agrupamento dos mesmos. Como o espaço foi dividido em 8 segmentos, serão criadas 8 árvores individuais, o que permite a paralelização deste fragmento do programa. 

	A criação destas árvores continua a segmentação do espaço tridimensional, criando, para cada semi-cubo obtido anteriormente, 8 novos semi-cubos, de forma que o último semi-cubo criado contenha apenas 1 corpo em seu interior. Estas árvores são descritas como *oct-trees* devido a esta propriedade. Uma visualização em 2 dimensões ajuda a compreender o processo de divisão realizado por estas árvores e é encontrado na imagem obtida do artigo (Springel et al.).

![octTrees](octTrees.png "Visualização 2D das divisões octais.")


TODO!!!!! REPLACE ALL THIS
 1. Se a partícula tiver designação do grupo **k**, continua para o próximo ponto, caso contrário repete este teste com a próxima partícula do vetor.
 2. Repete os testes seguintes para toda partícula da lista após a partícula **i (i + 1)**, nomeando-a **l**.
..1.  Se a partícula ainda não tem nenhum grupo assinalado, segue ao próximo item, caso contrário testa a próxima partícula.
 ..2. Se a distância entre as partículas **i** e **j** for menor ou igual ao raio de percolação, esta partícula é assinalada ao grupo **k**.


Desta forma, o algoritmo faz o agrupamento através da propagação, agrupando primeiramente todas as partículas próximas (**distância <= raio de percolação**) da partícula inicial **i** e em seguida todas as partículas próximas daquelas que foram agrupadas pelo primeiro passo.


##Sobre os testes                


Os testes foram executados em uma máquina com as seguintes configurações:


- Windows 10x64.
- Intel Core i5-4210U CPU @1.70GHz 2.40GHz
- Memória RAM: 8GB


Foram executados testes com o mesmo arquivo de entrada, que apresenta 318133 partículas, utilizando como raio de percolação os valores: 2,4,16,32.


Para a medição dos tempos de execução, foi utilizada a biblioteca *chronus* da linguagem C++, utilizando uma escala de microssegundos. Os resultados obtidos são mostrados na tabela abaixo, com a conversão do tempo em segundos para facilitar a visualização.


| Percolação | Nº de grupos | Tempo de leitura(s)  | Tempo de agrupamento(s) |
| -------------- |:-----------------:| -----------------------:|-------------------------------:|
| 1                 | 25772            |  1,107303              |   990,030889 |
| 2                 | 1029              |  1,110522              |   920,580808 |
| 4                 | 4                    |  1,433968              |   830,558760 |
| 16               | 3                    |  1,127073              |   339,744321 |
| 32               |  2                   |  1,131312              |   282,424131 |


A tabela dos resultados mostra uma consistência na redução do tempo de execução do algoritmo de classificação conforme o raio de percolação é aumentado e o número de grupos classificados diminui.