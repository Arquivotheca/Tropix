======================================================================
                   FVWM Frequently Asked Questions
======================================================================

INDEX

        1) What does FVWM stand for?
        2) Where do I find the current versions of fvwm?
        3) Any WWW Sites about FVWM?
        4) Where do I ask questions about FVWM?
        5) What are the differences between FVWM 1.xx and 2.xx?
        6) My .fvwmrc from version 1.xx no longer works.  What do I do?
        7) I have a window that is behaving unexpectedly under FVWM, but just
           fine under (whatever other window manager), or I have just some
           random bug.  What do I do?
        8) What's the relative memory usage for the various window managers
           out there?
        9) I used to use GoodStuff in FVWM 1.xx, but it's not in the 2.xx
           distribution.  What do I use now?
       10) I'm using FvwmButtons (or GoodStuff in 1.xx), and sometimes the
           buttons stay depressed, and other times they don't.  Why is that?
       11) What happened to the FVWM 1.xx 'include' command?
       12) I really like the horizontal bars that appear on the title bars of
           sticky windows.  Can I get those on other windows as well?
       13) I'm trying to build FVWM, but the linker is complaining
           about missing functions like 'XpmReadFileToPixmap' or the
           compiler can't find the include file X11/xpm.h.  Why is
           that?
       14) I'm trying to compile FVWM under SunOS using cc, but the compiler
           is having lots of problems.  What gives?
       15) Why does the screen go blank at startup and during Recaptures now
           when it didn't in FVWM 1.xx?
       16) I'd really like to see neat feature XYZ in FVWM.
           Wouldn't that be cool?  I even have a patch.  When can it
           be added?
       17) I know this question doesn't have to do with FVWM, but what
           happened to to rxvt and rclock which Rob Nation used to
           support?  Where can I find them now?
       18) Where can I get more XPMs for icons?
       19) How do I set the Sun keyboard key xxxx to an FVWM command?
           Or more generally, I'm having problems defining key
           bindings for FVWM - what can I do?
       20) I clicked on a program's Motif menubar and the menu pops up
           then dissappears.  What gives?
       21) I heard about this FvwmFileMgr module.  Where can I find it?
       22) I'd really like {NeXT, Win95, etc} look and feel.  Are you
           going to support that?
       23) Imake didn't come with my machine.  How do I build FVWM?
       24) How do I create/submit/apply patches?
       25) I'd like to bind a key to paste the current selection, how
           can I do that?
       26) You can bind mouse movements to keystrokes, how about mouse
           presses?

======================================================================


1) What does FVWM stand for?

A: "Fill_in_the_blank_with_whatever_f_word_you_like_at_the_time
   Virtual Window Manager".  Rob Nation (the original Author of FVWM)
   doesn't really remember what the F stood for origninally, so we
   have several potential answers:

     Feeble, Fabulous, Famous, Fast, Foobar, Fantastic, Flexible,
     F!@#$%, FVWM (the GNU recursive approach), Free, Final, Funky,
     Fred's (who the heck is Fred?), Freakin', etc.

   Just pick your favorite (which will of course change depending on
   your mood and whether or not you've run across any bugs recently).
   I prefer Fabulous or Fantastic myself.

----------------------------------------------------------------------

2) Where do I find the current versions of fvwm?

A: New main distribution site (official + beta code):

                ftp://ftp.hpc.uh.edu/pub/fvwm

   Alternate sites:

            USA:
                ftp://sunsite.unc.edu/pub/Linux/X11/window-managers/
                ftp://ftp.cs.columbia.edu/pub/cheah/

            Overseas:
                ftp://unix.hensa.ac.uk/pub/walnut.creek/FreeBSD/incoming/
                ftp://ftp.jussieu.fr/pub/X11/fvwm/
                ftp://ftp.loria.fr/pub/unix/fvwm/

   BTW, 1.24r was the last released version of the 1.xx series and the
   first official release of 2.xx will be 2.1.0.

----------------------------------------------------------------------

3) Any WWW Sites about FVWM?

A: Yup.  The official site is currently:

        http://www.hpc.uh.edu/fvwm/

   And there are others.  Try the following (note - the first one has
   changed recently):

        http://www.cs.hmc.edu/~tkelly/docs/proj/fvwm.html
        http://www.ssc.com/~roland/fvwm/fvwm.html
        http://xenon.chem.uidaho.edu/~fvwm/
        http://namu19.gwdg.de/fvwm/fvwm.html
        http://mars.superlink.net/user/ekahler/fvwm.html

   There are pointers to all of these (and maybe more) from the
   official fvwm WWW site.

   Here's an FVWM related one, for 'The Dotfile Generator' which can
   be used to create and customize .fvwmrc files (and .emacs and
   .tcshrc files too).  Some people have found it a handy tool:

        http://www.imada.ou.dk/~blackie/dotfile/

   Here' another related one for TKGoodStuff, which can be compiled to
   be an FVWM module (and is based on the idea of GoodStuff or
   FvwmButtons) but can be used with any window manager:

        http://www-personal.umich.edu/~markcrim/tkgoodstuff/

----------------------------------------------------------------------

4) Where do I ask questions about FVWM?

A: If your local FVWM maintainer can't help you, then the newsgroup
   comp.windows.x.apps is a good place, but the fvwm mailing list is
   usually better.  The new FVWM discussion mailing list address is:

                           fvwm@hpc.uh.edu

   And there is an announce mailing list as well:

                       fvwm-announce@hpc.uh.edu

   They are maintained by Jason Tibbitts, and are Majordomo based
   mailing lists.  To subscribe to the list, send "subscribe fvwm" in
   the body of a message to majordomo@hpc.uh.edu.  To unsubscribe from
   the list, send "unsubscribe fvwm" in the body of a message to
   majordomo@hpc.uh.edu.  To report problems, send mail to
   fvwm-owner@hpc.uh.edu.

   Here's the list descriptions and instructions directly from Jason:

   =================================================================
   (fvwm)

   Description
   -----------
   
   This list is for discussion relating to the FVWM window manager,
   which is intended to have a small memory footprint and a rich
   feature set, be extremely customizable and extendable and have a
   high degree of Motif mwm compatibility.  All relevant discussion is
   encouraged.  Posting of binaries or irrelevant discussion is
   strongly frowned upon.  Posting of short patches (less than 200
   lines) is acceptable, but all patches should be sent to the FVWM
   maintainer for inspection and possible inclusion in the
   distribution.  The current maintainer is
   
   Charles Hines <chuck_hines@VNET.IBM.COM>
   
   
   Instructions
   ------------
   
   To send a message to the list, mail to fvwm@hpc.uh.edu.  For help
   on dealing with the mailing list software (for getting the archives
   or other files which may be made available) send the message "help"
   to majordomo@hpc.uh.edu.  If you have problems with the mailing
   list itself and need to talk to a human, send mail to
   fvwm-owner@hpc.uh.edu.  This address is for mailing list issues
   only; reports on the FVWM software will be sent to /dev/null.
   
   Archives of previous messages are available; to get them send the
   above mentioned help message and investigate the "index" and "get"
   commands or ftp them from ftp.hpc.uh.edu in /pub/fvwm/list-archive.
   
   
   (fvwm-announce)
   
   This list is for announcements relating to the FVWM window manager,
   which is intended to have a small memory footprint and a rich
   feature set, be extremely customizable and extendable and have a
   high degree of Motif mwm compatibility.
   
   Replies to messages from this list are set by default to go the
   FVWM discussion list, fvwm@hpc.uh.edu.
   
   The current FVWM distribution is available for ftp at
   ftp.hpc.uh.edu in /pub/fvwm.
   =================================================================

----------------------------------------------------------------------

5) What are the differences between FVWM 1.xx and 2.xx?

A: A lot.  To name a few general ones:
        - Bugfixes.  1.xx is not worked on at all any more.
        - Better rc file format.  No longer order dependent.
        - More flexible and powerful.  For example, many previously
          global options now operate on a per window group level
          instead.
        - More and better modules.
        - OpaqueResize is gone (fear not, it may come back).
        - M4 preprocessing is no longer part of the fvwm exec, but
          rather has been moved to a module.  There is also a module
          to use cpp too.  See the FvwmM4 and FvwmCpp man pages.

----------------------------------------------------------------------

6) My .fvwmrc from version 1.xx no longer works.  What do I do?

A: Start with a new one or convert your old one.  This can be done by
   hand or with a little help from the 'conversion_script' in the
   sample.fvwmrc directory.

   And here is a list of rc file command changes compiled by Makoto
   'MAR_kun' MATSUSHITA <matusita@ics.es.osaka-u.ac.jp>.  It may or
   may not be 100% accurate or complete, especially as changes evolve,
   but it's a good start.

   ** Fvwm-1.xx commands **                   ** Fvwm-2.xx equiv **
   
   AppsBackingStore                           (obsoleted)
   AutoRaise delay                            (obsoleted, use FvwmAuto)
   BackingStore                               (obsoleted)
   BoundaryWidth Width                        Style (BorderWidth width)
   ButtonStyle button# WidthxHeight           <-
   CenterOnCirculate                          (obsoleted)
   CirculateSkip windowname                   Style (CirculateSkip)
   CirculateSkipIcons                         Style (CirculateSkipIcon)
   ClickTime delay                            <-
   ClickToFocus                               Style (ClickToFocus)
   Cursor  cursor_num cursor_type             (obsoleted)
   DecorateTransients                         Style (DecorateTransient)
   DeskTopScale Scale                         (obsoleted, use FvwmPager)
   DeskTopSize HorizontalxVertical            <-
   DontMoveOff                                (obsoleted)
   EdgeResistance scrolling moving            <-
   EdgeScroll horizontal vertical             <-
   Font fontname                              MenuStyle (arg4)
   Function FunctionName                      AddToFunc (not compatible)
   HiBackColor colorname                      HilightColor (arg1)
   HiForeColor colorname                      HilightColor (arg2)
   Icon windowname bitmap-file                Style (Icon iconname-file)
   IconBox left top right bottom              Style (IconBox l t r b)
   IconFont fontname                          <-
   IconPath path                              <-
   Key keyname Context Modifiers Function     <-
   Lenience                                   Style (Lenience)
   MenuBackColor colorname                    MenuStyle (arg2)
   MenuForeColor colorname                    MenuStyle (arg1)
   MenuStippleColor colorname                 MenuStyle (arg3)
   Module ModuleName                          <-
   ModulePath path                            <-
   Mouse Button Context Modifiers Function    <-
   MWMBorders                                 Style (MWMBorder)
   MWMButtons                                 Style (MWMButtons)
   MWMDecorHints                              Style (MWMDecor)
   MWMFunctionHints                           Style (MWMFunctions)
   MWMHintOverride                            Style (HintOverride)
   MWMMenus                                   MenuStyle (arg5)
   NoBorder windowname                        Style (NoBorder)
   NoBoundaryWidth Width                      Style (HandleWidth width)
   NoPPosition                                Style (NoPPosition)
   NoTitle windowname                         Style (NoTitle)
   OpaqueMove percentage                      OpaqueMoveSize percentage
   OpaqueResize                               (obsoleted, use FvwmPager)
   Pager  X_Location Y_Location               (obsoleted, use FvwmPager)
   PagerForeColor colorname                   (obsoleted, use FvwmPager)
   PagerBackColor colorname                   (obsoleted, use FvwmPager)
   PagerFont fontname                         (obsoleted, use FvwmPager)
   PagingDefault pagingdefaultvalue           (obsoleted)
   PixmapPath path                            <-
   Popup PopupName                            AddToMenu (not compatible)
   RandomPlacement                            Style (RandomPlacement)
   SaveUnders                                 (obsoleted)
   SloppyFocus                                Style (SloppyFocus)
   SmartPlacement                             Style (SmartPlacement)
   StartsOnDesk windowname desk-number        Style (StartsOnDesk desk-number)
   StaysOnTop windowname                      Style (StaysOnTop)
   StdBackColor colorname                     Style (BackColor color)
   StdForeColor colorname                     Style (ForeColor color)
   StickyBackColor colorname                  (obsoleted)
   StickyForeColor colorname                  (obsoleted)
   Sticky windowname                          Style (Sticky)
   StickyIcons                                Style (StickyIcon)
   StubbornIcons                              (obsoleted)
   StubbornIconPlacement                      (obsoleted)
   StubbornPlacement                          (obsoleted)
   Style windowname options                   <-
   SuppressIcons                              Style (NoIcon)
   WindowFont fontname                        <-
   WindowListSkip windowname                  Style (WindowListSkip)
   XORvalue number                            <-
                                              
   ** fvwm-1 built-in functions ***           
                                              
   Beep                                       <-
   CirculateDown [ name window_name ]         Next (not compatible)
   CirculateUp [ name window_name ]           Prev (not compatible)
   Close                                      <-
   CursorMove horizonal vertical              <-
   Delete                                     <-
   Desk arg1 arg2                             <-
   Destroy                                    <-
   Exec name command                          <-
   Focus                                      <-
   Function                                   <-
   GotoPage  x y                              <-
   Iconify [ value ]                          <-
   Lower                                      <-
   Maximize [  horizontal vertical ]          <-
   Module name ModuleName                     Module ModuleName
   Move [ x y ]                               <-
   Nop                                        <-
   Popup                                      <-
   Quit                                       <-
   Raise                                      <-
   RaiseLower                                 <-
   Refresh                                    <-
   Resize [ x y ]                             <-
   Restart  name WindowManagerName            <-
   Stick                                      <-
   Scroll horizonal vertical                  <-
   Title                                      <-
   TogglePage                                 (obsoleted)
   Wait name                                  <-
   Warp [ name window_name ]                  Next or Prev (not compatible)
   WindowsDesk new_desk                       <-
   WindowList arg1 arg2                       <-
   
   *** New in fvwm-2 **
   
   Destroy
   DestroyMenu
   KillModule
   Menu menu-name double-click-action
   Next [conditions] command
   None [arguments] command
   Prev [conditions] command
   Read filename
   Recapture
   +

----------------------------------------------------------------------

7) I have a window that is behaving unexpectedly under FVWM, but just
   fine under (whatever other window manager), or I have just some
   random bug.  What do I do?

A: First, check your rc file and your .Xdefaults to make sure that
   something blatantly obvious in there isn't causing the problem,
   plus check the FAQ, BUGS, TO-DO, and man pages (i.e. RTFM).
   Perhaps even the official WWW page and the mailing list archives
   stored there.

   If you still can't figure it out, send a DETAILED description of
   the problem to the FVWM mailing list.  By detailed I mean more than
   just "my window isn't behaving right" or "I found a bug":

        - Describe the problem as best you can
        - If applicable, include information from:
                xwininfo
                xprop
                FvwmIdent
                xdpyinfo (maybe)
        - What OS & version you are running under
        - What version of X11 are you running under
        - What exact version of FVWM you are running
        - How was FVWM compiled (compiler & version, options, etc)

----------------------------------------------------------------------

8) What's the relative memory usage for the various window managers
   out there?

A: Here's a little table comparing some of them.  It was done on an
   AIX based IBM RS6000 model 355 using the same number of windows (3)
   and XSession to switch between the window managers, and I used
   'top' to show the values:

      SIZE   RES
      545K  652K fvwm2 (fvwm 2.0-pl35)
      457K  528K fvwm  (fvwm 1.24rb)
      856K  960K ctwm  (ctwm 3.2p1)
     1004K 1156K mwm   (mwm 1.2)
      543K  632K twm   (???)
      263K  328K aixwm (a simple ugly window manager included w/ aix)

----------------------------------------------------------------------

9) I used to use GoodStuff in FVWM 1.xx, but it's not in the 2.xx
   distribution.  What do I use now?

A: GoodStuff was renamed to FvwmButtons.  Same module, new name (that
   fits in with the other modules naming convention).

----------------------------------------------------------------------

10) I'm using FvwmButtons (or GoodStuff in 1.xx), and sometimes the
    buttons stay depressed, and other times they don't.  Why is that?

A: This may be the biggest RTFM answerable question that we get on the
   mailing list!  But, since it is always asked anyways, I'll put it
   here.  From the FvwmButtons man page:

       If command is an fvwm Exec command, then the button
       will remain pushed in until a window whose name or
       class matches the qouted portion of the command is
       encountered. This is intended to provide visual
       feedback to the user that the action he has requested
       will be performed. If the qouted portion contains no
       characters, then the button will pop out immediately.
       Note that users can continue pressing the button, and
       re-executing the command, even when it looks "pressed
       in."

----------------------------------------------------------------------

11) What happened to the FVWM 1.xx 'include' command?

A: It was actually part of the M4 preprocessing (see above).  You can
   use the 'Read' builtin to get the same effect, or use the M4
   module.

----------------------------------------------------------------------

12) I really like the horizontal bars that appear on the title bars of
    sticky windows.  Can I get those on other windows as well?

A: Nope.  Maybe in the future, but not right now.  Unless you modify
   the code yourself, that is (which several people have for this).

----------------------------------------------------------------------

13) I'm trying to build FVWM, but the linker is complaining about
    missing functions like 'XpmReadFileToPixmap' or the compiler can't
    find the include file X11/xpm.h.  Why is that?

A: You need to have the Xpm library on your system.  You can find the
   source for it at ftp.x.org in /contrib/libraries.  Or you can
   compile without Xpm support (see the global.h file) and lose out
   on all the pretty color icons.

----------------------------------------------------------------------

14) I'm trying to compile FVWM under SunOS using cc, but the compiler
    is having lots of problems.  What gives?

A: cc under SunOS is not an ANSI C compiler.  Try using acc or gcc
   instead.

----------------------------------------------------------------------

15) Why does the screen go blank at startup and during Recaptures now
    when it didn't in FVWM 1.xx?

A: Rob put it in there for the following reason:

   Rob> Actually, I thought that this would make things happen more
   Rob> quickly/smoothly.  While fvwm is adding borders, it keeps
   Rob> raising and lowering and reparenting windows, which means that
   Rob> they get lots of expose events, and they redraw a lot, many
   Rob> times unnecessarily, because they soon get hidden again. I
   Rob> thought that the single black window covering everything would
   Rob> keep these windows from getting unneede expose events, and
   Rob> minimize redraws.

   Which makes a lot of sense.

   BTW, I've shut it off by default now.  Use the command line option
   -blackout to enable it.

----------------------------------------------------------------------

16) I'd really like to see neat feature XYZ in FVWM.  Wouldn't
    that be cool?  I even have a patch.  When can it be added?

A: People are always requesting or suggestion new features (many of
   which are great ideas and where a lot of the current features came
   from).  One of the more common ones for example is Windows 95 look
   and feel.  

   I'd like to make a statement about this.  FVWM is supposed to be
   small, fast, powerful, and flexible, and sometimes tradeoffs have
   to be made here.  The module interface helps here, as a lot of
   features that not everyone wants or needs don't have to be in the
   main module consuming resources for those people that don't want or
   need them.

   So if you have a suggestion (or a patch), please think of a way to
   make it as small and generic as possible if you feel it belongs in
   the main module.  Also, take a look in the TO-DO list and see if it
   isn't already in there.

   Bear in mind that I make no guarantees that I'll add any requested
   feature or apply any submitted patches to the official version, but
   please don't let this dissuade you from submitting them.  I like to
   get new ideas and I'm always curious to see how someone would
   implement a given feature, even if I never plan to put it in fvwm.

----------------------------------------------------------------------

17) I know this question doesn't have to do with FVWM, but what
    happened to to rxvt and rclock which Rob Nation used to support?
    Where can I find them now?

A: This has come up a few times, so I thought I'd put it here too.
   The new official (I believe) home for rxvt is:

        ftp://ftp.nuclecu.unam.mx/linux/local/rxvt/

   And rclock can be found at:

        ftp://ftp.best.com/pub/newton/rclock/

   which I'm not sure if it's the new official home for that or not.
   I actually have another version here which is a different date and
   size that I can't rememember where I picked it up from so there may
   be multiple copies of that one out there.

----------------------------------------------------------------------

18) Where can I get more XPMs for icons?

A: If you want more color icons, grab the ones out of the ctwm
   distribution (also at ftp.x.org) which has a lot of nice ones.  You
   can also find more in other distributions at ftp.x.org, and at
   http://www.sct.gu.edu.au/~anthony/icons/ (which has a lot, I
   believe).

----------------------------------------------------------------------

19) How do I set the Sun keyboard key xxxx to an FVWM command?  Or
    more generally, I'm having problems defining key bindings for FVWM
    - what can I do?

A: From Jon Mountjoy, one of FVWM's users:
     - Function keys on Sun Keyboard on Top Row are F1 - F8
     - Keys on the function keypad on the Left of the Sun Keyboard
       are F11 == Stop, F12 == Again, ..., F20 == Cut

     His Example:

     /* Function keys on Sun Keyboard on Top Row*/
     Key F1          A       N       Exec me(netscape) &
     Key F2          A       N       Exec me(netscape -install) &
     Key F5          A       N       Exec makex(Adder)
     Key F6          A       N       Exec makex(Lambda) 
     Key F7          A       N       Exec makex(Castor)
     Key F8          A       N       Exec xterm -T Local &
     /*  Keys on the function keypad on the Left of the Sun Keyboard:
         F11 = Stop, F12 = Again, ..., F20 = Cut
     */
     Key F11         AWF      N       Next [!iconic CurrentScreen CurrentDesk] Focus
     Key F12         AWF      N       Prev [!iconic CurrentScreen CurrentDesk] Focus
     Key F13         WF       N       Maximize     100 100
     Key F15         WF       N       RaiseLower ""
     Key F17         WIF      N       Iconify ""
     Key F18         WF       N       Stick ""
     Key F20         WIF      N       Delete ""
     Key Help        AWF      N       Iconify ""

   A more general solution is to use xev (usually distributed w/ X11)
   or xkeykaps (an X11 interface to xmodmap written by Jamie Zawinski,
   available from ftp.x.org) to find out what the keysym for whatever
   key you want REALLY is, and use that for binding fvwm commands.

----------------------------------------------------------------------

20) I clicked on a program's Motif menubar (with click to focus or
    stays on top set) and the menu pops up then dissappears.  What
    gives?

A: Known problem.  The window is being raised over the
   override_redirect window that is being used to display the menu.
   Be sure the window is raised before you click on the menubar.  I
   don't know if there is really anything I can do to help this.

----------------------------------------------------------------------

21) I heard about this FvwmFileMgr module.  Where can I find it?

A: The FvwmFileMgr module dissappeared because a file manager doesn't
   need to be integrated with a window manager and something like xfm
   does a much better job.  I recommend you pick that up instead (from
   ftp.x.org or your favorite mirror).

----------------------------------------------------------------------

22) I'd really like {OpenWindows, NeXT, Win95, Mac, etc} look and
    feel.  Are you going to support that?

A: Nope!  Generically applicable features might appear, but no work
   will be specifically added to FVWM just to make it look or feel
   specifically like another window manager or window manager like
   product.  Future enhancements may appear that allow you to alter
   the appearance to get similiar to other window managers though.

----------------------------------------------------------------------

23) Imake didn't come with my machine.  How do I build FVWM?

A: This is a problem with some IBM AIX and HP HP-UX installations.
   You really have to have Imake currently to build FVWM.  So, you'll
   either have to find out how to install Imake from your install
   media (it does come on the AIX install media), or get it from from
   MIT and install it yourself, or wait until I convert over to using
   GNU's AutoConf, which I would like to do someday...

   For HPs, the following note was sent to the FVWM mailing list that
   may be helpfull:

   ============================================================

   >>   I've been using and compiling fvwm on HP-UX since version
   >>   0.99 something.  I've always used HP's C compiler and make.
   >>   I can't remember when was the last time it didn't compile out
   >>   of the box.
   >
   > What do you do for imake?
   >
   
     (-: Not an official HP response or advertisement :-)
   
     Imake used to be available on a HP ftp server accessible from
     the "outside" (hpcvaaz.cv.hp.com).  The current imake I have,
     may or not have been obtained there, as I've installed a few
     different versions from a few different (HP) places, some that
     work on my system, some that don't, that I've lost track.
   
     Same goes for Xaw and Xmu, which along with imake, HP doesn't
     ship on 9.0x, and rather points to public sites.  This may have
     changed at 10.x, but I'm not sure what the "official line" is.
     A good place to get HP-UX stuff is at the archives maintained by
     the University of Liverpool, which are mirrored at the
     University of Wisconsin, and a few other places,
     http://hpux.cae.wisc.edu, http://hpux.csc.liv.ac.uk, etc.
   
   
   > My HPUX 9.03 doesn't have it, I had to install X11R5. Is this
   > normal for HPUX?
   
     X11R5 development is probably an optional product, as is the
     "real C" compiler, for example.  X11R5 run time libraries is
     probably part of the core HP-UX, as is the brain dead C compiler
     used to regen the kernel.
   ============================================================

----------------------------------------------------------------------

24) How do I create/submit/apply patches?

A: Creating a patch -

        1) Copy the originial source file(s) to the same name with the
           additional extension of .orig (or something like that).
        2) Then run diff with either the -c (context) or -u (unified)
           switch on the sets of files, with the .orig file FIRST.  I
           prefer unifified diff's because they are smaller, but
           sometimes they aren't as readable, so either context or
           unified diffs are fine.

        ex: cp fvwm.c fvwm.c.orig ; diff -u fvwm.c.orig fvwm.c

   Submitting a patch - 

        Just mail it to the fvwm mailing list, is the best way I
        guess.

   Applying a patch -

        Get a copy of the program 'patch' from your favorite source,
        such as prep.ai.mit.edu in /pub/gnu, compile it, and then
        follow it's directions (generally just cd into the appropriate
        directory and run 'patch < patchfile').

----------------------------------------------------------------------

25) I'd like to bind a key to paste the current selection, how can I
    do that?

A: You can't directly with fvwm, but here's a solution that is more
   generally applicable - use the program 'xcb' available at ftp.x.org
   in /contrib/utilities or from your favorite mirror.  It could be
   used to get the desired effect or close to it ('Exec xcb -p0' bound
   to a key or used in a function bound to a key).

----------------------------------------------------------------------

26) You can bind mouse movements to keystrokes, how about mouse
    presses?

A: Hm.  This one is a little tougher.  It could be done, but many
   programs ignore synthetic events, which is what would be generated.
   In X11R6.1 there is an extension (called XKBD?) which can do this,
   but I don't want to code to anything specific to one of the newer
   X11 versions like this.  You could try using Xse, which is a
   program to interface to XSentEvent, available at ftp.x.org.  It
   could probably be used in a smiliar manner to xcb above.

----------------------------------------------------------------------
