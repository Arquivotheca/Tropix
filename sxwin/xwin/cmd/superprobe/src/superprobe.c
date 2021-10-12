/*
 ****************************************************************
 *								*
 *			superprobe.c				*
 *								*
 *	Descobre as características da placa gráfica		*
 *								*
 *	Versão	1.0.0, de 10.01.97				*
 *		4.2.0, de 14.10.01				*
 *								*
 *	Módulo: SuperProbe					*
 *		Baseado no software homônimo de David Wexelblat	*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

#include "../h/probe.h"

/*
 ****************************************************************
 *	Nome e Versão do programa				*
 ****************************************************************
 */
const char	pgversion[]	=	"Versão 4.2.0, de 14.10.01";

/*
 ****************************************************************
 *	Variáveis						*
 ****************************************************************
 */
entry Word	vgaIOBase;
entry Bool	vflag = FALSE;
entry Long 	Chip_data = (Long)~0;
entry Byte	*Bios_Base = (Byte *)0;
entry Bool	NoBIOS = FALSE;

static Bool	No16Bits = FALSE;
static Bool	Exclusions = FALSE;
static Bool	Mask10 = FALSE;
static Range	*Excl_List = NULL;
static char	*NoProbe = (char *)0;

/*
 ****************************************************************
 *	Chipsets suportados					*
 ****************************************************************
 */
static Chip_Descriptor *SVGA_Descriptors[] =
{
	&WD_Descriptor,
	&Video7_Descriptor,
	&MX_Descriptor,
	&Genoa_Descriptor,
	&UMC_Descriptor,
	&Trident_Descriptor,
	&ATI_Descriptor,
	&Ahead_Descriptor,
	&NCR_Descriptor,
	&S3_Descriptor,
	&AL_Descriptor,
	&Cirrus54_Descriptor,
	&Cirrus64_Descriptor,
	&Tseng_Descriptor,
	&RealTek_Descriptor,
	&Primus_Descriptor,
	&Yamaha_Descriptor,
	&Oak_Descriptor,
	&Cirrus_Descriptor,
	&Compaq_Descriptor,
	&HMC_Descriptor,
	&Weitek_Descriptor,
	&CT_Descriptor, /* I think this is screwing people up, so put it last */
	NULL
};

/*
 ****************************************************************
 *	Coprocessadores gráficos suportados			*
 ****************************************************************
 */
static Chip_Descriptor *CoProc_Descriptors[] =
{
	&ATIMach_Descriptor,
	&IBM8514_Descriptor,	/* Make this the last 8514-type entry */
	&I128_Descriptor,
	NULL
};

/*
 ****************************************************************
 *	Protótipos de funções					*
 ****************************************************************
 */
static int	num_tokens (const char *, char);
static void	ParseExclusionList (Range *, const char *);
static Bool	TestChip (Chip_Descriptor *, int *);
static void	print_info (void);
static Byte	*FindBios (void);
static void	help (int);

/*
 ****************************************************************
 *	Conta os tokens em uma cadeia				*
 ****************************************************************
 */
static int
num_tokens (const char *list, char delim)
{
	const char	*p;
	int		cnt;

	for (cnt = 1, p = list; (p = strchr (p, delim)) != NULL; p++, cnt++)
		/* vazio */;

	return (cnt);

}	/* end num_tokens */

/*
 ****************************************************************
 *	Analisa uma lista com exclusões				*
 ****************************************************************
 */
static void
ParseExclusionList (Range *excl_list, const char *list)
{
	const char	*p = list;
	char		*p1, c;
	Bool		done = FALSE, lo = TRUE;
	int		i = 0;

    while (!done)
    {
    	p1 = strpbrk(p, "-,");
    	if (p1)
    	{
    	    c = *p1;
    	    *p1 = '\0';
    	    if ((c ==  '-') && (p == p1))
    	    {
    	    	error ("$Unbounded range in exclusion");
    	    }
    	    else if (p == p1)
    	    {
    	    	p++;
    	    	continue;
    	    }
    	}
    	else
    	{
    	    if ((!lo) && (p == p1))
    	    {
    	    	error ("$Unbounded range in exclusion");
    	    }
    	    done = TRUE;
    	}
    	if (lo)
    	{
    	    excl_list[i].lo = (Word)StrToUL(p);
    	}
    	else
    	{
    	    excl_list[i].hi = (Word)StrToUL(p);
    	}
    	if (!done)
    	{
    	    if (c == '-')
    	    {
    	    	lo = FALSE;
    	    }
    	    else
    	    {
    	    	if (lo)
    	    	{
    	    	    excl_list[i].hi = (Word)-1;
    	    	}
    	    	else
    	    	{
    	    	    lo = TRUE;
    	    	}
    	    	i++;
    	    }
    	    p = p1 + 1;
    	}
    	else
    	{
    	    if (lo)
    	    {
    	    	excl_list[i].hi = (Word)-1;
    	    }
    	    i++;
    	}
    }
    excl_list[i].lo = (Word)-1;
}

/*
 ****************************************************************
 *	Tenta um determinado tipo de placa			*
 ****************************************************************
 */
static Bool
TestChip (Chip_Descriptor *chip_p, int *Chipset)
{
	char	*p, *p1, name[64];

	if (No16Bits && chip_p->bit16)
	{
		if (vflag)
		{
			printf ("\tIgnorando %s (registros de 16-bits)...\n", chip_p->name);
			fflush (stdout);
		}

		return (FALSE);
	}

	if (NoBIOS && chip_p->uses_bios)
	{
		if (vflag)
		{
			printf ("\tIgnorando %s (necessita leitura da BIOS)...\n", chip_p->name);
			fflush (stdout);
		}

		return (FALSE);
	}

	if (Exclusions && Excluded (Excl_List, chip_p, Mask10))
	{
		if (vflag)
		{
		    	printf ("\tIgnorando %s (na lista de exclusão)...\n", chip_p->name);
		    	fflush (stdout);
		}

		return (FALSE);
	}

	if (NoProbe != (char *)0)
	{
		p1 = p = NoProbe;

	    	while (p)
	    	{
		    	p1 = strchr (p1, ',');
			if (p1 != NULL)
			{
		    	    	(void)strncpy (name, p, (p1 - p));
				name[p1 - p] = '\0';
				p1++;
			}
			else
			{
				(void)strcpy (name, p);
			}

			if (StrCaseCmp (name, chip_p->name) == 0)
			{
				if (vflag)
		    		{
			    	    	printf ("\tIgnorando %s (noprobe list)...\n", chip_p->name);
			    	    	fflush (stdout);
		    		}

		    		return (FALSE);
			}
		    	p = p1;
		}
	}

	if (vflag)
	{
		printf ("\tTentando %s...\n", chip_p->name);
		fflush (stdout);
	}

	if (chip_p->f (Chipset))
	    	return (TRUE);

	return (FALSE);

}	/* end TestChip */

/*
 ****************************************************************
 *	Imprime os dispositivos suportados			*
 ****************************************************************
 */
static void
print_info (void)
{
	Chip_Descriptor	*chip_p;
	int		i, len;

	printf
	(	"Modos de vídeo suportados:\n"
		"\tMDA, Hercules, CGA, MCGA, EGA, VGA\n"
	);

	printf ("\nChipsets suportados:\n", pgname);

	len = 0;

	putchar ('\t');

	for (i=0; SVGA_Descriptors[i] != NULL; i++)
	{
		chip_p = SVGA_Descriptors[i];
		len += strlen (chip_p->name) + 2;

		if (len > 70)
		{
			printf ("\n\t");
			len = strlen (chip_p->name) + 2;
		}

	        printf ("%s, ", chip_p->name);
	}

	printf ("\n\nCoprocessadores gráficos suportados:\n"); 

	len = 0;

	putchar ('\t');

	for (i=0; CoProc_Descriptors[i] != NULL; i++)
	{
		chip_p = CoProc_Descriptors[i];
		len += strlen (chip_p->name) + 2;

		if (len > 70)
		{
			printf ("\n\t");
			len = strlen (chip_p->name) + 2;
		}

		printf ("%s, ", chip_p->name);
	}

	printf ("\n\nRAMDACs suportados:\n", pgname);

	len = 0;

	putchar ('\t');

	for (i=0; i < DAC_MAX; i++)
	{
		len += strlen (RamDac_Names[i+1].Short) + 2;
		if (len > 70)
		{
			printf ("\n\t");
			len = strlen (RamDac_Names[i+1].Short) + 2;
		}

	        printf("%s, ", RamDac_Names[i+1].Short);
	}

	putchar ('\n');

	exit (0);

}	/* end print_info */

/*
 ****************************************************************
 *	Acha o endereço do BIOS da placa gráfica		*
 ****************************************************************
 */
static Byte
*FindBios (void)
{
	int	i, score[7];
	Byte	buf[3];
	Byte	*base = (Byte *)0;

	for (i = 0; i < 7; i++)
		score[i] = 1;

	for (i = 0; i < 7; i++)
	{
		buf[0] = buf[1] = buf[2] = (Byte)0;
		if (ReadBIOS ((unsigned)(0xC0000 + (i << 15)), buf, 3) != 3)
			score[i] = 0;
		else if ((buf[0] != 0x55) || (buf[1] != 0xAA) || (buf[2] < 48))
			score[i] = 0;
	}

	for (i = 6; i >= 0; i--)
	{
		if (score[i] != 0)
			base = (Byte *)(0xC0000 + (i << 15));
	}

	return (base);
}

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	char		*p, *order = NULL;
	char		copyright[3];
	Word		Ports[10];		/* For whatever we need */
	int		Primary = -1;
	int		Secondary = -1;
	int		RamDac = -1;
	int		CoProc = -1;
	int		MemVGA = -1;
	int		MemCoProc = -1;
	int		i, cnt, opt;
	Chip_Descriptor	*chip_p, *matched;
	Bool		Probe_DAC = TRUE;
	Bool		Probe_Mem = TRUE;
	Bool		NoEGA = FALSE;
	Bool		Check_CoProc;
	Bool		flag;

	while ((opt = getopt (argc, argv, "vkwbdmiHB:p:o:x:")) != EOF)
	{
		switch (opt)
		{
		    case 'v':
			vflag++;
			break;

		    case 'k':
			Mask10++;
			break;

		    case 'w':
			No16Bits++;
			break;

		    case 'b':
			NoBIOS++;
			break;

		    case 'd':
			Probe_DAC = 0;
			break;

		    case 'm':
			Probe_Mem = 0;
			break;

		    case 'i':
			print_info ();

		    case 'H':
			help (0);

		    case 'x':
			if (Excl_List != NULL)
				error ("Lista da opção '-x' duplicada");

		    	cnt = num_tokens (optarg, ',');

		    	Excl_List = (Range *)calloc (cnt + 1, sizeof (Range));
			if (Excl_List == (Range *)NULL)
				error ("$Memória insuficiente");

		    	ParseExclusionList (Excl_List, optarg);
		    	Exclusions = TRUE;
			break;

		    case 'o':
			if (order != NULL)
				error ("Lista da opção '-o' duplicada");

			if ((order = strdup (optarg)) == NULL)
				error ("$Memória insuficiente");
			break;

		    case 'p':
			if (NoProbe != NULL)
				error ("Lista da opção '-p' duplicada");

			if ((NoProbe = strdup (optarg)) == NULL)
				error ("$Memória insuficiente");
			break;

		    case 'B':
			Bios_Base = (Byte *)StrToUL (optarg);
			break;

		    default:
			fprintf
			(	stderr,
				"%s: Opção '-%c' inválida\n",
				pgname, opt
			);
			help (1);
		}
	}

#if (0)	/***********************************************************/
{
    FILE *f;

    f = fopen("/dev/tty", "w");
#ifdef __EMX__
    /* note: don't remove the fopen above ! */
    fclose(f);
    f = stderr;	/* OS/2 does not know /dev/tty, OTOH you cannot run this
                 * program in a window.
		 */
#endif
    if (f != (FILE *)NULL)
    {
        putc('\007', f);
        fprintf(f, "ATENÇÃO:  ESTE PROGRAMA PODE TRAVAR A MÁQUINA\n");
	fprintf(f, "          VOCÊ TEM 5 SEGUNDOS PARA DESISTIR...\n");
        fflush(f);
        sleep(5);
    }
}
#endif	/***********************************************************/

    if (enable_user_io () < 0)
    {
	fprintf (stderr, "%s: não posso ter acesso às portas de E/S\n", pgname);
	exit (-1);
    }

    if (!NoBIOS)
    {
	if (Bios_Base == (Byte *)0)
	{
	    Bios_Base = FindBios ();
	    if (Bios_Base == (Byte *)0)
	    {
		if (vflag)
		{
			fprintf
			(	stderr,
				"%s: não localizei o BIOS da placa gráfica",
				pgname
			);

		    fflush(stdout);
		}
		NoEGA = TRUE;
	    }
	}
    }

    if (vflag)
    {
	if (NoBIOS)
		printf
		(	"*** BIOS da placa gráfica não procurado.\n"
			"Assumido padrão EGA/VGA\n\n"
		);
	else
		printf
		(	"*** BIOS da placa gráfica no endereço 0x%X\n\n",
			(int)Bios_Base
		);

	fflush (stdout);
    }

    if ((!NoBIOS) && (!NoEGA))
    {
        /*
         * Look for 'IBM' at Bios_Base+0x1E of the BIOS.  It will be there for
         * an EGA or VGA.
         */
        if (ReadBIOS (COPYRIGHT_BASE, copyright, 3) < 0)
        {
    	    fprintf(stderr, "%s: Failed to read BIOS\n", pgname);
    	    disable_user_io ();
    	    return(1);
        }
    }

    if (!NoEGA && (NoBIOS || strncmp (copyright, "IBM", 3) == 0))
    {
    	/*
    	 * It's an EGA or VGA
    	 */
    	Ports[0] = 0x3CC;
    	EnableIOPorts(1, Ports);
    	vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
	DisableIOPorts(1, Ports);
    	if (vgaIOBase == 0x3D0)
    	{
    	    /*
	     * Color - probe for secondary mono.
    	     */
    	    if (Probe_MDA(&Secondary))
    	    {
    	    	(void)Probe_Herc(&Secondary);
    	    }
    	}
    	else
    	{
    	    /*
    	     * Mono - probe for secondary color.
    	     */
    	    (void)Probe_CGA(&Secondary);
    	}
    
    	if (!Probe_VGA(&Primary))
    	    Primary = CHIP_EGA;
    
    	/*
     	 * Do SVGA probing, even if no VGA is detected.  Some SVGA can emulate
	 * pre-VGA adapters (such as EGA) and be left in such a state by BIOS
	 * initialization.
     	 */
	if (vflag)
	{
	    printf ("*** Descobrindo placas Super-VGA...\n");
	    fflush(stdout);
	}
	matched = &VGA_Descriptor;

	if (order == NULL)
	{
	    /* 
	     * Use default ordering.
	     */
	    for (i=0; SVGA_Descriptors[i] != NULL; i++)
	    {
		chip_p = SVGA_Descriptors[i];
		if (TestChip (chip_p, &Primary))
		{
		    matched = chip_p;
		    break;
		}
	    }
	}
	else
	{
	    /*
	     * Use user specified order
	     */
	    p = order;
	    while (p)
	    {
		order = strchr(order, ',');
		if (order != NULL)
		{
		    *order = '\0';
		    order++;
		}
		flag = FALSE;
		for (i=0; SVGA_Descriptors[i] != NULL; i++)
		{
		    chip_p = SVGA_Descriptors[i];
		    if (StrCaseCmp (p, chip_p->name) == 0)
		    {
			flag = TRUE;
			break;
		    }
		}
		if (flag)
		{
		    if (TestChip (chip_p, &Primary))
		    {
			matched = chip_p;
			break;
		    }
		}
		else
		{
		    fprintf(stderr, "%s: Chip class '%s' not known\n",
			pgname, p);
		}
		p = order;
	    }
	}

	Check_CoProc = matched->check_coproc;

	if (Probe_Mem)
	{
	    if (matched->memcheck != (MemCheckFunc)NULL)
	    {
		MemVGA = (*matched->memcheck)(Primary);
	    }
	    else if (vflag)
	    {
		printf("Memory probe not supported for this chipset.\n");
		fflush(stdout);
	    }
	}
	else if (vflag)
	{
	    printf("Skipping memory probe\n");
	    fflush(stdout);
	}

	/*
	 * If this chipset doesn't exclude probing for a coprocessor,
	 * then look for one.
	 */
	if (Check_CoProc)
	{
	    if (vflag)
	    {
		printf("\n*** Descobrindo o coprocessador gráfico...\n");
		fflush(stdout);
	    }
    	    for (i=0; CoProc_Descriptors[i] != NULL; i++)
    	    {
    	        chip_p = CoProc_Descriptors[i];
    	        if (TestChip (chip_p, &CoProc))
    	        {
		    matched = chip_p;
    	    	    break;
    	    	}
	    }
	    if (CoProc_Descriptors[i] != NULL)
	    {
	    	if (Probe_Mem)
	    	{
	    	    if (matched->memcheck != (MemCheckFunc)NULL)
	    	    {
		        MemCoProc = (*matched->memcheck)(CoProc);
	    	    }
		    else if (vflag)
		    {
		        printf("Memory probe not supported ");
			printf("for this chipset.\n");
		        fflush(stdout);
		    }
	        }
	        else if (vflag)
	        {
		    printf("Skipping memory probe\n");
		    fflush(stdout);
	        }
	    }
	}
    }
    else if (Probe_MDA(&Primary))
    {
    	(void)Probe_Herc(&Primary);
    	(void)Probe_CGA(&Secondary);
    }
    else if (Probe_CGA(&Primary))
    {
    	if (Probe_MDA(&Secondary))
    	{
    	    (void)Probe_Herc(&Secondary);
    	}
    }

    putchar('\n');
    if (Primary == -1)
    {
    	printf("Could not identify any video\n");
    }
    else 
    {
    	printf ("Vídeo primário: ");
    	if (IS_MDA(Primary))
    	{
    	    printf("MDA\n");
    	}
    	else if (IS_HERC(Primary))
    	{
    	    printf("Hercules\n");
    	    Print_Herc_Name(Primary);
    	}
    	else if (IS_CGA(Primary))
    	{
    	    printf("CGA\n");
    	}
    	else if (IS_MCGA(Primary))
    	{
    	    printf("MCGA\n");
    	}
    	else if (IS_EGA(Primary))
    	{
    	    printf("EGA\n");
    	}
    	else if (IS_VGA(Primary))
    	{
    	    printf ("VGA Genérica (ou SVGA desconhecida)\n");
    	}
    	else if (IS_SVGA(Primary))
    	{
    	    printf("Super-VGA\n");
    	    Print_SVGA_Name(Primary);
	    if (MemVGA > -1)
		printf("\tMemória: %d Kbytes\n", MemVGA);

	    if (Probe_DAC)
	    {
    	        Probe_RamDac(Primary, &RamDac);
    	        Print_RamDac_Name(RamDac);
	    }
	    else if (vflag)
	    {
		printf("Skipping RAMDAC probe\n");
	    }
    	}

	if (CoProc != -1)
	{
	    printf("\tAttached graphics coprocessor:\n");
	    Print_CoProc_Name(CoProc);
	    if (MemCoProc > -1)
		printf("\t\tMemória: %d Kbytes\n", MemCoProc);
	}
    }

    if (Secondary != -1)
    {
    	printf ("Vídeo secundário: ");
    	if (IS_MDA(Secondary))
    	{
    	    printf("MDA\n");
    	}
    	else if (IS_HERC(Secondary))
    	{
    	    printf("Hercules\n");
    	    Print_Herc_Name(Secondary);
    	}
    	else if (IS_CGA(Secondary))
    	{
    	    printf("CGA\n");
    	}
    	else if (IS_MCGA(Secondary))
    	{
    	    printf("MCGA\n");
    	}
    }

    disable_user_io ();
    return(0);
}

/*
 ****************************************************************
 *	Ensina a usar o programa				*
 ****************************************************************
 */
void
help (int exitval)
{
	fprintf
	(	stderr,
		"%s - descobre características da placa gráfica\n\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vikwbdm] [-B <bios>] [-p <lista>] [-o <lista>] [-x <lista>]",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Modo verboso\n"
		"\t-i: Imprime os dispositivos suportados\n"
		"\t-k: Considera apenas os 10 bits menos significativos\n"
		"\t    dos endereços de portas antes de compará-los\n"
		"\t    com a lista da opção '-x'\n"
		"\t-w: Ignora testes que acessem portas de 16 bits\n"
		"\t-b: Não tenta localizar a BIOS da placa gráfica (assume EGA/VGA)\n"
		"\t-d: Não tenta descobrir o RAMDAC\n"
		"\t-m: Não tenta descobrir a quantidade de memória de vídeo\n"
		"\n"
		"\t-B: Estabelece <bios> como endereço da BIOS gráfica\n"
		"\t-p: Não tenta os chipsets mencionados em <lista>\n"
		"\t-o: Tenta os chipsets na ordem em que são mencionados em <lista>\n"
		"\t-x: Ignora testes que acessem portas mencionados em <lista>\n"
	);

	exit (exitval);

}	/* end help */
