.bp
.he 'CAT (cmd)'TROPIX: Manual de Referência'CAT (cmd)'
.fo 'Atualizado em 24.08.05'Versão 4.8.0'Pag. %'

.b NOME
.in 5
cat - concatena e copia arquivos

.in
.sp
.b SINTAXE
.in 5
cat [-nN] [-o <o>] [<arquivo> ...]

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "cat" lê um ou mais <arquivo>s sequencialmente e os escreve
na saída padrão. Assim

.sp
.nf
		cat arq
.fi

.sp
imprime o arquivo, e

.sp
.nf
		cat arq1 arq2 >arq3
.fi

.sp
concatena os arquivos "arq1" e "arq2", colocando o resultado em "arq3".

.sp
Um <arquivo> com nome "-" representa a entrada padrão.

.sp
As opções do comando são:

.in +5
.ip -n 5
Numera as linhas de saída sequencialmente a partir de 1.

.ip -o 5
Insere <o> brancos antes do início de cada linha.
Lembre-se no entanto, de que se <o> não for múltiplo de 8, as tabulações
podem se modificar.


.ip -N 5
Os nomes dos arquivos de entrada serão lidos da entrada padrão, um por linha.

.in
.lp
.b OBSERVAÇÃO
.in 5
Cuidado com o uso de comandos com a forma

.sp
.nf
		cat arq1 arq2 >arq1
.fi

.sp
porque o conteúdo original do arquivo de entrada "arq1" será perdido antes de
ser lido.
Portanto, atenção com os caracteres especiais de expansão do "sh" (cmd).

.in
.sp
.b "VEJA TAMBÉM"
.in 5
(cmd): cp, pr, sh

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
