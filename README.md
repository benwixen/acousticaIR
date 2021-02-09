# acusticaIR
Control your Geneva Acustica Lounge from you Samsung TV remote.

With an Arduino, an IR sensor, a soldering iron, and some optocouplers you can make your Acoustica remote controlled.

The code includes the custom *infra* library that parses incoming IR-signals, and exists in multiple versions (for fun and learning):

* arduino-cpp - Depends on the Arduino framework
* avr-c - Plain AVR-implementation in C
* avr-cpp - Plain AVR-implementation in C++

![Acustica Lounge](doc/acustica.jpg?raw=true "Acustica Lounge")

The IR sensor is attached below the speaker, so that the design is unaltered.

A rough breadboard recipe is presented below:

![Acustica IR Schematic](doc/acusticaIR.png?raw=true "acusticaIR breadboard")

You need to open up the box, and solder on the optocoupled connectors below the button pad PCB.