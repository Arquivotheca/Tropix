.bp
.he 'FSCKT1 (cmd)'TROPIX: Manual de Referência'FSCKT1 (cmd)'
.fo 'Atualizado em 12.09.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "fsckt1 -"
verifica a consistência de sistemas de arquivos T1
.br

.in
.sp
.b SINTAXE
.in 5
.(l
fsckt1 [-synpv] [<dev> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "fsckt1" verifica a consistência de sistemas de arquivos T1
contidos em um ou mais dispositivos ou arquivos regulares <dev>.
Em caso de encontrar erros, pede autorização do usuário para  
consertar o sistema de arquivos (ver as opções "-s", "-y" e "-n" abaixo).

.sp
Se <dev> for um diretório, "fsckt1" supõe que seja um ponto de
montagem e procura o dispositivo/arquivo regular correspondente na tabela
"fstab" (veja "fstab" (fmt)).

.sp
Se não for dado nenhum dispositivo <dev>, são processados os dispositivos
dados no arquivo "/etc/fstab" (veja "getmntent (libc)) que possuem a opção "fsck".

.sp
Se por acaso, algum dos <dev>s contiver um sistema de arquivos V7,
será automaticamente invocado o comando "fsckv7" (cmd).

.sp
Para poder consertar um sistema de arquivos, ele não pode estar montado.
A única exceção a esta regra é para o dispositivo contendo a raiz do sistema
("/"), que não pode ser desmontado. Neste caso, é importante que
não hajam processos escrevendo no dipositivo (o recomendado é
o modo "monousuário").

.sp
As inconsistências verificadas são as seguintes:
 
.in +3
.ip 1. 4
Blocos pertencentes a arquivos que
estejam fora dos limites físicos do sistema de
arquivos.
 
.ip 2. 4
Blocos pertencentes a mais de um arquivo.
 
.ip 3. 4
Número incorreto de referências a um arquivo.
 
.ip 4. 4
Formato inválido de diretório.
 
.ip 5. 4
Formato inválido de nó-índice.
 
.ip 6. 4
Arquivo com o número do nó-índice inválido.
 
.ip 7. 4
Mapas de bits inválidos.
 
.ip 8. 4
Total de blocos ou nós-índice livres incorreto
no superbloco.

.in -3
.ep

.sp
Arquivos não ligados a nenhum diretório podem ser reconectados
ao diretório padrão "lost+found" gerado automaticamente pelo
utilitário "mkfst1", caso este diretório exista e não esteja
repleto.

.sp
As opções do comando são:

.in +3
.ip -s
Com esta opção são automaticamente geradas respostas afirmativas
para todas as consultas feitas pelo programa.

.ip -y
Como acima.

.ip -n
Com esta opção são automaticamente geradas respostas negativas
para todas as consultas feitas pelo programa.

.ip -p
Modo de pânico: ignora os mapas de bits. Tenta reconstruir o
sistema de arquivos apenas pelos blocos de nós-índice, que
são reconhecidos por suas assinaturas.

.ip -v
Verboso.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
fsckv7, mkfst1
.br
.wo "(libc): "
getmntent
.br
.wo "(fmt):  "
fstab
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/fstab

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
