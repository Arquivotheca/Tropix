.bp
.he 'MKTEMP (libc)'TROPIX: Manual de Modificações'MKTEMP (libc)'
.fo 'Atualizado em 15.07.88'Versão 2.0.0'Pag. %'

.b NOME
.in 5
mktemp - gera um nome para um arquivo temporário

.in
.sp 2
.ip "01.12.87  1.1.0" 20
VERSÃO CORRENTE
.sp

.ip "15.07.88  2.0.0" 20
Está sendo tirada uma cópia da cadeia recebida,
pois a original agora pode estar na região "text"
do programa (decorrente da definição ANSI da linguagem "C").
