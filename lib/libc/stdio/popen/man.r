.bp 1
.he 'POPEN (libc)'TROPIX: Manual de Referência'POPEN (libc)'
.fo 'Atualizado em 10.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
funções para entrada/saída de/para processos:
.sp
.wo "popen  -"
inicia uma comunicação com um comando
.br
.wo "pclose -"
termina a comunicação com um comando
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

FILE	*popen	(const char *command, const char *mode);
int	pclose (FILE *fp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "popen" cria um "pipe" (sys) entre o processo que a chamou
e o processo resultante da execução de "command"
retornando um ponteiro para a estrutura FILE associada ao "pipe".

.sp
A variável "mode" define como o "pipe" é aberto
e deve ser "r" (para leitura) ou "w" (para escrita).
Um "pipe" de leitura (escrita) é uma forma de comunicação em que
a saída (entrada) padrão de "command" é desviada e pode ser lida (escrita)
pelo processo que o criou através de "fp".

.sp
Um "pipe" aberto por "popen" deve ser fechado por "pclose",
que espera o processo associado com o "pipe" terminar
e retorna o estado de término do comando.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função "popen", retorna um ponteiro NULO caso ocorra algum erro.
Neste caso, a variável externa  "errno" contém  a razão do erro.

.sp
A função "pclose" retorna -1 se "fp" não for associado
com um arquivo aberto por "popen".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
pipe
.br
.wo "(libc):"
system, stdio, perror
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
