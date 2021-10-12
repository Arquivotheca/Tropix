.bp
.he 'L3TOL (libc)'TROPIX: Manual de Referência'L3TOL (libc)'
.fo 'Atualizado em 22.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "l3tol -"
converte uma lista de inteiros de 3 bytes para 4 bytes
.br
.wo "ltol3 -"
converte uma lista de inteiros de 4 bytes para 3 bytes
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

void	l3tol (long *dp, const char *c3p, int n);
void	ltol3 (char *c3p, const long *dp, int n);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
.bc	/*************************************/
Endereços de discos usualmente contém 4 bytes (tipo "daddr_t").
No entanto, no sistema de arquivos (no disco),
eles contém apenas  3; esta compactação é possível
pelo fato de que os endereços nunca ultrapassam
2 ** 24 (16777216), ou em outras palavras, os bytes de mais alta
ordem são sempre NULOs.
Estas funções fazem as conversões entre os dois formatos.

Certos sistemas de arquivos utilizam endereços de blocos com
3 bytes. Estas funções convertem vetores de endereços do formato
de 3 para 4 bytes e vice-versa.
.ec	/*************************************/

A função "l3tol" converte uma lista de "n" inteiros de 3 bytes
apontada por "c3p" em uma lista de
inteiros de 4 bytes na área apontada por "dp".

.sp
A função "ltol3" realiza a conversão inversa,
do formato de 4 bytes ("dp") para
o formato de 3 bytes ("c3p").


.in
.sp
.b OBSERVAÇÃO
.in 5
Estas funções são úteis para os utilitários de manutenção de certos
sistemas de arquivos que utilizam endereços de blocos de 3 bytes.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mkfs, fsck
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
