.bp
.he 'XMANDEL (cmd)'TROPIX: Manual de Referência'XMANDEL (cmd)'
.fo 'Atualizado em 23.04.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.(l
xmandel - desenha as imagens fractais de Mandelbrot
.)l

.in
.sp
.b SINTAXE
.in 5
.(l
xmandel [-g <geo>] [-f <frente>] [-b <fundo>] [-d <video>] [-F <f>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O gerador de imagens fractais de Mandelbrot "xmandel" permite
selecionar e desenhar regiões do conjunto de Mandelbrot.

.sp
Além disto, permite escolher o tamanho da imagem e as suas cores.

.sp
As imagens geradas por "xmandel" podem ser salvas em arquivos no formato GIF.

.in
.sp
.b 'DISPOSIÇÃO DAS JANELAS'
.in 5
O programa "xmandel" contém (de cima para baixo) as seguintes janelas:

.in +3
.ip 1. 3
Janela de botões: opções (veja abaixo), geração da imagem anterior, geração da imagem
corrente, geração do arquivo GIF e finalização do programa.

.ip 2. 3
Janela de mensagens: Nesta janela são escritas 2 tipos de mensagens:

.in +6
.ip a. 3
Mensagens informativas sobre a imagem (com o fundo regular):
coordenadas do centro da imagem (real, imaginário), largura da imagem,
tamanho da imagem (em pixeis) e o número de cores utilizadas.

.sp
Na linha seguinte, são dadas as coordenadas do cursor.

.ip b. 3
Mensagens de erro (com o fundo vermelho).

.ep
.in -6

.ip 3. 3
Janela da image: Nesta janela é desenhada a região do conjunto de Mandelbrot selecionada.
A imagem inicial tem as coordenadas [-0.75, 0], largura 4.0 e tamanho 640x480.

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
Tamanho da Imagem.

.ip 2. 3
Gradiente das cores:
Durante a geração do conjunto de Mandelbrot, é obtido um número associado a cada
ponto do conjunto, ao qual é associada uma cor.
O gradiente das cores permite indicar a velocidade da variação das cores 
em relação à variação dos números.

.ip 3. 3
Brilho das cores:
Permite variar a intensidade das componentes vermelhas (R), verde (G) e azul (B)
das diversas cores associados aos pontos do conjunto.

.ip 4. 3
Ordem das cores:
Permite permutar a ordem das componentes das cores criadas gerando diferentes
palhetas de cores.

.ip 5. 3
Desenha o vetor de cores:
Permite visualizar a palheta de cores em vigor (escolhida através das opções acima).

.ip 6. 3
Gera a Imagem:
Idêntico ao botão "Gera a Imagem".

.ip 7. 3
Gera o Arquivo:
Idêntico ao botão "Gera o Arquivo".

.ip 8. 3
Geração LZW:
A alternativa "Portátil" produz um arquivo GIF legível por qualquer utilitário
gráfico.

.sp
A alternativa "compacta" produz um arquivo GIF de tamanho menor, mas que possivelmente
não possa ser lido por alguns utilitários gráficos (em particular "Paintshop").

.ip 9. 3
Versão: A versão do programa "xmandel".

.ep
.in -3


.in
.sp
.b SELEÇÃO DE UM SUBCONJUNTO DA IMAGEM CORRENTE
.in 5
Para definir uma região da imagem corrente e ampliá-la, usamos o "mouse"
do seguinte modo:

.in +3
.ip 1. 3
Para definir o tamanho da região arrastamos o "mouse" com o botão da esquerda
pressionado.

.ip 2. 3
Para posicionar a região, arrastamos o "mouse" com o botão da direita pressionado.

.in -3
.ep

.sp
Após escolhida convenientemente a região, podemos gerar a imagem ampliada através
do botão "Gera a Imagem".

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

.ip -F
Use a fonte <f> dada.

.ep
.in -3

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
