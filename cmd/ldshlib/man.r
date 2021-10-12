.bp
.he 'LDSHLIB (cmd)'TROPIX: Manual de Referência'LDSHLIB (cmd)'
.fo 'Atualizado em 21.06.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "ldshlib -"
carrega/descarrega uma biblioteca compartilhada
.br

.bc	/*************************************/
ldshlib - carrega/descarrega uma biblioteca compartilhada

Versão: 3.2.3, de 14.03.00

Sintaxe:
	ldshlib [-sv] [-u <índice>] [<biblioteca>]

Opções:	-u: Descarrega a biblioteca com o <índice> dado
	-s: Não imprime mensagem de erro caso a biblioteca já esteja carregada
	-v: Verboso

Obs.:	A biblioteca pode ser dada na forma abreviada "-l..."
.ec	/*************************************/

.in
.sp
.b SINTAXE
.in 5
.(l
ldshlib [-sv] [-u <índice>] [<biblioteca>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "ldshlib" gerencia a carga e descarga das bibliotecas compartilhadas.

.sp
Antes que possam ser usadas, as bibliotecas compartilhadas necessitam de ser
carregadas. Como o próprio comando "init" (cmd) usado durante o processo de inicialização
do sistema já usa a biblioteca compartilhada "/lib/libt.o", o próprio núcleo
carrega esta biblioteca. Todas as demais biblioteca compartilhadas têm de ser carregadas
através do comando "ldshlib".

.sp
Sem opções nem argumentos, o comando "ldshlib" imprime um mapa das bibliotecas carregadas,
com os seguintes campos:

.in +3
.ip I: 7
Índice da biblioteca.

.ip NOME: 7
Nome da biblioteca.

.ip DATA: 7
Data da geração da biblioteca.

.ip DEP: 7
Máscara de dependências das outras bibliotecas.

.ip HSEQ: 7
Ordem em que os símbolos da biblioteca foram introduzidos na tabela HASH.

.ip TEXT: 7
Tamanho da área de TEXT da biblioteca.

.ip DATA: 7
Tamanho da área de DATA da biblioteca.

.ip BSS: 7
Tamanho da área de BSS da biblioteca.

.ip SYMTB: 7
Tamanho da tabela de símbolos da biblioteca.

.ip NSYM: 7
Número de símbolos na tabela de símbolos da biblioteca.

.ip NTENT: 7
Número de tentativas (média) para encontrar um símbolo da biblioteca na tabela HASH.

.ep
.in -3

.sp 2
Se for dado o nome de uma <biblioteca>, ela será carregada.
O nome da <biblioteca> pode ser dado em sua forma completa (por exemplo "/usr/xwin/lib/libx.o"),
ou abreviada ("-lx.o"), conforme as convenções dadas em "ld" (cmd).

.sp
As opções do comando são:

.in +3
.ip -u
Descarrega a biblioteca com o <índice> dado. Este índice pode ser obtido através do mapa
das bibliotecas carregadas.

.ip -s
Não imprime uma mensagem de erro caso a biblioteca já esteja carregada.

.ip -v
Subsitui, no mapa das bibliotecas carregadas, os dois últimos campos (NSYM e NTENT)
pelos endereços virtuais das áreas de TEXT e DATA.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
init, ld, mkshlib
.br

.in
.sp
.b ARQUIVOS
.in 5
/lib/libt.o
 /usr/xwin/lib/libx.o
 /usr/xwin/lib/liby.o

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
