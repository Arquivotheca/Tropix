.bp
.he 'HPPRINT (cmd)'TROPIX: Manual de Referência'HPPRINT (cmd)'
.fo 'Atualizado em 30.12.04'Versão 4.7.0'Pag. %'

.b NOME
.in 5
.wo "hpprint -"
ferramenta de impressão para as impressoras HP
.br

.in
.sp
.b SINTAXE
.in 5
.(l
hpprint [-qvcN] [-P <dev>] [-p <papel>] [-l <nl>]
                [-t <tab>] [-o <nc>] [-CM] [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O utilitário "hpprint" destina-se a facilitar a impressão de textos
nas impressoras HP (e outras marcas) que utilizam a linguagem de controle "PCL".

.sp
Por omissão, "hpprint" imprime os textos com qualidade de rascunho, 75 linhas
por página, 8 caracteres por tabulação e sem margem à esquerda.

.sp
As opções do comando são:

.in +3
.ip -q 11
Impressão de boa qualidade (ao invés de rascunho).

.ip -v 11
Verboso (imprime estatísticas na saída padrão).

.ip -c 11
Utiliza caracteres comprimidos.

.ip -N 11
Obtém os nomes dos arquivos a imprimir da entrada padrão.

.ip "-P <dev>" 11
Imprime no dispositivo <dev> ao invés de "/dev/lp".

.ip "-p <papel>" 11
O papel sendo usado é "A4" ou "CARTA" (por omissão "A4").

.ip "-l <nl>" 11
Utiliza <nl> linhas/página (ao invés de 75).

.ip "-l <tab>" 11
Utiliza <tab> como tamanho do tabulador ao invés de 8).

.ip "-o <nc>" 11
Utiliza um margem esquerda de <nc> colunas.

.ip "-C" 11
Impressão de programas em "C" (equivalente às opções "-c -l 75 -o 16").

.ip "-M" 11
Impressão de manuais (equivalente às opções "-c -l 75 -o 16").

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Lembre-se de que "hpprint" NÃO escreve cabeçalhos nem numera as
páginas; para isto use o comando "pr" (cmd).

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
.(t
.b EXEMPLO
.in 7
Pagina e imprime um programa em "C":
.)t

.sp
.(l
		pr alpha.c | hpprint -C
.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
