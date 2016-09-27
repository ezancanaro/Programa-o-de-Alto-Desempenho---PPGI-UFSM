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

Esta classe não possui métodos implementados.

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

Sabemos que o programa divide os corpos em segmentos e que o agrupamento é primeiramente realizado de forma independente em cada um destes fragmentos do espaço. Este agrupamento é realizado durante a inclusão de um novo corpo na árvore existente dentro de cada segmento. Antes de explicar o processo de adição dos nós, é necessário porém que expliquemos o conceito de **fronteira** no âmbito deste algoritmo, já que o mesmo aparecerá durante o texto.

####Fronteira

Sabemos pela definição da palavra que fronteira é uma região que encontra-se nos limites entre duas áreas. Neste algoritmo não é diferente, a área de fronteira é toda aquela próxima o suficiente dos limites do segmento no qual o corpo se encontra. Esta definição levanta a questão de como definimos qual distância representa próxmia o suficiente dos limites. Para o propósito tratado aqui, um corpo está próximo o suficiente dos limites de um segmento quando existe a possibilidade de que este corpo seja agrupado com um corpo de fora deste segmento. Em termos matemáticos: 

Considerando um espaço 3D, onde **limX**,**limY** e **limZ** representam os limites de um segmento dentro deste espaço e **r** é um raio qualquer, um objeto de coordenadas (x,y,z) encontra-se próximo o suficiente dos limites quando: *_(x + raio >= limX)_* OU *_(y + raio >= limY)_* OU *_(z + raio >= limZ)_*.

Conhecendo a aplicação de fronteira neste programa, podemos partir para o funcionamento do restante do código. De início, são calculados os valores máximos e mínimos locais para as coordenadas dos corpos do segmento. Estes valores servem também para o cálculo dos limites da área de fronteira (max - raio e min + raio). Têm-se início a construção da árvore, através da instanciação de um objeto Corpo para representar o objeto da entrada, bem como de um objeto No contendo um ponteiro para tal Corpo, utilizando como raiz da árvore o primeiro corpo analisado. Caso este corpo esteja na [fronteira](####fronteira) do segmento, é alocado no vetor de fronteira da árvore. Segue um laço que cria um objeto Corpo para cada objeto da entrada e o adiciona na árvore através do método *add da classe No.

É sensato lembrar aqui que cada nó da árvore possui sua área de fronteira, já que cada nível dela representa uma subdivisão em um espaço cada vez menor. O método add pode agir de forma recursiva, iniciando no nó raíz da árvore e a percorrendo para encontrar a posição na qual um corpo deve ser alocado.

Para cada corpo adicionado a árvore, seguem-se os testes realizados para o seu posicionamento:
O algoritmo testa se o raio é maior que as coordenadas do cubo, o que significa que o corpo está envolto em um segmento muito pequeno.
Neste caso, são feitos testes com os nós já armazenados na fronteira do nó ao qual este é anexado, comparando as distâncias de forma a agrupar os nós caso estejam próximos. Este nó é então adicionado a fronteira e marcado como não populado, ou seja, não possuirá nós filhos. Executando estes passos, o método add retorna ao ponto em que foi chamado.

Caso o corpo não tenha caído no caso acima, o algoritmo testa então se este corpo está na fronteira do nó ao qual será anexado. Caso esteja, ele é adicionado ao vetor de fronteira.

Caso o corpo esteja sendo anexado a um nó populado, o algoritmo calcula os limites da próxima subdivisão de acordo com as coordenadas do corpo, de forma a determinar em qual dos 8 subespaços este corpo será alocado. Neste caso, será criado um nó representante deste corpo (chamamos nó2, para evitar confusão), alocado como um filho do nó populado, na posição determinada pelo passo anterior. Testa-se também se o corpo encontra-se na fronteira deste novo nó criado, sendo alocado na fronteira deste em caso positivo. O algoritmo marca ainda o nó original como não populado.

A seguir, o algoritmo calcula em qual subdivisão o corpo deverá ser alocado (se o nó já é populado, esse passo acontece repetido). Caso já exista um filho na posição calculada, verifica-se se o novo corpo está na região de fronteira deste filho, alocando uma flag *true* em caso positivo.

O algoritmo verifica o agrupamento entre todos os corpos vizinhos do novo corpo sendo alocado a árvore, modificando os identificadores de grupo quando necessário. Caso o corpo tenha sido detectado como de fronteira em algum dos passos anteriores, verifica-se o agrupamento também com os nós da fronteira.

Caso não exista um filho na posição calculada, aloca-se um objeto nó para este corpo, são calculados os limites da sua subdivisão e se este corpo encontra-se na fronteira da sua subdivisão. Este novo nó é então alocado como filho do nó original na posição calculada. Caso já exista um filho, o algoritmo tenta adicionar este corpo no nó filho através da função add. 



TODO!!!!! REPLACE ALL THIS
 1. Se a partícula tiver designação do grupo **k**, continua para o próximo ponto, caso contrário repete este teste com a próxima partícula do vetor.
 2. Repete os testes seguintes para toda partícula da lista após a partícula **i (i + 1)**, nomeando-a **l**.
..1.  Se a partícula ainda não tem nenhum grupo assinalado, segue ao próximo item, caso contrário testa a próxima partícula.
 ..2. Se a distância entre as partículas **i** e **j** for menor ou igual ao raio de percolação, esta partícula é assinalada ao grupo **k**.


Desta forma, o algoritmo faz o agrupamento através da propagação, agrupando primeiramente todas as partículas próximas (**distância <= raio de percolação**) da partícula inicial **i** e em seguida todas as partículas próximas daquelas que foram agrupadas pelo primeiro passo.


##Sobre os testes                


Os testes foram executados em uma máquina com as seguintes configurações:


- ?.
- ?.
- Memória RAM:?. 


Foram executados testes com o mesmo arquivo de entrada, que apresenta 318133 partículas, utilizando como raio de percolação os valores: 2,4,16,32. Para uma melhor representação dos tempos, foram realizadas 4 execuções de cada método e compilado um valor médio para o tempo de cada.


Para a medição dos tempos de execução, foi utilizada a biblioteca *chronus* da linguagem C++, utilizando uma escala de microssegundos. Os resultados obtidos são mostrados na tabela abaixo, com a conversão do tempo em segundos para facilitar a visualização.


| Percolação |Tempo Serial(s)|Tempo Static(s)|Tempo Dynamic(s)|Tempo Guided(s)|Speedup Static|Speedup Dynamic|Speedup Guided|
| ---------- |:------------:| --------------:|---------------:|--------------:|:------------:|:-------------:|:------------:|
| 1          |  951,200419  | 341,902268     | 404,945830     | 291,681382    | 2.7821       | 2.3490        | 3.2610
| 2          |  938,704370  | 374,151177     | 475,690339     | 329,976507    | 2.5089       | 1.9733        | 2.8448  
| 4          | 813,345729   | 385,955238     | 473,674422     | 358,032609    | 2.1073       | 1.7171        | 2.2717
| 16         |  297,991135  | 191,651810     | 323,526933     | 197,016100    | 1.5548       | 0.9211        | 1.5125
| 32         |  219,565154  | 584,27846      | 266,237279     | 594,90657     | 0.3758       | 0.8247        | 0.3691



A tabela dos resultados mostra uma consistência na redução do tempo de execução do algoritmo de classificação conforme o raio de percolação é aumentado e o número de grupos classificados diminui.

##Referências

SPRINGEL, Volker; YOSHIDA, Naoki; WHITE, Simon DM. GADGET: a code for collisionless and gasdynamical cosmological simulations. New Astronomy, v. 6, n. 2, p. 79-117, 2001. [Link](http://arxiv.org/pdf/astro-ph/0003162v3.pdf)