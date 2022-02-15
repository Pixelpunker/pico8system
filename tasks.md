# Todo

## Bug fixes

* ~~Screen shake issues -- noch mehr Space?~~
* ~~Replizierbarer Crash bei mehrmals hintereinander fallen, Level 3~~
* ~~Too few particles~~
* ~~Too fast~~
* ~~Garbage in some tiles~~
  * ~~Liegt am offset in negativen Bereich~~
  * ~~Anderen Buffertyp definieren...~~
* ~~Celeste moves to the right/left~~
* ~~Tile flags on left side~~
* ~~Sometimes crashes when falling~~
* Kameraanpassung für HUD Elemente
* Hintergrund für Zeit oben links größer

## Features

* Load gfx
* Palette support for sprites
* Palette blend mode (mask+)
* Palette copy mode (copy+)
* ~~Load second half of map~~
* Load sfx
* ~~Different font~~
* ~~30 FPS mode~~
* Sound effects
* Sound and music on second core
* ~~Camera system~~
* ~~Combine cameras~~
* ~~Menu system~~
  * ~~continue~~
  * ~~return to title~~
  * ~~sound: on/off~~
  * (colors: low/high)
  * (scanlines: on/off)
* ~~Intro animation~~

## Später

* Test der Funktionen rand, P8Floor und dem Ergebnis von / in Bezug auf number statt float
* Workaround für out of buffer durch richtige Lösung ersetzen (sollte Hintergrundfarbe liefern, wenn außerhalb des Grafikspeichers gelesen wird)

## Version 1.0

* Flash file system support für PicoSystem SDK
* Save state
* Flash save support
* Run export script during build
* High color mode
* Leuchtende Beeren
* Scanlines
* B-Sides Level, nach Komplettieren von Level 8 oder Sammeln von x Beeren
  * heimlich starten durch X+Y statt A+B
