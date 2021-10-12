======================================================================
                       TO-DO list for fvwm 2.xx
======================================================================

Please note that not everything on this list will be done, in
particular the ones that end in '?' which are really just meant to be
'think about this and perhaps investigate'.  But they are things that
I didn't want to lose track of.

----------------------------------------------------------------------

Doc cleanups:

  - Add to FAQ
  - Update disclaimers

----------------------------------------------------------------------

Old Patches:

  - CursorStyle patch, plus additional "kill" cursor
  - Safer config file reading patch?

----------------------------------------------------------------------

Bugfixes:

  - Change flags implementation to allow adding more Styles easily
    (bitfields?) - THE GREAT STYLE FLAG REWRITE
  - Fix Restart to not pass original (fvwm specific) options to other wm's
  - Run profiling on FVWM to see if I can speed it up any more
  - Try to decrease memory usage a little more
  - clean up code duplication (esp in modules) - more stuff in
    libfvwm2.a, plus perhaps a second module lib?
  - Change Motion vs Click vs DoubleClick to be calculated via a
    MotionThreshold and ClickTime:
        MotionThreshold exceeded -> Motion
        MotionThreshold not exceeded & ClickTime exceeded -> Click
        MotionThreshold not exceeded & ClickTime not exceeded -> DoubleClick
  - Make left justified pixmaps in menus center align vertically w/
    any text displayed?
  - Make transient FvwmWinList reposition itself & pointer if popped up
    off the screen (like built in menus)
  - Why are there separate CaptureAllWindows & Recapture functions?
  - If programs raise their own windows (XEmacs 'other-frame', for example)
    new stacking order isn't communicated to fvwm
  - RaiseLower doesn't work if only titlebar is obscured
  - Maximize XTerm, change font, UnMaximize, XTerm goes to wrong window size
  - Colormaps and xlock -install -mode blank (& swirl) interaction still
    isn't 100% correct
  - bring back 'TogglePage'?
  - Setting Key's via FvwmTalk or Read requires Recapture to take effect?
  - Popup menus require button press AND mouse movement to appear???
  - Builtin winlist (and other menus) has extra keystrokes or mouse
    movement needed sometimes
  - System Modal dialogs bug - popup menus shouldn't be allowed?
  - Fix FvwmDeskTopScale size calculation
  - Need to fix to work correctly under 24bit (TrueColor) displays
  - Esc during moves, etc can result it windows being "lost" off desktop
  - Should also have way to make windows off desktop be recovered
  - Transients of transients don't get raised correctly sometimes?

----------------------------------------------------------------------

New stuff:

  - Pin up menus?
  - Multi column menus? (for LONG lists)

  - Private colormap option for menus?
  - Reduce number of colors used in xpms included
  - Reduced (limit Xpm choices) color set

  - Recapture, one window only option

  - Access to certain window attribs from .fvwm2rc funcs, and
    simple if/else capabilities (or perhaps a module to do so)??
  - Simple static variables for .fvwm2rc functions (for toggles, etc)??
  - Add $d variable for functions to get desktop number?

  - Add StayOnBottom style?

  - Add ClickToFocusDontPassClick style (so initial click inside
    window doesn't get passed to the app, just changes focus).
  - Add ClickToFocusDontRaise style (so clicks in window don't
    automatically raise the window when giving it focus).
  - Should the previous be parms to ClickToFocus instead of styles?

  - Add NeverFocus style?  Better - make sure fvwm handles all 4 focus
    states from the ICCCM (I don't think it handles a certain one of
    them correctly) and then make style parms to make fvwm able to
    treat windows like they are any one of these styles, even if the
    programs didn't set themselves up that way....

  - Resurrect StubbornPlacement style
  - Add StickyOnDesk <number> style, to allow stickyness on certain
    desks only, or perhaps a StaysOnDesk <num> style, to replace
    StartsOnDesk and work with the Sticky attribute, I'm not sure...
    Either way, multiple values need to be allowed
  - Need to indicate sticky icons somehow, and perhaps sticky windows
    that don't have a titlebar.  I'd like to bring back the different
    color for the sticky windows as an option, if I can do it cleanly.

  - Function to simulate button presses, to go with CursorMove?  Might
    not be possible since many apps ignore SYNTHETIC events...

  - Switch from Imake to GNU autoconf?

  - Easy module name changes from .fvwm2rc (either using changes in
    module exec code & rc parser, or in modules themselves)

  - Improved FvwmPager (add/rename desktops on fly)
  - Make pager have option to show current working desk (not just in icon)
  - A module that just has buttons for the active desktops, like desktop
    switcher in dtwm (COSE).  Could be munged into FvwmPager.
  - See if it's possible to drop windows onto the pager ala olvwm?
    Don't really think it is.
  - Add option to not show sticky windows in pager.  Perhaps also add
    ability to filter out windows based on name/class/resource?

  - "Captive" fvwm (ala ctwm)?

  - FvwmPerl module (or perl lib), for making modules in Perl

  - Allow size geometry specs for FvwmButtons & perhaps other modules
    (Pager).

  - Module for shell to fvwm communicaion (ala gnuserv & gnuclient)
    (perhaps modify FvwmTalk or FvwmConsole to handle this?)
  - FvwmAuto additions for AutoLower & per window config (requires keywds?)
  - Module to X Select window Name, Class, Resource, ID, etc...??
  - FvwmTile module to tile/stack/layer/move windows?
  - FvwmMsgLog module to pop up a log of fvwm error msgs?

  - Support for passwords in FvwmForm
  - Support for cut & paste in FvwmForm text fields (paste at least)
  - Add simple history mechanism for FvwmForm text fields?
  - Make FvwmForm have Resource & Class values

  - Add to module commincations to pass titlebar & button window ids to
    allow modules to muck with those windows (for animation, or whatever)?
  - Add way to send arbitrary messages from fvwm to a module, eg 
    'SendToModule ModuleName "command string"' or something similar.

  - Mouse button chording?

  - Grid the icon box areas?  Add better overall icon handling options?
    Make an optional global free icon placement grid?

  - More controll over icon appearance (non 3D, fg/bg colors, etc)?

  - Add StartsOnPage x y or something similiar to compliment
    StartsOnDesk (and add X resource FvwmPage which will be analogous
    to the Desk X resource, which I think should be renamed to
    FvwmDesk)

  - Implement (or at least investigate) dynamic loading of functions
    on systems that support it?  
        AIX - load
        Linux - dld (gnu)
        SunOS, Solaris, OSF - dlopen/sym
        HP-UX - shl_*
    I don't know how much of a win this is over modules though.  Less
    portable.

  - A WindowGravity option that controlls placement direction choices
    for SmartPlacement (and perhaps RandomPlacement)?  Perhaps make it
    a Style option??

  - Add option for Prev/Next function to 'wrap' at ends of list?

  - Ressurrect OpaqueResize (as a style option, after the great style
    rewrite)

  - Add twm SqeezeTitle functionality to TitleStyle stuff & merge into
    Style command, perhaps???

  - Make WindowsDesk able to operate relative to the current desk
    (like the Desk command)

  - Allow neg vals for Maximize to indicate from bottom/right of screen

  - Investigate impact of adding 'zoom' lines on (de)iconify ala olvwm???

  - Add way to set keyword/value pairs to windows (via Atoms?) as a
    way for giving extra info to modules through style commands

  - Make GetConfigLine be more intelligent, to filter out what gets
    sent (for instance, pass module prefix to look for) and be able to
    ask for PixmapPath, etc instead of sending those w/ all the config
    info, so only the modules that need it could ask for it.

  - Go to just one IconPath instead of PixmapPath too?

  - Allow Resize to have units of "what the window resizes by"
  - Bug in:
        AddToFunc resize-and-move "I" resize 100 100
        + "I" move 100 300

  - ToggleButtonStyle (to keep pushed in)?
  - Allow bitmap/mask files to define buttons as well?
    (Better: new alternate def like hl(x1,y1,x2,y2) sh(x1,y1,x2,y2))

  - Make parm to Restart optional (to just restart current if not specified)

  - Add a WaitForExec to force waiting for the last Exec command to finish

  - Make fvwm look at gravity when moving windows when reparenting, to
    change corner that gets "anchored" so windows w/ neg offsets are
    still placed correctly when bordered initially

  - Investigate internationalization issues (handling of compound
    text, messages placed in a message catalog, etc) to see if they
    should be added at some point.

  - Allow env var to specify an additional read directory ($FVWMRCDIR
    or $FVWMHOME or something similiar)?

  - Allow multiple IconBox's for same style def?

  - Add way to specify max (& min?) desk number, and make desk change
    functions wrap at bounds?



