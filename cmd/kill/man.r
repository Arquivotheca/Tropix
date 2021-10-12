.bp
.he 'KILL (cmd)'TROPIX: Manual de Referência'KILL (cmd)'
.fo 'Atualizado em 08.05.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "kill -"
envia sinais a processos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
kill [-s <sinal>] <pid> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "kill" envia o sinal SIGTERM (15) (veja a opção "-s")
para os processos (ou grupo de processos)
dados através de suas identificações numéricas <pid>s.

.sp
Isto irá normalmente cancelar os processos, a não ser que eles
ignorem ou interceptem o sinal enviado.

.sp
A filosofia e uma lista completa dos sinais são dadas em "kill" (sys)
e "signal" (sys).

.sp
As opções do comando são:

.in +3
.ip -s
Com esta opção, iremos enviar o <sinal> no lugar de SIGTERM.
O <sinal> pode ser dado na forma numérica (de acordo com as convenções
da linguagem "C", por ex. "22", "0x16", "026") ou na forma simbólica
(por ex. "SIGABRT", "ABRT", "sigabrt" ou "abrt").

.ep
.in -3

.in
.sp
.b OBSERVAÇÕES
.in 5
Os <pid>s dos processos em "background" são dados diretamente pelo
"sh" (cmd); além disto, os <pid>s dos processos podem ser obtidos
através de "ps" (cmd).

.sp
Ao enviar um sinal para o <pid> igual a "0" , serão sinalizados todos
os processos do grupo de processos do usuário do "kill" (veja "intro" (sys)),
o que é útil para cancelar todos os processos em "background". 

.sp
Os processos sinalizados devem pertencer ao usuário do "kill", a não
ser que este seja um superusuário.

.sp
Como o sinal SIGKILL não pode ser ignorado nem interceptado,
ele garante o término de um processo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh, ps
.br
.wo "(sys): "
intro, signal, kill
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
