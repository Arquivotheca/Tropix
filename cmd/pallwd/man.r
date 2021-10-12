.bp
.he 'PALLWD (cmd)'TROPIX: Manual de Referência'PALLWD (cmd)'
.fo Átualizado em 10.08.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "pallwd -"
imprime todos os diretórios correntes
.br

.in
.sp
.b SINTAXE
.in 5
.(l
pallwd
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "pallwd" imprime informações sobre os diretórios correntes
de todos os processos.
Este comando é reservado para o superusuário.

.sp
Os diversos processos são agrupados conforme o seu terminal/video
virtual/pseudo-terminal associado.
Para cada terminal, "pallwd" imprime uma (ou mais) linhas contendo
o usuário em sessão no terminal, o nome do terminal, "<root>" se
o usuário tem privilégios do superusuário, "<sys>" se o usuário pertence
ao grupo "sys", "<n>" se tem algum processo com a prioridade aumentada
(veja "nice" (sys)) e os diretórios correntes dos diversos processos.

.sp
Finalmente, "pallwd" imprime mais dois grupos de linhas:
"<other tty>", relativo a processos associados a
terminais sem usuário em sessão e
"<no tty>" relativo a processos sem terminais associados.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ps
.br
.wo "(sys): "
nice
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/utmp

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
