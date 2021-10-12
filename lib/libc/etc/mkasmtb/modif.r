.bp
.he 'MKASMTB (libc)'TROPIX: Manual de Modificações'MKASMTB (libc)'
.fo 'Atualizado em 23.02.95'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "mkasmtb -"
gera módulos em linguagem simbólica
.br

.in
.sp 2
.ip "10.11.86  1.0.0" 20
VERSÃO CORRENTE
.sp

.ip "04.02.91  2.3.0" 20
Foram acertados os "void *" e "int" no "mkasmtb.h" e no "mkasmtb.c".

.ip "23.02.95  3.0.0" 20
A função foi estendida para gerar ".byte", ".word" ou ".long", para
atender CPUs BIG e LITTLE endian. Alterada a estrutura "OBJENT".
