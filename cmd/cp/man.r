.bp
.he 'CP (cmd)'TROPIX: Manual de Referência'CP (cmd)'
.fo 'Atualizado em 01.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "cp -"
copia arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
cp [-ifvmx] <arquivo1> <arquivo2>
cp [-ifvmx] <arquivo1> [<arquivo2> ...] <diretório>
cp [-ifvmx] [-N] <diretório>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Em sua primeira forma, o comando "cp" copia <arquivo1> para <arquivo2>.

.sp
Na segunda forma, um ou mais <arquivo>s são copiados para o
<diretório> especificado, com seus nomes originais.

.sp
A terceira forma é análoga à segunda, com exceção dos nomes
dos <arquivo>s, que são lidos da entrada padrão, um por linha.

.sp
As opções do comando são:

.in +3

.ip -i
Modo interativo:
é pedida a confirmação do usuário para cada <arquivo> a copiar.

.ip -f
Se o <arquivo> destino já existir, "cp" normalmente pede permissão
ao usuário para removê-lo. Com esta opção, o <arquivo> é
removido silenciosamente.

.ip -v
Lista os nomes dos <arquivo>s que estão sendo copiados.

.ip -m
Copia o estado do <arquivo>:
normalmente, o dono e o grupo do <arquivo> destino são conservados
se êste já existe, e são usados os do usuário do comando em caso contrário. 
Com a opção "-m", são utilizados o dono, grupo, proteção e datas do <arquivo>
fonte no <arquivo> destino. Isto só é inteiramente possível se o
usuário do comando for superusuário. 

.ip -x
Copia os nós-índices ao invés dos conteúdos dos <arquivo>s.

.sp
Se o <arquivo> fonte for um elo simbólico, copia o próprio elo simbólico
ao invés do conteúdo do <arquivo>.

.ip -N
Lê os nomes dos <arquivo>s da entrada padrão 
(usado para a terceira forma).
Esta opção é implícita se  for dado apenas o nome do <diretório>.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
O comando "cp" não copia árvores. Para esta função deve-se utilizar
"cptree" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cat, pr, cp, mv, ln, cptree
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
