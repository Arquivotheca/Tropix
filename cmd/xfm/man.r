.bp
.he 'XFM (cmd)'TROPIX: Manual de Referência'XFM (cmd)'
.fo 'Atualizado em 23.09.05'Versão 4.8.0'Pag. %'

.b NOME
.in 5
.(l
xfm - gerenciador gráfico de arquivos
.)l

.in
.sp
.b SINTAXE
.in 5
.(l
xfm [-g <geo>] [-f <frente>] [-b <fundo>] [-d <video>]
						[-M <m>] [-F <f>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O gerenciador gráfico de arquivos "xfm" permite a visualização completa da árvore
do sistema de arquivos e nela efetuar algumas operações, tais como cópias, comparações, edições, ...

.in
.sp
.b 'DISPOSIÇÃO DAS JANELAS'
.in 5
O programa "xfm" contém (de cima para baixo) as seguintes janelas:

.in +3
.ip 1. 3
Janela de botões: opções (veja abaixo), alocação de memória dinâmica, atualização
da árvore, releitura da árvore e finalização do programa.

.ip 2. 3
Janela de mensagens: Nesta janela são escritas 3 tipos de mensagens:

.in +6
.ip a. 3
Mensagens informativas (com o fundo regular).

.ip b. 3
Mensagens de êrro (com o fundo vermelho).

.ip c. 3
Mensagens consultando o usuário (com o fundo vermelho).
O usuário deve então clicar em "SIM" ou "NÃO".

.ep
.in -6

.ip 3. 3
Janela da árvore (à esquerda): Nesta janela é desenhada a árvore que representa
o completo sistema de arquivos, porém apenas os diretórios (os outros arquivos,
isto é, os conteúdos dos diretórios são mostrados na janela do diretório (veja abaixo)).

.sp
Cada diretório é representado por dois ícones: um botão (azul se o diretório contiver
filhos, ou vermelho em caso contrário) e o ícone amarelo de uma "pasta", seguido
do nome do diretório.

.sp
Se o diretório corresponder a um ponto de montagem (de acordo
com o arquivo "/etc/fstab" (veja "fstab" (fmt))), o ícone contém um pequeno ponto
vermelho.

.sp
Diretórios "irmãos", isto é, filhos do mesmo diretório "pai" são dispostos verticalmente
com a mesma indentação. À medida em que se aprofunda na árvore, os diretórios vão
sendo indentados para a direita.

.sp
Quando "xfm" inicia, somente são mostrados os filhos de "/". Para desenhar os filhos de
um diretório, basta clicar no seu botão (desde que ele seja azul). Se o botão for clicado
mais uma vez, os filhos (e todos os seus descendentes) são removidos da janela.

.sp
Clicando no próprio ícone do diretório, o seu contéudo é mostrado na janela do
diretório (veja abaixo).

.ip 4. 3
Janela do diretório (à direita): Nesta janela é desenhado o conteúdo do diretório
que foi clicado na janela da árvore (veja acima).

.sp
Cada linha corresponde a uma entrada do diretório; a primeira linha é a do pai do
diretório mostrado.

.sp
Cada linha contém:

.in +6
.ip a. 3
Botão de seleção:
Se este botão estiver azul, o arquivo correspondente está selecionado para
operações de cópia, comparação, movimentação (troca de nome ou mudança de diretório)
e remoção. Em caso contrário (cor normal de fundo), o arquivo não está selecionado.

.sp
O botão esquerdo do "mouse" seleciona/deseleciona um arquivo. Para selecionar vários
arquivos, o botão deve ser arrastado pressionado.
Repare que é possível selecionar um conjunto de arquivos, porém não mais do que um diretório.

.ip b. 3
Ícone do tipo do arquivo:
O ícone tenta dar uma idéia do tipo do arquivo:
para diretórios, o ícone amarelo de uma "pasta" (como na janela da árvore);
para arquivos regulares, uma "roda dentada" se for um módulo executável ou
uma "folha de papel" em caso contrário;
para dispositivos especiais (de blocos ou caracteres) a imagem de "uma unidade
de disquete"; para elos simbólicos dois elos entrelaçados (um azul e outro vermelho).

.ip c. 3
Letra indicativa do tipo do arquivo:

.sp
.in +2
.wo 'R  '
Arquivo regular
.br
.wo 'B  '
Dispositivo especial de blocos
.br
.wo 'C  '
Dispositivo especial de caracteres
.br
.wo 'L  '
Elo simbólico
.br
.wo 'D  '
Diretório
.br
.wo 'F  '
"Fifo"
.br
.wo '?  '
Arquivo de tipo inválido
.br
.in -2

.ip d. 3
Proteção do arquivo: ele tem a forma

.nf

			"rwxrwxrwx"

.fi

onde são dados os 9  caracteres relativos à proteção
do arquivo. Se algum indicador estiver desligado, o caracter
assume o valor '-' ao invés do caracter indicado acima.

.sp
Se, na opção "Modo dos arquivos" (veja abaixo)
foi selecionado o "modo completo",
serão dados 4 indicadores adicionais, na forma:

.nf

			"mtsg rwxrwxrwx"

.fi

Para maiores detalhes, veja "ls" (cmd).


.ip e. 3
Número de elos físicos do arquivo.

.ip f. 3
Dono e grupo do arquivo.

.ip g. 3
Tamanho do arquivo (ou índices do dispositivo).

.ip h. 3
Data da última modificação do arquivo.

.ip i. 3
Nome do arquivo (com a cor de acordo com a variável MODECOLOR do ambiente,
veja "modecolor" (libc)).

.ep
.in -6

.ip 5. 3
Janela do nome do diretório: Esta janela fica acima da janela do diretório e contém
o nome do diretório cujo conteúdo está mostrado na janela abaixo.

.ip 6. 3
Janelas das barras de rolamento das janelas da árvore e do diretório: Contém barras
de rolamento "tradicionais", com a adição de um botão adicional (segundo de baixo
para cima), com a mesma função do botão superior.

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
Padrões: permite selecionar, de acordo com o nome, os arquivos a serem processados
pelas operações de cópias/comparações.

.sp
A alternativa "Apenas os fontes" restringe as operações aos arquivos relevantes
à compilação de programas na linguagem "C", isto é, àqueles cujos nomes
satisfazem os padrões "*[Mm]akefile" e "*.[cshryv]" (veja "patmatch" (libc)).

.bc	/*************************************/
.ip 1. 3
Padrões: permite incluir/excluir arquivos para as operações de cópias/comparações
de acordo com o nome do arquivo.

.sp
A alternativa "Apenas os fontes" restringe as operações aos arquivos relevantes
à compilação de programas na linguagem "C", isto é, àqueles cujos nomes
satisfazem os padrões "*[Mm]akefile" e "*.[cshryv]" (veja "patmatch" (libc)).
.ec	/*************************************/

.ip 2. 3
Arquivos ocultos: inclui (alternativa "Processa") ou NÃO (alternativa "Ignora")
arquivos cujos nomes começam por "." nas operações de cópias/comparações.


.bc	/*************************************/
.ip 2. 3
Arquivos ocultos: assim como acima, permite incluir/excluir arquivos para as
operações de cópias/comparações de acordo com o nome do arquivo.

.sp
A alternativa "Ignora" exclui do processamento os arquivos cujos nomes começam
por ".".
.ec	/*************************************/

.ip 3. 3
Largura das janelas: permite (alternativa "Estende") ou NÃO (alternativa "Não estende")
o aumento automático da largura das janelas da árvore e do diretório.

.ip 4. 3
Modo dos arquivos: resumido (alternativa "Resumido") ou completo (alternativa "Completo"); 
veja acima, em "Proteção do arquivo".

.ip 5. 3
Versão: A versão do programa "xfm".

.ep
.in -3


.in
.sp
.b MENUS
.in 5
Para as diversas operações de cópias, comparações, ... temos os seguintes menus:

.in +3
.ip 1. 3
Menu da árvore: este menu é ativado clicando o botão da direita no ícone do diretório desejado da
janela da árvore.

.sp
Neste menu são oferecidas as seguintes operações: "ziptool", "mount", "umount", "eject",
"df", "du", "rm", "cp", "cmp", "mv", "sh" e "mkdir".

.sp
Estas operações funcionam de modo análogo aos comandos homônimos do TROPIX, tendo como
argumento o nome o nome do diretório.

.sp
Em particular, os arquivos fontes das operações
"cp", "cmp" e "mv" são os integrantes do conjunto selecionado na janela do diretório
(veja abaixo).

.ip 2. 3
Menu de um arquivo: este menu é ativado clicando o botão da direita no ícone do arquivo desejado da
janela do diretório.

.sp
Neste menu são oferecidas as seguintes operações: "rm", "rename", "sh", "show",
"vi", "xedit", "xpaint" e "mkdir".

.sp
Estas operações funcionam de modo análogo aos comandos homônimos do TROPIX, tendo como
argumento o nome do arquivo/diretório.

.ip 3. 3
Menu de um conjunto de arquivos: este menu é ativado clicando o botão da direita em qualquer
um dos botões da janela do diretório.

.sp
Neste menu são oferecidas as seguintes operações: "rm", "rename", "sh", "show",
"vi", "xedit", "xpaint" e "mkdir".

.sp
Estas operações funcionam de modo análogo aos comandos homônimos do TROPIX, tendo como
argumento os nomes dos arquivos do conjunto.

.sp
O conjunto de arquivos é selecionado de um dos seguintes modos:

.in +3
.ip 1. 3
Arquivo individual: clica-se com o botão esquerdo diretamente no botão do arquivo (ou diretório)
a selecionar.

.ip 2. 3
Seqüência de arquivos: clica-se com o botão esquerdo no botão do primeiro arquivo da seqüência, e
mantendo-se o botão pressinado, arrasta-se o "mouse" até o último arquivo da seqüência, onde
soltamos o botão.

.sp
Repetindo esta operação, podemos selecionar várias seqüências de arquivos.
Se esta mesma operação for efetuada em arquivos previamente selecionados, o efeito será
excluí-las do conjunto de arquivos selecionados.

.ep

.sp
Repare que podemos selecionar um ou mais arquivos, mas apenas um diretório.

.sp
A seleção de um conjunto de arquivos é usada também para especificar os arquivos fontes
das operações de "cp", "cmp" e "mv" do menu da árvore.

.in -3

.ep
.in -3

.sp
Para algumas das operações é pedida uma confirmação da operação a ser efetuada.

.in
.sp
.b 'OPÇÕES DO COMANDO'
.in 5
.in +3
.ip -g
Fornece a geometria inicial (em pixeis), no formato <largura>x<altura>.

.ip -f
Use a cor de <frente> dada.

.ip -b
Use a cor de <fundo> dada.

.ip -d
Conecte-se ao <vídeo> dado.

.ip -M
Use a fonte <m> dada para as janelas dos botões.

.ip -F
Use a fonte <f> dada para as demais janelas.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ziptool, mount, umount, eject, df, du, rm, cp, cmp,
.in 13
mv, sh, mkdir, rename, show, vi, xedit, xpaint
.br
.in 5
.wo "(libc):"
modecolor, patmatch
.br
.wo "(fmt): "
fstab
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
