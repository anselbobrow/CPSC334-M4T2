# Night/Day Diorama
## Sachi, Ansel, Siena

## Intro & Usage

To start this diorama, ensure all power is plugged in. This should be batteries, two ESPs, and the wall power. Once powered, press the button (in aluminum foil at the bottom left of the diorama), which will start the kinetic sculpture.

Once started, the diorama will be at night for a few moments, so the clouds will move, the stars will spin, and the moon will shine with the curtains open. After ~3 seconds, the cat will start strolling across the enclosure, while the sun rises (changing colors as it rises and sets), and the curtains close to hide the stars and moon. The clouds continue to move up and down. 

Once the sun sets, night will return for a few seconds, where the curtains open, and we can see the stars and moon again. The cat stays still. 

Finally, the piece will return to original state; the sun will move back, curtains close to hide night features, and the cat returns home. 

The diorama will stop reset after it is complete, so you can once again press the button to start it!

## Code Design
ESP #1, powering the sun, moon, and cat, runs `sun_and_belt.ino`. ESP #2, powering the stars, curtains, and clouds, runs `clouds_and_multithreading.ino`. They both run state machines to match the day and night cycle, and they use multithreading to power multiple (6) motors at once. 

## Videos
Demo: https://youtu.be/nfYbNIE1trE

360 view: https://www.youtube.com/watch?v=4GZB0_U9o7I&feature=youtu.be
