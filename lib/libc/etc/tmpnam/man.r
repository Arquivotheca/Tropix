.bp
.he 'TMPNAM (libc)'TROPIX: Manual de Referência'TMPNAM (libc)'
.fo 'Atualizado em 26.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "tmpnam -"
gera um nome para um arquivo temporário
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

char	*tmpnam (char *str);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "tmpnam" gera uma cadeia de caracteres que é um nome válido de um
arquivo, e que não é o mesmo de algum arquivo existente.

.sp
Ela gera um nome diferente a cada chamada,
pelo menos nas "TMP_MAX" primeiras chamadas.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Se "str" for um ponteiro NULO, a função "tmpnam" gera o nome em um vetor
estático interno, e devolve um ponteiro para ele. Chamadas subseqüêntes
a "tmpnam" podem modificar o mesmo vetor.

.sp
Se "str" for um ponteiro não NULO, a função "tmpnam" supõe que aponte
para um vetor de no mínimo "L_tmpnam" caracteres; ela armazena o nome
do arquivo neste vetor e devolve o próprio valor de "str". 

.sp
Se a função não conseguir obter um nome de arquivo ainda inexistente,
devolve um ponteiro NULO.

.in
.sp
.b OBSERVAÇÃO
.in 5
A função "tmpnam" apenas gera nomes para arquivos temporários,
NÃO removendo-os automaticamente ao final do uso.
É necessário usar a função "remove" (libc) ou "unlink" (sys) para remove-los.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
mktemp, tmpfile
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
