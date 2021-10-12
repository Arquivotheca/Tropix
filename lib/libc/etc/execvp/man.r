.bp
.he 'EXECVP (sys)'TROPIX: Manual de Referência'EXECVP (sys)'
.fo 'Atualizado em 03.01.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
Executa um arquivo, utilizando os diretórios do "PATH":
.sp
.wo "execlp  -"
dado um vetor de argumentos
.br
.wo "execvp  -"
dado um ponteiro para um vetor de argumentos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
int execlp (const char *arquivo, const char *arg0, ...);
int execvp (const char *arquivo, const char *argv[]);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As funções "execlp" e "execvp" são semelhantes respectivamente 
às chamadas ao sistema "execl" e "execv" (ver "exec" (sys)),
com a diferença de que o arquivo dado é procurado nos vários
diretórios dados pela variável do ambiente "PATH" (ver "environ" (fmt)).

.sp
Repare que a lista de argumentos "arg0", "arg1", ... da função
"execlp" deve ser terminada com um ponteiro NULO.

.sp
Se o nome do arquivo dado contiver um "/" (em qualquer posição),
este nome é considerado o nome definitivo, e a busca não é realizada.

.sp
Assim, se a variável "PATH" contiver o valor ".:/bin:/usr/bin",
e o nome do arquivo for "ld", será tentada a execução dos
arquivos 

.sp
.in +8
 "ld"
 "/bin/ld"
 "/usr/bin/ld"
.in -8

.sp
(nesta ordem). Se o nome do arquivo for "/bin/ld" ou "old/ld", somente
estes nomes são tentados.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Se a chamada ao sistema retornou, é porque ocorreu um erro;
o valor de retorno é -1 e é indicada em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Não pode haver retorno de um "execlp" ou "execvp" com sucesso, pois o
processo é sobreposto pelo conteúdo do novo arquivo executável.

.sp
Nestas duas funções o ambiente é passado para o novo programa
executável implicitamente, através da variável externa "environ".

.sp
Se o ambinte não contiver a variável "PATH", será utilizado 
o valor ".:/bin:/usr/bin".

.sp
Se um arquivo tem a permissão de acesso apropriada, mas não
se trata de um programa objeto executável, automaticamente
é invocado o "sh" para executar o (possível) arquivo
de comandos.

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exec
.br
.wo "(fmt): "
environ
.br
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.
