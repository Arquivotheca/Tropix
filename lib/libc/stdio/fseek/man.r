.bp 1
.he 'FSEEK (libc)'TROPIX: Manual de Referência'FSEEK (libc)'
.fo 'Atualizado em 09.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Manipulação do ponteiro da posição do arquivo:
.sp
.wo "fseek  -"
posiciona o ponteiro
.br
.wo "rewind -"
retorna o ponteiro ao início
.br
.wo "ftell  -"
obtem o valor do ponteiro
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int	fseek (FILE *fp, long offset, int whence);
void	rewind (FILE *fp);
long	ftell (FILE *fp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função  "fseek" posiciona o ponteiro do arquivo "fp"
para a próxima leitura ou escrita.
A nova posição do ponteiro será
à distância (com sinal) "offset"
do começo, da posição corrente ou do final do arquivo,
conforme "whence" tenha respectivamente os valores
SEEK_SET, SEEK_CUR ou SEEK_END.

.sp
A função "rewind" retorna ao início o ponteiro de posição
do arquivo "fp". Ela  é equivalente a 
"fseek (fp, 0, SEEK_SET)", exceto que o indicador de erro também
é apagado.

.sp
A função "ftell" devolve a posição corrente do ponteiro do 
arquivo "fp".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função  "fseek"
retorna um número negativo
caso ocorra algum erro, caso contrário
retorna zero.
Lembre-se que "fseek" não pode ser usado em arquivos associados
a terminais, nem a "pipes".

.in
.sp
.b OBSERVAÇÕES
.in 5
As funções "fseek" e "rewind" desfazem os efeitos de "ungetc" e
apagam o indicador de "fim de arquivo".

.sp
Após um "fseek" ou "rewind",
em um arquivo de atualização, a próxima operação pode ser
de leitura ou escrita.

.sp
Como no sistema TROPIX não há distinção entre arquivos de texto ou
binários, não há restrições no uso das funções "fseek" e "ftell".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
lseek
.br
.wo "(libc):"
fopen, popen, ungetc, fgetpos, fsetpos, feof
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
