.bp 1
.he 'GETENV (libc)'TROPIX: Manual de Modificações'GETENV (libc)'
.fo 'Atualizado em 01.09.89'Versão 2.3.0'Pag. %'

.b NOME
.in 5
getenv - obtém o valor de uma variável do ambiente

.in
.sp 2
.ip "30.04.89  2.1.0" 20
VERSÃO CORRENTE
.sp

.ip "01.09.89  2.3.0" 20
Na comparação, não era considerado o "=", de tal modo que a busca
de uma variável do tipo "CC" (por exemplo) iria devolver a primeira
variável encontrada cujo prefixo fosse "CC" (tal como "CCC", "CCCCD", etc...). 
