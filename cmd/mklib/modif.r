.bp
.he 'MKLIB (cmd)'TROPIX: Manual de Modificações'MODIF (cmd)'
.fo 'Atualizado em 01.10.01'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "mklib -"
cria/mantém bibliotecas de módulos objeto
.br

.in
.sp 2
.ip "01.04.87  1.0.0" 20
VERSÃO CORRENTE
.sp

.ip "23.07.87  1.0.1" 20
Foi corrigido o problema de proteção do arquivos "~LIBSYM~"
(agora é "rw-r--r--").

.in
.sp 2
.ip "01.01.88  1.1.0" 20
VERSÃO CORRENTE
.sp

.ip "18.12.90  2.3.0" 20
Alterado para trabalhar com símbolos externos de 31 caracteres.

.ip "24.12.93  3.0.0" 20
Foi feita uma revisão geral, incluindo logo a ordenação.
O ponteiro "lf_last" agora aponta para o primeiro símbolo do módulo seguinte.

.ip "25.04.95  3.0.1" 20
Adaptação para o PC.

.ip "30.05.97  3.0.2" 20
Uma pequena revisão.

.ip "20.11.97  3.0.3" 20
Corrigidos os problemas da não existência de "~LIBSYM~" e do diretório
intermediário faltando.

.ip "23.01.00  3.2.3" 20
Trocado ~LIBSYM~ por .LIBSYM.

.ip "11.08.01  4.0.0" 20
Usando "readdir".

.ip "21.08.01  4.0.0" 20
Novo formato da tabela de símbolos SYM.
.sp
Novo formato ISO de ".LIBSYM".

.ip "01.10.01  4.2.0" 20
Corrigido para módulos sem símbolos.
