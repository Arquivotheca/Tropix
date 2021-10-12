#************************************************************************
#*									*
#*				.fvwmrc					*
#*									*
#*	Configuração do Gerenciador de Janelas "fvwm"			*
#*									*
#*	Versão	1.0.0, de 17.07.97					*
#*		3.1.8, de 14.04.99					*
#*									*
#*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado		*
#*		Copyright (c) 1999 TROPIX Technologies Ltd.		*
#*									*
#************************************************************************
#
#************************************************************************
#	Botões								*
#************************************************************************
#
#	. 2:	um X, para destruir a janela
#	. 4:	triângulo para cima, para maximizar
#	. 6:	triângulo para baixo, para iconificar
#
ButtonStyle	2	13 26x29@1 34x21@1 50x35@1 70x21@1 79x29@1 63x48@0 79x65@1 70x75@0 50x61@0 34x75@0 26x65@0 44x48@1 26x29@0
ButtonStyle	4	 4 50x35@1 65x65@0 35x65@0 50x35@1
ButtonStyle	6	 4 50x65@1 35x35@1 65x35@1 50x65@0

#************************************************************************
#	Características gerais						*
#************************************************************************

EdgeResistance	0 10
WindowFont	9x15bold
IconFont	7x13bold
EdgeScroll	100 100

ModulePath	/usr/xwin/bin
PixmapPath	/usr/xwin/pixmaps/
IconPath	/usr/xwin/pixmaps/

HilightColor	white darkolivegreen
DeskTopSize	1x1
MenuStyle	midnightblue cadetblue lightblue 9x15bold fvwm
ClickTime	750

ColormapFocus	FollowsMouse

#************************************************************************
#	Alguns estilos							*
#************************************************************************

Style	"*"		BorderWidth 7, HandleWidth 7
Style	"*"		Icon unknown1.xpm, Color white/gray
Style	"*"		NoPPosition, FocusFollowsMouse
Style	"*"		MWMFunctions, MWMDecor, HintOverride
Style	"*"		DecorateTransient, IconBox 0 -10 -280 -1

Style	"Fvwm*"		NoTitle, NoHandles, Sticky, WindowListSkip
Style	"Fvwm*"		BorderWidth 0, CirculateSkipIcon

Style	"*lock"		NoButton 4, NoButton 6, NoButton 1
Style	"*lock"		Color white/darkolivegreen

Style	"XTerm"		Icon xterm.xpm, IconBox -70 1 -1 -140

Style	"SysMap"	NoHandles, NoButton 4, NoButton 6, NoButton 1
Style	"SysMap"	Color white/darkolivegreen

#************************************************************************
#	Cliques do Mouse						*
#************************************************************************

Mouse 0		1    	A      	Menu Window-Ops2 Close
Mouse 0		2	A	Close
Mouse 0		4    	A     	Maximize-Func
Mouse 0		6    	A     	Iconify

Mouse 1		R   	A       Menu Utilities Nop
Mouse 1		F	A	Resize-or-Raise
Mouse 1		TS	A	Move-or-Raise
Mouse 1		I	A	Move-or-Iconify

Mouse 2		R    	A      	Menu Window-Ops Nop
Mouse 2		I	A	Iconify
Mouse 2		FST	A	Menu Window-Ops2 Nop

Mouse 3		R    	A      	WindowList
Mouse 3		TSF	A	RaiseLower
Mouse 3		I	A	Iconify

#************************************************************************
#	Inicialização/Reinicialização					*
#************************************************************************

AddToFunc InitFunction		"I" Exec xpaint -R /usr/xwin/gifmaps/tropix.gif
+				"I" Exec xterm -C -t Console -g 80x10+0+0 -b black -f green
+				"I" Exec xterm -t xterm -g 80x24+0+180 -f white -b darkblue
+				"I" Exec xclock -g 170x170+600+0 -b darkgreen -f black

#************************************************************************
#	Menus								*
#************************************************************************

AddToMenu Utilities	"Utilitários"	Title 	
+	"xterm%mini-term.xpm%"		Exec xterm
+	"xclock%mini-clock.xpm%"	Exec xclock
+	"sysmap%mini-perf.xpm%"		Exec sysmap -b mediumspringgreen -f black
+	"xcoremap%mini-perf.xpm%"	Exec xcoremap
+	"xedit%mini-perf.xpm%"		Exec xedit
+	""				Nop
+      	"Reiniciar o Fvwm%mini-turn.xpm%" Restart fvwm	
+	"Sair%shutdown.xpm%"		Popup Quit-Verify

AddToMenu Window-Ops	"Window Ops"	Title
+	"Mover"				Move-or-Raise2
+	"Redimensionar"			Resize-or-Raise2
+	"Promover"			Raise
+	"Rebaixar"			Lower 
+ 	"(De)Iconificar"		Iconify
+	"(Des)Vincular"			Stick		
+	"(Min/Max)imizar"		maximize_func
+	""				Nop  
+	"Redesenhar"			Refresh		

AddToMenu Window-Ops2	"Mover"		Move-or-Raise
+		"Redimensionar"		Resize-or-Raise
+		"Promover"		Raise   	
+		"Rebaixar"		Lower   	
+	 	"(De)Iconificar"	Iconify
+		"(Des)Vincular"		Stick		
+		"Fechar"		Close

AddToMenu Quit-Verify	"Quer realmente sair?"	Title
+	      	"Sim, quero%mini-exclam.xpm%"	Quit	
+	      	"Não, não quero"		Nop	

#************************************************************************
#	Funções								*
#************************************************************************

AddToFunc Move-or-Raise		"I" Raise
+				"M" Move
+  				"D" Lower

AddToFunc Move-or-Raise2	"M" Raise
+				"M" Move
+  				"D" Lower

AddToFunc Maximize-Func		"M" Maximize	 0 100
+				"C" Maximize	 0 80
+				"D" Maximize	 100 100

AddToFunc Move-or-Iconify	"I" Raise
+				"M" Move
+				"D" Iconify

AddToFunc Resize-or-Raise	"I" Raise
+				"M" Resize
+				"D" Lower

AddToFunc Resize-or-Raise2	"M" Raise
+				"M" Resize
+				"D" Lower

AddToFunc Iconify-and-Raise     "I" Iconify

#************************************************************************
#	Paginador							*
#************************************************************************
#
Module FvwmPager 0 5
#
*FvwmPagerGeometry	800x80+0-0
#
*FvwmPagerBack		red4
*FvwmPagerHilight	red1
*FvwmPagerFore		white
#
*FvwmPagerFont		7x13
*FvwmPagerSmallFont	6x9
