.bp
.he 'ENDIAN (libc)'TROPIX: Manual de Referência'ENDIAN (libc)'
.fo 'Atualizado em 20.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Conversão da ordem dos bytes ("big/little endian")
.sp
.wo "short_endian_cv -"
de valor de 16 bits
.br
.wo "long_endian_cv -"
de valor de 32 bits
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

int	short_endian_cv (int);
long	long_endian_cv (long);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas funções convertem valores de 16 e 32 bits entre os
formatos "big-endian" e "little-endian".

.sp
O formato "big endian" é o que contem os bytes mais significativos
em endereços de memória mais baixos, enquanto que o "little endian"
contem os bytes mais significativos em endereços de memória mais altos.

.sp
Repare que as conversões são simétricas, isto é
.sp
.nf
		long_endian_cv (long_endian_cv (l)) == l
	e
		short_endian_cv (short_endian_cv (s)) == s
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
