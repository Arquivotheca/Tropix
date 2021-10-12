.bp
.he 'GAR (fmt)'TROPIX: Manual de Referência'GAR (fmt)'
.fo 'Atualizado em 28.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "gar -"
formato das coleções GAR
.br

.in
.sp
.b DESCRIÇÃO
.in 5
O utilitário "gar" (cmd) processa habitualmente coleções em seu
formato nativo, o formato GAR. Este texto descreve este formato. 

.sp
Uma coleção no formato GAR tem uma série de cabeçalhos (e outras
informações) intercaladas entre os conteúdos dos arquivos;
todos estes cabeçalhos são em ISO (isto é, não contém
números em forma binária), o que facilita o transporte para
outros computadores (que possivelmente tem ordenação
diferente dos bytes dentro das palavras). 

.sp
As diversas partes da coleção são contíguas, isto é,
o formato não prevê nenhum byte de alinhamento:
qualquer cabeçalho ou conteúdo pode estar posicionado
em qualquer endereço (isto é, distância em bytes do início)
do volume, inclusive ímpar.

.sp
Repare que não temos um cabeçalho de final de volume;
isto é indicado pelo final físico do volume (final da fita
magnética, ou do disquete).

.sp
Todos os números são codificados como sequências de  dígitos decimais.  
Todos os bytes não utilizados são preenchidos com <sp>, a não
ser o byte seguinte ao valor dos CRCs, que é preenchido com <nul>.
Todas as datas são codificadas na forma padrão do TROPIX,
isto é, em segundos desde 00:00 de 1.1.1970 GMT. 

.sp
Todos os CRCs são calculados pelo polinômio padrão da CCITT
(ver "crc16" (libc)). Embora "gar" (cmd) gere/verifique todos os
CRCs, um outro utilitário particular não tem necessariamente de
fazer esta verificação para ler coleções GAR em um outro computador.

.sp 2
.b "Cabeçalho de Volume:"

.sp
Uma coleção pode se estender a um ou mais
volumes; cada volume contém no início um cabeçalho de volume,
que tem o formato
da seguinte estrutura:

.sp 1
.nf
	typedef	struct
	{
		char	g_magic[12];	/* Cadeia mágica */
		char	g_date[12];	/* Data de criação */
		char	g_vol[6];	/* No. do Volume */
		char	g_version[9];	/* Versão do GAR */
		char	g_vres[13];	/* Reservado */
		char	g_vcrc[8];	/* CRC */
	
	}	GVOL;
.fi
.sp 2

O valor da cadeia mágica é "GAR VOLUME".

.sp
A data de criação é a da coleção.

.sp
O número do volume começa de 0 e é incrementado de 1 para cada
novo volume de continuação.

.sp
No volume 0, o cabeçalho do volume é seguido do cabeçalho do
primeiro arquivo da coleção; nos volumes seguintes é seguido
da continuação do conteúdo da coleção, que pode ter
sido seccionado em um ponto qualquer. 

.sp 3
.b "Cabeçalho de Arquivo:"

.sp
Uma coleção pode ter um número indefinido de arquivos;
o final da coleção é indicado por um cabeçalho de final
de coleção (ver adiante).
Cada arquivo contém no início um cabeçalho de arquivo,
que tem o formato da seguinte estrutura:

.sp 2
.nf
	typedef	struct
	{
		char	g_magic[12];	/* Cadeia magica */
		char	g_dev[6];	/* Dispositivo */
		char	g_ino[6];	/* No. do Inode */
		char	g_mode[12];	/* Modo do arquivo */
		char	g_nlink[6];	/* No. de Links */
		char	g_uid[6];	/* Dono */
		char	g_gid[6];	/* Grupo */
		char	g_rdev[6];	/* Dispositivo Especial */
		char	g_size[12];	/* Tamanho */
		char	g_atime[12];	/* tempo de acesso */
		char	g_mtime[12];	/* tempo de modificação */
		char	g_ctime[12];	/* tempo de criação */
		char	g_namesz[6];	/* Tamanho do nome */
		char	g_linkflag[2];	/* Este nome é um Link */
		char	g_origsz[12];	/* Tamanho original */
		char	g_bits[3];	/* Bits (L), nível (Z) */
		char	g_compcrc[2];	/* Compressão com CRC */
		char	g_method[3];	/* "C" (LZW), "D" (ZIP) */
		char	g_res[16];	/* Reservado */
		char	g_crc[8];	/* CRC */

	}	GHEAD;
.fi

.sp 2
O valor da cadeia mágica é "GAR HEADER", a não ser
se trate de um "link" (ver adiante).

.sp
Os valores de "dev", "ino", "mode", "nlink", "uid", "gid", "rdev",
"atime", "mtime", e "ctime" são as informações habituais
sobre o arquivo, que são obtidas através de "stat" (sys).  

.sp
O se o arquivo estiver comprimido, o campo "size" contém o tamanho
do arquivo e o campo "origsz" contém zero; em caso contrário,
"size" contém o tamanho comprimido e "origsz" o tamanho real do
arquivo.

.sp
O campo "g_linkflag" contém um <sp> a não ser que se trate de
um "link", caso no qual contém um "*".

.sp
Se o arquivo estiver comprimido, o campo "bits" contém o número
máximo de bits utilizado (método LZW) ou o nível de compressão
usado (método ZIP (deflação)), o campo "compcrc" contém "R" se
é usado um CRC adicional antes de comprimir o arquivo e o campo
"method" contém "C" para o método LSW e "D0" para o método ZIP.

Se o arquivo NÃO estiver comprimido, estes campos não são usados.

.sp 3
.b "Nome de Arquivo:"

.sp
Após este cabeçalho segue o nome do arquivo com o seu respectivo
CRC, com o formato da estrutura abaixo:  

.sp 2
.nf
	typedef	struct
	{
		char	g_name[size];	/* Nome do arquivo */
		char	g_crc[8];	/* CRC */

	}	GNAME;
.fi

.sp 2
O tamanho do nome "size" é obtido através de "namesz" do cabeçalho.

.sp 3
.b "Conteúdo do Arquivo:"

.sp
Em seguida, se o arquivo for regular, temos o conteúdo do arquivo
(a não ser que se trate de
um "link" - ver adiante).
Cada bloco do arquivo tem o formato da estrutura dada abaixo, exceto
o bloco final, que pode ter menos de 512 bytes.

.sp 2
.nf
	typedef	struct
	{
		char	g_block[512];	/* Bloco do arquivo */
		char	g_crc[8];	/* CRC */

	}	GBLOCK;
.fi

.sp 3
.(t
.b "Arquivo \"link\"ado:"

.sp
Se o arquivo é um "link" de um arquivo anterior, 
não temos o conteúdo do arquivo; ao nome do arquivo segue-se
outro cabeçalho de arquivo com a cadeia mágica "GAR LINK",
e outro nome de arquivo, conforme as respectivas partes.
A grande parte das informações deste segundo cabeçalho
é ignorada, pois importa apenas o nome do "link".
.)t

.sp
.(t
.in 5
Resumindo, temos a seguinte sequência para arquivos regulares:

.sp
.nf
	+---------------+-------+-------------------------------+
	|		|	|				|
	|   cabeçalho	| nome	|      c o n t e ú d o		|
	|   de arquivo	|	|				|
	|		|	|				|
	+---------------+-------+-------------------------------+
.fi
.)t

.sp 2
.(t
.in 5
Para arquivos não regulares:

.sp
.nf
	+---------------+-------+
	|		|	|
	|   cabeçalho	| nome	|
	|   de arquivo	|	|
	|		|	|
	+---------------+-------+
.fi
.)t
.sp 2

.(t
.in 5
Para "link" de arquivos (regulares ou não):
.sp

.nf
	+---------------+-------+---------------+-------+
	|		|	|		|	|
	|   cabeçalho	| nome	|   cabeçalho	| nome	|
	|   de arquivo	|	|   de "link"	|	|
	|		|	|		|	|
	+---------------+-------+---------------+-------+
.fi
.)t
.sp 2

.b "Cabeçalho de Final de Coleção:"
.sp
O final da coleção é indicada por um cabeçalho de final de coleção,
que é simplesmente a cadeia "GAR TRAILER". O restante do bloco
da coleção é preenchido com <sp>.

.in
.sp

.b
OBSERVAÇÃO
.r
.in 5
Não confunda "final de volume" com "final de coleção".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
gar
.br
.wo "(sys): "
stat
.br
.wo "(libc):"
crc16
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
