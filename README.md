# Trigger box prototype

In this repository, we will be tracking the deployment of the prototype of a trigger box. This trigger box, or a derivative of it, will be used to precisely trigger multiple cameras and a LIDAR sensor for autonomous driving research.

## Who are we?

We (ECK-Group-A) will be working on this, and some other components, for a school project.

## First prototype

We currently have a prototype of the trigger box working on an Arduino Leonardo, using a ATmega32u4 microcontroller. This prototype must be viewed as a proof of concept, and does not yes have all the functionality that our client requires. We will discuss the specifics of the prototype if it becomes relevant.

## Other versions

A second version is currently under development. This is using a STM32 Blue Pill microcontroller. We have decided to use this microcontroller because of the higher clock speed and larger counter/timers. This allows us more precision in the timing of the trigger box.

## Future

Other possible optimized components are also under consideration.

The functionality of the trigger box will be described. This will be available in future commits to this repository, and our final report.


## Use of the trigger box

The trigger box requires a 1Hz 5V pulse signal on the D4 pin of the arduino. It will multiply this signal by 10, to create multiple 10Hz 5V siganls on poort F of the ATmega32u4, wich are pin A5 through A0. A phase offset can be configured with a 1 degree resulution, by edeting the camera_position array in the src/main.cpp file.