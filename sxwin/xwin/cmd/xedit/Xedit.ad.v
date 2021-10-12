*input:					TRUE

*font:					9x15

*Paned*Text*allowResize:		True

*buttons*orientation:			horizontal
*buttons*showGrip:			False
*quit.label:				Sair
*quit.background:			gray80
*save.label:				Salvar
*save.background:			gray80
*load.label:				Ler
*load.background:			gray80

*Text*foreground:			yellow
*Text*background:			gray20

*filename*preferredPaneSize:		700

*bc_label*label:			Use Control-S e Control-R para procurar
*bc_label.background:			gray50
*bc_label.showGrip:			False

*messageWindow*preferredPaneSize:	50
*messageWindow.autoFill:		True
*messageWindow.scrollVertical:		WhenNeeded

*labelWindow*justify:			center
*labelWindow*label:			nenhum arquivo
*labelWindow.background:		gray50
*labelWindow.skipAdjust:		True
*labelWindow.showGrip:			False

*editWindow*preferredPaneSize:		500
*editWindow.background:			black
*editWindow*AsciiSink*foreground:	green
*editWindow*AsciiSink*background:	black
#*editWindow.background:		palegreen
#*editWindow*AsciiSink*foreground:	black
#*editWindow*AsciiSink*background:	palegreen
*editWindow.autoFill:			False
*editWindow.scrollVertical:		True
#*editWindow.scrollVertical:		WhenNeeded
*editWindow.scrollHorizontal:		True
#*editWindow.scrollHorizontal:		WhenNeeded


!
! Keep CR in filename window from confusing the user.
!

*filename.translations:	#override			\
			<Key>Return: end-of-line()
*filename.baseTranslations:	#override		\
				<Key>Return: end-of-line()

*messageWindow.translations:	#override		\
				<Key>: end-of-file()
*messageWindow.basetranslations:	#override		\
					<Key>: end-of-file()
