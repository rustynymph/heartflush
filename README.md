# heartflush
### A heart rate activated toilet flusher
##### Author: Annie Kelly ([personal website](http://rustynymph.com) / [academic website](http://anniekelly.me))

This project is being designed for a live performance where people have to collaborate using their heart rates to flush a toilet. When the 3 pulse sensors detect heart rates that average within a certain threshold, the servo pulls up on the toilet chain therefore flushing the toilet.

Hardware:  
* Arduino Uno  
* 1 [Servo (Generic High Torque)](https://www.sparkfun.com/products/11965)
* 3 [Pulse Sensors](https://pulsesensor.com/)

Software:  
Uses the Arduino [Servo](https://www.arduino.cc/en/reference/servo) and [PulseSensorPlayground](https://github.com/WorldFamousElectronics/PulseSensorPlayground) libraries.

See [heartflush.ino](./heartflush.ino) for the source code.

Project diagram:  
![Diagram](./diagram.jpg)
