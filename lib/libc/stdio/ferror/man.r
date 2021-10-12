.bp
.he 'FERROR (libc)'TROPIX: Manual de Referência'FERROR (libc)'
.fo 'Atualizado em 08.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Manipulação do estado de um arquivo:
.sp
.wo "ferror    -"
testa a ocorrência de um erro
.br
.wo "feof      -"
testa o final do arquivo
.br
.wo "clearerr  -"
desliga os indicadores de erro e final de arquivo
.br
.wo "fileno    -"
retorna o descritor do arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int	ferror (FILE *fp);
int	feof (FILE *fp);
void	clearerr (FILE *fp);
int	fileno (FILE *fp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A macro "ferror" retorna um valor diferente de zero se ocorreu um erro de
entrada/saída no arquivo "fp", e zero em caso contrário.

.sp
A macro "feof" retorna um valor diferente de zero se foi detetado o final do
arquivo (EOF) na leitura  do arquivo "fp", e zero em caso contrário.

.sp
A macro "clearerr"
desliga os indicadores de erro e de fim de arquivo (EOF) do arquivo "fp". 

.sp
A macro "fileno" devolve o descritor associado ao arquivo "fp".

.in
.sp
.b
OBSERVAÇÃO
.r
.in 5
Essas funções são implementadas como macros; elas não podem ser
declaradas nem redefinidas.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
open
.br
.wo "(libc):"
fopen, stdio
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
