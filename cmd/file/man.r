.bp
.he 'FILE (cmd)'TROPIX: Manual de Referência'FILE (cmd)'
.fo 'Atualizado em 24.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "file -"
tenta identificar o conteúdo de um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
file [-vN] [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "file" analisa cada um dos <arquivo>s dados, tentando
classificá-lo.

.sp
Se o <arquivo> for regular, examina os seus primeiros 1024 bytes
e tenta descobrir se se trata de um programa em alguma
linguagem conhecida ("C", pascal, fortran, basic, assembly),
ou um texto em um linguagem natural conhecida
(inglês, português, francês, alemão).

.sp
Se o <arquivo> NÃO for regular, dá o seu tipo (diretório, FIFO, especial
de blocos/caracteres).

.sp
As opções do comando são:

.in +3
.ip -v
Indica o elemento léxico que possibilitou a identificação.
Isto ajuda a avaliar se a classificação realizada pelo comando "file"
está correta.

.ip -N
Lê os nomes dos <arquivo>s de "stdin".
Esta opção é implícita se não forem dados argumentos.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
O comando "file" utiliza uma série de heurísticas para tentar classificar
o arquivo, que nem sempre alcançam o sucesso.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
