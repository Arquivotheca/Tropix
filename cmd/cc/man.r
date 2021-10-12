.bp
.he 'CC (cmd)'TROPIX: Manual de Referência'CC (cmd)'
.fo 'Atualizado em 28.08.01'Versão 4.1.0'Pag. %'

.b NOME
.in 5
.wo "cc -"
Compilador para linguagem "C"
.br

.in
.sp
.b SINTAXE
.in 5
cc [-casumvOSTPEC.] [-D <def>] ...
.in 14
[-U <id>] ... [-I <dir>] ... [-o <obj>] <arquivo> ...

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "cc" é a interface para o sistema de compilação da linguagem "C" do
TROPIX. O sistema é composto pelas seguintes ferramentas:
préprocessador ("cpp" (cmd)), compilador (do GNU, programa contribuído),
montador ("as" (cmd)) e "link-editor" (ld (cmd)).
O comando "cc" processa as opções dadas, e executa as diversas ferramentas
com os argumentos apropriados. 

.sp
Nesta versão de "cc", todos os módulos executáveis gerados são reentrantes.
Se for necessário gerar um módulo NÃO reentrante, é necessário usar "ld" diretamente.

.sp
O comando aceita diversos tipos de <arquivo>s como argumentos:

.in +3
.ip 1. 3
Arquivos cujos nomes terminam com ".c" são considerados programas fonte
em "C" e podem ser préprocessados, compilados, montados 
e "link-editados". O processo de compilação pode ser interrompido
após qualquer uma destas fases, através de opções adequadas.
Se a montagem é executada, então é produzido um módulo objeto em
um arquivo cujo nome é o do programa fonte com ".c" substituído por ".o".
No entanto, o arquivo "*.o" é normalmente apagado se um único programa
fonte em "C" é compilado e "link-editado". 

.ip 2. 3
Arquivos cujos nomes terminam com ".s" são considerados programas fonte
em linguagem simbólica ("assembly"), e podem ser montados e "link-editados".
É produzido um módulo objeto em um arquivo cujo nome é o do programa fonte
com ".s" substituído por ".o". No entanto, o arquivo "*.o"
é normalmente apagado se um único programa fonte é montado e "link-editado". 

.ip 3. 3
Arquivos cujos nomes não terminam com ".c" nem ".s" são considerados
nomes de módulos objeto ou bibliotecas, e são passados para o "link-editor".

.ep
.in -3

.sp
Se for gerado um módulo executável, o seu nome será, por omissão, "a.out" (veja a opção "-o").

.sp
Dependendo da variável "CC_SHLIB" do ambiente (ver "enrivon" (fmt)), a
"link-edição" é realizada com bibliotecas convencionais ou compartilhadas.
Se o ambiente contiver a variável "CC_SHLIB" com o valor "0", a "link-edição"
será realizada com as biliotecas convencionais "/lib/libc" e "/lib/libm".
Por outro lado, se o ambiente NÃO contiver a variável "CC_SHLIB",
ou a contiver com o valor "1", a "link-edição" será realizada com a
bilioteca compartilhada "/lib/libt.o". Se a variável "CC_SHLIB" contiver
outro valor, este será usado como nome para a biblioteca ser usada.
De qualquer modo, bibliotecas adicionais podem ser dadas no final da lista
de <arquivo>s.

.sp 2
As opções do comando são:

.in +3
.ip -c 9
Suprime a fase de "link-edição", e não remove os módulos objeto produzidos.

.bc	/*************************************/
.ip -n 9
Se for produzido um módulo executável, ele deve ser reentrante.
.ec	/*************************************/

.ip -a 9
Gera os módulos objeto e o módulo executável com as tabelas de símbolos
completas (normalmente elas contêm apenas os símbolos globais).

.ip -s 9
Se for produzido um módulo executável, ele não deve conter a tabela de símbolos.

.ip -u 9
Durante a fase de "link-edição", lista os símbolos não referenciados.

.ip -m 9
Durante a fase de "link-edição", produz um mapa dos módulos carregados.

.bc	/*************************************/
.ip -f 9
Durante a fase de "link-edição",
utiliza a biblioteca de ponto flutuante "falsa".
Isto é útil para programas que sabidamente não utilizam nenhuma operação
de ponto flutuante (seja de entrada/saída ou aritmética), pois o módulo
executável fica da ordem de 5 Kb menor.
É de responsabilidade do usuário certificar-se de que o programa
realmente não utiliza operações de ponto flutuante
(se alguma função da biblioteca "falsa" for chamada, será impressa uma
mensagem de erro, e a execução é cancelada).

.ip -l 9
Durante a fase de compilação, lista o programa fonte (saída do préprocessador).

.ip -L 9
Durante a fase de montagem, gera o arquivo de listagem do montador com o nome
"*.l". Esta opção faz também com que o compilador mantenha
trechos do programa fonte em "C" na saída simbólica. 

.ip -w 9
Durante a fase de compilação, gera as informações adicionais necessárias 
para utilizar o depurador simbólico "wsdb" (cmd).
Se for utilizada esta opção, não pode ser utilizada a fase de otimização
(opção "-O").
.ec	/*************************************/

.ip -v 9
Lista os nomes das fases sendo executadas, além de algumas informações
adicionais fornecidas por algumas das fases.

.ip -O 9
Executa a fase de otimização do compilador "C". Esta opção não tem efeito
para programas fonte em linguagem simbólica.
.bc	/*************************************/
Se for utilizada esta opção, não pode ser utilizada a depuração simbólica
(opção "-w").
.ec	/*************************************/

.ip -S 9
Compila os programas fonte em "C" dados, gerando os
programas correspondentes em linguagem simbólica (com o sufixo ".s").
Não é realizada nenhuma montagem nem "linkedição".

.ip -T 9
Executa apenas o préprocessador e a fase de análise sintática
do compilador nos programas fonte em "C" dados.
São executadas heurísticas especiais do compilador, com a finalidade
de apontar construções suspeitas dos programas em "C",
que normalmente não são indicadas.
Este modo é semelhante ao utilitário "lint" de alguns sistemas UNIX.
Não é gerado nenhum arquivo.

.ip -P 9
Executa apenas o préprocessador nos programas fonte em "C" dados,
gerando os programas correspondentes (com o sufixo ".i").

.ip -E 9
Executa apenas o préprocessador nos programas fonte em "C" dados,
pondo o resultado na saída padrão.

.ip -C 9
Não remove os comentários durante o préprocessamento; isto é
útil para as opções "-P" e "-E".

.ip -. 9
Aloca os arquivos temporários no diretório corrente
ao invés do diretório "/tmp".

.bc	/*************************************/
.ip "-p <cpu>" 9
Gera código objeto para a <cpu> dada.
Se esta opção não for dada, esta informação é procurada no ambiente (fmt).
Se também não for dada no ambiente, será gerado o código objeto
nativo do compilador.
Nesta versão, o código nativo é o do MC68010, e podem ser dados
"m10" (MC68010), "m20" (MC68020) ou "m20,m81" (MC68020 e MC68881).
.ec	/*************************************/

.ip '-D <def>' 9
Prédefine macros para a fase de préprocessamento:
<def> é uma definição de macro do tipo "identificador" ou
"identificador=valor".
No primeiro caso, a macro é definida com valor "1".
Veja maiores detalhes em "cpp" (cmd).

.ip '-U <id>' 9
Não permite macros com o nome <id> durante a fase de préprocessamento.
Mesmo que hajam definições da macro <id>
no decorrer do texto, elas são ignoradas.
Veja maiores detalhes em "cpp" (cmd).

.ip '-I <dir>' 9
Acrescenta <dir> à lista de diretórios de busca de arquivos "#include"
durante a fase de préprocessamento.
Veja maiores detalhes em "cpp" (cmd).

.ip "-o <obj>" 9
Se <obj> for um diretório já existente, os módulos objeto gerados
serão nele colocados com o sufixo ".o", e/ou o módulo executável gerado
será nele colocado com o nome "a.out".
Em caso contrário, <obj> será usado como o nome do módulo objeto
(executável ou não) gerado.
Se esta opção não for dada, o nome do módulo executável gerado
será "a.out".

.ep
.in -3

.in
.sp
.b DIAGNÓSTICOS
.in 5
Tanto o préprocessador como o compilador emitem diagnósticos,
que (por hipótese) são auto-explicativos.
O "link-editor" pode emitir diagnósticos sobre módulos objeto não encontrados.

.in
.sp
.b OBSERVAÇÕES
.in 5
Como "cc" usualmente cria arquivos no diretório corrente, é necessário
executar o comando em um diretório no qual podem ser criados arquivos.

.bc	/*************************************/
.sp
Com a utilização da opção "-f", podem ocorrer colisões entre símbolos
externos dos módulos objeto do usuário e a biblioteca "falsa".
Neste caso é necessário mudar os nomes dos símbolos do usuário
(ou não usar a opção "-f").
.ec	/*************************************/
 
.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cpp, as, ld
.bc	/*************************************/
cpp, as, ld, wsdb
.ec	/*************************************/
.br
.wo "(fmt): "
ambiente
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
Kernighan, B. W., e Ritchie, D. M., "The C programming Language",
Prentice-Hall, 1978. 

.sp
"American National Standard: Programming Language C" (ANSI),
Technical Commitee X3J11.

.in
.sp
.(t
.b ARQUIVOS
.in 5
.nf
     a.out		Saída da "link-edição" (nome por omissão)
     plcc*		Arquivos temporários
     /tmp/plcc*
     /lib/cpp		Préprocessador
     /lib/gcc.486	Compilador "C"
     /bin/as		Montador
     /bin/ld		"Link-editor"
     /lib/crt0.o	"Run time startoff"
     /lib/crt1.o	"Run time startoff"
     /lib/libc		Bibliotecas padrão
     /lib/libt.o
     /usr/include	Diretório padrão para arquivos de "#include"
.fi
.)t

.in
.sp
.b EXEMPLOS
.in 7
.sp
.nf
		cc -o obj alpha.c beta.c gamma.s
.fi
.in 5
.sp
compila "alpha.c" e "beta.c", monta "gamma.s" e gera um módulo executável.
Se "obj" for um diretório existente, o módulo gerado será
"obj/a.out" e em caso contrário "obj".

.sp
.nf
		cc -c -o redoma alpha.c beta.c gamma.c delta.c
.fi
.sp
compila "alpha.c", "beta.c", "gamma.c" e "delta.c", colocando
os módulos objetos ("alpha.o", ...) no diretório "redoma", que já deve existir.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
