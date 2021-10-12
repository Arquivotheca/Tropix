.bp
.he 'INITAB (fmt)'TROPIX: Manual de Referência'INITAB (fmt)'
.fo 'Atualizado em 15.07.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "initab -"
tabela de inicialização do sistema
.br

.in
.sp
.b DESCRIÇÃO
.in 5
A tabela "initab" é utilizada pelo processo de inicialização
"init" (cmd), durante a carga do sistema, para obter informação
sobre quais terminais (linhas de comunicações) devem ser ativados.

.sp
A tabela reside no arquivo "/etc/initab", e pode ser editada
diretamente pelo administrador do sistema através de "ed" (cmd)
ou "vi" (cmd). A tabela consiste de uma linha para cada terminal;
cada linha é composta de 4 campos separados por ":":

.sp
.nf
			c:ttyn:tipo:local
.fi

.sp
O primeiro campo deve consistir de um único caractere, que
deve ser um asterisco ("*") para indicar um terminal ativo, e um branco
para inativo.

.sp
O segundo campo dá o nome do terminal: apenas o último componente
do caminho (isto é, "tty009" se o nome completo for "/dev/tty009").

.sp
O terceiro campo dá o tipo do terminal.
Este tipo pode ser da forma "term" ou "term,stty,...".
O primeiro item ("term") é inserido no ambiente (fmt) como valor
da variável "TERM". Os demais itens "stty,..." são similares
aos argumentos do comando "stty" (cmd): nesta versão do sistema
são aceitos os itens "iso", "ascii", "user1", "user2", "clocal",
"-clocal" e as velocidades (por exemplo "9.6").

.sp
O quarto campo dá o local onde se encontra o terminal - este campo
não é examinado pelo utilitário "init" - a sua utilização
se dá pelo utilitário "who" (cmd) para informar a localização do terminal.

.sp
O comando "init" reconhece também uma linha especial da forma

.sp
.nf
			c:PRINT_CHILD_STATUS
.fi
.sp
que indica se devem ou não serem impressas mensagens de erro sobre
processos filhos de "init" com terminação anormal.
As mensagens são impressas se o caractere "c" for "*".

.in
.sp
.b OBSERVAÇÃO
.in 5 
A tabela de inicialização do sistema NÃO contém linhas para os pseudo terminais.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ed, vi, who, init, stty
.br
.wo "(libc):"
getinitnam
.br
.wo "(fmt): "
environ
.br

.in
.sp
.(t
.b ARQUIVOS
.in 5
/etc/initab
.)t

.in
.sp
.(t
.b EXEMPLOS
.sp
.nf
	*:console:ebc4020,user1:Laboratório Digital (232)
	 :tty005:vt100,iso:Sala E 1035              (298)
	*:tty009:tda,ascii:Sala U 3569              (365)
.fi
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
