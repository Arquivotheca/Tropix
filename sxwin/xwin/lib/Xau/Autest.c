/* $Xorg: Autest.c,v 1.3 2000/08/17 19:45:29 cpqbld Exp $ */

/*

Copyright 1988, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/

#include <X11/Xauth.h>

main (argc, argv)
char	**argv;
{
    Xauth   test_data;
    char    *name, *data, *file;
    int	    state = 0;
    FILE    *output;

    while (*++argv) {
	if (!strcmp (*argv, "-file"))
	    file = *++argv;
	else if (state == 0) {
	    name = *argv;
	    ++state;
	} else if (state == 1) {
	    data = *argv;
	    ++state;
	}
    }
    if(!file) {
	fprintf (stderr, "No file\n");
	exit (1);
    }
    test_data.family = 0;
    test_data.address_length = 0;
    test_data.address = "";
    test_data.number_length = 0;
    test_data.number = "";
    test_data.name_length = strlen (name);
    test_data.name = name;
    test_data.data_length = strlen (data);
    test_data.data = data;
    output = fopen (file, "w");
    if (output) {
	XauWriteAuth (output, &test_data);
	fclose (output);
    }
}
