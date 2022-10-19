# Todo

## Bug fixes

* Auf Trampolin springen: erzeugt beliebige Objekte
* Wolken zu schnell
* Haare kaputt
* Player sinkt 1 Pixel ein
* Sound lässt sich nicht immer im Menü anschalten
* Ich brauche einen "safe array" durch Überladen des [] Operators

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
* ~~Kameraanpassung für HUD Elemente~~
* ~~Hintergrund für Zeit oben links größer~~
* ~~Offsets durchgehen links/rechts~~
* ~~Button-Event löschen~~
* ~~Double buttons im Startscreen werden nicht abgefragt~~

## Features

* ~~Load sfx~~
* ~~Sound effects~~
* ~~Fadeout für menü~~
* ~~Palette blend mode (mask+)~~
* ~~Palette copy mode (copy+)~~
* ~~Load gfx~~
* ~~Palette support for sprites~~
* ~~Load second half of map~~
* ~~Different font~~
* ~~30 FPS mode~~
* ~~Sound and music on second core~~
* ~~Camera system~~
* ~~Combine cameras~~
* ~~Menu system~~
  * ~~continue~~
  * ~~return to title~~
  * ~~sound: on/off~~
* ~~Intro animation~~
* Flash save support
* Blinken wenn Bildschirm aus

## Später

* Versuchen, Importe rauszuwerfen
*  // rnd() // clever me uses the current battery voltage as a random seed.
* actually we should not access 'private' variables from the picosystem namespace
  * getpen
  * getcamera
  * getblendmode
* todo make a static class to hide some stuff from users of pico8 api and omit init method
* todo NUR fget/set kopieren, und alle uint32_t durch uint_fast16_t ersetzen,
  auch in meinem custom number type
* Load sprite flags
* ~~Statistiktest~~
* Hudcamera-Offset
* Test der Funktionen rand, P8Floor und dem Ergebnis von / in Bezug auf number statt float
* Workaround für out of buffer durch richtige Lösung ersetzen (sollte Hintergrundfarbe liefern, wenn außerhalb des Grafikspeichers gelesen wird)

## Version 1.0

* Menu system
* Run export script during build
* ~~High color mode?~~
* ~~Leuchtende Beeren?~~
* B-Sides Level, nach Komplettieren von Level 16 oder Sammeln von 8 Beeren
  * heimlich starten durch X+Y statt A+B

## Marketing

All that running on a tiny 1 $ chip.