.bp
.he 'INIT (cmd)'TROPIX: Manual de Referência'INIT (cmd)'
.fo 'Atualizado em 08.05.97'Versão 3.0.2'Pag. %'

.b NOME
.in 5
.wo "init -"
inicialização da operação do sistema
.br

.in
.sp
.b SINTAXE
.in 5
/etc/init

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "init" é o programa que assume o controle durante
a inicialização da operação do sistema, e cria todo o cenário
necessário para a operação normal do sistema. A sua atividade
pode ser dividida em 3 partes:

.ip MONOUSUÁRIO: 16
nesta primeira fase, o sistema é aberto apenas na console,
permitindo ao operador executar tarefas administrativas, tais
como verificar a integridade dos sistemas de arquivos, dar a
data ao sistema, etc... 

.ip "ARQUIVO rc:" 16
nesta fase, são executados os comandos constantes do arquivo
"/etc/rc", que se compõe de comandos de inicialização tais
como a montagem de sistemas de arquivos, remoção de arquivos
temporários, etc....

.ip MULTIUSUÁRIO: 16
nesta última fase, em que ele permanece até a próxima
carga do sistema, são criados
processos, um para cada terminal ativo, conforme indicados
no arquivo "/etc/initab". Cada um destes processos irá executar
um comando "login", possibilitando a um usuário entrar em sessão.
.ep
.sp

.in
.b OBSERVAÇÃO
.in 5
Este comando especial não pode ser utilizado. A sua execução
é iniciada automaticamente durante o processo de inicialização
da operação do sistema.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
login, sh
.br
.wo "(fmt): "
rc
.br

.in
.sp
.b ARQUIVOS
.in 5
 /dev/console
 /etc/mtab
 /etc/utmp
 /etc/initab
 /etc/globalenv
 /etc/rc

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
