/* 
 * Copyright (c) 2013 Dylan Reinhold
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * dsdtune is a wrapper around the dsdplus to find the best values to run under
 */
#include <ctype.h>
#include "dsdtune.h"
#include "utils.h"

unsigned int decode_rate(char *line) {
  /* Assume line is formated as :
   * decoding score = 42,072
   */
  char tmp_rate[15];
  int i,j,len;
  memset(tmp_rate,'0',14);
  tmp_rate[14] = '\0';
  len = strlen(line);
  line += len - 1 ;
  for(i=0,j=13; i < len; i++) {
    if(*(line-i) == ',' || *(line-i) == '\n' || *(line-i) == '\r')
      continue;
    if(isdigit(*(line-i))) {
      tmp_rate[j--] = *(line-i);
    } else {
      i=len+1;
    }
  }
  return atol(tmp_rate);
}
