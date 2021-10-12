.bp 1
.he 'FTW (libc)'TROPIX: Manual de Referência'FTW (libc)'
.fo 'Atualizado em 25.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
ftw - caminha em uma árvore do sistema de arquivos

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h> 

#include <stat.h> 
#include <ftw.h>

int	ftw (const char *caminho, int (*ftn) ());

int	ftn (const char *nome, STAT *sp, int n);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "ftw" caminha por profundidade em pré-ordem a árvore do sistema de arquivos
cuja raiz é "caminho".

.bc	/*************************************/
A função "ftw" visita um diretório antes de qualquer um de seus descendentes.
.ec	/*************************************/

.sp
Para cada arquivo visitado da árvore, "ftw" chama a função "ftn"
(que deve ser fornecida pelo usuário), com 3 argumentos: 
uma cadeia contendo o nome completo do arquivo, um ponteiro para
uma estrutura "stat" (sys) contendo informações sobre o arquivo,
e um inteiro, classificando o arquivo em:

.in +5
.ip 'FTW_D' 9
diretório

.ip 'FTW_SL' 9
elo simbólico

.ip 'FTW_F' 9
outro tipo de arquivo

.ip 'FTW_NS' 9
arquivo para o qual a chamada ao sistema "stat" não pôde ser executada

.ip 'FTW_DNR' 9
diretório que não pôde ser lido

.ep
.in -5

.bc	/*************************************/
.sp 2
Para o parametro "ordem" deve ser dado um dos valores "FTW_PRE"
ou "FTW_POST".
Se for dado "FTW_PRE", a árvore será caminhada em "pré-ordem",
isto é, a função "ftn" será chamada para cada um dos diretórios
antes dos seus descendentes.
Se for dado "FTW_POST", a árvore será caminhada em "pós-ordem",
isto é, a função "ftn" será chamada para cada um dos diretórios
após todos os seus descendentes.
.ec	/*************************************/

.sp
A árvore é caminhada até que todos os seus arquivos sejam visitados,
a função "ftn" devolva um valor não NULO (ver abaixo)
ou algum erro seja detetado por "ftw" (tal como um erro de E/S).

.sp
Se a raiz "caminho" for de um elo simbólico, será considerado o arquivo
referenciado por este elo. No entanto, se durante a caminhada da árvore
for encontrado um elo simbólico, será considerado o próprio elo.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função "ftw" retorna:

.in +3
.ip 1. 3
Zero, se "ftw" caminhou a árvore integralmente
(exceto subárvores explicitamente puladas, ver abaixo).

.ip 2. 3
Menos um (-1), se "ftw" detetou algum erro.

.ip 3. 3
Um valor negativo igual ao valor retornado por "ftn",
caso esta retorne um valor negativo (ver abaixo).

.ep
.in -3

.sp 2
A função "ftn" deve retornar:
.in +3

.ip 1. 3
Zero, para que o processamento da árvore continue normalmente.

.ip 2. 3
Um valor negativo,
para que "ftw" interrompa imediatamente 
o processamento da árvore, e retorne o valor recebido de "ftn".

.ip 3. 3
Um valor positivo,
no caso de "nome" ser um diretório 
e não for desejado que a subárvore cuja raíz é "nome"
seja caminhada.
Neste caso, o processamento continuará nas subárvores seguintes.

.ep
.in -3

.in
.sp
.b OBSERVAÇÕES
.in 5
A função "ftw" NÃO processa os arquivos "." e "..", isto é,
a função "ftn" NÃO é chamada para estes nomes.

.sp
A implementação da função é feita recursivamente,
mas não há limite na altura da árvore.
A função "ftw" aloca todas as suas áreas de trabalho através
de "alloca" (libc).

.sp
Como a alocação de memória é feita através de "alloca" (e NÃO
"malloc"), as áreas de trabalho são desalocadas mesmo que
a função "ftn" execute um "longjmp" (libc).

.bc	/*************************************/
A implementação da função é feita através de listas (e não
recursivamente), de tal modo que a altura da árvore
não é limitada pelo número de diretórios abertos simultâneamente.
.ec	/*************************************/

.sp
Os argumentos "nome" e "sp" da função chamada "ftn" são ponteiros
para áreas voláteis, e os seus conteúdos devem ser copiados caso
sejam necessários após o retorno da chamada a "ftn".

.sp
No caso de "ftn" ser chamada com o valor "FTW_NS", o conteúdo da
área STAT apontada por "sp" não é significativo.

.sp
Para interromper o processamento, recomenda-se não utilizar o valor
menos um (-1) como retorno de "ftn" porque poderá ser confundido
com uma condição de erro detetada por "ftw".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
stat
.br
.wo "(libc):"
alloca, longjmp
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
