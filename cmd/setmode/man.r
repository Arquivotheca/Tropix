.bp
.he 'SETMODE (cmd)'TROPIX: Manual de Referência'SETMODE (cmd)'
.fo 'Atualizado em 16.04.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "setmode -"
atribui as proteções corretas aos arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
setmode [-ueaf]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
.bc	/*************************************/
O utilitário "setmode" atribui as proteções corretas aos diversos arquivos do sistema
operacional TROPIX.
.ec	/*************************************/
O utilitário "setmode" analisa o modo dos diversos arquivos do sistema operacional TROPIX.
Para cada um deles, verifica se o dono/grupo/proteção estão corretos.

.sp
Isto é feito para os principais diretórios, módulos executáveis, membros das bibliotecas e arquivos
de inclusão, mas NÃO para arquivos e/ou diretórios dos usuários.

.sp
Sem opções, "setmode" apenas indica as incorreções encontradas, e as respectivas
correções sugeridas.

.sp
As opções do comando são:

.in +3
.ip -u
Corrige as incorreções encontradas. Para cada uma delas é pedida a confirmação do
usuário (veja a opção "-f", abaixo).

.ip -e
Informa sobre arquivos não encontrados. Normalmente, "setmode" nada diz sobre estes
arquivos.

.ip -a
Analisa o sistema de arquivos da raiz alternativa "/aroot" ao invés de "/".

.ip -f
Normalmente, para cada atualização a efetuar, é pedida a confirmação do usuário.
Com a opção "-f" isto NÃO é feito.

.sp
Um outro método de evitar as consultas é a de responder com letras
maíusculas: com a resposta "S" ou "Y" todas as perguntas seguintes
referentes ao mesmo diretório
serão automaticamente respondidas afirmativamente, e (analogamente)
com a resposta "N" serão respondidadas negativamente
(veja "askyesno" (libc)).

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
chmod, chown, chgrp
.br
.wo "(libc):"
askyesno
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
