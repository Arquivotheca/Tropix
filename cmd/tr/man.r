.bp
.he 'TR (cmd)'TROPIX: Manual de Referência'TR (cmd)'
.fo 'Atualizado em 12.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "tr -"
traduz caracteres
.br

.in
.sp
.b SINTAXE
.in 5
.(l
tr [-cds] [-1 <cadeia1>] [-2 <cadeia2>] [<entrada> [<saída>]] 
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "tr" copia o arquivo de <entrada> para o arquivo de <saída>,
com a substituição ou remoção de caracteres selecionados.

.sp
No modo normal (sem a opção "-d"), os caracteres de entrada
presentes em <cadeia1> são substituídos pelos caracteres correspondentes
de <cadeia2>.

.sp
Se <saída> não for dado, "tr" escreve na saída padrão.
Se <entrada> não for dado ou for "-", "tr" lê da entrada padrão.

.sp
As opções do comando são:

.in +3
.ip -c
Complementa o conjunto de caracteres da <cadeia1> em relação
ao universo de caracteres ISO, exceto o caractere '\0'.
Em outras palavras, a <cadeia1> efetiva será composta de todos os caracteres
ISO (em ordem lexicográfica) de 0x01 a 0xFF, menos os dados.

.ip -d
Ao invés de substituir, remove os caracteres da <entrada>
presentes na <cadeia1>.

.ip -s
Compacta os caracteres da <saída> presentes na <cadeia2>, isto é,
substitui todas as cadeias de caracteres da <saída> repetidos  
presentes na <cadeia2> por um único exemplar do caractere.

.ep
.in -3

.sp
Para facilitar a definição das <cadeia>s, podemos usar as seguintes
abreviações:

.in +3
.ip [a-z] 7
Representa a cadeia de caracteres cujos códigos ISO vão de "a" a "z".

.ip [a*n] 7
Representa a cadeia de caracteres contendo "n" repetições de "a".
O número "n" pode ser dado em decimal, octal ou hexadecimal, de acordo
com as convenções da linguagem "C" (ver "strtol" (libc)).
Se "n" for ZERO ou não for dado, assume-se um valor "enorme";
isto normalmente é usado para completar a <cadeia2> em relação ao
universo do código ISO.

.ip \... 7
O caractere "\" pode ser usado para tirar o efeito especial de qualquer
caractere em uma cadeia. Além disto podem ser usadas as convenções da
linguagem "C", tais como "\x34" (hexadecimal), "\123" (octal),
"\n", "\t", etc ...

.ep
.in -3

.in
.sp
.(t
.b OBSERVAÇÃO
.in 5
Todas as combinações de opções são válidas.
.)t

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ed, vi
.br
.wo "(libc):"
strtol
.br
.wo "(fmt): "
iso
.br

.in
.sp
.b EXEMPLOS
.in 5
Substituição:
.sp
.nf
		tr -1 "abc" -2 "xy"
.fi
.sp
substitui "a" por "x" e "b" por "y" ("c" não é modificado).

.sp 2
Substituição e compactação:
.sp
.nf
	tr -cs -1 "[A-Z][a-z]" -2 "[\n*]" caixas | sort | uniq 
.fi
.sp
Constrói uma lista de todas as palavras do arquivo "caixas", com
uma palavra por linha (uma palavra é considerada a seqüência máxima
de letras não acentuadas). Em seguida, esta lista é ordenada e 
finalmente, as palavras repetidas são eliminadas.

.sp 2
Remoção:
.sp
.nf
		tr -cd -1 "aeiouAEIOU" texto | wc -c
.fi
.sp
imprime o número de vogais (não acentuadas) do arquivo "texto".

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
