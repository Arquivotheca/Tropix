.bp
.he 'LC (cmd)'TROPIX: Manual de Referência'LC (cmd)'
.fo 'Atualizado em 02.10.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.wo "lc -"
lista o conteúdo de diretórios
.br

.in
.sp
.b SINTAXE
.in 5
.(l
lc [-rdbclfstaRN] [<diretório> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "lc" lista o conteúdo dos diretórios dados.
Se não for dado nenhum <diretório> (nem a opção "-N"),
"lc" lista o conteúdo do diretório corrente.

.sp
A listagem de cada diretório é dividida em sessões,
uma para cada tipo de arquivo, e disposta em 5 colunas.

.sp
Normalmente, são listados todos os tipos de arquivos.
Se isto não for desejado, podemos selecionar os tipos desejados
através das seguintes cinco opções. Se uma ou mais destas opções
forem dadas, somente serão listados os tipos selecionados.

.in +3
.ip -r
Arquivos regulares.

.ip -d
Diretórios.

.ip -b
Arquivos especiais de blocos.

.ip -c
Arquivos especiais de caracteres.

.ip -l
Elos simbólicos.

.ip -f
Fifos.

.ep
.in -3

.sp
Temos ainda, as seguintes outras opções:

.in +3
.ip -t
Ordena os arquivos pela data da última modificação, ao invés de
alfabeticamente pelo nome do arquivo, como é usual.

.ip -a
Inclui também as entradas "." e ".." na listagem
(não pode ser usada em conjunto com a opção "-R"). 

.ip -R
Lista os conteúdos dos subdiretórios encontrados recursivamente.
Deste modo, podemos obter listar todos os diretórios de uma árvore
(não pode ser usada em conjunto com a opção "-a"). 

.ip -N
Lê os nomes dos <diretório>s da entrada padrão.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ls
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
