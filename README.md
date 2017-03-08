# Base programs for Real Time projects with Allegro Library

[![N|Solid](https://cldup.com/dTxpPi9lDf.thumb.png)](https://nodesource.com/products/nsolid)
Tanks to [Dillinger](http://dillinger.io/) for the .MD file.
### Content of this repo
- *pthread_base*: test to check if everithing works well on your pc.
- *allegro_base*: drawing geometry, interactive text on screen, mouse integration.
- *Balls*: simulation of balls that fall cause of gravity and bounce on the ground.
- *Flies*: many flies that move randomly on a box.


Don't forget to run everything as superuser (otherwise threads doesn't spanw!):

```sh
$ sudo make clean && sudo make run
```