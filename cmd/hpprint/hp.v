HP DESKJET 600 SERIES PRINTER COMMANDS


  This document lists the HP DeskJet PCL printer commands for basic
  page formatting and font selection. If you need further information
  about PCL commands please contact the Customer Support Center (1-208-
  323-2551).



          A few notes about using PCL commands:
          
                    The commands are listed in hierarchical
                    order and also give the decimal and
                    hexidecimal equivalents.
          
                    Values in the parenthesis "(x)" identify
                    the lower case of the termination
                    character which is used for combining
                    commands.
          
                    Ec represents the escape character for
                    your software. In DOS Edit Ec is
                    represented by holding down the Ctrl key
                    and pressing the letter P. Release both
                    keys and press the Esc key.
          
                    # and #...#  represent variables that
                    you will replace with the numbers needed
                    to complete the commands required.
          
                    For example, to set the number of copies
                    to 14, Ec&#X, replace the # or #...#
                    with:
          
                    ascii     14
          
                    Decimal   049 052
          
                    Hexidecimal    31 34
  
  Use the following table to translate ascii values into decimal or
  hexidecimal values:

                   ASCII     Dec      Hex    Descript
                   value                       ion
                     0       48       30     Zero
                     1       49       31     One
                     2       50       32     Two
                     3       51       33     Three
                     4       52       34     Four
                     5       53       35     Five
                     6       54       36     Six
                     7       55       37     Seven
                     8       56       38     Eight
                     9       57       39     Nine
                     .       46       2E     Period
                                             (point)
                                   
 Printer      Printer         Decimal Value         Hexadecimal Value
 Feature      Command
                           Printer Control
Reset       EcE         027 069                     1B 45
Self Test   EcZ         027 122                     1B 7A
Paper Input Control                                    
(Media Source)
Eject Page  Ec&0H          (027 038 108 048 072 (104)   1B 26 6C 30 48
                           h                        (68)
                           )
Feed from   Ec&1H   (h) 027 038 108 049 072 (104)   1B 26 6C 31 48
Tray                                                (68)
Envelope    Ec&3H   (h) 027 038 108 051 072 (104)   1B 26 6C 33 48
Feed                                                (68)
Underline                                           
Single      Ec&d1D  (d) 027 038 100 049 068 (100)   1B 26 64 31 44
Fixed                                               (64)
Double      Ec&d2D(d)   027 038 100 050 068 (100)   1B 26 64 32 44
Fixed                                               (64)
Single      Ec&d3D(d)   027 038 100 051 068 (100)   1B 26 64 33 44
Floating                                            (64)
Double      Ec&d4D(d)   027 038 100 052 068 (100)   1B 26 64 34 44
floating                                            (64)
Turn off    Ec&d@       027 038 100 064             1B 26 64 40
Line                                                
Terminatio
n
CR=CR;LF=L  Ec&k0G  (g) 027 038 107 048 071 (103)   1B 26 6B 30 47
F; FF=FF                                            (67)
CR=CR+LF;L  Ec&k1G  (g) 027 038 107 049 071 (103)   1B 26 6B 31 47
F=LF FF=FF                                          (67)
CR=CR;      Ec&k2G  (g) 027 038 107 050 071 (103)   1B 26 6B 32 47
LF=CR+LF;                                           (67)
FF=CR+FF
CR=CR+LF;   Ec&k3G  (g) 027 038 107 051 071 (103)   1B 26 6B 33 47
LF=CR+LF;                                           (67)
FF=CR+FF
End 0f                                              
Line Wrap
Turn on     Ec&s0C  (c) 027 038 115 048 067 (99)    1B 26 73 30 43
                                                    (63)
Turn off    Ec&s1C  (c) 027 038 115 049 067 (99)    1B 26 73 31 43
                                                    (63)
Transparent Print                                   
Mode
No of       Ec&p#X      027 038 112 # 088 [data]    1B 26 70 # 58
bytes       [data]  (x) (120)                       [data]        (78)
Display Functions                                   
Mode
Turn on     EcY         027 089                     1B 59
Turn off    EcZ         027 090                     1B 5A
                             Page Control
Orientatio                                          
n
Portrait    Ec&0O   (o) 027 038 108 048 079 (111)   1B 26 6C 30 4F
                                                    (6F)
Landscape   Ec&1O   (o) 027 038 108 049 079 (111)   1B 26 6C 31 4F
                                                    (6F)
Page Size                                           
Executive   Ec&1A   (a) 027 038 108 049 065 (097)   1B 26 6C 31 41
                                                    (61)
Letter      Ec&2A   (a) 027 038 108 050 065 (097)   1B 26 6C 32 41
                                                    (61)
Legal       Ec&3A   (a) 027 038 108 051 065 (097)   1B 26 6C 33 41
                                                    (61)
A5 ISO/JIS  Ec&25A  (a) 027 038 108 050 055 065     1B 26 6C 32 37 41
                        (097)                       (61)
A4 ISO/JIS  Ec&26A  (a) 027 038 108 050 054 065     1B 26 6C 32 36 41
                        (097)                       (61)
B5 JIS      Ec&45A  (a) 027 038 108 052 053 065     1B 26 6C 34 35 41
                        (097)                       (61)
Card - 4 x  Ec&74A  (a) 027 038 108 055 052 065     1B 26 6C 37 34 41
6
Card - 5 x  Ec&75A  (a) 027 038 108 055 053 065     1B 26 6C 37 35 41
8
Card - A6   Ec&24A  (a) 027 038 108 050 052 065     1B 26 6C 32 34 41
ISO/JIS
Card -      Ec&71A  (a) 027 038 108 055 049 065     1B 26 6C 37 31 41
hagaki
Com-10      Ec&81A  (a) 027 038 108 056 049 065     1B 26 6C 38 31 41
                        (097)                       (61)
Int'l DL    Ec&90A  (a) 027 038 108 057 048 065     1B 26 6C 39 30 41
envelope                (097)                       (61)
Int'l C6    Ec&92A  (a) 027 038 108 057 050 065     1B 26 6C 39 32 41
envelope                (097)                       (61)
US A2       Ec&109A (a) 027 038 108 049 048 057     1B 26 6C 31 30 39
Envelope                065                 (097)   41            (61)
Line                                                
Spacing
Lines per   Ec&#D   (d) 027 038 108 # 068   (100)   1B 26 6C # 44 (64)
inch no.
of lines
Page                                                
Length
# if Lines  Ec&#P   (p) 027 038 108 #...# 080       1B 26 6C #...# 50
(5-128)                 (112)                       (70)
Perforatio                                          
n Skip
On          Ec&1L   (l) 027 038 108 049 076 (108)   1B 26 6C 31 4C
                                                    (6C)
Off         Ec&0L   (l) 027 038 108 048 076 (108)   1B 26 6C 30 4C
                                                    (6C)
Top Margin                                          
# of Lines  Ec&#E   (e) 027 038 108 #...# 069       1B 26 6C #...# 45
                        (101)                       (65)
Text                                                
Length
# of Lines  Ec&#F   (f) 027 038 108 #...# 070       1B 26 6C #...# 46
                        (102)                       (66)
Side                                                
Margins
Clear       Ec9         027 057                     1B 39
Left        Ec&a#L  (l) 027 038 097 #...# 076       1B 26 61 #...# 4C
(column                 (108)                       (6C)
no.)
Right       Ec&a#M  (m) 027 038 097 #...# 077       1B 26 61 #...# 4D
(column                 (109)                       (6D)
no.)
Text Scale                                          
Off         Ec&k5W  (w) 027 038 107 053 087 (119)   1B 26 6B 35 57
                                                    (77)
On          Ec&k6W  (w) 027 038 107 054 087 (119)   1B 26 6B 36 57
                                                    (77)
Horizontal Motion                                   
Index (HMI)
# of        Ec&k#H  (h) 027 038 107 #...# 072       1B 26 6B #...# 48
1/120"                  (104)                       (68)
Increments
The Horizontal Motion Index (HMI) command designates the distance
between columns in 1/120 inch increments.
When fixed pitch fonts are selected, all printable characters, including the space and backspace characters, are
affected by HMI. When proportional fonts are selected, the HMI affects only the control code space character.
The default HMI is equal to the pitch value in the font header. The printer escape sequence that you send
is as follows:
Ec&k#H   # is equal to a variable that is derived from the following formula:

Horizontal Printable Area
____________________________    X  120  =  #
Desired Characters Per Line

Horizontal                                          
Position
# of        Ec&a#C  (c) 027 038 097 #...# 067       1B 26 61 #...# 43
Columns                 (99)                        (63)
# of Dots   Ec*p#X  (x) 027 042 112 #...# 088       1B 2A 70 #...# 58
                        (120)                       (78)
# of        Ec&a#H  h)  027 038 097 #...# 072       1B 26 61 #...# 48
Decipoints              (104)                       (68)
Half Line   Ec=         027 061                     1B 3D
Feed
Vertical Motion                                     
Index (VMI)
# of 1/48"  Ec&#C   (c) 027 038 108 #...# 067       1B 26 6C #...# 43
Increments              (99)                        (63)
The Vertical Motion Index (VMI) command designates the distance
between rows in 1/48 inch increments (the
vertical distance the cursor will move for a line feed operation).
This command affects the line feed and half line
feed spacing. The factory default VMI is 8, which corresponds to 6
lines per inch. VMI can be selected from the
printer control panel or by sending a printer escape sequence: Ec&#C .
# is equal to a variable that is derived
from the following formula:

Vertical Printable Area
_____________________          X  48  =  #

Desired Lines Per Page

Vertical                                            
Position
# of Rows   Ec&a#R  (r) 027 038 097 #...# 082       1B 26 61 #...# 52
                        (114)                       (72)
# of Dots   Ec*p#Y  (y) 027 042 112 #...# 089       1B 2A 70 #...# 59
                        (121)                       (79)
# of        Ec&a#V  (v) 027 038 097 #...# 086       1B 26 61 #...# 56
Decipoints              (118)                       (76)
                            Font Selection
Symbol Set                                          
PC-8        Ec(10U      027 040 049 048 085         1B 28 31 30 55
HP Roman8   Ec(8U       027 040 056 085             1B 28 38 55
PC-8        Ec(11U      027 040 049 049 085         1B 28 31 31 55
Danish/Nor
wegian
PC 850      Ec(12U      027 040 049 050 085         1B 28 31 32 55
ECMA-94     Ec(0N       027 040 048 078             1B 28 30 4E
Latin 1
German      Ec(1G       027 040 049 071             1B 28 31 47
(ISO 21)
French      Ec(1F       027 040 049 070             1B 28 31 46
(ISO 69)
Italian     Ec(0I       027 040 048 073             1B 28 30 49
(ISO 15)
Spanish     Ec(2S       027 040 050 083             1B 28 32 53
(ISO 17)
Swedish     Ec(0S       027 040 048 083             1B 28 30 53
(ISO 11)
Norwegian1  Ec(0D       027 040 048 068             1B 28 30 44
(ISO 60)
ISO 4:      Ec(1E       027 040 049 069             1B 28 31 45
United
Kingdom
ANSI ASCII  Ec(0U       027 040 048 085             1B 28 30 55
(ISO 6)
HP Legal    Ec(1U       027 040 049 085             1B 28 31 55
PC-8        Ec(9T       027 040 057 084             1B 28 39 54
Turkish
PC-852      Ec(17U      027 040 049 055 085         1B 28 31 37 55
ISO 8859/2  Ec(2N       027 040 050 078             1B 28 32 4E
Latin 2
ISO 8859/5  Ec(5N       027 040 053 078             1B 28 35 4E
Latin 5
Windows     Ec(19U      027 040 049 057 085         1B 28 31 39 55
3.1 Latin
1
Windows     Ec(9E       027 040 057 069             1B 28 39 45
3.1 Latin
2
Windows     Ec(5T       027 040 053 084             1B 28 35 54
3.1 Latin
5
Refer to the PCL-5 Comparison Guide for additional supported symbol
sets.
Spacing                                             
Proportion  Ec(s1P  (p) 027 040 115 049 080 (112)   1B 28 73 31 50 (70)
al
Fixed       Ec(s0P  (p) 027 040 115 048 080 (112)   1B 28 73 30 50 (70)
                                                    (70)
                                                    
Print                                               
Pitch
#           Ec(s#H  (h) 027 040 115 #...# 072       1B 28 73 #...# 48
Characters              (104)                       (68)
/
inch
Point Size                                          
(Character Height)
# of        Ec(s#V  (v) 027 040 115 #...# 086       1B 28 73 #...# 56
1/72nd                  (118)                       (76)
inch
Style                                               
Upright     Ec(s0S  (s) 027 040 115 048 083 (115)   1B 28 73 30 53
                                                    (73)
Italic      Ec(s1S  (s) 027 040 115 049 083 (115)   1B 28 73 31 53
                                                    (73)
Stroke                                              
Weight
Bold        Ec(s3B (b)  027 040 115 051 066(98)     1B 28 73 33 42
                                                    (62)
Normal      Ec(s0B (b)  027 040 115 048 066(98)     1B 28 73 30 42
                                                    (62)
Extra       Ec(s7B (b)  027 040 115 055 066(98)     1B 28 73 37 42
Black                                               (62)
(optional)
Typeface                                            
Courier     Ec(s3T (t)  027 040 115 051 084(116)    1B 28 73 33 54
                                                    (74)
CG Times    Ec(s4101T   027 040 115 052 049 048     1B 28 73 34 31 30
            (t)         049 084           (116)     31 54        (74)
Letter      Ec(s6T (t)  027 040 115 054 084(116)    1B 28 73 36 54
Gothic                                              (74)
Univers     Ec(s52T     027 040 115 053 050 084     1B 28 73 35 32 54
            (t)         (116)                       (74)
Times New   Ec(s517T    027 040 115 053 049 055     1B 28 73 35 31 37
Roman       (t)         084       (116)             54           (74)
Arial       Ec(s218T    027 040 115 050 049 056     1B 28 73 32 31 38
            (t)         084      (116)              54           (74)
Symbol      Ec(s302T    027 040 115 051 048 050     1B 28 73 33 30 32
            (t)         084      (116)              54           (74)
Wingdings   Ec(s2730T   027 040 115 050 055 051     1B 28 73 32 37 33
            (t)         048 084      (116)          30 54        (74)
Print                                               
Quality
                                                    
Letter      Ec(s2Q (q)  027 040 115 050 081         1B 28 73 32 51
                        (113)                       (71)
Draft       Ec(s1Q (q)  027 040 115 049 081         1B 28 73 31 51
(economode              (113)                       (71)
)
Download Font                                       
Management
Font ID     Ec*c#D (d)  027 042 099 # 068 (100)     1B 2A 63 # 44 (64)
no.
ASCII code  Ec*c#E (e)  027 042 099 # 069 (65)      1B 2A 63 # 45 (65)
no.
Delete All  Ec*c0F (f)  027 042 099 048 070         1B 2A 63 30 46
                        (102)                       (66)
Delete      Ec*c1F (f)  027 042 099 049 070         1B 2A 63 31 46
Temporary               (102)                       (66)
Delete      Ec*c2F (f)  027 042 099 050 070         1B 2A 63 32 46
last                    (102)                       (66)
Make        Ec*c4F (f)  027 042 099 052 070         1B 2A 63 34 46
Temporary               (102)                       (66)
Make        Ec*c5F (f)  027 042 099 053 070         1B 2A 63 35 46
Permanent               (102)                       (66)
Create      Ec)s#W[data 027 041 115 # 087 [data]    1B 29 73 #
font        ]                                       57[data]
number of
bytes
Download    Ec(s#W[data 027 040 115 # 087 [data]    1B 28 73 # 57
chr. No.    ]                                       [data]
of bytes
                           Raster Graphics
Start Raster                                        
Graphics
At left     Ec*r0A      027 042 114 048 065         1B 2A 72 30 41
most
position
Current     Ec*r1A      027 042 114 049 065         1B 2A 72 31 41
Cursor
Position
End Raster                                          
Graphics
End         Ec*rC       027 042 114 067             1B 2A 72 43
Graphics
Resolution                                          
75 dots     Ec*t75R     027 042 116 055 053 082     1B 2A 74 37 35 52
per inch
150 dots    Ec*t150R    027 042 116 049 053 048     1B 2A 74 31 35 30
per inch                082                         52
300 dots    Ec*t300R    027 042 116 051 048 048     1B 2A 74 33 30 30
per inch                082                         52
600 dots    Ec*t600R    027 042 116 054 048 048     1B 2A 74 36 30 30
per inch                082                         52
Configure   Ec*g#W      027 042 103 # 087           1B 2A 67 # 57
Raster
Data
Set Raster Graphics                                 
Width
# of        Ec*r#S      027 042 114 # 083           1B 2A 72 # 53
pixels
Add Raster Graphics                                  
Conpression
Method0     Ec*b0M      027 042 098 048 077         1B 2A 62 30 4D
Method1     Ec*b1M      027 042 098 049 077         1B 2A 62 31 4D
Method2     Ec*b2M      027 042 098 050 077         1B 2A 62 32 4D
Method3     Ec*b3M      027 042 098 051 077         1B 2A 62 33 4D
Method9     Ec*b9M      027 042 098 057 077         1B 2A 62 39 4D
Seed Row    Ec*b#S      027 042 098 # 083           1B 2A 62 # 53
Source
Transfer Raster                                     
Graphics
Number of   Ec*b#W[data 027 042 098 # 087 [data]    1B 2A 62 # 57
byes        ]                                       [data]
Transfer    Ec*b#V[data 027 042 098 # 086 [data]    1B 2A 62 # 56
graphics    ]                                       [data]
data by
plane
Relative Vertical Pixel Movemnet                             
(formerly known as Y offset)
# of dots   Ec*b#Y      027 042  098 # 089          1B 2A 62 # 59
Set Number of Raster                                        
Planes Per Row
Single      Ec*r1U      027 042 114 049 085         1B 2A 72 31 55
plane
palette
3 planes,   Ec*r-3U     027 042 114 045 051 085     1B 2A 72 2D 33 55
CMY
palette
3 planes,   Ec*r3U      027 042 114 051 085         1B 2A 72 33 55
RGB
palette
4 planes,   Ec*r-4U     027 042 114 045 052 085     1B 2A 72 2D 34 55
KCMY
palette
Configure Raster                                    
Data (CRD)
Format 2    Ec*g#W      027 042 103 # 087           1B 2A 67 # 57
Color Text (graphics)                                
Foreground  Ec*v#S      027 042 118 # 083           1B 2A 76 # 53
color
Graphics Image                                       
Improvement
Raster      Ec*o#D      027 042 111 # 068           1B 2A 6F # 44
graphics
depletion
                                   
                                   
                                 HP FIRST #:  2997, 1-Feb-96, BPD02925
PostScript is a trademark of Adobe Systems Incorporated, which may be
registered in certain jurisdictions.
            Windows is a U.S. Trademark of Microsoft Corp.
                 Copyright  Hewlett-Packard Co. 1994
This information is subject to change without notice and is provided "
as is" with no warranty.
Hewlett-Packard shall not be liable for any direct, indirect, special,
incidental or consequential damages
             in connection with the use of this material.
