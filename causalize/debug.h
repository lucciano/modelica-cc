/*****************************************************************************

    This file is part of Modelica C Compiler.

    Modelica C Compiler is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Modelica C Compiler is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Modelica C Compiler.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


// This debugging routines allow the user to turn on selected
// debugging messages, controllable from the command line arguments
// passed to modelicacc after de -d option.

enum Level {ERROR, WARNING, INFO};

/**
 * Initialize so that only DEBUG messages with a flag in flagList
 * will be printed.
 *
 * If the flag is "+", we enable all DEBUG messages.
 *
 * "flagList" is a string of characters for whose DEBUG messages are to be enabled.
 */
void debugInit(const char* flags);

/**
 * Print a debug message, if flag is enabled.
 *
 * The format parameter and the subsequent vararg are pass to the printf function.
 **/
void DEBUG (char flag, Level level, const char* format, ...);
