//============================================================================
// Name        : MouseControl.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include "NiTE.h"
#include "NiteSampleUtilities.h"
#include <linux/input.h>//For calling uinput module to create a virtual mouse and write commands to it.
#include <fcntl.h>//Contains some basic definitions used to open the virtual device in read and write mode.
#include <unistd.h>

int main(int argc, char** argv) {
	nite::HandTracker handTracker;
	nite::Status niteRc;
	struct input_event eventx, eventy, event_click, event_end; //Variables that move the mouse cursor and for click.
	memset(&eventx, 0, sizeof(eventx)); //Reserve memory for event_x
	memset(&eventy, 0, sizeof(eventy)); //Reserve memory for event_y
	memset(&event_click, 0, sizeof(eventy)); //Reserve memory for event_click
	memset(&event_end, 0, sizeof(event_end)); //Reserve memory for event_end
	int lastX = 0, lastY = 0, moveX = 0, moveY = 0;
	int lastZ = 0, moveZ = 0;
	int firstGoFlag = 1;
	int btn_click = 1;
	int clicked = 0;

	int fd = open("/dev/input/event3", O_RDWR); //Open the eventx input mouse device for reading and writing.
	if (fd < 1) //if above function returns an error, error handling.
	{
		printf("-1");
		return -1;
	}

	niteRc = nite::NiTE::initialize();
	if (niteRc != nite::STATUS_OK) {
		printf("NiTE initialization failed\n");
		return 1;
	}

	niteRc = handTracker.create();
	if (niteRc != nite::STATUS_OK) {
		printf("Couldn't create user tracker\n");
		return 3;
	}

	handTracker.startGestureDetection(nite::GESTURE_WAVE);
	handTracker.startGestureDetection(nite::GESTURE_CLICK);
	printf("\nWave or click to start tracking your hand...\n");

	nite::HandTrackerFrameRef handTrackerFrame;
	while (!wasKeyboardHit()) {
		niteRc = handTracker.readFrame(&handTrackerFrame);
		if (niteRc != nite::STATUS_OK) {
			printf("Get next frame failed\n");
			continue;
		}

		const nite::Array<nite::GestureData>& gestures =
				handTrackerFrame.getGestures();
		for (int i = 0; i < gestures.getSize(); ++i) {
			if (gestures[i].isComplete()) {
				nite::HandId newId;
				handTracker.startHandTracking(gestures[i].getCurrentPosition(),
						&newId);
			}
		}

		const nite::Array<nite::HandData>& hands = handTrackerFrame.getHands();
		for (int i = 0; i < hands.getSize(); ++i) {
			const nite::HandData& hand = hands[i];
			if (hand.isTracking()) {
				printf("%d. (%5.2f, %5.2f, %5.2f)\n", hand.getId(),
						hand.getPosition().x, hand.getPosition().y,
						hand.getPosition().z);

				if (firstGoFlag) {
					firstGoFlag = 0;
					printf("FirstGoFlag!!!!!!\n");
					lastX = hand.getPosition().x;
					lastY = hand.getPosition().y;
					lastZ = hand.getPosition().z;
				} else {
					eventx.type = EV_REL; //set the type of the eventx to relative.
					eventx.code = REL_X; //tell the code of eventx to relative value of x.
					eventx.value = moveX; //the amount of movement in the x axis is stored in value.
					eventy.type = EV_REL;
					eventy.code = REL_Y;
					eventy.value = -moveY;

					event_click.type = EV_KEY; //set the key as left
					event_click.code = BTN_LEFT; //set the code/event as button left  

					if (moveZ > 30) {

						if	(clicked) {
							event_click.value = 0;
							printf("click Released !!!!!!!!!!!!!!!!!!!!!");
							clicked = 0;
						}
						else {
							event_click.value = 1;
							printf("clicked !!!!!!!!!!!!!!");
							clicked = 1;
						}
					}

					event_end.type = EV_SYN;
					event_end.code = SYN_REPORT;
					event_end.value = 0;

					int c = write(fd, &eventx, sizeof(eventx)); // Move the mouse in the x direction.
					if (c < 1)
						printf("1\n"); //error handling.

					c = write(fd, &eventy, sizeof(eventy)); // Move the mouse in the y direction.
					if (c < 1)
						printf("2\n"); //error handling.

					c = write(fd, &event_click, sizeof(event_click)); // click on the screen.
					if (c < 1)
						printf("3\n"); //error handling.

					c = write(fd, &event_end, sizeof(event_end)); // Show move on the screen.
					if (c < 1)
						printf("4\n"); //error handling.


					moveX = hand.getPosition().x - lastX;
					moveY = hand.getPosition().y - lastY;
					moveZ = hand.getPosition().z - lastZ;
					lastX = hand.getPosition().x;
					lastY = hand.getPosition().y;
					lastZ = hand.getPosition().z;
					printf("\t\tmoveX = %d moveY = %d moveZ = %d\n", moveX,
							moveY, moveZ);
				}
			}
		}
	}

	nite::NiTE::shutdown();

}
