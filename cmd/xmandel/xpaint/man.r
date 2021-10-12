.bp
.he 'XPAINT (xwin)'TROPIX: Manual de Referência'XPAINT (xwin)'
.fo 'Escrito em 17.04.03'Versão 4.4.0'Pag. %'

.b NOME
.in 5
xpaint - exibe imagens em uma janela X-Window.

.in
.sp
.b SINTAXE
.in 5
xpaint [opções] <arquivo> ...

.in
.sp
.b DESCRIÇÃO
.in 5
O cliente "xpaint" permite visualizar, em uma janela, imagens nos seguintes
formatos: GIF, JPEG, BMP e XPM (um formato próprio do X-Window).
.sp

Os <arquivo>s especificados podem ser regulares ou diretórios.
No primeiro caso, devem conter as imagens a serem visualizadas.
Sempre que um diretório é dado, a subárvore por ele enraizada é
percorrida à procura de arquivos que contenham imagens.
.sp

Se o nome de um arquivo possuir a extensão correspondente ao formato
(".gif", "jpg", ".bmp" ou ".xpm"), o "xpaint" assumirá que a imagem
contida no arquivo possui o formato especificado; caso contrário, tentará
descobrir o formato, analisando o arquivo.
.sp

Há também a possibilidade de mostrar a seqüência em "slideshow",
especificando o intervalo de tempo entre exibições consecutivas.
.sp

.in 5
As opções disponíveis são as seguintes:
.in 8

.ip "-d <display>" 20
especifica o servidor X a contactar. Por omissão, assume-se o servidor local.

.ip "-f <fator>" 20
especifica o fator de escala a utilizar. Valores para ampliação:
"2", "3", ..., "8". Valores para redução: "1/2", "1/3", "1/4", ..., "1/8".

.ip "-N" 20
os <arquivo>s, em vez de mencionados como argumentos do programa,
são lidos da entrada padrão.

.ip "-R" 20
dentre os arquivos encontrados, seleciona um aleatoriamente e
usa a imagem nele contida como fundo da janela-mãe (RootWindow).

.ip "-s <tempo>" 20
especifica o intervalo de <tempo> segundos entre exibições consecutivas.

.ip "-v" 20
imprime, na janela de texto onde o "xpaint" foi invocado, informações sobre
as imagens sendo mostradas.
.ep
.sp
.sp

.in
.b
NAVEGAÇÃO
.r
.in 5
Sempre que vários arquivos contendo imagens forem encontrados,
o usuário terá a opção de navegar pela seqüência de imagens,
utilizando as setas e algumas teclas especiais:

.in 8
.ip "Seta para cima" 20
retrocede à imagem anterior

.ip "Seta para baixo" 20
passa à imagem seguinte

.ip "Seta para esquerda" 20
reduz o fator de escala, na seqüência "1", "1/2", "1/3", ..., "1/8",
diminuindo a imagem corrente

.ip "Seta para direita" 20
aumenta em uma unidade o fator de escala, ampliando a imagem corrente

.ip "Home" 20
volta à primeira imagem

.ip "End" 20
passa à última imagem

.ip "Esc" 20
encerra a navegação
.ep
.sp

.in
.b
OBSERVAÇÕES
.r
.in 5
A alteração do fator de escala durante a exibição de uma imagem é
preservada em exibições subseqüentes da mesma imagem.
.sp

Imagens no formato XPM são imunes a fatores de escala, só podendo
ser exibidas em tamanho natural.
.sp

Se o intervalo de tempo entre imagens for especificado (veja opção "-s"),
ainda assim o usuário poderá utilizar as teclas de navegação. A única
diferença é que, expirado o tempo mencionado, a transição para a imagem
seguinte far-se-á automaticamente.
.sp

.in
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xwin): "
X, fvwm
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
