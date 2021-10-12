.bp
.he 'A2PS (cmd)'TROPIX: Manual de Referência'A2PS (cmd)'
.fo 'Atualizado em 03.10.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "a2ps -"
converte arquivos texto para PostScript
.br

.in
.sp
.b SINTAXE
.in 5
.(l
a2ps [-bN] [-c <nc>] [-F <ps>] [-l <nl>] [-o <mar>]
     [-t <tab>] [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "a2ps" converte o conteúdo dos arquivos dados para PostScript,
gerando o resultado na saída padrão.
Se não for dado nenhum <arquivo> (nem a opção "-N"), a entrada padrão
é assumida.

.sp
Temos ainda as seguintes outras opções:

.in +3
.ip -b
Divide as linhas grandes em várias, ao invés de truncá-las.

.ip -c
Utiliza <nc> caracteres como tamanho máximo de uma linha. Por omissão,
é assumido o valor 95.

.ip -F
Gera o resultado no arquivo <ps>, ao invés de utilizar a saída padrão.

.ip -l
Produz páginas de <nl> linhas. Por omissão, é assumido o valor 74.

.ip -o
A margem esquerda será de <mar> caracteres. Por omissão, é assumido o valor 0.

.ip -t
O tabulador tem espaçamento de <tab> caracteres. Por omissão, é assumido
o valor 8.

.ip -N
Lê os nomes dos <arquivo>s a processar da entrada padrão.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
pr
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
