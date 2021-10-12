.bp 1
.he 'CRC16 (libc)'TROPIX: Manual de Referência'CRC16 (libc)'
.fo 'Atualizado em 20.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Funções para a geração de CRC de 16 bits:
.sp
.wo "crc16    -"
gera o CRC rapidamente
.br
.wo "crc16s   -"
gera o CRC usando pouca memória
.br
.wo "crc1641  -"
gera o CRC (norma V.41) rapidamente
.br
.wo "crc1641s -"
gera o CRC (norma V.41) usando pouca memória

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

int	crc16 (const void *area, int count);
int	crc16s (const void *area, int count);
int	crc1641 (const void *area, int count);
int	crc1641s (const void *area, int count);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Todas estas funções calculam um CRC ("Cyclic Redundancy Check") 
de 16 bits para a área "area" contendo "count" bytes utilizando o polinômio
.sp
.nf
		 16    12    5
		x   + x   + x  + 1,
.fi
.sp
padrão de 16 bits recomendado pela CCITT para transmissão de informação.

.sp
Nas funções "crc16" e "crc16s",
o CRC é definido como o resto da divisão (módulo 2) do conteúdo da
mensagem em "area" pelo polinômio indicado.
As funções "crc1641" e "crc1641s" seguem a norma V.41 do CCITT e
o CRC é definido como o resto da divisão (módulo 2) do conteúdo da
mensagem em "area" acrescida de 16 bits zero pelo polinômio indicado.

.sp
As funções são capazes de detectar todos os erros tipo rajada de
comprimento menor ou igual a 16, detectam 99,997\% dos erros tipo
rajada de 17 bits além de todos os erros simples, duplos ou com um número
ímpar de bits errados.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As funções retornam o CRC calculado (16 bits).

.in
.sp
.b OBSERVAÇÃO
.in 5
As funções "crc16" e "crc1641" calculam o CRC indicado de forma bastante
rápida; elas utilizam uma tabela de 256 entradas para poder
calcular o CRC de cada byte em apenas um passo.

.sp
As funções "crc16s" e "crc1641s" não são tão rápidas quanto as funções
"crc16" e "crc1641" mas são mais curtas. Utilizam uma tabela de apenas
16 entradas e calculam o CRC de cada byte em dois passos.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
