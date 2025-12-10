# AURE Beta (WIP)
A domain-agnostic, audio-reactive application framework designed for musical live-coding environments.

AURE is currently available only for Mac OS X systems.



Screen shot from AURE
![AURE](AURE-01.jpg)

# Instructions
1. Compile the app using openframeworks or download [here]() for Mac.
2. Open the AURE app and follow the instructions on startup.
3. Open Sonic Pi or any other live-coding tool capable of sending OSC
4. Set the OSC port 9333 to send messages. Run `use_osc "localhost", 9333` on Sonic Pi.
5. Use *osc "/par1"* messages to morph scene content inside `live_loops` to sync audio with generative visuals.
6. Use *osc "/fx_on"* messages to morph scene content inside `live_loops` to apply post processing effects.


[comment1:] https://youtu.be/aaX35AG2F0E

[comment2:] https://youtu.be/Ls-0NSt6PM0
