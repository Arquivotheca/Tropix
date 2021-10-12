.bp
.he 'UPDATE (cmd)'TROPIX: Manual de Referência'UPDATE (cmd)'
.fo 'Atualizado em 14.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "update -"
atualiza os sistemas de arquivos periodicamente
.br

.in
.sp
.b SINTAXE
.in 5
.(l
update [<n>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "update" atualiza periodicamente o conteúdo dos diversos
sistemas de arquivos em disco.
Isto é realizado através da escrita de todas as informações contidas no
sistema operacional que ainda estão mais atuais na memória do que no disco.
Entre estas informações estão os superblocos modificados, os nós-índices
modificados e os blocos de E/S ainda não escritos.

.sp
O argumento <n> indica a periodicidade (em segundos) das atualizações.
Se <n> não for dado, são feitas atualizações a cada 30 segundos.

.sp
O comando "update", ao ser chamado, inicia um processo que
não tem fim. Ele foi concebido para ser utilizado
apenas pelo administrador do sistema operacional.

.in
.sp
.b OBSERVAÇÃO
.in 5
O parâmetro <n> não deve ser muito grande, pois se ocorrer
uma falha no sistema ("crash") e a última atualização tiver
sido feita há muito tempo, é provável que o disco esteja
com muitos blocos desatualizados. Por outro lado, se <n> 
tiver um valor muito
baixo, o desempenho do sistema pode ser afetado.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sync
.br
.wo "(sys): "
sync
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
