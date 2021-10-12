.bp
.he 'CTIME (libc)'TROPIX: Manual de Modificações'CTIME (libc)'
.fo 'Atualizado em 07.09.05'Versão 4.8.0'Pag. %'

.b NOME
.in 5
.wo "ctime -"
conversão de datas e tempos.
.br

.in
.sp 2
.ip "01.04.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "01.07.87  1.0.1" 20
Corrigido o problema que fazia com que nos anos bissextos
as datas entre 29 de fev. e 31 de dez. fossem indicadas
como um dia antes.

.in
.sp 2
.ip "01.01.88  1.1" 20
VERSÃO CORRENTE
.sp

.ip "10.03.90  2.3.0" 20
Foi desmembrado o módulo "ctime" em "ctime" e "localtime".

.ip "07.09.05  4.8.0" 20
Novas versões de "localtime", "mktime" e " "strtotime",
evitando a malha pelos meses. Para simplificar, "strtotime"
agora está chamando "mktime".
