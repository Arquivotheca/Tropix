.bp
.he 'UNAME (cmd)'TROPIX: Manual de Referência'UNAME (cmd)'
.fo 'Atualizado em 13.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "uname -"
imprime a identificação do sistema
.br

.in
.sp
.b SINTAXE
.in 5
.(l
uname [-v]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "uname" imprime a identificação do computador,
do sistema operacional em uso, e do cliente.

.sp
Na primeira linha, são impressos o nome do sistema operacional, seu tipo,
sua versão/modificação com data e hora de geração, o tipo do
computador e o nome simbólico do nó nas redes nas quais está conectado.

.sp
Na segunda linha, são impressos o número de série do sistema,
o nome do cliente, e o nome do departamento. 

.sp
As opções do comando são:

.in +3
.ip -v
São impressas as mesmas informações mencionadas acima,
mas com o formato dos membros da estrutura "uts"
(ver o arquivo "/usr/include/sys/utsname.h")

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
uname
.br

.in
.sp
.b ARQUIVOS
.in 5
/usr/include/sys/utsname.h

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
