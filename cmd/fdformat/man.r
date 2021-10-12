.bp
.he 'FDFORMAT (cmd)'TROPIX: Manual de Referência'FDFORMAT (cmd)'
.fo 'Atualizado em 13.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "fdformat -"
formata disquetes
.br

.in
.sp
.b SINTAXE
.in 5
.(l
fdformat [-fvt] [-s <tamanho>] [-e <entre>] [-01] [<dispositivo>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O utilitário "fdformat" formata o disquete <dispositivo>.
Este <dispositivo> deve ser dado na forma "não estruturada"
(de caracteres), "/dev/rfd0" ou "/dev/rfd1".

.sp
Na presente versão, "fdformat" processa os sequintes formatos:

.bc	/*************************************/
   |    3½"   |  1722   |     82    |     21     |  3444  |
.ec	/*************************************/

.sp
.(l
   +----------+---------+-----------+------------+--------+
   | Diametro | Tamanho | Cilindros |   Setores  | Blocos |
   |          |  (KB)   |           | por trilha |        |
   +----------+---------+-----------+------------+--------+
   |    3½"   |  1440   |     80    |     18     |  2880  |
   |    3½"   |   720   |     80    |      9     |  1440  |
   |          |         |           |            |        |
   |    5¼"   |  1200   |     80    |     15     |  2400  |
   |    5¼"   |   360   |     40    |      9     |   720  |
   +----------+---------+-----------+------------+--------+
.)l

.sp
Um parâmetro importante a ser considerado na formatação de disquetes
é o fator de entrelaçamento, que define a posição de cada setor
dentro da trilha.
Com o fator de entrelaçamento 1, isto é, com a numeração seqüencial
dos setores na forma

.sp
.nf
		1 2 3 4 5 6 7 8 9
.fi
.sp
possivelmente a leitura/escrita do disquete será lenta.
Isto ocorre porque entre a leitura/escrita de dois setores seguidos (5 e 6,
por exemplo) não há uma "pausa" necessária para a preparação da
leitura/escrita seguinte.
Isto significa que quando for iniciada a operação para acessar o setor 6,
a cabeça de leitura/escrita terá acabado de passar por ele, forçando
uma rotação extra do disquete.

.sp
Por outro lado, com o fator de entrelaçamento 2 teremos a disposição
da forma

.sp
.nf
		1 6 2 7 3 8 4 9 5
.fi
.sp
com a qual (por exemplo, após a leitura do setor 7),
temos o tempo durante a cabeça está em cima do setor 3
para preparar a leitura/escrita do setor 8.
Desta maneira evitamos a rotação supérflua e aumentamos a velocidade
do dispositivo. 

.sp
Os valores por omissão do fator de entrelaçamento
é 2 para o tamanho de 1200 KB, e 1 para os demais.
.bc	/*************************************/
são 2 para os tamanhos de 1722 e 1200 KB, e 1 para os demais.
.ec	/*************************************/

.sp
.(t
As opções do comando são:

.in +3
.ip -f
Normalmente, o comando pede uma confirmação do usuário antes de
efetivamente iniciar a formatação (que irá destruir o conteúdo
antigo do disquete).
Com a opção "-f", esta confirmação não é pedida.
.)t

.ip -v
Verboso: com esta opção, são dados as seguintes informações sobre
a formatação: número de setores por trilha, número de cilindros,
número total de setores e o entrelaçamento usado.

.ip -t
Apenas identificação: com esta opção, o comando ao invés de formatar o
disquete, apenas tenta descobrir se o disquete já está formatado,
e (se sim) com qual formatação.

.ip -s
Tamanho do conteúdo do disquete: o <tamanho> dado deve ser um dos
tamanhos em KB da tabela acima. Se esta opção não for dada, é
assumido o tamanho "1440".
.bc	/*************************************/
assumido o tamanho "1722".
.ec	/*************************************/

.sp
Podem também ser usados os seguintes
.bc	/*************************************/
sinônimos: "1.72" para "1722", "1.44" para "1440", "1.2" para "1200",
.ec	/*************************************/
sinônimos: "1.44" para "1440", "1.2" para "1200",
".72", "0.72" para 720 e ".36", "0.36" para 360.

.ip -e
Usa o fator de entrelaçamento <entre> ao invés do valor por omissão
(comentado acima).

.ip -0
Uma abreviação para o <dispositivo> "/dev/rfd0".

.ip -1
Uma abreviação para o <dispositivo> "/dev/rfd1".

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
gar, dosmp
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
