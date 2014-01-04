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
#include <time.h>

int fileexists(char *filename) {
  /* check if a file exists */
  return access( filename, 0 );
}

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

void get_time_stamp(char *timestamp) {
  time_t sec;
  sec =  time(NULL);
  sprintf(timestamp, "%s", ctime(&sec));
  *(timestamp + strlen(timestamp) - 1) = '\0';
}

void write_log(dsd_params *params, options *opts) {
  FILE *log_fh;
  char timestamp[30] = "";
  int i = 0;
  char logline[100] = "";
  char invert_x2_tdma_str[] = "-xx";
  char psk_mod_str[] = "-mp";

  if(! opts->invert_x2_tdma ) { 
    invert_x2_tdma_str[0] = '\0';
  }
  if(! opts->psk_mod ) {
    psk_mod_str[0] = '\0';
  }
  log_fh = fopen(opts->logfile, "a");
  if(log_fh == NULL)
    return;
  get_time_stamp(timestamp);
  while(params[i].name[0] != ' ') {
    char tmp[10] = "";
    strcat(logline, " -");
    strcat(logline, params[i].name);
    sprintf(tmp, "%d", params[i].best_setting);
    strcat(logline, tmp);
    i++;
  }
  if(opts->decode_option_set) {
    fprintf(log_fh, "%s : Best Decode on file %s was %d with options -f%s %s %s %s\n", timestamp, opts->infile, params[i-1].best_results, opts->decode_option, invert_x2_tdma_str, psk_mod_str, logline);
  } else {
    fprintf(log_fh, "%s : Best Decode on file %s was %d with options %s %s %s\n", timestamp, opts->infile, params[i-1].best_results, invert_x2_tdma_str, psk_mod_str, logline);
  }
  fclose(log_fh);
}
  
void write_batch(dsd_params *params, options *opts) {
  /* Write a batch file with the sugested settings */
  FILE *b_fh;
  char timestamp[30] = "";
  int i = 0;
  char invert_x2_tdma_str[] = "-xx";
  char psk_mod_str[] = "-mp";

  if(! opts->invert_x2_tdma ) {
    invert_x2_tdma_str[0] = '\0';
  }
  if(! opts->psk_mod ) {
    psk_mod_str[0] = '\0';
  }

  b_fh = fopen(opts->batch_name, "wt");
  if(b_fh == NULL) {
    fprintf(stderr, "ERROR : Could not create batch file %s (%d)\n", opts->batch_name, errno);
    return;
  }
  get_time_stamp(timestamp);
  fprintf(b_fh, "@echo off\n");
  fprintf(b_fh, "REM : batch file created with dsdtune utility on %s\n", timestamp);
  fprintf(b_fh, "\n");
  if(opts->decode_option_set) {
    if(opts->batch_options != NULL)
      fprintf(b_fh, "%s %s -f%s %s %s", opts->exe_name, opts->batch_options, opts->decode_option, invert_x2_tdma_str, psk_mod_str);
    else
      fprintf(b_fh, "%s -f%s %s %s", opts->exe_name, opts->decode_option, invert_x2_tdma_str, psk_mod_str);
  } else {
    fprintf(b_fh, "%s %s %s", opts->exe_name, invert_x2_tdma_str, psk_mod_str);
  }

  while(params[i].name[0] != ' ') {
    fprintf(b_fh, " -%s%u",params[i].name, params[i].best_setting);
    i++;
  }
  fprintf(b_fh, "\n");
  fclose(b_fh);
  printf("\nCreated batch file %s to run dsd\n", opts->batch_name);
}

