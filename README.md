Material List:
N64 Console (technically not required, but the project is useless without one)
N64 Controller (can be broken, for donor wire)
Arduino DUE
Arduino USB Host Sheild 2.0
PS5 Controller (other USB controllers can be used with modification of the souce code)
Breadboard
10k ohm pull up resistor
USB-A to USB-C wire.

Hardware Setup:
Connect the USB Host Sheild 2.0 to the Arduino DUE
Plug the USB-A to USB-C cable into the Host sheild
Blug the USB-C side into the PS5 Controller
Cut the cord off of your N64 Controller
  Connect the White wire to the breadboard then to Pin 12 of the Arduino
  Connect the 10k ohm pull-up resistor connected to the 3.3V line of the arduino
  Connect the pull up resistor to the white wire
  Connect the black wire to the GRND of the Arduino

The button mappings for the controller are as follows:
PS5            >     N64
======================================================
X              >     A

Square         >     B

LB             >     Z

Menu           >     Start

Up             >     Up

Down           >     Down

Left           >     Left

Right          >     Right

LT             >     L

RB             >     R

R-Stick Up     >     C Up

R-Stick Down   >     C Down

R-Stick Left   >     C Left

R-Stick Right  >     C Right

L-Stick        >     Analog Stick

