.bp
.he 'RC (fmt)'TROPIX: Manual de Referência'RC (fmt)'
.fo 'Atualizado em 29.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "rc -"
arquivo de comandos de inicialização
.br

.in
.sp
.b DESCRIÇÃO
.in 5
O arquivo "/etc/rc" contém uma série de comandos que irão
ser executados durante a carga do sistema operacional, entre a fase de
"monousuário" e "multiusuário" (ver "init" (cmd)). 

.sp
O arquivo contém simplesmente um comando por linha, e pode
ser editado diretamente pelo administrador do sistema através
de "ed" (cmd) ou "vi" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
init, ed, vi
.br

.in
.sp
.b ARQUIVOS
.in 5
 /etc/rc

.in
.sp
.b EXEMPLO
.sp
.nf
	/etc/mount -w /dev/sys /src/sys
	/etc/update
	rm -f /tmp/*
	rm -f /usr/tmp/*
	/usr/lib/lpd 
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
