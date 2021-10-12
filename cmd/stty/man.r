.bp 1
.he 'STTY (cmd)'TROPIX: Manual de Referência'STTY (cmd)'
.fo 'Atualizado em 09.07.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "stty -"
consulta/altera parâmetros de terminais/vídeos
.br

.in
.sp
.b SINTAXE
.in 5
stty [-at] [<param> ou -<param> ...]
.sp
STTY [-at] [<param> ou -<param> ...]

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "stty" (ou "STTY") consulta/altera uma série de parâmetros
de terminais, pseudo-terminais, vídeos, linhas de comunicação, impressoras,
etc ...

.sp
Se forem dados parâmetros da forma <param> ou -<param>,
estes são respectivamente ativados ou desativados (veja a lista abaixo).
Em caso contrário, é impressa uma lista
resumida dos parâmetros com os seus estados correntes (veja as opções abaixo).

.sp
As opções do comando são:

.in +3
.ip -a
Fornece a lista completa de parâmetros ao invés de uma lista resumida.

.ip -t
Fornece a lista de cores disponíveis para o parâmetro "color", com
os respectivos códigos numéricos.

.ep
.in -3

.sp
Nas sessões seguintes é dada uma lista completa dos parâmetros.
Maiores detalhes sobre cada parâmetro podem ser encontrados em
"termio" (fmt).

.sp 2
.b
Caracteres de controle:
.r

.sp
.in +5
Alguns caracteres têm função especial na entrada por terminal/vídeo;
eles são dados nesta sessão, juntamente com alguns parâmetros
que na realidade não são caracteres, mas são valores que
são armazenados em um byte.
O valor dado após o nome do caractere (<c> ou <n>) passa a
ser o novo valor do caractere de controle.

.sp
O valor de <c> pode ser dado diretamente como um caracter, como por
exemplo <^C> ou então na forma "^C", isto é, os 2 caracteres
"^" e "C". Neste segundo caso, os 2 caracteres devem vir entre
aspas ou apóstrofes.

.sp
O valor de <n> deve ser dado como um número decimal, octal ou hexadecimal
(conforme a sintaxe de "strtol" (libc)).
Isto também é o caso dos valores de <f> e <b> para o caso de "color".

.ip "intr <c>" 16
Este caractere gera um sinal de interrupção ("SIGINT"), 
que é enviado a todos os processos associados ao terminal/vídeo
de controle. Normalmente, estes processos terminam
ao receber este sinal, mas eles podem utilizar a chamada
ao sistema "signal" (sys) para ignorá-lo ou interceptá-lo.
O valor inicial deste caractere é <^C>. 

.ip "quit <c>" 16
Este caractere gera um sinal de "quit" ("SIGQUIT"), 
que é enviado a todos os processos associados ao terminal/vídeo
de controle. Normalmente, estes processos terminam com a gravação
da imagem do processo no disco ("core-dump")
ao receber este sinal, mas eles podem utilizar a chamada
ao sistema "signal" (sys) para ignorá-lo ou interceptá-lo.
O valor inicial deste caractere é <^X>. 

.ip "erase <c>" 16
Este caractere apaga o caractere anterior; se estivermos
no começo de uma linha, nada faz.
O valor inicial deste caractere é <del> ("^*"). 

.ip "kill <c>" 16
Este caractere apaga a linha inteira.
O valor inicial deste caractere é <^U>.

.ip "eof <c>" 16
Este caractere pode ser usado para gerar uma condição de 
fim de arquivo ("EOF") do terminal/vídeo. Quando recebido, todos os
caracteres da fila de entrada são passados imediatamente para
o programa, sem esperar um <nl>, e o caractere é ignorado.
Se não havia nenhum caractere na fila, zero caracteres são
passados para o programa, o que é a indicação padrão
de EOF. 
O valor inicial deste caractere é <^D>.

.ip "min <n>" 16
Este valor só é utilizado no modos "raw" ou "comm",
e representa o número mínimo de caracteres
necessários na fila de entrada para que eles sejam passados
para o processo. Em outras palavras, o processo espera
até que a fila de entrada contenha <n> caracteres.  
O valor inicial deste parâmetro é 1.

.ip "eol <c>" 16
Este caractere é um delimitador adicional (como <nl>).
Não é utilizado normalmente, e no momento não está implementado.
O valor inicial deste parâmetro é <nul> (<^@>).

.ip "time <n>" 16
Este valor só é utilizado no modos "raw" ou "comm",
e representa um tempo utilizado para medir o tempo decorrido
para receber "min" caracteres.
No momento não está implementado.
O valor inicial deste parâmetro é 0.

.ip "res <c>" 16
Este caractere no momento não tem função; está reservado
para uso futuro.
O valor inicial deste parâmetro é <nul> (<^@>).

.ip "switch <c>" 16
Este caractere é utilizado pelas rotinas de gerenciamento de janelas
de terminais.
O valor inicial deste parâmetro é <nul> (<^@>).

.ip "page <n>" 16
Este valor representa o número de linhas após o qual a saída
no terminal/vídeo pára, aguardando um <^Q> (ou um caracter qualquer,
veja "ixany").
Isto é útil para terminais de vídeo para evitar que a saída
desapareça da tela antes que possa ser lida.
Fazendo <n> igual a zero (ou usando "-page),
o controle não é realizado.
O valor inicial deste parâmetro é 23.

.ip "aerase <c>" 16
Este caractere é análogo ao "erase":
apaga o caractere anterior; se estivermos
no começo de uma linha, nada faz.
O valor inicial deste caractere é <bs>.

.ip "retype <c>" 16
Este caractere re-imprime o a linha no terminal/vídeo.
É útil quando a linha sendo teclada é misturada com
mensagens de processos executando em "background".
O valor inicial deste caractere é <^R>.

.ip "word <c>" 16
Este caractere apaga a última palavra teclada.
Uma palavra é definida como sendo um conjunto de caracteres
que não contenham <sp> nem <ht>.
O valor inicial deste caractere é <^W>.

.ip "color <f>[,<b>]" 16
Este caracter define a cor de frente e fundo definido respectivamente
pelos números <f> e <b> (veja a opção "-t").
Se a cor de fundo <b> for omitida, será assumido preta.
O valor inicial deste caractere é dependente do dispositivo.

.ip "nline <l>" 16
Número de linhas do terminal/vídeo.
O valor inicial deste caractere é dependente do dispositivo
(em geral 24).

.ip "ncol <c>" 16
Número de colunas do terminal/vídeo.
O valor inicial deste caractere é dependente do dispositivo
(em geral 80).

.ep
.in -5
.sp 2
.b
Modos de controle:
.r
.in +5

.ip "parenb" 10
Liga a geração/deteção de paridade.

.ip "-parenb" 10
Desliga a geração/deteção de paridade.

.ip "parodd" 10
Utiliza paridade ímpar.

.ip "-parodd" 10
Utiliza paridade par.

.ip "cs5" 10
Os caracteres tem 5 bites.

.ip "cs6" 10
Os caracteres tem 6 bites.

.ip "cs7" 10
Os caracteres tem 7 bites.

.ip "cs8" 10
Os caracteres tem 8 bites.

.ip "0" 10
Desconecta a linha imediatamente.

.ip "n" 10
A linha deve transmitir/receber a "n" baud.
O valor de "n" pode ser: 110, 134, 150, 200, 300, 600, 1200,
1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200. Nem todas as interfaces
aceitam todas estas velocidades.
As velocidades acima de 1000 baud tem os sinônimos 1.2, 1.8, 2.4, 4.8,
9.6, 19.2, 38.4, 57.6 e 115.2.

.ip "hupcl" 10
Desconecta a linha no último "close".

.ip "-hupcl" 10
Não desconecta a linha no último "close".

.ip "cstop" 10
Utiliza dois bites de "stop" por caractere.

.ip "-cstop" 10
Utiliza um bite de "stop" por caractere.

.ip "cread" 10
Liga o receptor.

.ip "-cread" 10
Desliga o receptor.

.ip "clocal" 10
A linha não tem "modem" (não usa controle de fluxo por "hardware").

.ip "-clocal" 10
A linha tem "modem" (usa controle de fluxo por "hardware").

.ep
.in -5
.sp 2
.b
Modos de entrada:
.r
.in +5

.ip "ignbrk" 10
Ignora "break" na entrada.

.ip "-ignbrk" 10
Não ignora "break" na entrada.

.ip "brkint" 10
Envia o sinal "SIGINT" com um "break".

.ip "-brkint" 10
Não envia o sinal "SIGINT" com um "break".

.ip "ignpar" 10
Ignora erros de paridade.

.ip "-ignpar" 10
Não ignora erros de paridade.

.ip "parmrk" 10
Marca erros de paridade.

.ip "-parmrk" 10
Não marca erros de paridade.

.ip "inpck" 10
Liga a verificação de paridade.

.ip "-inpck" 10
Desliga a verificação de paridade.

.ip "istrip" 10
Remove o oitavo bite do caracter de entrada.

.ip "-istrip" 10
Não remove o oitavo bite do caracter de entrada.

.ip "inlcr" 10
Converte <nl> em <cr> na entrada.

.ip "-inlcr" 10
Não converte <nl> em <cr> na entrada.

.ip "igncr" 10
Ignora <cr> na entrada.

.ip "-igncr" 10
Não ignora <cr> na entrada.

.ip "icrnl" 10
Converte <cr> em <nl> na entrada.

.ip "-icrnl" 10
Não converte <cr> em <nl> na entrada.

.ip "iuclc" 10
Converte letras maiúsculas em minúsculas na entrada.

.ip "-iuclc" 10
Não converte letras maiúsculas em minúsculas na entrada.

.ip "ixon" 10
Liga o controle de saída com <^Q>, <^S>.

.ip "-ixon" 10
Desliga o controle de saída com <^Q>, <^S>.

.ip "ixany" 10
Aceita qualquer caracter para a função de <^Q>.

.ip "-ixany" 10
Aceita apenas o caracter <^Q>.

.ip "ixoff" 10
Envia os caracteres <^S>, <^Q> quando a fila de entrada
está quase cheia/vazia.

.ip "-ixoff" 10
Não envia os caracteres <^S>, <^Q> quando a fila de entrada
está quase cheia/vazia.


.ep
.in -5
.sp 2
.(t
.b
Modos de saída:
.r
.in +5

.ip "opost" 10
Processa os caracteres de saída.
.)t

.ip "-opost" 10
Não processa os caracteres de saída. Ignora todos
os modos de saída dados a seguir.

.ip "olcuc" 10
Converte letras minúsculas em maiúsculas na saída.

.ip "-olcuc" 10
Não converte letras minúsculas em maiúsculas na saída.

.ip "onlcr" 10
Converte <nl> em <cr><nl> na saída.

.ip "-onlcr" 10
Não converte <nl> em <cr><nl> na saída.

.ip "ocrnl" 10
Converte <cr> em <nl> na saída.

.ip "-ocrnl" 10
Não converte <cr> em <nl> na saída.

.ip "onocr" 10
Não escreve <cr>s no começo da linha.

.ip "-onocr" 10
Escreve <cr>s no começo da linha.

.ip "onlret" 10
No terminal/vídeo, o caractere <nl> faz a função do <cr>. 

.ip "-onlret" 10
No terminal/vídeo, o caractere <nl> não faz a função do <cr>. 

.ip "ofill" 10
Usa caracteres de "enchimento" para produzir atrasos.

.ip "-ofill" 10
Usa o relógio interno para produzir atrasos.

.ip "ofdel" 10
Os caracteres de "enchimento" são <del>s.

.ip "-ofdel" 10
Os caracteres de "enchimento" são <nul>s (<^@>)s.

.ip "cr[i]" 10
Seleciona o modo do atraso para <cr>s: cr0, cr1, cr2, cr3
(ver termio (fmt)).

.ip "nl[i]" 10
Seleciona o modo do atraso para <nl>s: nl0, nl1
(ver termio (fmt)).

.ip "tab[i]" 10
Seleciona o modo do atraso para <ht>s: tab0, tab1, tab2, tab3
(ver termio (fmt)).

.ip "bs[i]" 10
Seleciona o modo do atraso para <bs>s: bs0, bs1
(ver termio (fmt)).

.ip "ff[i]" 10
Seleciona o modo do atraso para <ff>s: ff0, ff1
(ver termio (fmt)).

.ip "vt[i]" 10
Seleciona o modo do atraso para <vt>s: vt0, vt1
(ver termio (fmt)).

.ep
.in -5
.sp 2
.b
Modos locais:
.r
.in +5

.ip "isig" 10
Processa os caracteres especiais "intr" e "quit".

.ip "-isig" 10
Não processa os caracteres especiais "intr" e "quit".

.ip "icanon" 10
Processa os caracteres na entrada, aplicando os modos de entrada
escolhidos.

.ip "-icanon" 10
Não processa os caracteres na entrada, os modos de entrada
são ignorados.

.ip "xcase" 10
Apresentação canônica de maiúsculas/minúsculas. 

.ip "-xcase" 10
Apresentação não processada de maiúsculas/minúsculas. 

.ip "echo" 10
Ecoa todos os caracteres teclados.

.ip "-echo" 10
Não ecoa os caracteres teclados.

.ip "echoe" 10
Ecoa os caracteres "erase" e "aerase" como <bs><sp><bs>.

.ip "-echoe" 10
Ecoa os caracteres "erase" e "aerase" como <bs>.

.ip "-echok" 10
Ecoa <nl> após o caractere "kill".

.ip "echok" 10
Não ecoa <nl> após o caractere "kill".

.ip "echonl" 10
Ecoa <nl>.

.ip "-echonl" 10
Não ecoa <nl>.

.ip "noflsh" 10
Esvazia as filas após um "intr" ou "quit".

.ip "-noflsh" 10
Não esvazia as filas após um "intr" ou "quit".

.ip "cntrlx" 10
Converte os caracteres de controle na saída para a forma "^X". 

.ip "-cntrlx" 10
Não converte os caracteres de controle.

.ip "isokey" 10
Não converte pares de caracteres de entrada (acento, letra) na letra acentuada
correspondente.

.ip "-isokey" 10
Converte pares de caracteres de entrada (acento, letra) na letra acentuada
correspondente.

.ip "iso" 10
Informa a interface que o código deste terminal/vídeo é ISO completo.

.ip "ascii" 10
Informa a interface que o código deste terminal/vídeo é ASCII.

.ip "user1" 10
Informa a interface que o código deste terminal/vídeo é o código
"user1" (normalmente o código ABICOMP, pode ser alterado
por "kcntl" (sys)).

.ip "user2" 10
Informa a interface que o código deste terminal/vídeo é o código
"user2" (normalmente o código ISO, pode ser alterado
por "kcntl" (sys)).

.ip "video" 10
Informa a interface que o terminal é de vídeo,
e processe os caracteres "kill", "erase", "aerase", "retype"  
de acordo.

.ip "-video" 10
Informa a interface que o terminal não é de vídeo,
e processe os caracteres "kill", "erase", "aerase", "retype"  
de acordo.

.ip "icomm" 10
Habilita na entrada, o "modo de comunicações", que é
equivalente a desligar "icanon", "isig", "opost" e "echo"
porém mais eficiente. 

.ip "-icomm" 10
Desabilita na entrada, o "modo de comunicações".

.ep
.in -5
.sp 2
.b
Macro argumentos:
.r
.in +5
.sp
Os macro argumentos são nomes simbólicos
para algumas combinações dos modos dados acima.
Eles unem as combinações mais usuais, e simplificam o uso
no caso normal.

.ip "evenp" 10
Habilita a paridade par:
liga "parenb" e "cs7"; desliga "parodd".

.ip "parity" 10
Como "evenp".

.ip "oddp" 10
Habilita a paridade ímpar:
liga "parenb", "cs7" e "parodd".

.ip "-parity" 10
Desabilita o processamento de paridade:
desliga "parenb"; liga "cs8".

.ip "-evenp" 10
Como "-parity".

.ip "-oddp" 10
Como "-parity".

.ip "cooked" 10
Habilita o processamento de caracteres na entrada e saída:
liga "opost" e "icanon".

.ip "-cooked" 10
Desabilita o processamento de caracteres na entrada e saída:
desliga "opost" e "icanon".

.ip "raw" 10
Como "-cooked".

.ip "-raw" 10
Como "cooked".

.ip "comm" 10
Habilita na entrada, o "modo de comunicações", que é o modo
mais rápido e transparente, para comunicação máquina/máquina.
Este parâmetro desliga todo e qualquer processamento de entrada, como
por ex. "isig" e "icanon", o processamento de  de saída "opost"
e o "echo" na saída.

.ip "-comm" 10
Como "cooked".

.ip "-nl" 10
Tratamento de <nl> e <cr> normal para terminais:
liga "icrnl" e "onlcr"; desliga "inlcr", "igncr", "ocrnl", "onlret".

.ip "nl" 10
Desabilita o tratamento de <nl> e <cr>:
desliga "icrnl" e "onlcr".

.ip "-lcase" 10
Tratamento normal de maiúsculas/minúsculas:
desliga "xcase", "iuclc", "olcuc".

.ip "-LCASE" 10
Como "-lcase".

.ip "lcase" 10
Tratamento para terminais que só têm letras maíusculas:
liga "xcase", "iuclc", "olcuc".

.ip "LCASE" 10
Como "lcase".

.ip "tabs" 10
Preserva os caracteres de tabulação:
liga "tab0".

.ip "-tabs" 10
Substitue os caracteres de tabulação por brancos:
liga "tab3".

.ip "std" 10
Atribue valores "razoáveis" para todos os modos e caracteres.

.ip "sane" 10
Como "std".

.ip "ebc" 10
Realiza as seguintes funções de inicialização do terminal EBC-4020:
seleciona os acentos longos, atribui a data corrente,  e apaga as
duas linhas entre a parte inferior da tela e a linha da data.

.ep
.in
.sp
.(t
.b OBSERVAÇÕES
.in 5
Nem todos os dispositivos admitem todos os modos descritos acima.
.)t
.sp
Nem todas as combinações dos modos descritos acima fazem
sentido.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
ioctl, kcntl
.br
.wo "(libc):"
strtol
.br
.wo "(fmt): "
termio
.br

.in
.sp
.b ARQUIVOS
.in 5
<termio.h>

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
