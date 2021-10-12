.bp 1
.he 'PUTENV (libc)'TROPIX: Manual de Modificações'PUTENV (libc)'
.fo 'Atualizado em 30.05.95'Versão 3.0.0'Pag. %'

.b NOME
.in 5
putenv - modifica ou acrescenta um valor ao ambiente

.in
.sp 2
.ip "30.04.89  2.1.0" 20
VERSÃO CORRENTE
.sp

.ip "01.09.89  2.3.0" 20
Na comparação, não era considerado o "=", de tal modo que a modificação
de uma variável do tipo "CC" (por exemplo) iria modificar a primeira
variável encontrada cujo prefixo fosse "CC" (tal como "CCC", "CCCCD", etc...). 

.ip "30.05.95  3.0.0" 20
A função foi reescrita. Agora não há mais limite no número de variáveis
no ambiente.
