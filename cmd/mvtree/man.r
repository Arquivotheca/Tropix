.bp
.he 'MVTREE (cmd)'TROPIX: Manual de Referência'MVTREE (cmd)'
.fo 'Atualizado em 26.04.02'Versão 4.2.0'Pag. %'


.b NOME
.in 5
.wo "mvtree -"
move (troca o nome) de uma árvore/diretório
.br

.in
.sp
.b SINTAXE
.in 5
.(l
mvtree <árvore1> <árvore2>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "mvtree" move (troca o nome) de árvores (diretórios).

.sp
Se o diretório <árvore2> ainda não existe, o novo nome de <árvore1>
será <árvore2>. Em caso contrário, será <árvore2>/<nome1>, onde
<nome1> é o último componente do caminho de <árvore1>.

.sp
Se o nome antigo e o novo residirem em um mesmo sistema de arquivos,
será feita apenas uma mudança de nome. Em caso contrário, toda
a árvore será copiada e em seguida a árvore original será removida. 

.in
.sp
.b OBSERVAÇÕES
.in 5
Repare que ao mover uma árvore, modificam-se os caminhos
de todos os arquivos nela contidos.

.sp
O comando tenta ser o mais geral possível, processando
inclusive casos da forma
.sp
.ce
"mvtree  alpha/../../beta  gamma" 
e
.ce
"mvtree  alpha  ../..".

.sp
Os poucos casos não permitidos ocorrem quando o nome de <árvore1> termina
por "/.", "/.." ou quando  <árvore1> é um ancestral de <árvore2>.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mv, cptree, rmtree
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
