.bp 1
.he 'MKASMTB (libc)'TROPIX: Manual de Referência'MKASMTB (libc)'
.fo 'Atualizado em 23.02.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "mkasmtb -"
gera módulos em linguagem simbólica ("assembly")

.in
.sp
.b SINTAXE
.(l
#include <mkasmtb.h>

int
mkasmtb
(
  const char *as_nm,	/* Nome do módulo a gerar (com ".s") */
  const char *tb_nm,	/* Nome da tabela dada (sem o '_') */
  const void *tb_addr,	/* Endereco da tabela dada */
  long tb_sz,		/* Tamanho da tabela (em bytes) */
  ROW tb_access,		/* Tipo da proteção da tabela */
  int tb_ent_sz,		/* Tamanho de cada entrada da tabela */
  const OBJENT *tb_ent_desc /* Vetor de descrição das entradas */
);

.)l

.sp
.b DESCRIÇÃO
.in 5
A função "mkasmtb" gera um arquivo contendo um programa
em linguagem simbólica ("assembly")
a partir de uma tabela previamente construída.
Este programa por sua vez, quando montado irá
produzir um programa objeto (contendo a tabela), pronto para
ser "link-editado".
.sp

A principal aplicação é a de criar automaticamente tabelas
que são complexas demais para serem definidas diretamente.
O exemplo classico é a tabela "hash" de um compilador: antes
de começar a compilação, a tabela "hash" já deve conter as
palavras reservadas da linguagem. Uma solução lenta (que em
geral é usada), é a de em uma fase de inicializaçao, colocar as
palavras reservadas na tabela "hash". Uma outra solução, bem mais
rápida e elegante é a de, na fase de geração do compilador,
preparar a tabela "hash", e por intermédio da função "mkasmtb"
e do montador, gerar um modulo objeto, que é então "link-editado"
com o compilador.

.sp
O parâmetro "tb_access" dá a área de endereçamente da tabela no
programa: no caso de "RO" e "ROL" a tabela residirá na área do texto
(que será protegida contra escritas se o programa for
"link-editado" como reentrante), e no caso de "RW" e "RWL" a tabela
residirá na área de dados do programa.
A diferença entre "RO" e "ROL" (que é a mesma entre "RW" e "RWL"),
é a de que com o "L" a tabela é gerada localmente ao programa,
e neste caso, ela deve ser concatenada com outros programas em
linguagem simbólica antes de ser montada.

.sp
Normalmente (isto é, com um ponteiro NULO para o argumento "tb_ent_desc"),
o módulo é gerado
como uma seqüência de "bytes" ("chars") sem relocação.
Se um ou mais membros da entrada da tabela
forem "words" ("shorts"), "longs" ou se
forem ponteiros (isto é, elementos relocáveis do ponto de vista
do montador/"link-editor"), temos de informar isto
à função "mkasmtb", dando um ponteiro para um vetor de
estruturas "OBJENT" (o argumento "tb_ent_desc").

.sp
Para cada membro da tabela nas condições acima corresponde
a uma entrada do vetor de estruturas "OBJENT", com os seguintes elementos:
"o_size": tamanho do membro; "o_offset": posição do membro na entrada;
"o_count": no. de exemplares deste membro (se for um vetor);
"o_rel_nm": nome da variável relocável (ponteiro NULO se for absoluto)
"o_rel_addr": endereço da variável relocadora.
Um membro com "o_size" NULO marca o final de "tb_ent_desc".

.in
.sp
.(t
.b
VALOR DE RETORNO
.r
.in 5
Normalmente, a função devolve zero. No caso de não conseguir
criar o arquivo contendo o programa em linguagem simbólica, devolve (-1).
.)t

.in
.sp
.b OBSERVAÇÕES
.in 5
Cada chamada a "mkasmtb" produz exatamente
um arquivo com um programa em linguagem simbólica,
contendo uma tabela.
Para obter um módulo objeto único contendo
várias tabelas, pode-se montar
conjuntamente os diversos arquivos gerados
por várias chamadas a "mkasmtb".

.sp
No caso de ponteiros, ponteiros NULOs são conservados, isto é,
o ponteiro NULO é convertido na constante zero, e não no valor
relocável de uma tabela menos uma constante.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cc, as, ld
.br

.in
.sp
.b ARQUIVOS
.in 5
/usr/include/mkasmtb.h

.in
.sp
.b EXEMPLOS
.in 5
.br
(Nas páginas seguintes)

.in
.sp
.(t
.b EXEMPLO
(Sem ponteiros):
.in 5
Repare que é necessário marcar o final do vetor de estruturas "OBJENT".
.sp
.nf
	#include <stddef.h>
	#include <mkasmtb.h>

	#define	NULL	0
	#define	NOSTR	(char *)NULL
	#define	NOVOID	(void *)NULL

	typedef	struct
	{
		short	a;
		short	c;

	}	HASH;

	HASH	hash[4] =
	{
		{ 0, 6 },
		{ 1, 5 },
		{ 2, 4 },
		{ 3, 3 }
	};

	const OBJENT tab[] =
	{
		{ sizeof (short), offsetof (HASH, a),
					1, NOSTR, NOVOID },
		{ sizeof (short), offsetof (HASH, c),
					1, NOSTR, NOVOID },
		{ 0 }
	};

	main ()
	{
		mkasmtb
		(	"hash.s",
			"hash",
			hash,
			sizeof hash,
			RWL, 
			sizeof (HASH),
			tab
		);
	}
.fi
.)t
.sp 2
irá gerar a tabela "hash.s":
.sp 2
.nf
		.data

	_hash:
		.word	0x0000
		.word	0x0006

		.word	0x0001
		.word	0x0005

		.word	0x0002
		.word	0x0004

		.word	0x0003
		.word	0x0003
.fi

.in
.sp
.b EXEMPLO
(Com ponteiros):
.in 5
No programa dado abaixo, cada entrada da tabela contem 2 ponteiros.
.in
.sp
.nf
	#include <stddef.h>
	#include <mkasmtb.h>

	#define NOSTR	(char *)NULL
	#define NOVOID	(void *)NULL

	typedef	struct
	{
		short	a;
		short	c;
		short	*b[2];
		long	*d;

	}	HASH;

	short	symtb[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

	long	abs[]	=
	{
		9, 8, 7, 6, 5, 4, 3, 2,
		1, 9, 8, 7, 6, 5, 4
	};

	HASH	hash[4] =
	{
		{ 0, 6, &symtb[1], &symtb[1], &abs[7] },
		{ 1, 5, &symtb[2], NOVOID,    &abs[1] },
		{ 2, 4, &symtb[3], &symtb[0], &abs[9] },
		{ 3, 3, &symtb[4], &symtb[2], &abs[0] }
	};

	const OBJENT tab[] =
	{
		{ sizeof (short),   offsetof (HASH, a),
					1, NOSTR, NOVOID },
		{ sizeof (short),   offsetof (HASH, c),
					1, NOSTR, NOVOID },
		{ sizeof (short *), offsetof (HASH, b[0]),
					2, "symtb", symtb },
		{ sizeof (long *),  offsetof (HASH, d),
					1, "abs", abs },
		{ 0 }

	};

	main ()
	{
		mkasmtb
		(	"table.s",
			"hash",
			hash,
			sizeof hash,
			RO, 
			sizeof (HASH),
			tab
		);
	}
.fi
.sp 2
irá gerar a tabela "table.s":
.sp 2
.nf
		.global	_hash
		.global	_symtb
		.global	_abs
		.const

	_hash:
		.word	0x0000
		.word	0x0006
		.long	_symtb+2
		.long	_symtb+2
		.long	_abs+28

		.word	0x0001
		.word	0x0005
		.long	_symtb+4
		.long	0
		.long	_abs+4

		.word	0x0002
		.word	0x0004
		.long	_symtb+6
		.long	_symtb
		.long	_abs+36

		.word	0x0003
		.word	0x0003
		.long	_symtb+8
		.long	_symtb+4
		.long	_abs
.fi
.in

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
