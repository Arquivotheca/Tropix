.bp
.he 'TOUCH (cmd)'TROPIX: Manual de Referência'TOUCH (cmd)'
.fo 'Atualizado em 08.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "touch -"
atualiza/modifica os tempos de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
touch [-vcN] [-d <tempo>] [-r <dias>] [-f <base>]
.nf
						    [<arquivo> ...]
.fi

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "touch" altera os tempos de acesso, modificação
(e opcionalmente o tempo de criação) dos <arquivos> dados
(veja "stat" (sys)).
Se não for dado nenhum tempo (veja abaixo),
é usado o tempo corrente.

.sp
Se algum <arquivo> ainda não existir, ele é criado como um arquivo regular
de tamanho NULO.

.sp
Como alternativa ao tempo corrente, o novo tempo pode ser dado de vários modos:
.bc	/*************************************/
Dependendo das opções, o novo tempo pode ser dado de vários modos:
.ec	/*************************************/

.in +3
.ip "-d <tempo>" 12
Tempo absoluto: com esta opção, o novo tempo deve ser dado no formato
do comando "data" (cmd).

.ip "-r <dias>" 12
Tempo relativo: o novo tempo deve ser dado pelo número de
<dias> no passado, relativos ao tempo corrente.

.ip "-f <base>" 12
Tempo do arquivo <base>: o novo tempo será o tempo de modificação
do arquivo <base>.

.bc	/*************************************/
.ip "-- " 12
Tempo corrente: se não for dada nenhuma das opções anteriores,
o novo tempo do arquivo será o momento corrente.
.ec	/*************************************/

.ep
.in -3
.sp
Além disto, temos ainda as seguintes opções:

.in +3
.ip "-v "
Lista os nomes dos <arquivo>s modificados, juntamente com o novo tempo.

.ip "-c "
Altera também o tempo de criação do arquivo.
Isto só pode ser feito pelo superusuário.

.ip "-N "
Lê os nomes dos <arquivo>s da entrada padrão.
Esta opção é implícita se não forem dados <arquivo>s.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
O uso indiscriminado de "touch" pode causar o comando "make"
a perder o controle dos módulos a atualizar.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
data, make
.br
.wo "(sys): "
stat, utime
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
