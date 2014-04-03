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

#include "dsdtune.h"
#include "utils.h"

void usage(void) {
  printf("\n");
  printf("dsdtune Version %s\n", VERSION);
  printf("Usage: \n");
  printf(" dsdtune [options] \n");
  printf(" dsdtune -i <in_file>\tUse default dsd decode\n");
  printf("\n");
  printf(" -f<x> dsd decode option\n");
  printf(" -fa Auto-detect frame type (default)\n");
  printf(" -f1 Decode only P25 Phase 1\n");
  printf(" -fd Decode only D-STAR\n");
  printf(" -fi Decode only NXDN48* (6.25 kHz) / IDAS*\n");
  printf(" -fn Decode only NXDN96 (12.5 kHz)\n");
  printf(" -fp Decode only ProVoice*\n");
  printf(" -fr Decode only DMR/MOTOTRBO\n");
  printf(" -fx Decode only X2-TDMA\n");
  printf(" -xx Expect inverted X2-TDMA signal\n");
  printf("\n");
  printf(" -x Name of dsd executable. (dsdplus.exe default)\n");
  printf("    If dsdplus.exe is not found dsd.exe will be tried\n");
  printf(" -b <file> Create batch file with results\n");
  printf(" -o \"<dsd options>\" Options to pass to dsd from\n");
  printf("     the batch file. The double quotes are required\n");
  printf(" -l <file> Log file name (dsdtune.log default)\n");
  /* not setup yet
  printf(" -v <num> Verbosity 1-5 (5 is MAX)\n"); */
}

int main(int argc, char *argv[]) {
  int i = 0;
  FILE *fp;
  char buffer[1024];
  char decode_str[] = "decoding score = ";
  int c;
  extern char *optarg;
  extern int optind, opterr, optopt;
  int decode_str_len;
  char base_command[255];
  dsd_params params[] = {  { "dr",  1,   5, 0, 0, 0 }, 
                           { "dh",  1,   8, 0, 0, 0 },
                           { "ds", 50,  80, 0, 0, 1 },
                           { "dd",  1, 100, 0, 0, 1 },
                           { "dv",  1,  30, 0, 0, 1 },
                           { "  ",  0,   0, 0, 0, 0 } }; /* GCC warns if this is 0 */
  options opts;
  /* Check if dsdplus.exe exists if so us that first */
  if(fileexists("dsdplus.exe") == 0) {
    strncpy(opts.exe_name, "dsdplus.exe", 12);
  } else {
    strncpy(opts.exe_name, "dsd.exe", 8);
  }
  opts.decode_option_set = 0;
  opts.infile_set = 0;
  opts.write_batch = 0;
  opts.batch_options[0] = '\0';
  opts.invert_x2_tdma_str[0] = '\0';
  opts.psk_mod_str[0] = '\0';
  opts.skip_params = 0;
  strcpy(opts.logfile, "dsdtune.log");
  while ((c = getopt (argc, argv, "hf:i:x:b:o:m:")) != -1) {
    opterr = 0;
    switch (c) {
       case 'h':    usage();
                    exit(0);
       case 'b':    strncpy(opts.batch_name, optarg, 99);
                    opts.write_batch = 1;
                    break;
       /* not setup yet
       case 'v':    sscanf (optarg, "%d", &opts.verbose);
                    break; */
       case 'i':    strncpy(opts.infile, optarg, MAX_IN_FILE_LEN-1);
                    opts.infile[MAX_IN_FILE_LEN-1] = '\0';
                    opts.infile_set = 1;
                    break;
       case 'f':    opts.decode_option[0] = optarg[0];
                    opts.decode_option[1] = '\0';
                    opts.decode_option_set = 1;
                    if(optarg[0] == 'd' || optarg[0] == 'p')
                      opts.skip_params = 1;
                    break;
       case 'l':    strncpy(opts.logfile, optarg, 99);
                    break;
       case 'm':    if(*optarg == 'p' && *(optarg+1) == '\0') {
                      strncpy(opts.psk_mod_str, "-mp", 4);
                    } else {
                      usage();
                      exit(1);
                    }
                    break;
       case 'o':    strncpy(opts.batch_options, optarg, 99);
                    break;
       case 'x':    /* check for -xx first */
                    if(*optarg == 'x' && *(optarg+1) == '\0') {
                      strncpy(opts.invert_x2_tdma_str, "-xx", 4);
                    } else {
                      strncpy(opts.exe_name, optarg, 99);
                    }
                    break;
       default:     usage();
                    exit(1);
    }
  }
  if(!opts.infile_set) {
    fprintf(stderr, " Missing required argument of -i <input file>\n");
    usage();
    exit(1);
  }
  if(opts.decode_option_set)
    sprintf(base_command, "%s < %s -o0 -O NUL -f%s ", opts.exe_name, opts.infile, opts.decode_option);
  else
    sprintf(base_command, "%s < %s -o0 -O NUL ", opts.exe_name, opts.infile);
  strcat(base_command, opts.invert_x2_tdma_str);
  strcat(base_command, opts.psk_mod_str);
  decode_str_len = strlen(decode_str);

  while(params[i].name[0] != ' ') {
    int j = params[i].min;
    char tmp_command[255];
    if(opts.skip_params && params[i].skip) {
      i++;
      continue;
    }
    printf("Checking option -%s\n", params[i].name);
    printf("Running %s -%sXXX\n", base_command, params[i].name);
    while(j <= params[i].max) {
        char command[255];
        int found = 0;
        sprintf(command, "%s -%s%d 2>&1", base_command, params[i].name, j);
        if( (fp = popen( command, "r" )) == NULL ) {
           perror("Failed to open ");
           fprintf(stderr, "failed to open dsd\n");
           exit(5);
        }
        while(fgets(buffer, 1024, fp))  {
           if( strncmp(buffer, decode_str, decode_str_len-1) == 0 ) {
             unsigned int rate;
             found = 1;
             rate = decode_rate(buffer);
             if(rate >= params[i].best_results) {
               params[i].best_results = rate;
               params[i].best_setting = j;
             }
           }
             
        }
        pclose( fp );
        if(!found) {
          fprintf(stderr, "Could not get decode string\n");
          exit(5);
        }
        j++;
    }
    printf("best value of %u with -%s%d\n", params[i].best_results, params[i].name, params[i].best_setting);
    sprintf(tmp_command, "%s -%s%d", base_command, params[i].name, params[i].best_setting);
    strcpy(base_command, tmp_command);
    i++;
  }
  /* Resutls */
  i=0;
  printf("\n+------ RESULTS -----+\n");
  printf("Switches you want to use :\n");
  printf("  ");
  while(params[i].name[0] != ' ') {
    if(opts.skip_params && params[i].skip) {
      i++;
      continue;
    }
    printf(" -%s%u",params[i].name, params[i].best_setting);
    i++;
  }
  printf("\n");
  write_log(params, &opts);
  if(opts.write_batch) 
    write_batch(params, &opts);
  return 0;
}
