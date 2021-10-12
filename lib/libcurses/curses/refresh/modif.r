.bp
.he 'REFRESH (curses)'TROPIX: Manual de Modificações'REFRESH (curses)'
.fo 'Atualizado em 28.06.89'Versão 1.0'Pag. %'

.b NOME
.in 5
Atualiza uma janela no vídeo:
.sp
.wo "wrefresh -"
uma janela dada
.br
.wo "refresh  -"
a janela padrão ("stdwin")
.br

.in
.sp 2
.ip "01.04.87  2.1" 20
VERSÃO CORRENTE
.sp

.ip "28.06.89  2.2.1" 20
A rotina passou efetivamente a utilizar
os comandos para tornar o cursor invisível e retornar ao normal.

.ip "08.08.89  2.2.2" 20
Melhoria na utilização das seqüências de campos com atributos.
A capacidade de definição de uma região de rolamento
não abrangendo todo o vídeo passou a ser utilizada quando conveniente.

.ip "10.09.90  2.3.0" 20
Foram implementados os atributos "A_BLINK", "A_BOLD" e "A_STANDOUT".
