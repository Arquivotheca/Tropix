.bp
.he 'PRSYNC (cmd)'TROPIX: Manual de Referência'PRSYNC (cmd)'
.fo 'Atualizado em 26.10.05'Versão 4.8.0'Pag. %'

.b NOME
.in 5
.wo "prsync -"
imprime o estado da sincronização dos processos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
prsync [<pid> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "prsync" imprime informações sobre o sincronismo dos
diversos processos. Se forem dados <pid>s de processos, somente
serão dadas informações sobre estes processos; em caso contrário
serão dadas informações sobre todos processos.

.sp
As informações dadas são as seguintes:

.in +3
.ip PID 10
Identificação do processo.

.ip TIPO 10
Tipo do semáforo pelo qual o processo está esperando:
SLEEP: recurso único; SEMA: recursos múltiplos; EVENT: ocorrência de um
evento; (em branco): o processo não está esperando por nenhum
semáforo (isto é, está sendo executado ou pronto para executar).

.ip ENDEREÇO  10
Endereço do semáforo (em hexadecimal).

.ip MAX 10
Número máximo de recursos já alocados simultaneamente pelo processo.

.ip TABELA 10
Nome da tabela e número da entrada contendo o semáforo.

.ip ENTRADA 10
Nome simbólico do semáforo. Se este nome não puder ser obtido,
é dado o deslocamento do semáforo dentro da entrada da tabela.

.ip CMD 10
Comando em execução referente ao processo, com os respectivos argumentos.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ps
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
