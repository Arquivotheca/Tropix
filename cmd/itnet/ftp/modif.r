.bp
.he 'FTP (cmd)'TROPIX: Manual de Modificações'FTP (cmd)'
.fo 'Atualizado em 11.08.01'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "ftp -"
programa de cópia remota de arquivos da INTERNET
.br

.in
.sp 2
.ip "01.10.92  2.9.9" 20
VERSÃO CORRENTE
.sp

.ip "20.11.92  3.0.0" 20
Agora usando as chamadas ao sistema "mkdir" e "rmdir".

.ip "24.06.93  3.0.1" 20
Estendido o "list" para aceitar o nome do arquivo local.

.ip "25.09.94  3.0.2" 20
Alterado "error" para escrever no arquivo "/etc/itnetlog".

.ip "17.02.97  3.0.3" 20
Retirado o <superuser> da conferência de senhas.

.ip "23.08.97  3.0.4" 20
Pequena revisão.

.ip "11.11.97  3.0.5" 20
Conserto na parte de proteção do servidor, e evitar
chamadas recursivas (fatais) na função "error".

.ip "18.11.98  3.1.0" 20
Colocado um "alarm" no servidor, e tirado o "timeout"
do "cntl_fd".

.ip "20.12.00  4.0.0" 20
Agora, no servidor, se houver um erro na escrita da conexão de dados TCP,
sai da malha.

.ip "03.03.01  4.0.0" 20
Adicionado o modo "passivo" tanto no cliente, como no servidor.

.ip "11.08.01  4.0.0" 20
Usando "readdir".
