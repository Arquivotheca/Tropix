.bp 1
.he 'CRT0 (libc)'TROPIX: Manual de Referência'CRT0 (libc)'
.fo 'Atualizado em 26.09.88'Versão 2.3'Pag. %'

.b NOME
.in 5
crt0 - função prólogo para todos os programas em "C"
.sp

.in
.sp
.b DESCRIÇÃO
.in 5
Esta função é usada pelo sistema como prólogo de
todos os programas escritos em linguagem "C". Ela trata
do manuseio dos parametros "argc" e "argv" junto com
a chamada ao sistema "exec".

.in
.sp
.b OBSERVAÇÃO
.in 5
A função "crt0" não foi concebida para ser chamada
por um programa de usuário.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exec
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
