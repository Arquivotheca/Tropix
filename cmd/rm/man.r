.bp
.he 'RM (cmd)'TROPIX: Manual de Referência'RM (cmd)'
.fo 'Atualizado em 02.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "rm -"
remove arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
rm [-fivN] [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Para cada nome de <arquivo> dado, o comando "rm" remove a entrada
do seu respectivo diretório (elo físico).
No entando, o arquivo só é efetivamente removido se este elo físico era o último.
.bc	/*************************************/
Se este elo a entrada referente a este nome era a última (ou a única) do arquivo,
então este arquivo é removido.
.ec	/*************************************/
Se não era o caso, o arquivo é conservado,
mas não pode mais ser acessado pelo nome removido.

.sp
Se o nome do <arquivo> for o de um elo simbólico, este elo simbólico é removido,
e não o arquivo referenciado.

.sp
A remoção de um arquivo requer permissão de
escrita em seu diretório, mas não são necessárias
permissões de leitura nem de escrita no próprio arquivo.

.sp
Para maior segurança, o comando "rm" é normalmente interativo,
isto é, ele pede a confirmação do usuário para cada remoção.
Além disto, o usuário é normalmente informado caso o arquivo não possua
permissão de escrita (veja a opção "-f", abaixo). 

.sp
As opções do comando são:

.in +3

.ip -i
Pede confirmação do usuário para a remoção de cada arquivo
(modo normal).

.ip -f
Remove o arquivo sem pedir confirmação do usuário,
mesmo que o arquivo não possua permissão de escrita. 

.sp
Um outro método de evitar as consultas é a de responder com letras
maíusculas: com a resposta "S" ou "Y" todas as perguntas seguintes
serão automaticamente respondidas afirmativamente, e (analogamente)
com a resposta "N" serão respondidadas negativamente
(veja "askyesno" (libc)).

.ip -v
Lista os nomes dos <arquivo>s sendo removidos.
Isto é útil em conjunto com as opções "-f" e "-N".

.ip -N
Lê os nomes dos arquivos da entrada padrão.
Esta opção é implícita se não forem dados <arquivo>s.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
O comando "rm" NÃO remove diretórios nem árvores. 
Para estas funções deve-se utilizar respectivamente "rmdir" e "rmtree" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
rmdir, rmtree
.br
.wo "(sys): "
unlink
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
