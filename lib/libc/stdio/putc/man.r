.bp 1
.he 'PUTC (libc)'TROPIX: Manual de Referência'PUTC (libc)'
.fo 'Atualizado em 10.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Escreve um caractere ou uma palavra em um arquivo:
.sp
.wo "putc    -"
escreve um caractere em um arquivo dado
.br
.wo "putchar -"
escreve um caractere na saída padrão ("stdout")
.br
.wo "fputc   -"
escreve um caractere em um arquivo dado (função)
.br
.wo "putw    -"
escreve uma palavra em um arquivo dado
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int	putc (int c, FILE *fp);
int	putchar (int c);
int	fputc (int c, FILE *fp);
int	putw (int w, FILE *fp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A macro "putc" escreve o caractere "c" no arquivo de saída "fp"
(na localização dada pelo  ponteiro da posição do arquivo
se definido).

.sp
A macro  "putchar" é equivalente à "putc" com o segundo
argumento igual a "stdout".

.sp
A função "fputc" é equivalente à macro "putc".
A função é mais lenta do que a macro, 
mas cada chamada ocupa menos espaço de memória,
e o seu nome pode ser passado como um argumento
para uma função.

.sp
A função "putw" escreve uma palavra (ou seja um inteiro de 4 bytes)
no arquivo de saída "fp"
(na localização dada pelo  ponteiro da posição do arquivo se definido).
A função não exige nenhum tipo de alinhamento da palavra.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As funções retornam o valor escrito e EOF em caso de erro
(exceto "putw", que retorna "ferror (fp)").
Um erro ocorre se o arquivo não for estiver aberto para
escrita ou o arquivo não possa mais crescer.

.in
.sp
.b OBSERVAÇÕES
.in 5
A função "putw" é dependente de implementação, pois
nem todas as máquinas tem o inteiro com 4 bytes, nem a ordem
dos bytes dentro da palavra é padronizada.
Isto significa que um arquivo escrito com "putw" em um computador
pode não ser lido corretamente com "getw" (libc) em outro computador.

.sp
É importante lembrar que "putc" e "putchar" são macros,
e portanto construções do tipo "putc (cp, *fp++)" podem não produzir
o efeito esperado. Deve-se utilizar "fputc" para estes casos.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fclose, ferror, fopen, fread, printf, puts, setbuf,
.br
.wo "        stdio, getw"
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
