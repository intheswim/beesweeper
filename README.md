#### Background 

<pre>

Beesweeper (or Hex Minsweeper) is  a  simple game I  originally created back  in
1996-1997 when  I was learning Windows API programming.  I had Windows  3.1 back
then  with  free Minesweeper installed  and got inspired  by this game to create
something  similar, also  as  a  learning  excercise,  so of  course  the  "hex"
minesweeper using hexagons cells  was my idea. As it later turned out, I was not
the only one who came with this concept. 

The original  Windows  version  was using pure Windows  API (with custom  Window
procs, etc.)  following the  Charles Petzold book.  It was extremely lightweight
and quick. 

Fast forward  to 2010-2011 I was  involved in mobile development for  Blackberry
and iOS. 

The game was ported first to Blackberry  Java and then to iOS. The apps were  in
corresponding app stores between 2010 and 2015.  These versions included sharing
results  online  so people  could  compare their  personal  records. The "record
store" was shared between users of all devices. Since Blackberry devices had all
different  screen  sizes, I  included  an option  to  "simulate"  other  devices
screens. 

</pre>

A screenshot of the hex option:

![Screenshot](/screenshot1.png)

#### To build and install 

<pre>

Prerequisites  are  SDL2.  To build, simply run  make.  This app  has also  been
published   on   Ubuntu   Store    as   a   snap.   You   can    get   it   from
https://snapcraft.io/beesweeper 

</pre>

#### On this version of the source code 

<pre>

In 2022 I  was  planning  to  port  some of my  games to SDL. Before jumping  to
something more complex I decided to port this relatively simple game to use SDL,
otherwise it will be lost. Hopefully it will work as long as SDL is supported as
I lost all my hope with corporate platforms such as  iOS, Windows, Android which
over time  break  things making any software obsolete and  eventually completely
unusable. 

Through the code, you might find some remnants of Windows, Java or iOS code even
though I tried to eliminate it as much as I could.  Compared to older  versions,
for Windows 3.1 or  Blackberry  in particular,  this version  redraws  the whole
screen  on each update.  Older  versions took great deal of care to  only redraw
specific cells  (or  parts  of  the screen)  that changed, to make  the  app  as
responsive as possible;  with fast modern  computers and SDL in particular, this
is not needed as no practical advantage is achieved. 

I also noticed that on Ubuntu (the only Linux platform I tried it on so  far) it
works  better when renderer is "software", while on  Windows 10 it's better with
"accelerated" (GPU) renderer. This differencea has been hard-coded in game.cpp. 

</pre>

#### Differences from classic Minesweeper 

<pre>

1. The game  supports both hexagon and  classic  (squares) modes.  One important
difference from classic  Minesweeper is so-called "auto-mark" mode (turned on by
default). In this mode all mines that are  obvious  marked automatically as user
opens additional  cells. I had mixed feedback on  this  feature from users (when
game was in the app stores) but I personally like it. It makes the game somewhat
easier  to play  while not  really  helping with  anything non-obvious. A little
automation of sorts :) 

2.  Secondly, original  (Windows 3.1) version had the "batch  mode" (aka chords)
support. This  is when user presses both left  and right  mouse keys at the same
time  to open  cells -  a feature which was  present  in  the original Microsoft
Minesweeper. Mobile  versions for obvious reasons never needed this  feature  as
mobile devices don't have left and right  mouse keys. It was  re-enabled in this
version with #USE_BATCHED  define; there's  also a  setting/command line  option
that controls it. 

3. The very first cell you click on never has a mine - eliminating the situation
when you step on the mine immediately at the start. 

</pre>

### Options / Settings 

<pre>

Options are invoked at command line level or by clicking on a small black button
next to timer. They are read  from optional config.ini file  when no options are
given. Enter ./mines --help to see command line syntax. 

The  game  has two modes (squares and hexagons), three levels (the differ by how
many mines are present),  custom choice of the field size and option to  show or
hide question marks with right mouse click. Enjoy. 

</pre>

A screenshot of the classic option:

![Screenshot](/screenshot2.png)


### License 

[MIT](https://choosealicense.com/licenses/mit/) 
