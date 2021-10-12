.bp 1
.he 'MODETOSTR (libc)'TROPIX: Manual de Referência'MODETOSTR (libc)'
.fo 'Atualizado em 23.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
modetostr - obtém a forma simbólica do modo de um arquivo

.in
.sp
.b SINTAXE
.(l
#include <stdlib.h>

char	*modetostr (long mode);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O argumento "mode" é o modo de um arquivo 
tal como obtido de uma chamada
ao sistema "stat" ou "fstat" (ver "stat" (sys)).
A função "modetostr" converte o modo de um arquivo 
de sua forma interna para uma forma simbólica.

.sp
A função devolve um ponteiro para uma cadeia de 23 caracteres
da forma
.nf

		"R 7654321mtsg rwxrwxrwx".

.fi

.bc	/*************************************/
onde cada caractere (exceto o primeiro) corresponde a um indicador do modo.
.sp
.ec	/*************************************/

O primeiro caractere indica o tipo de arquivo:
"B" para um dispositivo especial de blocos (estruturado),
"C" para um dispositivo especial de caracteres (não estruturado),
"D" para um diretório, "F" para um "fifo", "R" para um arquivo regular
e "?" para um arquivo de tipo inválido.

.sp
O segundo caractere é branco. Há então 11 
caracteres relativos aos atributos, mais um branco
e 9 caracteres com o modo de proteção do arquivo.
Se o indicador estiver desligado, o caracter assume o valor '-'
ao invés do caracter indicado acima.
Para informações sobre os diversos indicadores do modo,
veja "chmod" (sys).

.in
.sp
.b OBSERVAÇÃO
.in 5
A função "modetostr" devolve um ponteiro para uma área estática, e o
seu conteúdo deve ser copiado caso
ele seja necessário após a função ser chamada novamente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
chmod, ls
.br
.wo "(sys): "
stat, chmod
.br

.in
.sp
.b EXEMPLO
.r
.in 5
Uma chamada para "modetostr" poderá retornar:
.br
.nf

		"R -------m-s- rwxr-x--x"

.fi
o que significa que se trata de um arquivo regular,
com permissão para leitura, escrita e execução para o dono,
leitura e execução para os usuários do mesmo grupo
e de execução para os outros usuários, e com os atributos "set user ID"
e "salva a imagem do processo na memória, mesmo sem usuários".

.in
.sp
.b ESTADO
.in 5
Efetivo.
