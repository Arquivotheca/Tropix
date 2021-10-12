.bp
.he 'DELWIN (curses)'TROPIX: Manual de Referência'DELWIN (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
.wo "delwin -"
remove uma janela
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void delwin (WINDOW *janela);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "delwin" remove "janela",
retirando a sua imagem do vídeo e
liberando todos os recursos alocados.

.sp
Após a remoção de uma janela,
o vídeo do terminal é atualizado
de modo a refletir a nova disposição
apenas com as janelas restantes.

.in
.sp
.b OBSERVAÇÃO
.in 5
A janela padrão ("stdwin") não pode ser removida.
.sp
A rotina "delwin" atualiza todas as janelas visíveis no terminal.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(curses): "
werase, newwin
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
