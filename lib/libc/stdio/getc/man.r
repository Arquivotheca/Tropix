.bp 1
.he 'GETC (libc)'TROPIX: Manual de Referência'GETC (libc)'
.fo 'Atualizado em 10.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Lê um caracter ou palavra de um arquivo
.sp
.wo "getc    -"
lê um caracter de um arquivo
.br
.wo "getchar -"
lê um caracter da entrada padrão ("stdin")
.br
.wo "fgetc   -"
lê um caracter de um arquivo (função)
.br
.wo "getw    -"
lê uma palavra de um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
int	getc (FILE *fp);
int	getchar (void);
int	fgetc (FILE *fp);
int	getw (FILE *fp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A macro "getc" lê um caracter do arquivo de entrada "fp".
O ponteiro da posição do arquivo (se definido) é atualizado para o caracter
seguinte.

.sp
A macro  "getchar" é equivalente a "getc (stdin)".

.sp
A função "fgetc" é equivalente à
macro "getc". A função é mais lenta do que a macro, 
mas cada chamada ocupa menos espaço na memória,
e o seu nome pode ser passado como um argumento
para uma função.

.sp
A função "getw" lê uma palavra (ou seja, um inteiro de 4 bytes)
do arquivo de entrada  "fp". O ponteiro da posição do arquivo
(se definido) é atualizado para a palavra seguinte.
A função não exige nenhum tipo de alinhamento da palavra.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Estas funções retornam a constante EOF na condição de final de arquivo
ou em caso de  erro.
Como EOF é um inteiro válido, "feof" e "ferror"
devem ser usadas para detetar erros da função "getw".

.in
.sp
.b OBSERVAÇÕES
.in 5
A função "getw" é dependente de implementação, pois
nem todas as máquinas tem o inteiro com 4 bytes, nem a ordem
dos bytes dentro da palavra é padronizada.
Isto significa que um arquivo escrito com "putw" (libc) em um computador
pode não ser lido corretamente com "getw" em outro computador.

.sp
Se o valor retornado  pelas funções
"getc", "getchar" e "fgetc" for armazenado
em uma variável do tipo "char",
e em seguida comparado com a constante EOF, esta comparação poderá nunca
ser verdadeira  por problemas de extensão de sinal.

.sp
É importante lembrar que "getc" e "getchar" são macros,
e portanto construções do tipo "getc (fp++)" podem não produzir
o efeito esperado. Deve-se utilizar "fgetc" para estes casos.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
stdio, ferror, fread, gets, putc, scanf, putw
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
