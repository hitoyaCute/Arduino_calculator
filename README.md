This project is originally for school project, but interesting enough to be invested on and give some time to get serious

# this project contains the following
- simple arduino UI
- expression evaluator (calculator but for CS people)
- and usage of the OLED

# this project uses
- arduino uno r3
- SSD1306 0.91 OLED display (super small)
- 4x4 keypad (not enough)
- GNU make for build, upload, monitor
- arduino-cli (cus why not)

on this project i use all my knowledge in computer engineering and programming skill to explore embeded for the first time

# compile
first prerequisite
- arduino-cli
- GNU make

compilation
```console
$ make # or `make compiile` to compile all
$ make upload # or add `PORT=/dev/<PORT>` to enter without promting, this will also monitor and compile
$ make monitor # or add `PORT=/dev/<PORT>` to enter without promting
```
