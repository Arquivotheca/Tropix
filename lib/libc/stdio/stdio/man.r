.bp 1
.he 'STDIO (libc)'TROPIX: Manual de Referência'STDIO (libc)'
.fo 'Atualizado em 11.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
stdio - pacote padrão de funções de E/S para o usuário

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

FILE	*stdin, *stdout, *stderr;
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A "stdio" constitui um pacote eficiente de funções de entrada/saída
gerenciando áreas intermediárias para serem usadas a nível de usuário.

.sp
As macros "getc" e "putc" lêem/escrevem caracteres rapidamente.
As macros "getchar" e "putchar", e as funções "fgetc", "fgets", "fngets",
"fprintf", "fputc", "fread", "fscanf", "fwrite", "gets", "getw",
"printf", "puts", "putw", e "scanf" são todas baseadas em "getc" e "putc";
elas podem todas serem usadas no mesmo arquivo aberto.

.sp
Um arquivo utilizando as facilidades deste pacote é identificado
por um ponteiro para uma estrutura do tipo FILE.
A função "fopen" cria certos dados para o arquivo e devolve
um ponteiro para identifica-lo em tôdas  as transações seguintes.

.sp
Normalmente, existem 3 arquivos abertos com ponteiros constantes
definidos em <stdio.h>, e associados com os 3 arquivos padrão:
.sp
.nf
	"stdin"		arquivo padrão de entrada
	"stdout"	arquivo padrão de saída
	"stderr"	arquivo padrão de mensagens de erro
.fi
.sp
Um ponteiro NULO ("NOFILE") identifica um ponteiro inexistente.

.sp
A constante inteira EOF (-1) é devolvida em condição de
fim-de-arquivo ou erro pela maioria das funções inteiras que
trata dos arquivos (veja maiores detalhes nas descrições
individuais).

.sp
A constante inteira "BUFSIZ" define o tamanho das área de entrada/saída
usadas pela implementação em particular.

.sp
Qualquer programa que utiliza este pacote necessita incluir
o arquivo <stdio.h>. As funções e constantes relativas a
este pacote estão declaradas neste arquivo, e não necessitam
de declarações adicionais. As constantes e as seguintes
funções são implementadas como macros, e não podem ser
redeclaradas: "getc", "getchar", "putc", "putchar", "ferror",
"feof", "clearerr" e "fileno".

.sp
Arquivos de saída possuem normalmente áreas de entrada/saída regulares
se estão associados a um arquivo em disco e áreas de entrada/saída de
linha se estão associados a um terminal.

As funções "setbuf" e "setvbuf" podem ser usadas para mudar
esta estratégia de áreas de entrada/saída (inclusive NÃO usar
área de entrada/saída); nas suas descrições se encontram maiores detalhes.

.sp
Quando um arquivo não possui área de entrada/saída, a informação é posta
imediatamente no terminal ou no arquivo em disco, assim que ela é escrita;
com uma área de entrada/saída regular, muitos caracteres são
guardados e  escritos como um bloco quando a área estiver cheia;
com uma área de entrada/saída de linha, cada linha da saída é posta
no terminal assim que a linha for terminada, isto é, assim que o <nl>
for escrito, ou se a área estiver cheia.

.in
.sp
.b DIAGNÓSTICOS
.in 5
Ponteiros FILE inválidos em geral causam erros graves, incluindo
a terminação do programa. As descrições individuais das
funções dão detalhes das possíveis condições de erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
open, close, lseek, pipe, read, write
.br
.wo "(libc):"
fclose, ferror, fopen, fread, fseek, getc, gets,
.br
.nf
             popen, printf, putc, puts, scanf, setbuf, system,
             ungetc
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
