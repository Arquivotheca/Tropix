.bp 1
.he 'FOPEN (libc)'TROPIX: Manual de Referência'FOPEN (libc)'
.fo 'Atualizado em 08.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Funções para abertura de arquivos:
.sp
.wo "fopen    -"
abre um arquivo dado o nome
.br
.wo "freopen  -"
reabre um arquivo antigo 
.br
.wo "fdopen   -"
abre um arquivo dado o descritor
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

FILE *fopen (const char *nome, const char *modo);
FILE *freopen (const char *nome, const char *modo, FILE *fp);
FILE *fdopen (int fd, const char *modo);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "fopen" abre o arquivo cujo nome é dado em "nome", e
retorna um ponteiro para a estrutura FILE associada ao arquivo.

.sp
O argumento "modo" é um ponteiro para uma cadeia de caracteres
que define como o arquivo deve ser aberto.
Ela deve ter um dos seguintes valores:

.in +5
.ip '"r", "rb"' 20
abre para leitura.
.ip '"w", "wb"' 20
abre truncando ou cria o arquivo para escrita.
.ip '"a", "ab"' 20
acréscimo: abre para escrita no final do arquivo ou cria para escrita.
.ip '"r+", "r+b", "rb+"' 20
abre para atualização, ou seja, para leitura e escrita.
.ip '"w+", "w+b", "wb+"' 20
abre truncando ou cria o arquivo para atualização.
.ip '"a+", "a+b", "ab+"' 20
acréscimo: abre ou cria para atualização no final do arquivo.
.ep
.in -5

.sp
A função "freopen" abre o arquivo cujo nome é dado em "nome", e
associa o ponteiro de arquivo "fp" a ele.
O argumento "modo" é similar ao da função "fopen" visto acima.
Se o ponteiro de arquivo "fp" dado apontava para um arquivo aberto,
ele é fechado antes da associação.
Esta função é normalmente usada para associar ponteiros de arquivos
já abertos como "stdin", "stdout" e "stderr" a outros arquivos.

.sp
A função "fdopen" associa um ponteiro de arquivo a um descritor de arquivo.
Descritores de arquivos são obtidos através das chamadas ao sistema
"open", "dup", "creat", "fcntl", ou "pipe" (sys),
que abrem o arquivo mas não retornam um ponteiro de arquivo e sim
um descritor de arquivos. O argumento "modo"
deve coincidir com o modo do arquivo aberto.

.sp
Quando um arquivo é aberto para atualização, operações
tanto de entrada como de saída podem ser realizadas no arquivo.
No entanto, uma operação de saída não pode ser
seguida diretamente de uma operação de entrada sem que um 
"fseek" ou um "rewind" (libc)
tenha sido executado neste meio tempo.
Analogamente, uma operação de entrada não pode ser 
seguida diretamente de uma operação de saída sem que um
"fseek" ou um "rewind"
tenha sido executado neste meio tempo,
ou que a operação de entrada
tenha encontrado o final do arquivo (EOF).

.sp
Quando um arquivo é aberto para "acréscimo" (isto é, quando o
"modo" for  "a", "a+", "ab+" ou "a+b"), é impossível reescrever informações
já presentes no arquivo.
A função "fseek" pode ser usada para alterar o ponteiro do
arquivo para qualquer posição no arquivo, mas quando há uma escrita no
arquivo, o ponteiro de arquivo corrente é ignorado e todas
as escritas são feitas no final do arquivo.

.sp
Caso dois processos distintos
abram o mesmo arquivo para "acréscimo", cada processo pode escrever livremente
no arquivo sem que um processo destrua os dados escritos
pelo outro processo.
A saída dos dois processos serão misturadas no arquivo, na ordem
em que eles os escrevem.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As funções "fopen", "freopen" e "fdopen" 
retornam um ponteiro NULO caso haja algum erro.
Neste caso, a variável externa "errno"
contem a razão do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Repare que no sistema TROPIX, o caractere "b" do modo de abertura
de um arquivo não é relevante, já que não há distinção entre
arquivos de textos ou binários.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
creat, dup, open, pipe
.br
.wo "(libc):"
fclose, fseek, stdio, perror, strerror
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
