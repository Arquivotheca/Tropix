.bp
.he 'GETCTLCH (curses)'TROPIX: Manual de Referência'GETCTLCH (curses)'
.fo 'Atualizado em 14.02.89'Versão 2.3'Pag. %'

.b NOME
.in 5
Obtenção e alteração de caracteres de controle:
.sp
getctlch - obtém um caractere de controle.
.br
setctlch - altera um caractere de controle.
.br

.in
.sp
.b SINTAXE
.(l
#include <curses.h>

int getctlch (int função);
void setctlch (int função, int caractere);
.)l

.sp
.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "getctlch" retorna o caractere controle
associado a "função" na entrada do teclado.

.sp
A rotina "setctlch" altera o caractere associado à "função" para "caractere".

.sp
A tabela abaixo mostra os valores para "função"
correspondentes a todos os caracteres de controle
com os nomes pelos quais eles são apresentados e reconhecidos
pelo comando "stty":
.sp
.ts
center
allbox;
c c.
Função	Nome para o comando "stty"
C_INTR	intr
C_QUIT	quit
C_SWITCH	switch
C_ERASE	erase
C_AERASE	aerase
C_WORD	word
C_KILL	kill
C_RETYPE	retype
C_EOF	eof
.te

.sp
Veja em "wgetset" (curses)
o significado de cada um dos caracteres de controle acima.

.in
.sp
.b
OBSERVAÇÕES
.r
.in 5
As funções C_ERASE, C_AERASE, C_WORD, C_KILL, C_RETYPE e C_EOF
só estão ativas nas janelas em que a edição da entrada está habilitada
(veja "wgetset" (curses)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses): "
wgetset
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
