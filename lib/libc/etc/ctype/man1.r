.bp 1
.he 'CONV (libc)'TROPIX: Manual de Referência'CONV (libc)'
.fo 'Escrito em 20.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Macros para a conversão de caracteres:
.sp
.in 5
.wo "toupper  -"
converte um caractere minúsculo em maiúsculo
.br
.wo "tolower  -"
converte um caractere maiúsculo em minúsculo
.br
.wo "_toupper -"
converte um caractere para maiúsculo
.br
.wo "_tolower -"
converte um caractere para minúsculo
.br
.wo "toiso    -"
converte um caractere para ISO
.br
.wo "toascii  -"
converte um caractere para ASCII
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <ctype.h>

int	toupper (int c);
int	tolower (int c);
int	_toupper (int c);
int	_tolower (int c);
int	toiso (int c);
int	toascii (int c);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A macro "toupper" converte uma letra minúscula na letra maiúscula
correspondente.
Se o argumento dado for ISO, mas não for uma letra minúscula,
ele é retornado sem modificação.
Se o argumento não for ISO, o resultado será imprevisível.

.sp
A macro "tolower" converte uma letra maiúscula na letra minúscula 
correspondente.
Se o argumento dado for ISO, mas não for uma letra maiúscula
ele é retornado sem modificação.
Se o argumento não for ISO, o resultado será imprevisível.

.sp
As macros "_toupper" e "_tolower" são simplificações, respectivamente,
das macros "toupper" e "tolower", sendo mais rápidas pois não testam
o argumento. Se o argumento não for, respectivamente,
uma letra minúscula ou uma letra maiúscula, o resultado será imprevisível.

.sp
.(t
A macro "toiso" zera os bites (8 - 31) de um caractere,
para obter um valor que necessariamente
faz parte do conjunto de caracteres ISO. 
.)t

.sp
A macro "toascii" zera os bites (7 - 31) de um caractere,
para obter um valor que necessariamente
faz parte do conjunto de caracteres ASCII.
Repare que esta macro NÃO converte vogais acentuadas do ISO nas
vogais correspondentes sem acento do ASCII.

.in
.sp
.b
OBSERVAÇÕES
.r
.in 5
O conjunto de caracteres do TROPIX é o código ISO, de 8 bites,
e as vogais acentuadas e o "cê-cedilha" são levados em conta
pelas macros "toupper", "tolower", etc ...

.sp
Repare, que existe um (e apenas um) caso em que a macro de conversão
"toupper" falha: no caso do caractere 0xFF ("y" com trema, do francês),
ela fornece 0xDF ("s" forte alemão), que certamente não é o seu
correspondente maiúsculo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.br
.wo "(libc):"
ctype, getc
.br
.wo "(fmt): "
ascii, iso
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
