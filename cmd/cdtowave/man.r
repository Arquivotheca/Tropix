.bp
.he 'CDTOWAVE (cmd)'TROPIX: Manual de Referência'CDTOWAVE (cmd)'
.fo 'Atualizado em 03.04.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "cdtowave -"
extrai faixas de CDs de audio
.br

.in
.sp
.b SINTAXE
.in 5
.(l
cdtowave [-vt] [-s <inicial>] [-f <final>] [-o <arq>] [<dev>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "cdtowave" permite extrair faixas de áudio em CDs,
gerando a saída em formato WAVE.

.sp
As opções do comando são:

.in +3
.ip -e
Ejeta o CD.

.ip -s
Começa a extração a partir da faixa <inicial>. O default é 1.

.ip -f
Encerra a extração na faixa <final>. O default é a faixa inicial.

.ip -o
Gera o arquivo <arq> com o resultado da extração. Se omitida, é 
assumida a saída padrão.

.ip -t
Mostra o conteúdo do CD (TOC).

.ip -v
Diz, a cada passo, quantos blocos faltam para completar a extração.

.ep
.in -3

.sp
Se invocado sem argumentos, o programa "cdtowave" extrai a
primeira faixa do CD.
.sp
Se <dev> for omitido, é assumido "/dev/cdrom".

.in
.sp
.b OBSERVAÇÃO
.in 5

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cdplay
.r
.in 5
.wo "(cmd): "
sbwave
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
