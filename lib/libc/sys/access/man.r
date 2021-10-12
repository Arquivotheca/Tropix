.bp
.he 'ACCESS (sys)'TROPIX: Manual de Referência'ACCESS (sys)'
.fo 'Atualizado em 11.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "access -"
informa sobre a permissão de acesso a um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>
#include <unistd.h>

int	access (const char *path, int mode);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "access" verifica a permissão de acesso ao arquivo cujo
nome é dado pela cadeia de caracteres apontada por "path".
O argumento "mode" indica o tipo de acesso desejado ao arquivo.

.sp
Esta verificação é realizada
utilizando-se as identificações do usuário real (UID real) e
do grupo real (GID real), ao invés das identificações efetivas
(UID efetivo e GID efetivo).

.sp
O argumento "mode" contém um ou mais indicadores,
formado a partir de um outório de:
.sp 1
.nf
		F_OK	testa a existencia do arquivo
		R_OK	leitura
		W_OK	escrita
		X_OK	execução (ou busca em diretório)
.fi
.sp 1
A chamada ao sistema só anuncia permissão de acesso
se todos os indicadores dados forem satisfeitos.  

.sp
Para o dono do arquivo, as permissões são verificadas na parte do
"dono" do modo do arquivo; para membros do grupo do arquivo
(que não são o dono),  as permissões são verificadas na parte do
"grupo"; para os demais as permissões são verificadas na parte dos
"outros" (ver "intro" (sys)).

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A chamada ao sistema devolve zero se o acesso é permitido.
No caso contrário, devolve -1 e indica em "errno" a causa da
proibição de acesso.

.in
.sp
.b OBSERVAÇÕES
.in 5
Esta chamada ao sistema é especialmente útil para programas
que utilizam o indicador "set user/group ID", pois verificam o acesso
em relação ao UID/GID real, e não ao efetivo.

.sp
Mesmo que o usuário real for super-usuário,
existem três casos em ele não pode escrever em um arquivo:
quando o arquivo está em um sistema de arquivos montado
apenas para leitura; quando o arquivo é um programa reentrante
sendo executado no momento; quando o arquivo é um diretório.

.sp
Repare que apenas os indicadores são verificados; "access" pode
informar que podemos escrever em um certo diretório, mas a
tentativa de abri-lo para escrita irá falhar, porque não é
permitido escrever diretamente em diretórios;
um arquivo pode parecer executável para "access", mas a chamada ao
sistema "exec" falhar porque o arquivo não tem o formato próprio.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
intro, chmod, stat
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
