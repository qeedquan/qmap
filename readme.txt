         QMAP Copyright 1997 Sean Barrett
  See "redist.txt" for redistribution information

                    --==--==--

Quickstart:
   "qmap <bspfile>" from directory containing palette.lmp & colormap.lmp
   Terra Nova/System Shock-style controls
   spacebar to halt movement, esc to quit

   If you move outside of the level, execution will be very slow.

                    --==--==--

Files:
   redist.txt       redistribution information
   readme.txt       (this file) instructions for executable
   source.txt       instructions for source code
   qmap.txt         general description, goals, feature list
   manifest.txt     list of all files
   bug.txt          information about bugs and bug reporting

   other text documents are described in source.txt

                    --==--==--

Commandline usage:

   Unpak a .pak file so you have a palette.lmp and
   a colormap.lmp in your current directory.  Then
   run

       qmap <bspfile>

   where <bspfile> is an unpak'd bsp file.

   For example:

       qmap start.bsp

   The starting location is about right for start.bsp,
   and works ok for e1m1.bsp as well.

   QMAP uses a lot of memory, because it uses a dumb
   memory layout for reading the bsp file.  I chose
   smaller max sizes than id's utilities, so it's
   possible to have levels which are too big (and
   they'll just barf without being detected).
   
                    --==--==--

Controls:

   QMAP uses a pretty wacky keyboard control scheme
   which is my standard testbed layout.

   Press <spacebar> to halt all movement.
   Press <esc> to quit.

   Use the 'qwe/asd/zxc/rv' cluster to tilt and move,
   a la System Shock, except that each keystroke _accelerates_
   your movement.  This can be disturbing until you're used to
   it; just remember to hit spacebar to halt.

    w   x          move forward & backward
    a   d          turn left & right
    z   c          sidestep left & right
    r   v          look up & down
    q   e          bank view
    1   3          "sidestep" up & down
   spacebar        stop moving

  esc shift-Q      quit
