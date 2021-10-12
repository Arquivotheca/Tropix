.bp
.he 'XPOP3 (cmd)'TROPIX: Manual de Referência'XPOP3 (cmd)'
.fo 'Atualizado em 23.04.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.(l
xpop3 - acessa uma caixa postal remota através do protocolo POP3
.)l

.in
.sp
.b SINTAXE
.in 5
.(l
xpop3 [-d <d>] [-g <g>] [-f <frente>] [-b <fundo>] [-F <f>]
.)l

.bc	/*************************************/
xpop3 - acessa uma caixa postal remota através do protocolo POP3

Versão: 4.5.0, de 23.03.04

Sintaxe:
	xpop3 [-d <day>] [-g <geometria>] [-f <frente>] [-b <fundo>] [-F <f>]

Opções:	-d: Conecta-se ao servidor X <display>
	-g: Define a <geometria>
	-f: Define a cor de <frente>
	-b: Define a cor de <fundo>
	-F: Use a fonte <f>
.ec	/*************************************/

.in
.sp
.b DESCRIÇÃO
.in 5
O programa gráfico "xpop3" permite o acesso a uma caixa postal remota, através
do protocolo POP3 (Post Office Protocol, Version 3).  

.in
.sp
.b 'DISPOSIÇÃO DAS JANELAS'
.in 5
O programa "xpop3" contém (de cima para baixo) as seguintes janelas:

.in +3
.ip 1. 3
Janela de botões: opções (veja abaixo), conecta, desconecta, recebe, remove
e finalização do programa.

.ip 2. 3
Janela de mensagens: Nesta janela são escritas mensagens referentes ao
andamento da comunicação com o servidor, e pedidas senhas.

.ip 3. 3
Janela da conexão: contém dois componentes:

.in +3
.ip a. 3
Estado da conexão:
Cor de fundo: desconectado; verde: conectado, em fase de autenticação;
azul: conectado e autenticado.

.ip b. 3
Barra de progresso das transferências.

.ep
.in -3

.ip 4. 3
Janela da mensagens: contém 4 colunas:

.in +3
.ip a. 3
Botões de recebimento: preto: não recebe; verde: recebe; azul: recebida.

.ip b. 3
Botões de remoção: preto: não remove; laranja: remove; vermelho: removido.

.ip c. 3
Dados da mensagem: Índice, Endereço eletrônico remoto, data, tamanho e assunto.

.ip d. 3
Barra de rolagem vertical.

.ep
.in -3

.ep
.in -3

.in
.sp
.b 'BOTÃO DE OPÇÕES'
.in 5
Este botão, o primeiro à esquerda na janela de botões, fornece o seguinte "menu"
de opções:

.in +3
.ip 1. 3
Recebe todas as cartas: seleciona todas as cartas para recebimento.

.ip 2. 3
NÃO recebe nenhuma carta: desfaz as seleções para o recebimento das cartas.

.ip 3. 3
Remove todas as cartas: seleciona todas as cartas para remoção.

.ip 4. 3
NÃO remove nenhuma carta: desfaz as seleções para a remoção das cartas.

.ip 5. 3
Cancela a remoção das cartas: as cartas só são efetivamente removidas quando
o programa "xpop3" termina; através desta opção indicamos ao servidor para
desistir da remoção destas cartas.

.ip 6. 3
Limita o tamanho das cartas: Normalmente, não há limite no tamanho das cartas
a receber/remover. Com esta opção é possível definir um limite para o tamanho.

.ip 7. 3
Caixa postal local: Indica o nome do usuário local, que será usado para definir
o nome da caixa postal local.
Repare que alterando o usuário local, será alterado automaticante o usuário
remoto (mas não reciprocamente).

.ip 8. 3
Caixa postal remota: Indica o nome da caixa postal remota, contendo o nome do
usuário remoto, e o nome do nó remoto aonde será contactado o servidor POP3.
O nome do nó remoto pode ser dado na variável XPOP3 do ambiente,
como (por exemplo) na forma:

.sp
.nf
		XPOP3=pop.ig.com.br; export XPOP3
.fi

.ip 9. 3
Versão: A versão do programa "xpop3".

.ep
.in -3


.in
.sp
.b 'MODO DE USO'
.in 5
Supondo que a variável do ambiente XPOP3 esteja convenientemente definida,
o uso "normal" se dá através das seguintes etapas:

.in +3
.ip 1. 3
Apertar o botão "Conecta".

.ip 2. 3
Dar a senha da caixa postal remota.

.ip 3. 3
Na janela de mensagens estarão as mensagens novas.

.ip 4. 3
Clicar nas mensagens a receber/remover.

.ip 5. 3
Clicar nos botões "Recebe" e "Remove".

.ip 6. 3
Clicar no botão "Fim".

.in -3
.ep

.sp
Para acessar outras caixas postais, devemos clicar no botão "Desconecta",
usar as opções do menu "Caixa postal local" e/ou "Caixa postal remota"
e conectar novamente.

.in
.sp
.b 'OPÇÕES DO COMANDO'
.in 5
.in +3
.ip -d
Conecta-se ao servidor X <d>

.ip -g
Fornece a geometria inicial (em pixeis), no formato <largura>x<altura>.

.ip -f
Use a cor de <frente> dada.

.ip -b
Use a cor de <fundo> dada.

.ip -F
Use a fonte <f> dada.

.bc	/*************************************/
Opções:	-d: Conecta-se ao servidor X <display>
	-g: Define a <geometria>
	-f: Define a cor de <frente>
	-b: Define a cor de <fundo>
	-F: Use a fonte <f>
.ec	/*************************************/

.ep
.in -3

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
