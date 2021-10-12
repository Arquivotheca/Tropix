.bp 1
.he 'GETMNTENT (libc)'TROPIX: Manual de Referência'GETMNTENT (libc)'
.fo 'Atualizado em 06.10.05'Versão 4.8.0'Pag. %'

.b NOME
.in 5
Leitura de uma linha do arquivo de sistemas de arquivos

.in
.sp
.b SINTAXE
.(l
#include <mntent.h>
#include <stdio.h>

int getmntent (FILE *fp, char *dev, char *dir,
					struct mntent *flags);

void defmntent (struct mntent *flags);

int getmntopt (const char *str, struct mntent *flags);

const char *editmntopt (char *str,
			const struct mntent *flags, int vflag);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas funções destinam-se a analisar linhas do arquivo de informações
dos sistemas de arquivos (normalmente "/etc/fstab").
Cada linha deste arquivo contém 3 campos (separados por espaços ou <ht>):
o nome de um dispositivo, o nome de um diretório, e uma lista de opções de montagem
(separadas por vírgulas), como no exemplo:

.sp
.(l
		/dev/zip   /zip   noauto,user,noexec
.)l

.sp
A cada chamada, a função "getmntent" lê uma linha do arquivo dado por "fp",
copia o nome do dispositivo na área apontada por "dev",
copia o nome do diretório na área apontada por "dir" e atribui os valores dos indicadores
associados às opções de montagem na estrutura apontada por "flags"
(indicadores NÃO presentes na lista dada recebem os valores "default", veja abaixo).
Com sucessivas chamadas a esta função, pode-se obter seqüencialmente tôdas as entradas do arquivo.

.sp
Antes de chamar esta função, o arquivo deve ser aberto através da função "fopen" (libc) ou
equivalentes, e o ponteiro FILE retornado usado como o valor para argumento "fp".

.sp
A função "defmntent" atribui os valores "default" (veja abaixo) aos indicadores
da estrutura apontada por "flags". Ela pode ser útil antes de uma chamada à função "getmntopt"
(veja abaixo).

.sp
A função "getmntopt" recebe valores iniciais dos indicadores associados às opções de montagem
através da estrutura apontada por "flags", analisa uma lista de opções de montagem dada em "str"
(alterando estes indicadores), e atribui os valores atualizados na (mesma) estrutura apontada por "flags".

.sp
A função "editmntopt" edita no vetor de caracteres apontado por "str" uma linha contendo
os indicadores da estrutura apontada por "flags" em sua forma simbólica, análoga à
linha fornecida para a função "getmntopt".
Se "vflag" for não NULO, edita a lista completa; em caso contrário inclui somente
os indicadores com valores diferentes dos "default".

.in
.sp
.(t
.b "OPCÕES DE MONTAGEM"
.in 5
As opções de montagem são dadas abaixo
(os indicadores associados estão definidos no arquivo <mntent.h>):

.in +3
.ip "ro" 9
Monta o sistema de arquivos apenas para leituras
(indicador SB_RONLY ligado).
.)t

.ip "rw" 9
Monta o sistema de arquivos para leituras/escritas
(indicador SB_RONLY desligado,
valor "default").

.ip "user" 9
Permite que usuários regulares montem este sistema de arquivos
(somente o mesmo usuário ou o superusuário poderá desmontá-lo).
Esta opção implica nas opções "noexec", "nosuid" e "nodev"
(a não ser elas sejam alteradas subseqüentemente).

.sp
Caso o sistema de arquivos não possua
identificação de usuários/grupos, todos os arquivos pertencerão ao usuário/grupo
do usuário que o montou
(indicador SB_USER ligado).

.ip "nouser" 9
Apenas o superusuário pode montar/desmontar este sistema de arquivos
(indicador SB_USER desligado, valor "default").

.ip "atime" 9
Atualiza o tempo de acesso aos arquivos
(indicador SB_ATIME ligado).

.ip "noatime" 9
NÃO atualiza o tempo de acesso aos arquivos
(indicador SB_ATIME desligado, valor "default").

.ip "dev" 9
Interpreta os arquivos especiais estruturados/não estruturados
(de bloco/caracteres)
(indicador SB_DEV ligado, valor "default").

.ip "nodev" 9
NÃO interpreta os arquivos especiais estruturados/não estruturados
(de bloco/caracteres)
(indicador SB_DEV desligado).

.ip "exec" 9
Permite a execução de programas deste sistema de arquivos
(indicador SB_EXEC ligado, valor "default").

.ip "noexec" 9
NÃO permite a execução de programas deste sistema de arquivos
(indicador SB_EXEC desligado).

.ip "suid" 9
Permite que os indicadores "set user ID" e "set group ID" dos arquivos façam efeito
(indicador SB_SUID ligado, valor "default").

.ip "nosuid" 9
NÃO permite que os indicadores "set user ID" e "set group ID" dos arquivos façam efeito
(indicador SB_SUID desligado).

.ip "eject" 9
Tenta ejetar o meio removível do dispositivo após a desmontagem
(indicador SB_EJECT ligado).

.ip "noeject" 9
NÃO tenta ejetar o meio removível do dispositivo após a desmontagem
(indicador SB_EJECT desligado, valor "default").

.ip "case" 9
Distingue letras maíusculas de minúsculas em comparações de nomes de arquivos do sistema de
arquivos FAT12, FAT16, FAT32 e ISO9660 (indicador SB_CASE ligado, valor "default").

.ip "nocase" 9
NÃO distingue letras maíusculas de minúsculas em comparações de nomes de arquivos do sistema de
arquivos FAT12, FAT16, FAT32 e ISO9660 (indicador SB_CASE desligado).

.ip "rock" 9
Para o caso de CDROMs, use a extensão "Rock Ridge", se presente
(indicador SB_ROCK ligado, valor "default").
A extensão "Rock Ridge" tem precedência sobre a "Joliet".

.ip "norock" 9
Para o caso de CDROMs, NÃO use a extensão "Rock Ridge", mesmo se presente
(indicador SB_ROCK desligado).

.ip "joliet" 9
Para o caso de CDROMs, use a extensão "Joliet", se presente
(indicador SB_JOLIET ligado, valor "default").

.ip "nojoliet" 9
Para o caso de CDROMs, NÃO use a extensão "Joliet", mesmo se presente
(indicador SB_JOLIET desligado).

.ip "lfn" 9
Usa os nomes longos ("long file names") durante a leitura e escrita
em arquivos do sistema de arquivos FAT12, FAT16 e FAT32 (indicador SB_LFN ligado, valor "default").

.ip "nolfn" 9
Usa os nomes estilo DOS ("8.3") durante a leitura e escrita
em arquivos do sistema de arquivos FAT12, FAT16 e FAT32 (indicador SB_LFN desligado).

.bc	/*************************************/
.ip "uid=<user>" 9
.ip "gid=<group>" 9
.ec	/*************************************/

.ip "auto" 9
Sinaliza o programa "mount" (cmd) para montar este dispositivo, quando executado
com a opção "-a" (indicador SB_AUTO ligado, valor "default").

.ip "noauto" 9
Sinaliza o programa "mount" (cmd) para NÃO montar este dispositivo, quando executado
com a opção "-a" (indicador SB_AUTO desligado).

.ip "fsck" 9
Indicador para o programa "fsck" (cmd) para verificar o sistema de arquivos
(na realidade não é exatamente uma opção de montagem)
(indicador SB_FSCK ligado).

.ip "nofsck" 9
Indicador para o programa "fsck" (cmd) para NÃO verificar o sistema de arquivos
(indicador SB_FSCK desligado, valor "default").

.ip "default" 9
Equivalente a "rw,nouser,noatime,dev,exec,suid, noeject,case,auto,nofsck,rock,joliet,lfn".
Estas são as opções válidas se não for dada nenhuma opção explicitamente.

.ep
.in -3

.in
.sp
.(t
.b "OPCÕES DE MONTAGEM NFS"
.in 5
Para os sistemas de arquivos NFS temos as seguintes opções de montagem adicionais:

.in +3
.ip "soft" 12
Se o servidor NFS não responder, retransmite 8 vezes (veja "retrans", abaixo), após o qual desiste,
reportando um erro ao programa cliente (indicador SB_HARD desligado, valor "default").
.)t

.ip "hard" 12
Se o servidor NFS não responder, continua retransmitindo indefinidamente
(indicador SB_HARD ligado).

.ip "rsize=<n>" 12
O número de bytes a usar para os pedidos de leitura ao servidor NFS. O valor "default" é 1024.

.ip "wsize=<n>" 12
O número de bytes a usar para os pedidos de escrita ao servidor NFS. O valor "default" é 1024.

.ip "timeout=<n>" 12
O número de segundos a esperar entre as retransmissões ao servidor NFS. O valor "default" é 1 segundo. 

.ip "retrans=<n>" 12
O número máximo de retransmissões ao servidor NFS. O valor "default" é 8 vezes. 

.ip "port=<n>" 12
Para contactar o servidor NFS, use a porta <n> dada, ao invés da porta fornecida pelo "port mapper".
Ainda não implementado, na presente versão sempre é consultado o "port mapper".

.ip "nfsvers=<n>" 12
A versão do protocolo NFS a usar. No momento, somente a versão 2 está disponível.

.ep
.in -3


.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função "getmntent" retorna um código de retorno indicando o sucesso (ou não) da operação.
Em caso de sucesso, retorna um valor positivo; em caso de campo(s) faltando e/ou com erros,
retorna 0, após emitir um diagnóstico em "stderr"; caso o arquivo tenha chegado ao final,
retorna EOF.

.sp
A função "getmntopt" retorna um código de retorno indicando o sucesso (ou não) da operação.
Em caso de sucesso, retorna um valor positivo; em caso de opções inválidas,
retorna 0, após emitir um diagnóstico em "stderr".

.in
.sp
.b OBSERVAÇÃO
.in 5
Repare que a função "getmntent" supoõe que os vetores de caracteres nos
quais irá armazenar "dev" e "dir" serão suficientes paras as cadeias de caracteres.

.sp
Repare que a função "editmntopt" supoõe que os vetor "str" de caracteres
será de tamanho suficientes para a linha editada.

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc): "
fopen
.br
.wo "(cmd):  "
mount, umount, fsck
.br
.)t

.in
.sp
.b ARQUIVOS
.in 5
.(l
/etc/fstab
.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
