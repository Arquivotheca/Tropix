.bp 1
.he 'REGCMP (libc)'TROPIX: Manual de Referência'REGCMP (libc)'
.fo 'Atualizado em 23.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Processamento de expressões regulares:
.sp
.wo "regcmp -"
compila a expressão
.br
.wo "regex  -"
executa a expressão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

char	*regcmp (const char *pattern);
char	*regex (const char *automata,
				const char *str, char *match);
extern char *_loc1;
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "regcmp" compila a expressão regular
dada na cadeia apontada por "pattern"
e retorna um ponteiro para a descrição do autômato gerado.

.sp
A função "regex" executa a descrição do autômato "automata" na cadeia "str".
A sub-cadeia em "str" correspondente à expressão regular "pattern"
é copiada em "match".
A função "regex" retorna um ponteiro para
o próximo caractere em "str" após esta sub-cadeia.

.sp
O ponteiro "_loc1" aponta para o início de "match" em "str".

.sp
Normalmente um caractere da expressão regular corresponde a si próprio.
No entanto, os seguintes meta-caracteres têm significado especial:

.in 10

.ip ^ 4
No início de uma expressão regular, corresponde ao início
da cadeia "str".

.ip $ 4
No final de uma expressão regular, corresponde ao final
da cadeia "str".

.ip . 4
Corresponde a qualquer caractere, exceto <nl>.

.ip [] 4
Uma sequência não vazia de caracteres dentro de um par de colchetes
especifica um conjunto de caracteres,
que corresponde a um caractere pertencente a este conjunto.

.sp
Existem algumas convenções que simplificam a especificação
de um conjunto de caracteres.
Um "-" (menos) entre dois caracteres (em ordem crescente ISO),
na forma "<ch1>-<ch2>",
equivale a todos os caracteres entre <ch1> e <ch2>, inclusive.
Um "^" (acento circunflexo) logo após o colchete esquerdo
indica que o conjunto de caracteres equivalente será 
o complemento do conjunto especificado a seguir.

.ip * 4
Corresponde a zero ou mais repetições do caractere anterior.

.ip + 4
Corresponde a uma ou mais repetições do caractere anterior.

.ip \ 4
Um meta-caractere perde o seu significado especial
se vier precedido do caractere "\" (barra invertida).

.ep
.sp
O reconhecimento de expressões regulares tenta sempre
obter a cadeia mais longa possível.

.in
.sp
.b
DIAGNÓSTICOS
.r
.in 5
Em caso de erro na cadeia com a expressão regular,
"regcmp" retorna um ponteiro NULO.
.br
.sp
Se o autômato não puder ser executado,
"regex" retorna um ponteiro NULO.

.in
.sp
.b OBSERVAÇÃO
.in 5
O espaço de memória necessário para armazenar a descrição do autômato
é obtido através de "malloc" (libc).
Após o uso, o usuário (se deseja) pode liberar este espaço
usando "free" (libc).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
(libc): patmatch, malloc, free

.in
.sp
.b EXEMPLOS
.in 5
O trecho de programa abaixo lê uma linha da entrada padrão
e caso haja um identificador presente na linha, escreve-o na saída padrão:
.sp
.nf
	char	*automato, linha[128], identificador[128];

		. . .

	automato = regcmp ("[a-zA-Z_][a-zA-Z_0-9]*");

	gets (linha);

	if (regex (automato, linha, identificador) != NULL)
	{
		printf
		(	"Achei o identificador: '%s'\n",
			identificador
		);
	}
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
