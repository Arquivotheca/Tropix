.bp 1
.he 'SCANF (libc)'TROPIX: Manual de Referência'SCANF (libc)'
.fo 'Atualizado em 25.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Leitura formatada:
.sp
.wo "scanf  -"
da entrada padrão ("stdin")
.br
.wo "fscanf -"
de um arquivo dado
.br
.wo "sscanf -"
de uma cadeia dada
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int	scanf (const char *format, ...)
int	fscanf (FILE *fp, const char *format, ...)
int	sscanf (const char *str, const char *format, ...)
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "scanf" lê da entrada padrão ("stdin"), "fscanf"
lê do arquivo identificado pelo ponteiro "fp", e "sscanf"
lê da cadeia de caracteres apontada por "str".
.bc	/*************************************/
No texto abaixo, para o caso da função "sscanf", o "arquivo" deve
ser entendido como a cadeia "s".
.ec	/*************************************/

.sp
Cada uma destas funções lê caracteres da entrada sob o contrôle
da cadeia de formato apontada por "format", que especifica as
seqüências de entrada admissíveis e como elas são convertidas
para as atribuições. Os argumentos subseqüentes a "format" são
usados como ponteiros para os objetos a receber as entradas convertidas.
Se forem dados argumentos insuficientes para a cadeia de formato,
o comportamento é imprevisível; por outro lado, argumentos em excesso
são simplesmente ignorados.

.sp
A cadeia de formato é composta de zero ou mais diretivas:
um ou mais caracteres brancos (conforme "isspace" (libc));
um caractere regular (diferente de "\%"); ou uma especificação de conversão.

.sp
Cada especificação de conversão é introduzida pelo caractere "\%",
após o qual temos (na ordem dada):

.in +3
.ip * 2
Um caractere opcional de supressão de atribuição "*".

.ip * 2
Uma cadeia decimal opcional especificando o "tamanho de campo" máximo.

.ip * 2
Um "h" opcional especificando que uma conversão "d", "i" ou "n" se refere
a um argumento que é um ponteiro para um "short int" ao invés de "int";
um "h" opcional especificando que uma conversão "o", "u" ou "x" se refere
a um argumento que é um ponteiro para um "unsigned short int" ao invés
de "unsigned int";
um "l" opcional especificando que uma conversão "d", "i" ou "n" se refere
a um argumento que é um ponteiro para um "long int" ao invés de "int";
um "l" opcional especificando que uma conversão "o", "u" ou "x" se refere
a um argumento que é um ponteiro para um "unsigned long int" ao invés
de "unsigned int";
um "l" opcional especificando que uma conversão "e", "f" ou "g" se refere
a um argumento que é um ponteiro para um "double" ao invés de "float";
um "L" opcional especificando que uma conversão "e", "f" ou "g" se refere
a um argumento que é um ponteiro para um "long double" ao invés de "float";

.ip * 2
O caractere que indica o tipo da conversão a ser realizada
(ver abaixo).

.ep
.in -3
.sp
As diretivas são executadas seqüencialmente. Se uma delas falhar,
como descrito abaixo, a função retorna. Falhas são classificadas
em falhas de fim-de-arquivo (devido à falta de caracteres de entrada) ou
falhas de formato (devido à entrada inapropriada).

.in +3
.ip * 2
Uma diretiva composta de brancos é executada pela leitura
de caracteres, até o primeiro caractere não-branco (que permanece não-lido),
ou até que seja atingido o final do arquivo.

.ip * 2
Uma diretiva que é um caractere regular é executada pela leitura
de um caractere do arquivo. Se o caractere não for igual à própria
diretiva, ela falha, e o caractere que diferiu e os subseqüentes
permanecem não-lidos.

.ip * 2
Uma diretiva que é uma especificação de conversão define um conjunto
de seqüencias de entrada admissíveis, como descritas abaixo para
cada especificador. 
Uma especificação de conversão é executada nos seguintes passos:

.in +3
.ip 1. 3
Caracteres brancos da entrada são pulados (conforme "isspace"),
exceto no caso dos especificadores "[", "c" ou "n". 

.ip 2. 3
Um item de entrada é lido, exceto no caso do especificador "n".
Um item de entrada é definido como a seqüencia mais longa de caracteres
de entrada (até o máximo comprimento do campo, se dado), que é
uma subseqüencia inicial aceita pelo formato definido pelo
especificador (as seqüências admissíveis).
O primeiro caractere (se houver) após o item de entrada
permanece não-lido. Se o comprimento do item de entrada é zero,
a execução da diretiva falha: esta condição é uma falha de formato
a não ser que não havia mais caracteres disponíveis,
tratando-se então de uma falha de fim-de-arquivo.

.ip 3. 3
Exceto no caso de um especificador "\%", o item de entrada
(ou o número de caracteres já lidos, no caso da diretiva "\%n") 
é convertido para o tipo apropriado do especificador de conversão.
Se o item de entrada não for uma seqüencia de caracteres admissível,
a execução da diretiva falha: esta condição é uma falha de formato.
A não ser que a supressão da atribuição foi indicada por um "*",
o resultado da conversão é posto no objeto apontado pelo primeiro
argumento seguindo o argumento "formato" que ainda não tenha
recebido um resultado. Se este objeto não tem o tipo apropriado,
ou se o resultado da conversão não pode ser representado
no espaço disponível, o comportamento é indefinido.

.ep
.in -3
.in -3
.sp
Os especificadores de conversão válidos são os seguintes:

.in +3
.ip d 7
Aceita um inteiro decimal opcionalmente com sinal, cujo formato
é o mesmo esperado pela função "strtol" (libc) com o valor
"10" para o argumento "base".
O argumento correspondente deve ser um ponteiro para um inteiro.

.ip i 7
Aceita um inteiro opcionalmente com sinal, cujo formato
é o mesmo esperado pela função "strtol" (libc) com o valor
"0" para o argumento "base".
O argumento correspondente deve ser um ponteiro para um inteiro.

.ip o 7
Aceita um inteiro octal opcionalmente com sinal, cujo formato
é o mesmo esperado pela função "strtoul" (libc) com o valor
"8" para o argumento "base".
O argumento correspondente deve ser um ponteiro para um inteiro sem sinal.

.ip u 7
Aceita um inteiro decimal opcionalmente com sinal, cujo formato
é o mesmo esperado pela função "strtoul" (libc) com o valor
"10" para o argumento "base".
O argumento correspondente deve ser um ponteiro para um inteiro sem sinal.

.ip x 7
Aceita um inteiro hexadecimal opcionalmente com sinal, cujo formato
é o mesmo esperado pela função "strtoul" (libc) com o valor
"16" para o argumento "base".
O argumento correspondente deve ser um ponteiro para um inteiro sem sinal.

.ip e,f,g 7
Aceita um número flutuante opcionalmente com sinal, cujo formato
é o mesmo esperado pela função "strtod" (libc).
O argumento correspondente deve ser um ponteiro para um flutuante.

.ip s 7
Aceita uma seqüencia de caracteres não-brancos.
O argumento correspondente deve ser um ponteiro para um
vetor de caracteres com tamanho suficiente para receber
a seqüencia e mais o caractere NULO conclusivo,
que é adicionado automaticamente. 

.ip [ 7
Aceita uma seqüencia não vazia de caracteres de um conjunto
de caracteres admissíveis (o "scanset").
O argumento correspondente deve ser um ponteiro para um
vetor de caracteres com tamanho suficiente para receber
a seqüencia e mais o caractere NULO conclusivo,
que é adicionado automaticamente. 

.sp
A especificação de conversão inclui todos os caracteres subseqüentes
na cadeia do "formato" até (e incluindo) o "]" final.
Os caracteres entre os colchetes (a "scanlist") compõe o "scanset",
a não ser que o primeiro caractere após o "[" seja um "^", caso no
qual o "scanset" contém todos os caracteres que NÃO estão na "scanlist"
(entre o "^" e o "]").

.sp
Em particular, se a especificação começar por "[]" ou "[^]",
o "]" pertence à "scanlist", e é procurado um outro "]", que irá indicar
o final da especificação.

.sp
Um intervalo de caracteres pode ser representado pela construção
<começo>-<final>: assim "[0123456789]" pode ser expresso por "[0-9]".
Em particular, se a especificação começar por "[-",  "[]-" ou "[^]-",
o "-" pertence à "scanlist"; este modo deve ser usado para incluir
o "-" na "scanlist" sem indicar uma construção <começo>-<final>.

.ip c 7
Aceita uma seqüencia de "n" caracteres, onde "n" é o tamanho do campo
dado (ou "1", se ele não foi dado). 
O argumento correspondente deve ser um ponteiro para um
vetor de caracteres com tamanho suficiente para receber a seqüencia.
NÃO é adicionado um caractere NULO ao final.

.ip p 7
Aceita um inteiro hexadecimal (representando o valor de um ponteiro),
cujo formato é o mesmo que é produzido pela conversão "\%p" ou "\%#p"
da função "printf" (ou seja, "XXXX XXXX" ou "0xXXXXXXXX).
O argumento correspondente deve ser um ponteiro para um ponteiro para "void".

.ip n 7
Não são lidos caracteres da entrada.
O argumento correspondente deve ser um ponteiro para um inteiro,
no qual é armazenado o número de caracteres lidos da entrada
até este ponto por esta chamada da função.
A execução de uma diretiva "\%n" não incrementa o contador de atribuições,
que é devolvido quando do retorno da função.

.ip \% 7
Aceita um único "\%"; não ocorre nenhuma conversão nem atribuição.
A especificação completa deve ser "\%\%".

.ep
.in -3
.sp 2
Os especificadores de conversão "E", "G", "X" e "P" também são aceitos
e são equivalentes aos respectivos especificadores minúsculos.

.sp
Se é encontrado um fim-de-arquivo durante a entrada de caracteres,
a conversão é encerrada. Se isto ocorrer antes de se encontrar
caracteres admissíveis pela diretiva corrente (além de caracteres
brancos, onde permitidos), a execução da diretiva corrente termina
com um erro de fim-de-arquivo; em caso contrário, a não ser
que a diretiva corrente é terminada por um erro de formato, a
execução da diretiva seguinte (caso haja) é terminada por
um erro de fim-de-arquivo.

.sp
Se a conversão termina por um caractere não admissível, este caractere
é deixado não-lido no arquivo de entrada. Caracteres brancos
(incluindo caracteres <nl>) após o item não são lidos, a não ser
que haja uma diretiva apropriada (por exemplo, um ou mais caracteres brancos).
O sucesso de diretivas literais (caracteres regulares do "formato",
que devem se encontrar literalmente no arquivo) e atribuições
suprimidas somente pode ser determinado pela diretiva "\%n".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Estas funções devolvem o número de ítens que foram lidos e atribuídos
com sucesso. Este número pode ser menor do que o esperado, ou até zero,
no caso de uma falha de formato prematura.

.sp
Se ocorrer uma falha de fim-de-arquivo antes de alguma atribuição,
ou se a cadeia de formato tiver construções inválidas,
as funções devolvem EOF.

.in
.sp
.b OBSERVAÇÃO
.in 5
Repare que "\%E" é equivalente a "\%e" e NÃO a "\%le".
O mesmo vale para "\%G" e "\%g".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
isspace, getc, printf, stdio, strtod, strtol, strtoul
.br

.in
.sp
.b EXEMPLOS
.in 5
.(t
A chamada:
.sp
.nf
		#include <stdio.h>
		int i, n; float x; char name[50];
		n = fscanf (stdin, "\%d\%f\%s", &i, &x, name);

.fi
.)t
com a entrada:
.nf

		25  54.32e-1 thompson

.fi
irá atribuir "3" a "n", "25" a "i", "5.432" a "x" e "thompson"
(com caractere NULO no final) a "name".
.sp 2
A chamada:
.sp
.nf
		#include <stdio.h>
		int i;  float x; char name[50];
		scanf ("\%2d\%f\%*d \%[0123456789]", &i, &x, name);

.fi
.(t
com a entrada:
.nf

		56789 0123 56a72

.fi
.)t
irá atribuir "56" a "i", "789.0" a "x", irá pular "0123",
e atribuir "56" (com caractere NULO no final) a "name".
O próximo caractere a ser lido da entrada será um "a". 
.sp 2
.(t
Finalmente, para ler repetidamente da entrada padrão
uma quantidade, um unidade e o nome de um item:
.sp
.nf
	int count; float quant; char units[21], item[21];
	do
	{
	    count = scanf ("\%f\%20s of \%20s", &quant, units, item);
	    scanf ("\%*[^\n]");
	}
	while (count != EOF);

.fi
Se "stdin" contiver as seguintes linhas:
.nf

	2     quarts		of            oil
	-12.8degrees Celsius
	lots of luck
	10.0LBS        of        fertilizer
	100joules       of       energy
	
.fi
.)t
a execução do exemplo é equivalente à execução das seguintes
atribuições:
.sp

.nf
+-------+-------+---------+------------+-------------------------+
| count	| quant	| units	  |    item    |	observação	 |
+-------+-------+---------+------------+-------------------------+
| 3	| 2.0	| quarts  | oil	       |			 |
| 2	| -12.8	| degrees | -	       | "C" não aceito por "o"  |
| 0	| -	| -	  | -	       | "l" não aceito por "\%f" |
| 3	| 10.0	| LBS	  | fertilizer |			 |
| 3	| 100.0	| joules  | energy     |			 |
| EOF	|	|	  |	       |			 |
+-------+-------+---------+------------+-------------------------+
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
