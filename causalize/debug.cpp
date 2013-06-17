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

#include <causalize/debug.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static const char *enableFlags = NULL; // controls which DEBUG messages are printed

void debugInit(const char *flagList) {
    enableFlags = flagList;
}

bool debugIsEnabled(char flag) {
    if (enableFlags != NULL)
       return (strchr(enableFlags, flag) != 0)
    || (strchr(enableFlags, '+') != 0);
    else
      return false;
}

void vDEBUG(char flag, Level level, const char *format, va_list ap) {
  if (debugIsEnabled(flag)) {
    switch(level) {
      case ERROR:
        vfprintf(stderr, format, ap);
        fflush(stderr);
        break;
      case WARNING:
        vfprintf(stderr, format, ap);
        fflush(stderr);
        break;
      case INFO:
        vfprintf(stdout, format, ap);
        fflush(stdout);
        break;
    }
  }
}

void DEBUG(char flag, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vDEBUG(flag, INFO, format, ap);
  va_end(ap);
}

void DEBUG(char flag, Level level, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vDEBUG(flag, level, format, ap);
  va_end(ap);
}


