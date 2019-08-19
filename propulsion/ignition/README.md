# Ignition system programs and schematics

Name says it all.

Schematic and PCB design are done using [KiCAD](http://kicad-pcb.org/).


## Files

* ```schematic/```  --- Schematics for ignition system
* ```comparator-circuits/```  --- Schematics and PCB design files for the
  continuity tester circuits used in the ignition system

### Things to know about the schematics

* Suggested changes to the schematics are presented in the ```future``` branch.


## Explanation of system

> _Bug avnoks for further details._

The ignition system consists of three parts: the part to light the ematch,
the part to open the oxidizer ball valve,
and the radio device (the "football") that controls the first two parts.
The system is designed around the following principles:

* Safety is implemented in hardware.
  The system's safety should arise from its physical properties,
  not from software checks.
  > Use physical switches as opposed to if statements.
  Never should software be the only thing preventing an unauthorised launch.
* The system should fail safe.
  Should a component of the system stop working,
  it should do so in a way that does not compromise the rest of the system
  and does not cause an unauthorised launch.
* Ultimate control of the system and the launch of the rocket
  should reside in one person.
  This is accomplished by giving the one key
  that arms all switches throughout the system and launches the rocket
  to only one person who is also the last person to leave the pad.
* The system should communicate unambiguously.

### Ematch

The ematch part of the system consists of a battery,
an ematch,
and circuitry to prevent the battery from lighting the ematch
when it's not supposed to.
The battery is housed alongside a relay, radio, and other circuitry
in a relay box located 50 imperial feet away from the rocket
(the radio recieves the command to light the ematch
and activates the relay to connect the battery
to the wires going to the ematch).
The ematch is buried in a block of propellant
suspended in the fuel grain of the rocket;
it is connected to the relay box
through a long wire and some more safety circuitry.
The safety circuitry consists primarily of voltage warning signals,
two switches,
and continuity testers

#### Voltage warning signals

A buzzer and a white LED
in the relay box
and another buzzer and white LED in the rocket
warn launch personnel when there is voltage across the wire
that supplies the ematch with electricity.
Should launch personnel observe these buzzers and LEDs activate,
they should immediately retreat from the pad
and await further instructions.

**FIXME:** Identify frequency of buzzer.

#### Switches

Two key switches are built into the circuit:
a shunt switch,
which is on the relay box,
and a safety switch,
which is on the rocket.
The shunt switch shorts out the wires
that supply electricity to the ematch.
It should be closed whenever personnel are closer to the rocket
than the relay box is (within 50 feet of the rocket).
The safety switch interrupts the wires supplying electricity to the ematch.
It should remain open unless personnel
are about to retreat to a safe area for launch.
Personnel should never close the safety switch
if they observe the voltage warning signals described above.

#### Continuity testers

The continuity testers detect when the switches,
relay,
and ematch in the system have continuity.
For the relay and switches,
the testers sound a buzzer and light an LED
when the component monitored is in an unsafe state
(unsafe means that fewer things need to happen
in order for the rocket to launch).

The continuity testers are built from comparators.
The theory behind them can be found at David Johnson-Davies's
[post on a similar tester.](http://www.technoblogy.com/show?1YON)
In short,
one of the comparator's inputs is tied to a voltage reference
created using a voltage divider.
The other input is connected to one side of the device
whose continuity is to be tested;
the ground of the comparator
is connected to the other side.
When there is continuity,
the second input of the comparator is shorted to ground,
which is less than the reference voltage.
This activates the comparator's output,
which turns on various buzzers and LEDs.

The LM393 comparators used in the system
is an open collector device ---
its output conducts to ground when it is active.


### Motorised Ball Valve (MBV)

Radio module receives launch signal from football.
Waits a bit for relay box to light ematch
and start combustion of fuel grain.
Then checks to see if there still is continuity across the ematch.
If there isn't (match has burnt through),
opens ball valve.

**FIXME:** Where's all the code?

### Football

Radio transceiver that transmits launch command to MBV and relay box.
Should in future receive and display continuity tester data
back from relay box and rocket
(state of relay, switches, and ematch).


## Things to fix


- [ ] **TODO:** Teach everyone on the team what the different signals
                (voltage, continuity)
                look and sound like.
- [ ] **TODO:** Post labels on system describing what to do if a particular signal is observed.

- [ ] __TODO:__ Check design of system in ```future``` branch,
                then order new PCBs.
- [ ] __TODO:__ Assemble modified system

- [ ] __TODO:__ Label physical components with designators from schematics
- [ ] __TODO:__ Build physical housing for system

- [ ] __VERIFY:__ Does MBV controller know when ematch has burnt through?

- [ ] __FIXME:__ Optoisolate inputs between different parts of system
- [ ] __FIXME:__ 
