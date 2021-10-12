.bp
.he 'XDEFRAG (cmd)'TROPIX: Manual de Referência'XDEFRAG (cmd)'
.fo 'Atualizado em 03.11.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.(l
xdefrag - melhora a alocação dos blocos de sistemas de arquivos
.)l

.in
.sp
.b SINTAXE
.in 5
.(l
xdefrag [-r] [-g <geo>] [-f <frente>] [-b <fundo>]
					[-d <video>] [-F <fonte>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando gráfico "xdefrag" tenta melhorar a alocação dos blocos
de sistemas de arquivos T1.

.sp
À medida em que um sistema de arquivos vai sendo usado
(isto é, arquivos vão sendo criados e removidos),
há uma tendência natural de os blocos dos arquivos novos não serem
alocados contiguamente no disco. Isto acarreta um tempo maior no
acesso a estes arquivos, pois o braço do disco terá de executar movimentos
maiores para ler/escrever cada um dos blocos do arquivo
(que estarão espalhados pelo disco).

.sp
O comando "xdefrag" tenta realocar os blocos
dos sistemas de arquivos para que os blocos de dados e endereços de um mesmo
arquivo fiquem contíguos e sequenciais (isto é, "compactados").
.bc	/*************************************/
Repare que esta versão de "xdefrag" NÃO realoca os blocos de nós-índice.
.ec	/*************************************/

.sp
Ele oferece tres métodos para a realocação dos blocos
(que podem ser selecionados no botão "opções", veja abaixo):

.in +3
.ip 1. 3
Apenas os arquivos: É feita uma análise arquivo a arquivo, e somente são
realocados os blocos dos arquivos que não estão compactados.
Este método é o mais simples e rápido, e normalmente é suficiente.
No entanto (possivelmente), irá deixar regiões com blocos não alocados
entre os arquivos (lacunas).
Além disto, não são realocados os blocos de nós-índice.

.ip 2. 3
Arquivos e lacunas: Este método é uma extensão do método anterior para
tentar eliminar (ou pelo menos reduzir) as lacunas.

Como acima, não são realocados os blocos de nós-índice.

.ip 3. 3
Completo: A árvore do sistema de arquivos é caminhada em profundidade,
realocando todos os diretórios, arquivos e nós-índices.
Este método é o mais demorado, mas tem a vantagem
de eliminar inteiramente as lacunas.
.sp
Além disto, são feitos esforços para que
os nós-índices fiquem próximos dos diretórios que os referenciam,
e os blocos dos arquivos próximos aos seus respectivos nós-índice.

.ep
.in -3

.in
.sp
.b 'DISPOSIÇÃO DAS JANELAS'
.in 5
O programa "xdefrag" contém (de cima para baixo) as seguintes janelas:

.in +3
.ip 1. 3
Janela de botões: opções (veja acima), analisa o sistema de arquivos, desfragmenta
o sistema de arquivos e encerra a execução do programa.

.ip 2. 3
Janela de dispositivos:
A primeira coluna contém um código de cores, que significa:

.in +5
.ip Vermelho: 10
Sistema de arquivos NÃO disponível (não pronto, não é um sistema de arquivos T1):
NÃO pode ser analisado nem compactado.

.bc	/*************************************/
Sistema de arquivos NÃO disponível (não pronto, não pode ser desmontado,
dispositivo permite apenas leituras,
não é um sistema de arquivos T1).
.ec	/*************************************/

.ip Amarelo: 10
Sistema de arquivos montado, dispositivo permite apenas leituras:
pode ser analisado mas NÃO compactado.

.ip Preto: 10
Sistema de arquivos disponível para ser analisado e compactado.

.ip Verde: 10
Sistema de arquivos escolhido para a análise e (possivelmente) a compactação.

.in -5
.ep

.in +3
.bc	/*************************************/
.sp
Cada vez que o botão de um dispositivo é clicado, o seu estado é reavaliado
(por exemplo, se ele foi montado/desmontado, ou se passou a permitir a escrita).
.ec	/*************************************/

.sp
As colunas seguintes contêm as seguintes infomações: nome do dispositivo
(sem "/dev"), nome contido no sistema de arquivos, tamanho do sistema de
arquivos, e estado do dispositivo.
.in -3

.ip 3. 3
Janela de informações sobre o sistema de arquivos escolhido:
contém estatísticas sobre quantos arquivos/blocos serão atualizados.

.ip 4. 3
Janela de alocação dos blocos antes da compactação:
barras vermelhas indicam regiões mal alocadas (que serão movidas),
barras azuis indicam regiões que já estão compactadas e
barras pretas indicam regiões não alocadas.

.ip 5. 3
Janela de alocação dos blocos durante/depois da compactação:
como acima.

.ip 6. 3
Barra de progresso: as barras verdes indicam a fração já processada.

.ep
.in -3

.in
.sp
.b 'MODO de USO'
.in 5
Antes de usar o utilitário "xdefrag", devemos verificar a integridade dos sistemas
de arquivos que desejamos desfragmentar (através de "fsck" (cmd)).

.sp
Em seguida, podemos seguir os seguintes passos:

.in +3
.ip 1. 3
Selecionar o dispositivo contendo o sistema de arquivos
a desfragmentar. Isto é realizado clicando o dispositivo desejado na janela de dispositivos.
Cada vez que o botão de um dispositivo é clicado, o seu estado é reavaliado
(por exemplo, se ele foi montado/desmontado, ou se passou a permitir a escrita).

.ip 2. 3
Escolher o método desejado
(Apenas os arquivos, Arquivos e lacunas ou completo), veja acima.

.ip 3. 3
Clicar em "análise", quando obteremos uma noção gráfica do estado
do sistema de arquivos e também uma estatística de quantos arquivos/blocos serão
movidos/modificados.

.ip 4. 3
Clicar em "desfragmenta" (se for permitido no caso),
e poderemos acompanhar o progresso da compactação na janela inferior.

.ep
.in -3

.in
.sp
.b 'OPÇÕES DO COMANDO'
.in 5
.in +3
.ip -r
Não modifica o sistema de arquivos. Realiza todas as operações, mas não escreve
os blocos do dispositivo (para testes).

.ip -g
Fornece a geometria inicial (em pixeis), no formato <largura>x<altura>.

.ip -f
Use a cor de <frente> dada.

.ip -b
Use a cor de <fundo> dada.

.ip -d
Conecte-se ao <vídeo> dado.

.ip -F
Use a <fonte> dada.

.ep
.in -3

.in
.sp
.b OBSERVAÇÕES
.in 5
É fundamental verificar a integridade do sistema de arquivos (através de "fsck" (cmd))
antes da desfragmentação. O uso de "xdefrag" em um sistema de arquivos com erros
poderá causar-lhe danos imprevisíveis.

.sp
A utilização do comando "xdefrag" é uma operação potencialmente
perigosa, pois em caso de interrupção da energia elétrica,
provavelmente todo o conteúdo do sistema de arquivos será perdido.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mkfs, fsck
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
