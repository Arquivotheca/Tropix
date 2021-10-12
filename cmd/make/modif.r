.bp
.he 'MAKE (cmd)'TROPIX: Manual de Modificações'MAKE (cmd)'
.fo 'Atualizado em 09.01.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "make -"
Gerencia a manutenção de grupos de programas
.br

.in
.sp 2
.ip "25.07.88  2.0.0" 20
VERSÃO CORRENTE
.sp

.ip "09.09.88  2.1.0" 20
As variáveis definidas no "makefile" agora são exportadas
para o ambiente.
Alterado "src/parser.c".

.ip "29.09.88  2.1.1" 20
No caso do "make" executar um comando indiretamente através
do "sh", não estava sendo passado o ambiente correto.
Modificado "src/update". 

.ip "03.09.89  2.3.0" 20
Introduzida a opção "-D", alterado "src/main.c".

.sp
Introduzida a linha do tipo ".putenv". As variáveis NÃO são mais
automaticamente colocadas no ambiente.

.ip "20.03.96  3.0.0" 20
O programa foi reescrito por Oswaldo Vernet.

.ip "17.10.96  3.0.0" 20
Corrigida a reliberação de um bloco.

.ip "25.10.96  3.0.0" 20
Aumentado o tamanho da tabela hash de 511 para 1009. Modificado "h/sym.h"

.ip "30.10.96  3.0.0" 20
Corrigido o valor passado para o "exit" em "src/make.c".
Modificados: "src/make.c", "src/update.c" e "h/proto.h".

.ip "03.02.97  3.0.0" 20
Corrigido o processamento da opção "-n", que passa a funcionar também com "-t"
(modificados "src/make.c", "h/extern.h" e "src/update.c").
Corrigido o tratamento de comentários (modificado "src/scan.c").

.ip "03.03.97  3.0.1" 20
Suprimida a opção "-c", que tornou-se "default".
Introduzido o conceito de "alvos virtuais", declarados com a palavra
reservada ".virtual".
(modificados os módulos "h/sym.h", "h/extern.h", "src/make.c", "src/parse.c",
"src/scan.c", "tbl/mksym.c" e "src/update.c").

.ip "01.05.97  3.0.1" 20
Introduzida a opção "-a", cujo efeito é listar os alvos constantes do
arquivo de descrição.
(modificados os módulos "h/sym.h", "h/extern.h", "h/proto.h", "src/make.c"
e "src/parse.c").

.ip "25.01.99  3.1.0" 20
Quando um "putenv" é redefinido, agora verifica se por acaso a cadeia
é a mesma.

.ip "14.12.99  3.2.3" 20
Introduzida a opção "-b", cujo efeito é reconstruir todos os módulos,
independentemente das datas.

.sp
Introduzida a propagação das opções "-a", "-b", "-i", "-n", "-t" e "-v".
Quando o programa "make" é chamado dentro de um Makefile, essas opções,
se estiverem ativas, são propagadas.

.sp
A expansão de uma macro pode, agora, fazer parte do nome de outra.

.ip "28.08.01  4.1.0" 20
As variáveis definidas no ambiente prevalecem sobre as macros homônimas
no Makefile.
