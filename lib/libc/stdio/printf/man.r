.bp 1
.he 'PRINTF (libc)'TROPIX: Manual de Referência'PRINTF (libc)'
.fo 'Atualizado em 01.11.02'Versão 4.4.0'Pag. %'

.b NOME
.in 5
Formata a saída:
.sp
.wo "printf   -"
escreve na saída padrão ("stdout")
.br
.wo "fprintf  -"
escreve no arquivo dado
.br
.wo "sprintf  -"
armazena em um vetor dado
.br
.wo "snprintf -"
armazena em um vetor de tamanho dado
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int printf (const char *format, ...);
int fprintf (FILE *fp, const char *format, ...);
int sprintf (char *str, const char *format, ...);
int snprintf (char *str, size_t size, const char *format, ...);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "printf" escreve na saída padrão ("stdout"),
"fprintf" escreve no arquivo identificado pelo ponteiro "fp",
"sprintf" e "snprintf" escrevem a saída (seguida de um caractere NULO) no vetor apontado por "str".
A função "snprintf" observa para que o tamanho "size" do vetor não seja ultrapassado
(o que não é o caso de "sprintf").

.sp
Estas funções geram a saída sob o contrôle da cadeia de formato apontada
por "format", que especifica como os argumentos subseqüêntes são
convertidos. Se forem dados argumentos insuficientes para a cadeia de formato,
o comportamento é imprevisível; por outro lado, argumentos em excesso
são simplesmente ignorados. As funções retornam quando é encontrado
o final da cadeia de formato.

.sp
A cadeia de formato é composta de zero ou mais diretivas:
caracteres regulares (diferentes de "\%"), que são copiados
sem alteração para a saída e especificações de conversão,
cada um dos quais utiliza zero ou mais argumentos subseqüêntes.
Cada especificacão de conversão é introduzida pelo caractere "\%",
após o qual temos os seguintes itens (na ordem dada):

.in +3
.ip * 2
Zero ou mais "indicadores", que alteram o significado
da especificação de conversão (ver abaixo).

.ip * 2
Uma cadeia decimal opcional especificando o "tamanho de campo" mínimo.
Se o valor convertido tiver menos caracteres do que o tamanho do campo,
ele será completado com brancos (caractere <sp>) do lado esquerdo
(ou direito, ver abaixo) para o tamanho pedido.

.ip * 2
Uma "precisão" opcional que dá o número mínimo de dígitos
para as conversões "b", "B", "d", "i", "o", "u", "x" e "X",
o número de dígitos após o ponto decimal para as conversões "e", "E" e "f",
o número máximo de dígitos significativos para as conversões "g" e "G", ou
o número máximo de caracteres impressos de uma cadeia para a conversão
"s". A precisão tem a forma de um ponto (".") seguido de uma
cadeia opcional de dígitos decimais; se a cadeia for otimida o valor
da precisão é zero.

.ip * 2
Um tamanho opcional:

.in +2
.ip "h" 3
Um "h" especifica que uma conversão "b", "B", "d", "i", "o",
"u", "x" ou "X" se refere a um argumento "short int" ou
"unsigned short int" (isto é, só são considerados os 16 bits menos
significativos do argumento).

.sp
Um "h" especifica que uma conversão "n" se refere
a um ponteiro para um argumento "short int".

.ip "l" 3
Um "l" especifica que uma conversão "b", "B", "d", "i", "o",
"u", "x" ou "X" se refere a um argumento "long int" ou
"unsigned long int".

.sp
Um "l" especifica que uma conversão "n" se refere
a um ponteiro para um argumento "long int".

.ip "L" 3
Um "L" especifica que uma conversão "e", "E", "f",
"g" ou "G" se refere a um argumento "long double";

.ip "q" 3
um "q" especifica que uma conversão "d" ou "u"
se refere a um argumento "long long int" ou "unsigned long long int"
(pode ser usado "ll" no lugar de "q").

.sp
Se um "h", "l", "L" ou "q" for usado com outras conversões ele será ignorado.

.ep
.in -2

.ip * 2
O caractere que indica o tipo da conversão a ser realizada
(ver abaixo).

.ep
.in -3
.sp
Tanto o tamanho do campo como a precisão (ou ambos) podem ser indicados
por um asterisco ("*") ao invés de uma cadeia de dígitos.
Neste caso, o tamanho do campo ou a precisão é fornecido por um
argumento inteiro.
O argumento a ser convertido deve vir após os argumentos referentes
ao tamanho do campo e precisão. Se o argumento referente ao tamanho
do campo for negativo, ele é interpretado como sendo um indicador "-" seguido
de um tamanho de campo positivo. Se o argumento referente à precisão
for negativo, ele será considerado ausente.

.sp
Os indicadores e os seus significados são:

.in +3
.ip - 5
O resultado da conversão é ajustado à esquerda dentro do campo.

.ip + 5
O resultado de uma conversão com sinal começa sempre com um sinal
("+" ou "-").

.ip <sp> 5
Se o primeiro caractere de uma conversão com sinal não é um sinal,
um branco é anteposto ao resultado. Se ambos os indicadores
<sp> e "+" forem dados, o indicador <sp> é ignorado.

.ip # 5
O valor é convertido para uma forma "alternativa".
Para a conversão "o", a precisão é aumentada de modo que o
resultado sempre comece por um zero.
Para as conversões "b", "B", "x" e "X", o resultado é precedido de "0x".
Para as conversões "p" e "P", o resultado é precedido de "0x",
e é suprimida a separação entre os dois grupos de 4 dígitos hexadecimais.

.ip 0 5
Para as conversões "b", "B", "d", "i", "o", "u", "x", "X",
"e", "E", "f", "g" e "G" é utilizado o dígito "0" para ajustar
o campo à esquerda (após o possível sinal) ao invés do <sp>.
Se ambos os indicadores "0" e <sp> forem dados, o indicador "0"
é ignorado. Se for dada uma precisão para uma das conversões
"b", "B", "d", "i", "o", "u", "x" ou "X", o indicador "0" é
ignorado.

.ep
.in -3

.in 5
.sp
Os caracteres de conversão e os seus significados são:

.in +3
.ip d,i,o,u,x,X 12
O argumento inteiro é convertido para decimal com sinal ("d" ou "i"),
octal sem sinal ("o"), decimal sem sinal ("u"),
hexadecimal sem sinal ("x" ou "X");
as letras "abcdef" são usadas para a conversão "x" e
"ABCDEF" para a conversão "X".
A precisão especifica o número mínimo de dígitos;
se o valor puder ser representado com menos dígitos, será expandido
com zeros à esquerda. Se a precisão não for dada será usado o valor 1.

.ip f 12
O argumento "double" é convertido para a notação decimal no estilo
"[-]ddd.ddd", onde o número de dígitos após o ponto decimal
é dado pela precisão.
Se a precisão não for  dada, são utilizados 6 dígitos após o ponto decimal;
se a precisão dada for zero, o ponto decimal é omitido.

.ip e,E 12
O argumento "double" é convertido para a notação decimal no estilo
"[-]d.dddesddd", com um dígito antes do ponto decimal (que é diferente
de zero se o argumento é diferente de zero) e um número de dígitos
após o ponto decimal que é igual à precisão; se a precisão não for dada
são utilizados 6 dígitos após o ponto decimal;
se a precisão dada for zero, o ponto decimal é omitido.
O caractere "s" representa o sinal "+" ou "-" do expoente.
A conversão "E" gera o número com "E" ao invés de "e" introduzindo
o expoente.

.ip g,G 12
O argumento "double" é convertido no estilo "f" ou "e"
(ou "E" no caso de "G"), conforme as descrições acima.
Se a precisão dada for zero, é utilizado o valor 1.
O estilo usado depende do valor do argumento:
é usado o estilo "e" se o expoente for menor do que -4 ou maior ou igual
à precisão. Zeros ao final da parte fracionária são omitidos;
o ponto decimal é omitido se não for seguido de um dígito.

.ip c 12
O argumento inteiro é convertido para "unsigned char" e escrito.

.ip s 12
O argumento dado deve ser um ponteiro para um vetor de caracteres.
O caracteres do vetor são escritos até (mas não incluindo) um
possível caractere NULO indicando o final do vetor.
Se for dada uma precisão, ela indica o número máximo de caracteres
a escrever; se a precisão não for dada ou for maior do que o
vetor, o vetor deve conter um caractere NULO.

.ip p,P 12
O argumento dado deve ser um ponteiro, cujo valor
é convertido para hexadecimal, no formato "XXXX XXXX". 
as letras "abcdef" são usadas para a conversão "p" e
"ABCDEF" para a conversão "P".
A precisão (se dada) é ignorada.

.ip n 12
O argumento dado deve ser um ponteiro para um inteiro no qual
é ESCRITO o número de caracteres escritos na saída até este ponto por
esta chamada da função. Esta conversão não produz saída.

.ip b,B 12
Esta conversão espera dois argumentos: o primeiro deve conter um
valor inteiro, que é convertido para hexadecimal no estilo "x"
(ou "X" no caso de "B"), conforme as descrições acima;
o segundo argumento deve ser um ponteiro para uma cadeia de
caracteres.

.sp
Esta cadeia de caracteres é composta de zero ou mais seções.
Cada seção contém um caractere inicial com um valor de 1 a 32
(valores de caracteres não gráficos), indicando um dos
32 bites do primeiro argumento (o valor 1 indica o bite menos
significativo); os caracteres seguintes (gráficos) da seção
dão uma descrição deste bite.

.sp
A saída produzida é da forma
.wo '"XXX <desc0|desc1|...>",'
onde "XXX" é o valor hexadecimal do primeiro argumento e os vários
"desci" são as descrições dos bites iguais a 1 no primeiro argumento,
que são obtidas através da cadeia de caracteres (ver o exemplo abaixo).
A ordem em que as descrições são escritas é dada pela ordem em
que elas estão na cadeia; se nenhum bite é igual a 1, a descrição
"<desc0|desc1|...>" é omitida.

.ip \% 12
Escreve o caractere "\%"; nenhum argumento é convertido.
A especificação de conversão completa deve ser "\%\%".

.ep
.in -3

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
As funções "printf", "fprintf", "sprintf" e "snprintf" retornam o
número de caracteres transmitidos 
(não incluindo o NULO nos casos de "sprintf" e "snprintf"),
ou um número negativo em caso de erro.

.in
.sp
.(t
.b OBSERVAÇÕES
.in 5
Em nenhum caso, um tamanho de campo não dado ou pequeno irá causar
o truncamento de um campo; simplesmente o campo será aumentado
de modo a nele caber o valor convertido. 
.)t

.sp
No caso da função "sprintf", é de responsabilidade do usuário
garantir que o vetor apontado por "str" seja suficientemente grande para receber a saída gerada.

.sp
No caso da função "snprintf" é dado o tamanho do vetor através de "size", e se a saída gerada
for maior do que este tamanho, ela é truncada. Esta função é mais segura do
que "sprintf", mas ela não pertence ao padrão ANSI.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
putc, scanf, stdio
.br

.in
.sp
.b EXEMPLO
.in 5
.nf
	O trecho de programa abaixo:

		const char	status_bits[] =
		{
			"\x0C" "Erro de CRC"
			"\x0A" "Setor inválido"
			"\x09" "Setor não encontrado"
			"\x06" "Proteção de escrita"
			"\x05" "Cilindro inexistente"
			"\x04" "Erro de DMA"
			"\x03" "Unidade inválida"
			"\x01" "Erro irrecuperável"
		};

		printf ("\%#0.4B\n", 0x0201, status_bits);

	irá escrever em <stdout>:

		0x0201 <Setor inválido|Erro irrecuperável>
.fi

.bc	/*************************************/
Escrever o dia 25 de junho de 1986, quarta-feira às 9:30h:

	printf
	(	"\%2d:\%02d \%s \%02d/\%02d/\%02d\n",
		hora, min, dia_semana,
		dia, mes, ano
	);

escreverá

	9:30 QUARTA-FEIRA 25/06/85



Escrever PI:

	printf ("PI = \%.5f = \%.5e", 4 * atan (1.0), 4 * atan (1.0));

escreverá

	PI = 3.14159 = 3.14159e+00
.ec	/*************************************/

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
