.bp
.he 'PASTE (cmd)'TROPIX: Manual de Referência'PASTE (cmd)'
.fo 'Atualizado em 23.11.99'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "paste -"
concatena linhas de vários arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
paste [-d <delimitador>] <arquivo> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "paste" concatena as linhas correspondentes de um ou mais <arquivo>s
de entrada, e escreve as linhas resultantes na saída padrão.

.sp
Normalmente é usado o caratere <ht> como separador entre as colunas
resultantes (veja a opção "-d", abaixo).

.sp
Se for dada a cadeia "-" como nome de um <arquivo>, ela representa a entrada
padrão. Se isto ocorrer mais de uma vez, será lida uma linha da entrada padrão,
circularmente, para cada ocorrência de "-".

.sp
As opções do comando são:

.in +3
.ip -d
Usa a cadeia <delimitador> como separador entre as várias colunas, ao
invés do caractere <ht>.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
As linhas lidas e geradas estão limitadas a 1024 bytes.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cat
.br

.in
.sp
.b EXEMPLOS
.in 7
Imprimir 3 arquivos, em colunas separadas por " | ".
.sp
.nf
		paste -d " | " a b c
.fi

.sp
Listar um diretório em 5 colunas:
.sp
.nf
		ls | paste - - - - -
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
