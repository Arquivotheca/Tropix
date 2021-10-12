.bp 1
.he 'INITENT (libc)'TROPIX: Manual de Referência'INITENT (libc)'
.fo 'Atualizado em 15.07.05'Versão 4.6.0'Pag. %'

.b NOME
.in 5
Leitura do arquivo de inicialização do sistema:
.sp
.nf
     getinitent  - obtém a entrada seguinte do arquivo
     setinitent  - prepara a releitura do arquivo
     endinitent  - conclui a leitura do arquivo
     getinitnam  - obtém a entrada da linha de nome dado
.fi
.in

.sp
.b SINTAXE
.(l
#include <sys/types.h>
#include <init.h>

INITAB	*getinitent (void);
void	setinitent (void);
void	endinitent (void);
INITAB	*getinitnam (const char *name);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "getinitent" fornece, a cada chamada,
uma entrada do arquivo de inicialização do sistema
("/etc/initab"), no formato da estrutura "INITAB". Com sucessivas
chamadas a esta função, pode-se ler sequencialmente todo o arquivo.

.sp
A função "setinitent" prepara o arquivo para que seja feita uma nova leitura.
Não é necessário chamá-la para o primeiro processamento.

.sp
A função "endinitent" encerra o processamento do arquivo.

.sp
A função "getinitnam" procura a entrada com o nome da linha dada
(sem "/dev/"); não é necessário chamar "setinitent".

.in
.sp
.(t
.b
VALOR DE RETORNO
.r
.in 5
A função "getinitent" retorna um ponteiro para a estrutura INITAB contendo
a entrada seguinte do arquivo. No caso de final do arquivo,
ela devolve um ponteiro NULO. 
.)t

.sp
A funções "getinitnam" devolve um ponteiro NULO, caso a entrada desejada
não seja encontrada.

.in
.sp
.b OBSERVAÇÃO
.in 5
A funções "getinitent" e "getinitnam" devolvem ponteiros para uma área estática, e o
seu conteúdo deve ser copiado caso ele seja necessário após
as funções serem chamadas novamente.

.sp
A função "getinitnam" simula entradas para os pseudo terminais, cujos nomes são
"ptyc??".

.in
.sp
.b
VEJA TAMBÉM
.r
.nf
     (cmd):  init
     (libc): getpty
     (fmt):  initab
.fi

.in
.sp
.b ARQUIVOS
.in 5
/etc/initab

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
