.bp
.he 'REWIND (stdio)'TROPIX: Manual de Modificações'REWIND (stdio)'
.fo 'Atualizado em 28.11.91'Versão 2.3.1'Pag. %'

.b NOME
.in 5
.wo "rewind -"
retorna o ponteiro de posição ao início do arquivo
.br

.in
.sp 2
.ip "18.11.86  1.0.0" 20
VERSÃO CORRENTE
.sp

.ip "28.11.91  2.3.1" 20
Agora passa a chamar "fseek", pois assim a complexidade fica concentrada.
