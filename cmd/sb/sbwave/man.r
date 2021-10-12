.bp
.he 'SBWAVE (cmd)'TROPIX: Manual de Referência'SBWAVE (cmd)'
.fo 'Atualizado em 23.5.98'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "sbwave -"
toca música PCM através da placa de som SB-16
.br

.in
.sp
.b SINTAXE
.in 5
.(l
sbwave [-vdt] <arquivo> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "sbwave" executa as músicas PCM dadas nos <arquivo>s
através da placa de som "Sound Blaster 16" ou sucessoras
("AWE-32", "AWE-64", ...).

.sp
Os arquivos devem estar no formato "wave"
(como por exemplo os arquivos "*.wav" do padrão da Microsoft).

.sp
Se a interface gráfica X-Window estiver ativa, é aberta uma janela
contendo 3 linhas:

.in +3
.ip "Linha 1:" 9
Contém o nome do arquivo em execução
e as suas características
(isto é, Mono/Stereo, Freqüencia de amostragem, No. de bits de cada
amostra e o Tamanho do arquivo).

.ip "Linha 2:" 9
Nesta linha é desenhada
uma barra crescente acompanhando a execução do arquivo.

.ip "Linha 3:" 9
Contém os tempos na forma
"mm:ss:cc", onde "mm" representam os minutos, "ss" os segundos e
"cc" centésimos de segundo, correspondentes à barra da segunda linha.

.ep
.in -3

.sp
As opções do comando são:

.in +3
.ip -v
Fornece algumas informações sobre cada uma dos <arquivo>s.

.ip -d
Não executa, apenas fornece as informações
sobre cada um dos <arquivo>s,
no formato da linha 1, acima.

.ip -t
Usa o modo de texto (isto é, NÃO gráfico), mesmo que
a interface gráfica X-Window esteja ativa.

.ep
.in -3

.sp 2
Se você tiver Win95/Win98 instalado em seu computador, você pode
obter algumas amostras de arquivos "*.wav", usando o programa
"dosmp" para copiar arquivos do diretório "\WINDOWS\MEDIA".
Não esqueça de usar o modo "binário".

.in
.sp
.b OBSERVAÇÃO
.in 5
Na presente versão, o TROPIX somente aceita placas de som com
interface digital (DSP) de 16 bits. Observe, durante a carga
do sistema operacional, qual o modelo da sua placa de som, e se
o TROPIX a reconhece/aceita.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
dosmp
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
