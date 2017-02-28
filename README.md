# dsdtune
Copyright (c) 2013 Dylan Reinhold
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
    
dsdtune is a wrapper that will call the new dsdplus trying each of the fine tunning 
options, -ds# -dd# -dv#.

When complete will report the options that showed the best decode.

##Usage
 * Run dsdplus while live decoding press r, this will start recording raw 
discriminator audio. After about one minute press r agian to stop.
 * You will have a wav file named somthing like `DSDPlus-Raw-Input_2013-12-28@140307.wav`
 * Copy dsdtune.exe into the same folder as dsdplus and the wav file.
 * Run `dsdtune -i DSDPlus-Raw-Input_2013-12-28@140307.wav` you should also pass
the decoder options `-fd, -fn -fN, -fr...`

```
Usage: 
 dsdtune [options] 
 dsdtune -i <in_file>	Use default dsd decode

 -f<x> dsd decode option
 -fa Auto-detect frame type (default)
 -f1 Decode only P25 Phase 1
 -fd Decode only D-STAR
 -fi Decode only NXDN48* (6.25 kHz) / IDAS*
 -fn Decode only NXDN96 (12.5 kHz)
 -fp Decode only ProVoice*
 -fr Decode only DMR/MOTOTRBO
 -fx Decode only X2-TDMA

 -xx Expect inverted X2-TDMA signal
 -mp Optimize for PSK modulation

 -x Name of dsd executable. (dsd.exe default)
 -b <file> Create batch file with results
 -o "<dsd options>" Options to pass to dsd from
     the batch file. The double quotes are required
 -l <file> Log file name (dsdtune.log default)
```



##Developer Info
 * This was complied with mingwin cross compiler using `i686-pc-mingw32-gcc -Wall -o dsdtune.exe utils.c dsdtune.c` (gcc version 4.8.2 (GCC))
