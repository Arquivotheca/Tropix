.bp 1
.he 'STRTOL (libc)'TROPIX: Manual de Referência'STRTOL (libc)'
.fo 'Atualizado em 11.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Conversão de cadeias em inteiros:
.sp
.wo "strtol  -"
cadeias em qualquer base numérica
.br
.wo "strtuol -"
cadeias em qualquer base numérica (sem sinal)
.br
.wo "atol    -"
cadeias decimais em longos
.br
.wo "atoi    -"
cadeias decimais em inteiros
.br

.in
.sp
.b SINTAXE
.in 5
.(l
long		strtol (const char *str, const char **ptr,
						int base);
unsigned long	strtoul (const char *str, const char **ptr,
						int base);
long		atol (const char *str);
int		atoi (const char *str);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "strtol" retorna como um inteiro longo o valor representado
pela cadeia de caracteres apontada por "str". A cadeia é percorrida
até o primeiro caracter inconsistente com a "base" dada.
Caracteres brancos e tabulações (<ht>) antes do início do número são ignorados.

.sp
Se o valor de "ptr" não for NULO, um ponteiro para o primeiro caracter
após o número reconhecido é armazenado na posição apontada por "ptr". 
Se nenhum inteiro pôde ser obtido, a função devolve zero.

.sp
Se a "base" é positiva (e não maior do que 36), ela é usada como
base para a conversão. Após um sinal opcional, zeros à esquerda
são ignorados, e "0x" ou "OX" é ignorado se a base for 16.
Os dígitos acima de 9 são representados pelas letras maiúsculas
ou minúsculas: "A" para 10, "B" para 11, ......, "Z" para 35.

.sp
Se a "base" é zero, a base é determinada pela própria cadeia
utilizado as convenções da linguagem "C":
após um sinal opcional, um zero determina conversão octal,
e "0x" ou "0X" determina conversão hexadecimal. Em outros casos,
será utilizada conversão decimal. 

.sp
A função "strtoul" é análoga à "strtol", porém devolve um valor
"unsigned long".

.sp
A chamada "atol (str)" é equivalente a
.sp
.nf
		"strtol (str, (char **)0, 10)",
.fi

.sp
e a chamada "atoi (str)" é equivalente a
.sp
.nf
		"(int)strtol (str, (char **)0, 10)".
.fi

.(t
.in
.sp
.b OBSERVAÇÕES
.in 5
Condições de "overflow" são ignoradas.

.sp
Como no TROPIX os inteiros são sempre longos (32 bits), as funções
"atol" e "atoi" são identicas.
.)t

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
ctype, scanf, strtod
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
