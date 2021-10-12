.bp 1
.he 'UNGETC (libc)'TROPIX: Manual de Referência'UNGETC (libc)'
.fo 'Atualizado em 11.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
ungetc - insere um caractere na área de entrada/saída

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int	ungetc (int c, FILE *fp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "ungetc" insere o caractere "c" na área de entrada/saída
associada ao arquivo de entrada "fp", de tal modo que este caractere
será retornado na próxima chamada a "getc".

.sp
A função "ungetc" retorna "c" e não modifica o arquivo.

.sp
Se algum caractere já foi lido do arquivo e o arquivo possui uma área
de entrada/saída, garante-se que pelo menos um caractere pode ser
inserido na área.

.sp
Se o valor de "c" for EOF, "ungetc" nada faz e EOF é retornado.

.in
.sp
.b
DIAGNÓSTICOS
.r
.in 5
A função "ungetc" retorna EOF se não foi possível inserir
o caractere na área de entrada/saída.

.in
.sp
.b OBSERVAÇÃO
.in 5
A função "fseek" desfaz todos os efeitos de "ungetc".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fseek, getc, setbuf, stdio
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
