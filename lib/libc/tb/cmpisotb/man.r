.bp
.he 'CMPTB (libc)'TROPIX: Manual de Referência'CMPTB (libc)'
.fo 'Atualizado em 11.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Tabelas de comparação de caracteres:
.sp
.wo "cmpisotb   -"
tabela para ordenação de caracteres ISO
.br
.wo "cmpfoldtb  -"
tabela mapeando maiúsculas em minúsculas ("FOLD")
.br
.wo "cmpafoldtb -"
tabela adicional para desempate da "FOLD"
.br
.wo "cmpdictb   -"
tabela onde não alfanuméricos são brancos

.in
.sp
.b SINTAXE
.in 5
.(l
#include <string.h>

extern const char	cmpisotb[];
extern const char	cmpfoldtb[];
extern const char	cmpafoldtb[];
extern const char	cmpdictb[];
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas tabelas são usadas para comparação de caracteres quando a
ordem desejada não coincide com aquela dada pelo código no qual
os caracteres estão representados.
Elas podem ser usadas com as funções da série "str" tais como
"strtcmp", "strttcmp", "strntcmp", "strnttcmp" e suas análogas
da série "mem", tais como "memtcmp" e "memttcmp" (libc).

.sp
A tabela "cmpisotb" mapeia todos os caracteres acentuados
em caracteres sem acento, segundo a definição do
código ISO.

.sp
A tabela "cmpfoldtb" mapeia todos os caracteres 
maiúsculos em seus respectivos minúsculos.

.sp
A tabela "cmpfoldatb" serve para o desempate no caso de
igualdade quando se utiliza a tabela "cmpfoldtb". Ela
faz com que as letras minúsculas sejam consideradas de
valor menor que as maiúsculas, i.e., as letras minúsculas
serão listadas antes das maiúsculas numa classificação
ascendente.

.sp
A tabela "cmpdictb" mapeia todos os caracteres que não
são alfanuméricos em <sp>. Isto é utilizado para obter a ordem
utilizada em dicionários, onde a pontuação não influi na ordenação
das palavras.

.in
.sp
.b OBSERVAÇÃO
.in 5
Todas as tabelas assumem um código de representação de
caracteres contendo 8 bits. Desta forma, todas elas tem
um tamanho de 256 bytes.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sort
.br
.wo "(libc):"
strtcmp, strntcmp, strttcmp, strnttcmp, memtcmp,
.br
.wo "        memttcmp"
.br

.in
.sp
.(t
.b EXEMPLO
.in 7
.sp
.nf
	#include <string.h>

	main ()
	{
		char	 	*s1, *s2;
		const char	*tab1, *tab2;
		int	 	i;

		tab1 = cmpfoldtb;
		tab2 = cmpafoldtb;
		. . . . . .

		i = strttcmp (s1, s2, tab1, tab2);
	}


.fi
.)t

A igualdade das cadeias "s1" e "s2" será analisada
utilizando-se uma tabela que não diferencia letras
maiúsculas de minúsculas. Se a comparação usando
a tabela "tab1" der igual, usa-se "tab2", onde as
letras minúsculas precedem as maiúsculas numa
classificação ascendente.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
