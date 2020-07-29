# TR-DOS List

TR-DOS List is a utility to display the TR-DOS image listing in the format similar to the output of the TR-DOS LIST command.

The following image formats are supported:

1. TRD:
   ```txt
   $ trdos-list ZX-REVU1.TRD
   Title:  AMD4ever Disk Drive: A
   7 File(s)       80 Track D. Side
   0 Del. File(s)  Free Sector 1000

     File Name    Start Length Line
   boot    <B>255  00261 00122
   REVIEW  <1>255  49152 01995
   REVIEW  <2>255  24576 01718
   REVIEW  <3>255  34048 14729
   REVIEW  <4>255  24576 08992
   REVIEW  <5>255  49152 02048
   REVIEW  <6> 14  49152 03181
   ```
2. SCL:
   ```
   $ trdos-list Dizzy\ 4r.scl
   Title:           Disk Drive: A
   2 File(s)       80 Track D. Side
   0 Del. File(s)  Free Sector 2248

     File Name    Start Length Line
   boot    <B>100  00219 00219
   dizzy+4r<C>196  00000 00000
   ```

## Known issues:

1. The utility doesn't currently display the autostart line of BASIC programs.
