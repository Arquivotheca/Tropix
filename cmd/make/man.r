.bp
.he 'MAKE (cmd)'TROPIX: Manual de Referência'MAKE (cmd)'
.fo 'Escrito em 13.12.99'Versão 3.2.3'Pag. %'

.b NOME
.in 5
make - gerencia a manutenção de grupos de programas
.sp

.in
.sp
.b SINTAXE
.in 5
make [-intvab] [-f <desc>] [-D <macro>=<valor>] <módulo>...

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "make" gerencia a manutenção de sistemas compostos por
vários módulos.
As relações de dependência entre os módulos são expressas em um
arquivo de descrição, que contém os comandos necessários para
reconstituir o sistema em virtude de alterações produzidas nos
seus módulos integrantes.
.sp
.in 5
O comando aceita como argumentos nomes de módulos.
Se não for dado nenhum nome de módulo, será criado/atualizado o primeiro
módulo do arquivo de descrição.
Um módulo é criado/atualizado se não existe ou então depende
de outros módulos
que tenham sido modificados desde a última modificação do módulo em questão.
.sp
.in 5
Se o arquivo de descrição não for especificado (opção "-f"),
procuram-se os arquivos "Makefile" e "makefile"
(nessa ordem), no diretório corrente.

.in 5
.sp 2
As Opções são as seguintes:
.in 8

.ip -i 5
Ignore erros em todos os comandos.

.ip -n 5
Liste os comandos que são necessários para a atualização, mas não os execute.

.ip -t 5
Atualize a data de modificação dos módulos, sem executar os comandos.

.ip -v 5
Explica o motivo das atualizações.

.ip -a 5
Apenas lista os alvos definidos no arquivo de descrição, exceto os da
forma "*.o". Se for duplicada a opção ("-aa"), todos os alvos são
listados, sem exceção.

.ip -b 5
Provoca a atualização de todos os módulos, independentemente das datas.

.ip -f 5
Utilize <desc> como arquivo de descrição.

.ip -D 5
Defina a macro <macro> com o valor <valor>.
Esta definição se sobrepõe a outras definições da mesma macro
(por exemplo no arquivo de descrição e/ou no ambiente).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh, touch
.br
.in 5
.wo "(ref): "
make
.br

.in
.sp
.b ARQUIVOS
.in 5
 Makefile - primeira opção para o arquivo descrição.
 makefile - segunda opção para o arquivo descrição.

.(t
.in
.sp
.b EXEMPLO
.in 7
.sp 1
Exemplo de um arquivo de descrição:
.sp
.nf
	mod:	mod1.o mod2.o mod3.o
		cc -o mod mod1.o mod2.o mod3.o

	mod1.o:	mod1.h mod1.c
		cc -c mod1.c

	mod2.o: mod2.h mod2.c
		cc -c mod2.c

	mod3.o: mod3.h mod3.c
		cc -c mod3.c
.fi
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.
