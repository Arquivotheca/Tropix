.bp 1
.he 'LIB (fmt)'TROPIX: Manual de Referência'LIB (fmt)'
.fo 'Atualizado em 22.08.01'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "lib -"
formato da biblioteca de módulos objeto
.br

.in
.sp
.b DESCRIÇÃO
.r
.in 5
Este documento descreve o formato adotado para organização de bibliotecas
de módulos objeto não compartilhadas no sistema TROPIX.

.sp
Para obter informações sobre as bibliotecas compartilhadas, veja "mkshlib" (cmd).

.sp 2
.b
1. Estrutura das bibliotecas:
.r

.in +3
.sp
A organização de bibliotecas no sistema TROPIX aproveita a estrutura
hierárquica inerente ao sistema de arquivos do TROPIX. De maneira genérica,
uma biblioteca deve estar contida em uma árvore.
Os módulos objeto podem estar na raiz desta árvore ou em subdiretórios.

.sp
Na raiz da biblioteca temos o "arquivo de sinopse", uma coletânea de
informações sobre os módulos contidos na biblioteca.
Este arquivo tem o nome padrão de ".LIBSYM".
Abaixo, vemos um exemplo de  organização de biblioteca:

.sp 2
.(t
.nf
                          /lib/libx  (Diretório Raiz)
                               x
                               |
        +--------------+-------+-------+-------+-------+
        |              |               |               |
     .LIBSYM          dirm            dirn    ...     dirp
                       |               |               |
                 +---+---+---+   +---+---+---+   +---+---+---+
                 |   |  ...  |   |   |  ...  |   |   |  ...  |
                m1  m2      mi  n1  n2      nj  p1  p2      pk
.fi
.)t

.sp 2
A escolha deste formato trouxe uma grande vantagem do ponto de vista do
usuário: a antiga ordenação, eminentemente topológica, que prevalecia
para as bibliotecas do sistema UNIX original, deixa de ser aqui necessária.
O usuário é, assim, liberado da preocupação de conhecer as
interdependências entre os módulos da biblioteca para poder construi-la
corretamente, evitando falhas no processo de "link-edição".

.in -3
.sp 1
.b
2. Estrutura do arquivo de sinopse de símbolos:
.r

.sp
.in +3
O arquivo de sinopse ".LIBSYM" é um arquivo de texto.
É composto de um cabeçalho, com um "número mágico", para que possa
ser identificado como um arquivo de sinopse.
Em seguida, consiste de uma linha para cada módulo, contendo o seu nome,
número do seu nó-índice e a respectiva lista de símbolos definidos.


.sp
Como exemplo, damos abaixo o arquivo de sinopse da biblioteca "/lib/libm",
isto é, o arquivo "/lib/libm/.LIBSYM":
.sp
.nf
	   299792458
	   conv/fcvt.o,3001:__fcvt
	   conv/pottb.o,2999:__pottb
	   conv/strtod.o,2998:_strtod
	   float/fexcep.o,2996:__fgcr,__fgiar,__fgsr,__fscr, ...
	   float/fgman.o,2995:_HUGE_VAL,_faexp,_fgexp,_fgfra, ...
	   float/fixdfsi.o,2994:___fixdfsi
	   float/fixunsdfsi.o,2993:___fixunsdfsi
	   float/frexp.o,2992:_frexp,_ldexp,_modf
	   math/asin.o,2990:_acos,_asin
	   math/atan.o,2989:_atan,_atan2
	   math/exp.o,2988:_exp,_exp10,_exp2,_expb,_pow
	   math/floor.o,2987:_ceil,_fabs,_floor,_fmod
	   math/hypot.o,2986:_hypot
	   math/log.o,2985:_log,_log10,_log2,_logb
	   math/sin.o,2984:_cos,_sin
	   math/sinh.o,2983:_cosh,_sinh,_tanh
	   math/sqrt.o,2982:_sqrt
	   math/tan.o,2981:_tan
.fi

.in -3
.sp 1
.b
3. Outros detalhes
.r

.in +3
.ip a. 3
Não há limites nem na altura da árvore, nem nos comprimentos dos nomes
dos módulos e símbolos.

.ip b. 3
A manipulação de bibliotecas, envolvendo operações de inserção,
remoção e atualização de módulos deve ser feita via o utilitário
"mklib" (cmd), especialmente projetado para esta finalidade. Qualquer outro
procedimento utilizado no sentido de alterar o conteúdo de uma
biblioteca não mais garantirá a integridade estrutural da mesma.
Em particular, o utilitário "mklib" possui uma opção em seu funcionamento
cuja finalidade é simplesmente a verificação desta integridade.

.ip c. 3
Se a árvore de uma biblioteca for copiada, será necessário
reconstruir o arquivo de sinopse através de "mklib",
pois os nós-índices serão outros.

.ep

.in
.sp
.b
ARQUIVOS
.r
.in 5
.wo ".../.LIBSYM"
.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ld, mklib, mkshlib
.br
.wo "(sys): "
inopen
.br
.wo "(fmt): "
a.out
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
