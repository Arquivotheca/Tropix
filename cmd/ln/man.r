.bp
.he 'LN (cmd)'TROPIX: Manual de Referência'LN (cmd)'
.fo 'Atualizado em 22.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "ln -"
cria elos (físicos ou simbólicos) para arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
ln [-sifv] <arquivo1> <arquivo2>
ln [-sifv] <arquivo1> [<arquivo2> ...] <diretório>
ln [-sifv] -N <diretório>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Um elo (físico ou simbólico) é uma referência a um arquivo,
que lhe dá um nome (ou seja, um caminho). 
Um arquivo (juntamente com seu tamanho, informações sobre proteção, ...)
pode ter várias referências (isto é, vários nomes).
Não há distinção entre o arquivo referido por intermédio de cada um
de seus nomes; qualquer mudança realizada no arquivo independe
do nome utilizado.

.sp
Um elo físico ("hard link") é uma entrada de um diretório contendo um nome.
Um arquivo pode ter vários elos físicos, todos tendo o mesmo estado,
isto é, não há o elo físico "original".
Todos os elos físicos de um arquivo devem estar no mesmo sistema de arquivos.

.sp
Um elo simbólico ("soft link") é um pequeno arquivo especial cujo conteúdo
é o nome de um outro arquivo.
Um elo simbólico pode referenciar um arquivo de um outro sistema de arquivos.

.sp
Em sua primeira forma, o comando "ln" cria a nova entrada de diretório 
<arquivo2> para <arquivo1>.

.sp
Na segunda forma, são criadas novas entradas para
<arquivo1>, <arquivo2> ..., no <diretório> especificado.

.sp
A terceira forma é análoga à segunda, com exceção dos nomes
dos <arquivo>s, que são lidos da entrada padrão, um por linha.

.sp
As opções do comando são:

.in +3
.ip -s
Cria um elo simbólico (ao invés de físico).

.ip -i
Modo interativo:
é pedida a confirmação do usuário para cada elo a criar. 

.ip -f
Se o arquivo destino já existir, "ln" normalmente pede permissão
ao usuário para removê-lo. Com esta opção, o arquivo é
removido silenciosamente.

.ip -v
Lista os nomes dos elos que estão sendo criados.

.ip -N
Lê os nomes dos <arquivo>s da entrada padrão 
(usado para a terceira forma).

.ep
.in -3

.in
.lp
.b
OBSERVAÇÕES
.r
.in 5
Não é permitido criar elos físicos para diretórios.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
rm, cp, mv
.br

.in
.sp 2
.b ESTADO
.in 5
Efetivo.
.in
