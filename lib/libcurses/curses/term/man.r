.bp
.he 'NEWTERM (curses)'TROPIX: Manual de Referência'NEWTERM (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
Manipulação de vários terminais:
.sp
.wo "newterm -"
preparação de um novo terminal
.br
.wo "setterm -"
estabelecimento do terminal corrente
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>
#include <curses.h>

int newterm (const char *nome, FILE *saída, FILE *entrada);
int setterm (int term);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A biblioteca "curses" pode manipular mais de um terminal
utilizando-se do conceito de terminal corrente.
Assim a cada instante "curses" está trabalhando em apenas um terminal,
denominado terminal corrente.
Quando for desejado trabalhar com outro terminal,
basta estabelecê-lo como terminal corrente.

.sp
A rotina "newterm" prepara "curses" para trabalhar com mais um terminal,
cujo tipo é identificado pelo parâmetro "nome"
e arquivos utilizados para escrita e leitura são respectivamente
"saída" e "entrada".
Quando o parâmetro "nome" é nulo é utilizada
o valor da variável "TERM" do ambiente.

.sp
A rotina "setterm" estabelece o terminal correntemente em uso.
O parâmetro "term" é um descritor de terminal retornado
numa chamada bem sucedida à rotina "newterm".


.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A rotina "newterm" normalmente retorna um descritor para o novo terminal;
em caso de erro ela retorna um valor negativo.
.sp
A rotina "setterm" retorna o descritor do terminal corrente anterior
ou um valor negativo se o descritor de terminal fornecido for inválido.

.in
.sp
.b DIAGNÓSTICOS
.in 5
As seguintes situações causam erro na rotina "newterm":
.ip (1)
indisponibilidade de memória,
.ip (2)
incapacidade de leitura das características do terminal,
.ip (3)
os arquivos de saída ou de entrada não representarem terminais e
.ip (4)
a variável "TERM" não definida no ambiente
(para o caso do parâmetro "nome" nulo).
.ep

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses): "
incurses
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
