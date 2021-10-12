.bp 1
.he 'CTYPE (libc)'TROPIX: Manual de Referência'CTYPE (libc)'
.fo 'Escrito em 01.11.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Macros para a classificação de caracteres:
.sp
.wo "isalpha  -"
verifica se é uma letra
.br
.wo "isupper  -"
verifica se é uma letra maiúscula
.br
.wo "islower  -"
verifica se é uma letra minúscula
.br
.wo "isdigit  -"
verifica se é um dígito decimal
.br
.wo "isxdigit -"
verifica se é um dígito hexadecimal
.br
.wo "isalnum  -"
verifica se é alfanumérico
.br
.wo "isspace  -"
verifica se é um espaço
.br
.wo "ispunct  -"
verifica se é um caractere de pontuação
.br
.wo "isprint  -"
verifica se pode ser impresso
.br
.wo "isgraph  -"
verifica se tem representação gráfica
.br
.wo "iscntrl  -"
verifica se é um caractere de controle
.br
.wo "isascii  -"
verifica se é um caractere ASCII
.br
.wo "isiso    -"
verifica se é um caractere ISO
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <ctype.h>

int	isalpha (int c);
int	isupper (int c);
int	islower (int c);
int	isdigit (int c);
int	isxdigit (int c);
int	isalnum (int c);
int	isspace (int c);
int	ispunct (int c);
int	isprint (int c);
int	isgraph (int c);
int	iscntrl (int c);
int	isascii (int c);
int	isiso (int c);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas macros classificam caracteres através de uma tabela de acesso direto.
Cada uma delas é um predicado, retornando um valor não nulo para verdadeiro,
e zero para falso.

.sp
As macros "isascii" e "isiso" são definidas para todos os valores inteiros.
As demais macros, no entanto são apenas definidas para os valores
para os quais "isiso" é verdadeira e mais o valor EOF (isto é, de -1 a 255).
Se o argumento dado para algumas destas macros não estiver no seu
domínio de definição, o resultado será imprevisível.

.sp
As macros são:

.in +3
.ip "isalpha" 12
Verifica se é uma letra. Inclui as letras maiúsculas (0x41-0x5A), letras
minúsculas (0x61-0x7A) e as letras maiúsculas/minúsculas acentuadas
(0xC0 a 0xFF).

.ip "isupper" 12
Verifica se é uma letra maiúscula. Inclui as letras sem acento (0x41-0x5A)
e as letras acentuadas (0xC0 a 0xDE).

.ip "islower" 12
Verifica se é uma letra minúscula. Inclui as letras sem acento (0x61-0x7A)
e as letras acentuadas (0xDF a 0xFF).

.ip "isdigit" 12
Verifica se é um dígito decimal (0-9).

.ip "isxdigit" 12
Verifica se é um dígito hexadecimal (0-9, A-F, a-f).

.ip "isalnum" 12
Verifica se é alfanumérico (letra ou dígito decimal).

.ip "isspace" 12
Verifica se é um espaço. Inclui <sp> (0x20): o espaço normal, <nbsp> (0xA0):
o espaço "no-break", <ht> (0x09): tabulação horizontal,
<lf> (0x0A): "new-line", <vt> (0x0B): tabulação vertical,
<ff> (0x0C): "form-feed" e <cr> (0x0D): "carriage-return".

.ip "ispunct" 12
Verifica se é um caractere de pontuação, ou seja, que não é de controle
nem alfanumérico. Inclui os caracteres  0x20-0x2F, 0x3A-0x4O, 0x5B-0x60,
0x7B-0x7E e 0x80-0xBF.

.ip "isprint" 12
Verifica se é um caractere possível de ser impresso. Inclui os caracteres
0x20-0x7E e 0x80-0xFF.

.ip "isgraph" 12
Verifica se é um caractere com representação gráfica.
Trata-se de uma variante de "isprint", que exclui <sp> e <nbsp>.
Inclui os caracteres 0x21-0x7E, 0x80-0x9F e 0xA1-0xFF.

.ip "iscntrl" 12
Verifica se é um caractere de controle.
Inclui os caracteres EOF, 0x00-0x1F e 0x7F.

.ip "isascii" 12
Verifica se é um caractere ASCII (um inteiro 0-127).

.ip "isiso  " 12
Verifica se é um caractere ISO (um inteiro 0-255).

.ep
.in -3

.sp
.in
.sp
.b OBSERVAÇÃO
.in 5
O conjunto de caracteres do TROPIX é o código ISO, de 8 bites.

.sp
Repare que "isprint" é exatamente o inverso de "iscntrl".

.sp
O caractere EOF é considerado de controle.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
stdio
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
