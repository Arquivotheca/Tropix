.bp 1
.he 'A.OUT (fmt)'TROPIX: Manual de Referência'A.OUT (fmt)'
.fo 'Atualizado em 28.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "a.out -"
formato do módulo objeto
.br

.in
.sp
.b DESCRIÇÃO
.sp
.in 5
Este documento descreve o formato dos módulos objeto do sistema TROPIX.
São descritas as seguintes estruturas:
cabeçalho, tabela de símbolos e relocação.
Outros detalhes podem ser vistos no arquivo de inclusão <a.out.h>.

.sp
.b
Componentes de um módulo objeto
.r
.ip
Módulos objeto no sistema TROPIX são gerados como saída
do montador "as" (cmd), e por razões de homogeneidade, seus nomes devem
conter o sufixo ".o".
Um módulo objeto é composto das seguintes partes:

.in +5
.ip a. 3
Cabeçalho. Além de permitir a identificação do arquivo como objeto,
o cabeçalho contém informações sobre os tamanhos de todas as outras partes
integrantes do módulo. São as seguintes as informações presentes:

.in +5
.ip - 2
Computador de destino.

.ip - 2
Número mágico, que identifica o módulo como objeto. Pode ser (em octal)
0407 ou 0410, indicando respectivamente que o processo de cuja imagem fará
parte o módulo em questão será regular ou reentrante.

.ip - 2
Identificação da versão do módulo objeto.

.ip - 2
Diversos indicadores informando se o módulo é executável, se
é necessário o processador de ponto flutuante, etc...

.ip - 2
Data e hora da geração do módulo.

.ip - 2
Número de série do computador de destino.

.ip - 2
Endereço (virtual) do início da seção TEXT.

.ip - 2
Endereço (virtual) do início da seção DATA.

.ip - 2
Endereço inicial de execução do programa.

.ip - 2
Tamanho (em bytes) da seção TEXT.

.ip - 2
Tamanho (em bytes) da seção DATA.

.ip - 2
Tamanho (em bytes) da seção BSS.

.ip - 2
Tamanho (em bytes) da tabela de símbolos.

.ip - 2
Tamanho (em bytes) da relocação do TEXT.

.ip - 2
Tamanho (em bytes) da relocação do DATA.

.ip - 2
Tamanho (em bytes) da tabela de números de linha para ser
utilizado pelo depurador simbólico "wsdb" (cmd).

.ip - 2
Tamanho (em bytes) da tabela de símbolos para ser
utilizado pelo depurador simbólico "wsdb" (cmd).

.in -5
.ip b. 3
.wo "Seção TEXT."
Nesta seção do módulo encontram-se normalmente instruções de máquina
que foram traduzidas pelo "as", que constituirão a parte executável
do módulo.

.ip c. 3
.wo "Seção DATA."
Encontram-se na seção DATA as áreas de memória destinadas às variáveis
inicializadas do módulo.

.ip d. 3
.wo "Tabela de símbolos."
A tabela de símbolos de um módulo é composta de entradas que descrevem
a natureza dos símbolos constantes daquele módulo. O montador "as"
gera, se nada for especificado, entradas apenas para os símbolos 
externos, já que apenas estes símbolos
realmente participam do processo de "link-edição". No entanto, entradas
podem ser geradas para todos os símbolos, se desejado.
O formato da tabela será visto no próximo item.

.ip e. 3
Estrutura de relocação para TEXT.
O processo de "link-edição" consiste, acima de tudo, em atribuir endereços
definitivos a posições do código consideradas relocáveis pelo montador.
Para cada posição relocável é gerada uma entrada na estrutura respectiva,
dependendo da seção à qual ela pertence. Neste caso, aplica-se às
posições que se encontram na seção TEXT.

.ip f. 3
Estrutura de relocação para DATA.
O mesmo que o item anterior aplicado às posições que se encontram
na seção DATA.

.ip g. 3
Tabela dos números das linhas.
Contém informação sobre os números das linhas do código fonte
que geraram este módulo objeto, de forma que o depurador simbólico
"wsdb" possa
identificar as linhas do código fonte a partir do módulo objeto.

.ip h. 3
Tabela dos símbolos para o depurador simbólico "wsdb".
Contém informação sobre as variáveis do código fonte
que geraram este módulo objeto, de forma que "wsdb" possa
consultar/alterar as diversas variáveis do módulo objeto
durante a execuação.

.in -5
.ep
.(t
.b
Formato da tabela de símbolos
.r

.ip
Uma entrada na tabela de símbolos contém as seguintes informações:
.)t

.in +5
.ip a. 3
Nome do símbolo.
Um identificador de até 31 caracteres.

.ip b. 3
Tipo do Símbolo, que pode ser:

.in +3
.ip "UNDEF:" 7
O símbolo ainda não possui tipo associado.

.ip "ABS:  " 7
O símbolo é absoluto.

.ip "TEXT: " 7
O símbolo pertence à seção TEXT.

.ip "DATA: " 7
O símbolo pertence à seção DATA.

.ip "BSS:  " 7
O símbolo pertence à seção BSS.

.ep
.in -3
.ip c. 3
Uma indicação de símbolo externo.

.ip d. 3
Uma indicação de símbolo já referenciado.

.ip e. 3
O valor associado ao símbolo.

.ep
.in -5
.ip
Símbolos declarados em ".common" referenciam o início de uma área
a ser reservada pelo "link-editor" na seção BSS, cujo tamanho vem 
especificado como valor do símbolo. Um símbolo nestas condições é
inicialmente considerado do tipo UNDEF
e o "link-editor" percebe que se trata de
".common" em virtude de seu valor ser diferente de zero.

.sp 
Embora o usuário tenha a opção de ver incluídos na tabela mesmo os
símbolos locais, o "link-editor" simplesmente ignorará tais entradas na 
tabela, servindo apenas para onerar o arquivo em tamanho.

.sp
O "link-editor" testa também se os símbolos declarados são utilizados 
realmente pelo programa ou não, produzindo uma advertência na segunda
hipótese. Esta advertência é particularmente útil quando, por exemplo,
durante a depuração de um programa incluímos variáveis ou mesmo
funções auxiliares e esquecemos de removê-las por fim. O "link-editor"
avisará que tais objetos nunca são referenciados pelo programa, sendo
de bom alvitre removê-los.

.ep
.sp
.b
Formato das estruturas de relocação
.r
.ip
Cada entrada da estrutura de relocação é constituída pelas seguintes
informações:

.in +5
.(t
.ip a. 3
Tamanho do objeto a ser relocado:

.in +3
.ip "RBYTE: " 7
Não é utilizado, pois não é suportado pelo "link-editor".
.)t

.ip "RWORD: " 7
O objeto a ser relocado é uma palavra (WORD).

.ip "RLONG: " 7
O objeto a ser relocado é um longo (LONG).

.ep
.in -3
.ip b. 3
Tipo da Relocação:
.in +3

.ip "RTEXT: " 9
A posição refere-se a um endereço da seção TEXT.

.ip "RDATA: " 9
A posição refere-se a um endereço da seção DATA.

.ip "RBSS:  " 9
A posição refere-se a um endereço da seção BSS.

.ip "REXT:  " 9
A posição relocada será atribuído um valor que dependerá de um símbolo
ainda indefinido. Neste caso, acrescentamos a posição deste símbolo dentro
da tabela de símbolos como informação adicional à estrutura de relocação.

.ip "REXTREL:" 9
Esta relocação é uma variante da anterior (REXT) necessária para os
processadores da família INTEL (386, 486, PENTIUM), que não possuem
desvios com endereços absolutos.

.ep
.in -3
.ip c. 3
Deslocamento dentro da seção.
Esta informação indica a quantos bytes do início da seção figura a
posição a ser relocada.

.ip d. 3
Posição do símbolo na tabela.
Esta informação só tem sentido quando o tipo da relocação é REXT ou REXTREL,
conforme já explicado acima.

.in -5
.ep
.sp
.b
Formato da tabela de números das linhas.
.r
.ip
Cada entrada contém um número de linha do programa fonte
juntamente com o respectivo endereço do código objeto. 

.ep
.sp
.b
Formato da tabela dos símbolos do depurador simbólico "wsdb".
.r
.ip
O formato desta tabela é específica do "wsdb", e não será discutida
aqui.

.ep
.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ld, mklib, nm, wsdb
.br
.wo "(sys): "
exec
.br
.wo "(fmt): "
lib
.br

.in
.sp
.b ARQUIVOS
.in 5
/usr/include/a.out.h

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
