.bp 1
.he 'CRYPT (libc)'TROPIX: Manual de Referência'CRYPT (libc)'
.fo 'Atualizado em 19.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
crypt - encripta uma senha

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

char	*crypt (const char *key, const char *salt);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "crypt" encripta a senha "key", influenciada pela
cadeia de 2 caracteres apontada por "salt".

.sp
Quaisquer caracteres podem ser usados na cadeia apontada por "key",
da qual apenas os primeiros 14 caracteres são considerados.
Os 2 caracteres da cadeia apontada por "salt"
devem pertencer ao conjunto [./0-9A-Za-z].

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função devolve um ponteiro para uma cadeia de 13 caracteres
(terminada por um NULO) contendo a senha encriptada,
na qual os dois primeiros caracteres
são os próprios caracteres apontados por "salt".

.sp
Os caracteres da senha encriptada pertencem ao mesmo conjunto
dado acima.

.in
.sp
.b OBSERVAÇÕES
.in 5
A função usa 7 transformações que encriptam sucessivamente um
vetor padronizado contendo apenas símbolos 0 e 1.
Estas transformações são "perturbadas" pelos caracteres da cadeia "salt".
A senha encriptada é uma permutação dos elementos do vetor.

.sp
A função "crypt" devolve um ponteiro para uma área estática, e o
seu conteúdo deve ser copiado caso ela seja chamada novamente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
login, passwd
.br
.wo "(libc):"
getpass
.br
.wo "(fmt): "
passwd
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
