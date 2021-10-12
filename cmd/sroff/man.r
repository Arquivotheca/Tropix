.bp 1
.he 'SROFF (cmd)'TROPIX: Manual de Referência'SROFF (cmd)'
.fo 'Atualizado em 07.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
sroff - formatador de textos

.in
.sp
.b SINTAXE
.in 5
sroff [-lN] [<texto> ...]

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "sroff" formata o texto contido nos arquivos <texto> dados,
escrevendo o texto formatado na saída padrão.

.sp
As opções do comando são:

.in +3
.ip -l
Gera o texto formatado para impressora "laser".

.ip -N
Le os nomes dos arquivos <texto> da entrada padrão "stdin".

.ep
.in -3

.sp
Caso NÃO sejam dados nomes de arquivos <texto> nem a opção "-N",
é editado o texto lido de "stdin".

.in
.sp
.b "COMANDOS DE EDIÇÃO"
.in 5
A edição é controlada pelos seguintes comandos:

.sp
.wo '- '
Padronização vertical:
.sp
.in +3
 .pl - Número de linhas por página
 .m1 - Espaço no início da página
 .m2 - Espaço abaixo do cabeçalho (se houver)
 .m3 - Espaço entre a última linha e o pé de página
.in +6
(se houver)
.in -6
 .m4 - Espaço no fim da página
 .ls - Espaço entre duas linhas consecutivas no texto
.in -3

.sp
.wo '- '
Padronização horizontal:
.sp
.in +3
 .ll - comprimento da linha, em caracteres
 .po - margem à esquerda de todas as linhas do texto
.br
.wo '.in - número'
de caracteres em branco a partir da margem
.in +6
esquerda
.in -9

.sp
.wo '- '
Cabeçalho e pé de página:
.sp
.in +3
 .he '<esq>'<centro>'<dir>' - Cabeçalho
 .eh '<esq>'<centro>'<dir>' - Cabeçalho de páginas pares
 .oh '<esq>'<centro>'<dir>' - Cabeçalho de páginas ímpares
 .fo '<esq>'<centro>'<dir>' - Pé de página
 .ef '<esq>'<centro>'<dir>' - Pé de página de páginas pares
 .of '<esq>'<centro>'<dir>' - Pé de página de páginas
.in +29
ímpares
.in -29
.in -3

.sp
.wo '- '
Controle de página:
.sp
.in +3
 .bp - Término da página corrente e começo da outra
 .pn - Especifica o número da página corrente
 .tp - Término da página e começo da página de título
.in -3

.sp
.wo '- '
Controle de linha:
.sp
.br
.in +3
.wo '.br (ou branco na primeira coluna) -'
.in +6
Quebra a linha corrente
.in -6
 .wo - Define uma palavra entre '...' ou "..."
 .ce - Centra linhas no texto
.br
.wo ".tl '<esq>'<centro>'<dir>' -"
.in +6
Imprime uma linha da mesma forma que um cabeçalho
.in -6
 .sp - Quebra a linha atual e pula linhas
 .ul - Sublinha as próxima linhas do texto
 .nf - Imprime as linhas como estão, sem formatar
 .li - Como ".nf"
 .fi - Retorna ao modo de ajustamento normal (termina ".nf")
.br
.wo ".sh - Numera"
o capítulo automaticamente
 .nr - Indenta os títulos dos capítulos
.in -3
        
.sp
.wo '- '
Controle de parágrafo:
.sp
.br
.in +3
.wo '.pp -'
Quebra a linha corrente, pula outra e imprime a
.in +6
próxima com indentação
.in -6

 .lp - Idem sem indentação
.br
.wo ".ip '<titulo>' <n> -"
.in +6
Quebra, pula uma linha e imprime o <título>, com linhas a seguir sendo
impressas com indentação constante <n> (valor por omissão: 5)
.in -6
.wo ".ep -"
Termina um parágrafo
.in -3

.sp
.wo '- '
Controle de blocos:
.sp
.in +3

.(l
   .bc		- Comentários: o <texto> será ignorado.
   <texto>
   .ec
.)l
.sp

.(l
   .(q		- Indenta o <texto> à esquerda e à direita
   <texto>		  em 5 caracteres
   .)q
.)l
.sp

.(l
   .(l		- Impressão literal do <texto> indentado à
   <texto>		  esquerda em 5 caracteres
   .)l
.)l
.sp

.(l
   .(b [f]		- Impressão de bloco fixo (sem o parâmetro
   <texto>		  "f") ou flutuante
   .)b
.)l
.sp

.(l
   .(t		- Impressão de bloco fixo sem alteração
   <texto>		  automatica da indentação anterior 
   .)t
.)l
.sp

.(l
   .(f		- Impressão de rodapé de página
   <texto>
   .)f
.)l
.sp

.in -3

.(t
.wo '- '
Controle do tipo de caracter:
.sp
.in +3
  .r [<pal>] - Caracter tipo romano (normal)
.)t
  .i [<pal>] - Caracter itálico
  .b [<pal>] - Muda o tipo dos caracteres para negrito

.ip OBS.: 6
Se a palavra <pal> for passada como parâmetro, somente esta será impressa
com o tipo determinado no comando. Em caso contrário, o tipo de caracter
será válido para todo o texto até um novo comando de
controle de tipo de caracter.
.lp
.in -3

.sp
.wo '- '
Formatação de tabelas:
.sp
.in +3
 .ts - Início da tabela  
 .t& - Redefinição dos formatos das linhas da tabela
 .te - Fim da tabela

.sp
Opções:
.sp
.in +3
.wo "center -     Centra a tabela" 
.br
.wo "expand -     Expande a tabela"
.br
.wo "box -        Coloca a tabela em uma moldura"
.br
.wo "allbox -     Coloca todos os elementos da tabela em"
.br
.wo "             molduras"
.br
.wo "doublebox -  Coloca a tabela em uma moldura dupla"
.br
.wo "tab (<x>) -  Troca o caracter separador dos dados"
.br
.wo "             por <x>"
.br
.wo "title <n> -  A <n>-ésima linha de dados será usada como"
.br
.wo "             título em cada página que a tabela"
.br
.wo "             continuar"
.in -3

.sp
Formatos:
.in +3
.sp
.wo "l - Campo ajustado à esquerda"
.br
.wo "c - Campo centrado"
.br
.wo "r - Campo ajustado à direita"
.br
.wo "n - Campo numérico (ajustado pelo ponto decimal)"
.br
.wo "a - Subcoluna"
.br
.wo "s - Campo incorporado ao anterior"
.br
.wo "^ - Campo em branco"
.br
.wo "- - Campo com uma linha ('-')"
.br
.wo "= - Campo com uma linha dupla ('=')"
.br
.wo "| - Linha vertical entre duas colunas"
.in -3

.sp
Atributos dos formatos:
.sp
.in +3
.wo "f -       Troca o tipo de caracter do campo"
.br
.wo "b -       Campo em negrito"
.br
.wo "r -       Campo em romano"
.br
.wo "i -       Campo em itálico"
.br
.wo "e -       Colunas com mesmo tamanho"
.br
.wo 'w (<n>) - <n> é a largura mínima da coluna'
.br
.wo '<n> -     <n> é o espacejamento mínimo entre esta'
.br
.wo '          e a próxima coluna'
.in -3

.sp
Dados:
.sp
.in +3
- Comando para o "sroff"
 - '-' ou '=' - Linha simples ou dupla (na linha inteira)
 - Campos separados por <tb>:
.in +3
- '-' ou '=' - Linha simples ou dupla no campo
 - '\-' ou '\=' - Linha simples ou dupla no elemento 
 - 'Rx' - 'x' em toda a extensão do campo
 - '\&x' - 'x' é o elemento do campo
 - '\^' - Campo em branco
 - <s> - <s> é a cadeia de caracteres (elemento)
 - Campo vazio (dois <tb> seguidos) - campo em branco
.in -3
.in -3

.sp
Exemplo:
.sp
.in +3

.nf
              .ts
              allbox;
              c s s
              r c s
              l n n.
              titulo
              nome	valores
              x		1.2	45
              y		3.5	120
              z		34	490
              .te
.fi

.sp
Irá gerar:

.sp
.ts
allbox;
c s s
r c s
l n n.
titulo
nome	valores
x	1.2	45
y	3.5	120
z	34	490
.te

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
vi
.br

.in
.sp
.b EXEMPLOS
.in 7
.sp
sroff arquivo
.sp
.in +3
formata o "arquivo" pondo o texto editado na saída padrão.
.in -3
.sp

.(t
sroff man.* >manual
.sp
.in +3
concatena todos os arquivos cujos nomes começam por "man.",
formata-os, e põe o texto editado no arquivo "manual".
.in -3
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo

.in
