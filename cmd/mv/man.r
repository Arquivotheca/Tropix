.bp
.he 'MV (cmd)'TROPIX: Manual de Referência'MV (cmd)'
.fo 'Atualizado em 02.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "mv -"
move (troca o nome de) arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
mv [-ifv] <arquivo1> <arquivo2>
mv [-ifv] <arquivo1> [<arquivo2> ...] <diretório>
mv [-ifv] [-N] <diretório>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Em sua primeira forma, o comando "mv" move
(troca o nome de) <arquivo1> para <arquivo2>.

.sp
Na segunda forma, um ou mais <arquivo>s são movidos para o
<diretório> especificado, com seus nomes originais.

.sp
A terceira forma é análoga à segunda, com exceção dos nomes
dos <arquivo>s, que são lidos da entrada padrão, um por linha.

.sp
As opções do comando são:

.in +3

.ip -i
Modo interativo:
é pedida a confirmação do usuário para cada arquivo a mover.

.ip -f
Se o arquivo destino já existir, "mv" normalmente pede permissão
ao usuário para removê-lo. Com esta opção, o arquivo é
removido silenciosamente.

.ip -v
Lista os nomes dos arquivos que estão sendo movidos.

.ip -N
Lê os nomes dos <arquivo>s da entrada padrão 
(usado para a terceira forma).
Esta opção é implícita se  for dado apenas o nome do <diretório>.

.ep
.in -3

.in
.sp
.b OBSERVAÇÕES
.in 5
O comando "mv" não move diretórios. Para esta função deve-se utilizar
"mvtree" (cmd).

.sp
Se durante uma mudança, o comando perceber que o arquivo destino irá
residir em um outro sistema de arquivos, "mv" copia o conteúdo do arquivo,
removendo o original.
Ao realizar esta cópia, "mv" tenta preservar o estado do arquivo
(dono, grupo, modo, datas, etc ...), o que só é inteiramente
possível se o usuário for superusuário.

.sp
Além disto, no caso de cópia, as relações dos elos físicos dos arquivos originais
(apenas com arquivos também não movidos) são perdidas.

.sp
A mudança de elos simbólicos para outros diretórios pode produzir
referências inexistentes.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cp, ln, mvtree
.br

.in
.sp
.(t
.b ESTADO
.in 5
Efetivo.
.)t

.in
