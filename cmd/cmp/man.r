.bp
.he 'CMP (cmd)'TROPIX: Manual de Referência'CMP (cmd)'
.fo 'Atualizado em 27.08.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "cmp -"
compara pares de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
cmp [-lpqsx] <arquivo1> <arquivo2>
cmp [-lpqsx] <arquivo1> [<arquivo2> ...] <diretório>
cmp [-lpqsx] [-N] <diretório>
.)l

.lp
.in
.b DESCRIÇÃO
.in 5
Em sua primeira forma, o comando "cmp" compara <arquivo1> com
<arquivo2>. Se <arquivo1> ou <arquivo2> for "-", o outro arquivo
será comparado com a entrada padrão.

.sp
Na segunda forma, compara um ou mais <arquivo>s com
os respectivos <arquivo>s homônimos do <diretório> especificado.

.sp
A terceira forma é análoga à segunda, com exceção dos nomes
dos <arquivo>s, que são lidos da entrada padrão, um por linha.

.sp
O comando "cmp" nada comenta sobre pares de <arquivo>s iguais.
Para <arquivo>s diferentes (se não for dada nenhuma opção),
ele indica o número da linha e da coluna do primeiro caractere diferente.

.sp
O comando "cmp" indica se um dos <arquivo>s é vazio, ou um
prefixo do outro.
 
.sp
O programa "cmp" obedece às convenções da função "modecolor",
consultando a variável MODECOLOR do ambiente para obter
as cores dos nomes dos arquivos e das mensagens
de erro (veja "modecolor" (libc)).

.sp
As opções do comando são:

.in +3
.ip -l
Ao encontrar um caractere diferente entre os <arquivo>s,
lista as linhas correspondente de cada <arquivo>.
Isto só é realizado se nenhum dos <arquivo>s for um módulo objeto.
Esta opção é ignorada se for dada a opção "-s".

.ip -p
Considera como iguais pares de <arquivo>s quando um deles for um
prefixo do outro (mas NÃO se um deles for vazio).

.ip -q
Comparação rápida: apenas compara os tamanhos e os primeiros 4 KB bytes
dos pares de <arquivo>s. Não pode ser usado juntamente com as opções "-l" new "-p".

.ip -s
Funcionamento silencioso:
"cmp" nada comenta nem lista, apenas retorna os códigos.

.ip -x
Compara os nós-índices ao invés dos conteúdos dos <arquivo>s.

.sp
No caso de elos simbólicos,
compara os conteúdos dos próprios elos ao invés dos <arquivo>s
por eles referenciados.
.bc	/*************************************/
Não dereferencia os elos simbólicos, isto é, 
.ec	/*************************************/

.sp
Quando esta opção é usada na segunda ou terceira forma, e o <diretório>
também for um elo simbólico, podem ser comparados <arquivo>s não desejados.

.ip -N
Lê os nomes dos <arquivo>s da entrada padrão 
(usado para a terceira forma).
Esta opção é implícita se  for dado apenas o nome do <diretório>.

.ep
.in -3

.in
.sp
.(t
.b
VALOR DE RETORNO
.r
.in 5
Em sua primeira forma, o comando "cmp" retorna 0 para arquivos iguais,
1 para  arquivos diferentes e 2 para argumentos faltando ou inacessíveis.
Nas outras formas, o valor de retorno é o somatório dos valores
indicados para a primeira forma, para cada par de arquivos comparados.
.)t

.in
.sp
.b OBSERVAÇÕES
.in 5
O comando "cmp" NÃO é indicado para comparar módulos objeto,
pois o cabeçalho destes módulos contêm informações 
sôbre a data de geração, versão/modificação, ..., que podem
ocasionar uma indicação (errônea) de que os conteúdos
dos módulos são diferentes.
Para comparação de módulos objeto deve-se utilizar "cmpobj" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc): "
modecolor
.br
.wo "(cmd):  "
cmpobj, cmptree, comm
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
