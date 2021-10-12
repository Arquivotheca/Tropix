.bp
.he 'CURSES (curses)'TROPIX: Manual de Modificações'CURSES (curses)'
.fo 'Atualizado em 06.07.89'Versão 2.2.1'Pag. %'

.b NOME
.in 5
.wo "Curses -"
Biblioteca para construção/manipulação de janelas em terminais alfanuméricos.
.br

.in
.sp 2
.ip "01.04.87  2.0.0" 20
VERSÃO CORRENTE
.sp

.ip "15.09.88  2.1.1" 20
Nova disposição para os fontes.
Os arquivos para inclusão,
que antes estavam no diretório "include"
foram transferidos para o diretório "terminfo/h"
e são instalados no diretório "/usr/include/curses".
Os fontes dos programas "tic" e "tac"
foram colocados no diretório de fontes dos comandos.
.sp

.ip "15.09.88  2.1.2" 20
Foram realizadas várias modificações com o objetivo de reduzir
o tempo de processamento demandado pelas rotinas da biblioteca.
Estas modificações envolveram a estrutura "WINDOW"
e portanto implicam na necessidade de recompilação
de seus aplicativos (como por exemplo: "vi" e "wsdb")
nos módulos que incluem o arquivo <curses.h>.

.sp
A partir desta versão, a Curses é capaz de utilizar
a capacidade de inserir e remover linhas oferecida pelo
hardware de alguns terminais.

.sp
Foram acrescentadas as seguintes opções de configuração para as janelas:
A_REAL, G_SIGRET e R_INPUT.

.sp
Foram acrescentadas as rotinas "getctlch" e "setctlch"
que permitem a um aplicativo obter e/ou alterar os valores
para os caracteres de controle utilizados pelo sistema e pela Curses.

.ip "01.04.87  2.2.0" 20
VERSÃO CORRENTE

.ip "06.07.89  2.2.1" 20
A "terminfo" foi alterada para reconhecer todas as variáveis de descrição.
.sp
Impacto:
(1) Os programas que utilizam a Curses precisam ser religados.
(2) As novas descrições de terminais são incompatíveis com as anteriores.
(3) Os programas já religados devem trabalhar com as novas descrições.
.ep
