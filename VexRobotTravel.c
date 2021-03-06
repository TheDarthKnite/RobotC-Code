//This code was designed to run on a vex robot. 
//it implements p/PI controllers with sensro readings to let the robot complete the obstacle course (in image in google folder)
//I worked on this code with my teammate Sam Howarth for the MECHENG 201 course

#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    _lightLeft,      sensorReflection)
#pragma config(Sensor, in2,    _lightMid,       sensorReflection)
#pragma config(Sensor, in3,    _lightRight,     sensorReflection)
#pragma config(Sensor, dgtl1,  _btnStop,        sensorTouch)
#pragma config(Sensor, dgtl2,  _btnStart,       sensorTouch)
#pragma config(Sensor, dgtl3,  _sonar,          sensorSONAR_mm)
#pragma config(Sensor, dgtl5,  _encRight,       sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  _encLeft,        sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  _LED_Right,      sensorDigitalOut)
#pragma config(Sensor, dgtl10, _LED_Left,       sensorDigitalOut)
#pragma config(Sensor, dgtl11, _armLimit_low,   sensorTouch)
#pragma config(Sensor, dgtl12, _armLimit_high,  sensorTouch)
#pragma config(Sensor, I2C_1,  _armEncoder,     sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port2,           _motorArm,      tmotorVex269_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port7,           _motorRight,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           _motorLeft,     tmotorVex393_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// -----------  DO NOT MODIFY anything ABOVE this line! ----------- //

#include "backgroundProcesses2021.c"

/* Timers:
T_1	Free
T_2	Free
T_3	Free
T_4	Used by checkButtons() -- DO NOT USE
*/

// ---------------------- Defining physical robot parameters --------------------------
// Update these numbers to match the physical robot (information found in the lab manual)
int drivingWheelDiameter = 103;		// diameter of the driving wheels [mm]
int robotWidth = 250;							// width of the robot including the wheel thickness [mm]
int wheelWidth = 22;							// width of the driving wheel [mm]
float drivingWheelRatio = 0.6;	// ratio of wheel shaft rotations to wheel encoder shaft rotations
float armRatio = 0.04761904762;						// ratio of arm shaft rotations to arm encoder shaft rotations
float wheelEncoderCounts = 360;	// number of encoder ticks per 1 revolution of wheel encoder
float armEncoderCounts = 240.448;		// number of encoder ticks per 1 revolution of arm encoder

// ------------------------------------------------------------------------------------

// Functions
void waitForStart();
void driveDistance(float distance);
void sonarDriving(float target, float Kp);
int count_to_mm(int encoderCount);
void turnRobot(float angle);
void rotateArm(float angle);
float count_to_angle(float encoderCount);
int getPosition();
void drivePCont(float target, float Kp); // driving with a p controller.
void followLine();
void drivePICont(float target, float Kp, float Ki);
void turnArm(float angle);
int exitFunction(int error, int tollerance);

// Globla variables
int initialSonar = 0;
int passPI = 0;


// *************************** Main Task ***************************************
task main() {
	//Background Tasks
	startTask(checkArm);       // DO NOT DELETE THIS LINE
	startTask(checkButtons);   // DO NOT DELETE THIS LINE
	// DO NOT PUT YOUR CODE BEFORE THIS LINE!!!


	waitForStart();
	//armup
	armup(50);
	//turns arm to angle to pick up
	turnArm(7);
	//moves to a good distance of the can to pick up can
	sonarDriving(280,1);
	delay(500);
	//pick up can
  armUp(50);
  //drive back to initial position
	sonarDriving(initialSonar - 40,1);
	delay(500);
	//turnleft
	turnRobot(100);
	//drives so near brown line
	drivePICont(700,2.5, 0.09);
	delay(500);
	//turns to face brown line
	turnRobot(-90);
	delay(500);
	////goes to black line, stops, then follows brownline till it hits the other blackline
	followLine();
	//turnRobot(-10);
	//drives to target
	drivePICont(550,2.5,0.09);
	// drops can
	turnArm(0);
	delay(1000);
	//drive back
	drivePCont(-100,1);
	//armup
	armup(50);
	//turn Right
	turnRobot(-90);
	//drive forward using PI controller
	drivePICont(1385,2,0.01);
	//turn Right
	turnRobot(-90);
	//drive to end zone
	drivePICont(513,2.5,0.09);

	stopAllTasks();	// end of program - stop everything
}
// *****************************************************************************

float count_to_angle(float encoderCount){
	//calculates the angle, given arm encoder counts.
	float armAngle;

 float x = (encoderCount*360);
 //printf("%f", x);
 float y = (x*armRatio);
 //printf("%f", y);
	armAngle = y/armEncoderCounts;
	//printf("%f", armAngle);
	//printf("%c", "test2");
	//printf("%d", armAngle);
	return armAngle;
}

void turnArm(float angle){

	//declaration of variables
	int currentAng, error, u=0;
	int Pwr=0;
	float target;
	//kp value we are using
	float Kp=2;
	float maxAngle=48;
	target = angle-20;
	//set encoder to arm angle at highest

 setSensor(EncoderArm, 673); //set encoder arm at 673 counts

	printf("%d", (((maxAngle*21)/360)*(armEncoderCounts));
	printf("%f", readSensor(EncoderArm));

	do{
	//checks current angle
	currentAng = count_to_angle(readSensor(EncoderArm))+48;
	//printf("%d", currentAng);

	//compares current encoder count to the target
	error = target-currentAng;
	//printf("%d", error);

	//uses above compariason to find motorpower
	u=Kp*error;
	//Saturates power  level
	Pwr = saturate(u,-50,50);
	//gives motor required power
	motorPower(ArmMotor,Pwr);

	}while((currentAng < (target - 20)) || (currentAng > (target + 20));
	motorPower(ArmMotor,0);
	}


void waitForStart(){

	// will continuously delay for 50 millisedconds when start button is not pressed.
  // when start button is pressed, the while loop is no longer true and the function waits 0.5s before ending.
		while (readSensor(StartButton)==0){
					delay(50);
		}
		setSensor(leftLED, 1);
		delay(1000);
		setSensor(leftLED, 0);
}


int count_to_mm(int encoderCount) {
	// calculates the total rotations the wheel does, then multiplies this by the
	// shaft:encoder rotation ratio
	float wheelRotations, DistanceTravelled;

	wheelRotations = drivingWheelRatio*encoderCount;
	DistanceTravelled = (drivingWheelDiameter * PI * wheelRotations)/wheelEncoderCounts;
	return DistanceTravelled;
}



int getPosition(){
 int position;
 position = (count_to_mm(readSensor(LeftEnc)) + count_to_mm(readSensor(RightEnc))) /2;
 return position;
}


void drivePCont(float target, float Kp){
 	// initialise variables
	float currentPos, error, u=0;
	int Pwr = 0;
	int encError, deltaU;
	float Kp_straight = 0.5;

	// zero encoders
	setSensor(LeftEnc, 0);
	setSensor(RightEnc, 0);

	do{
		currentPos = getPosition(); // get current position from wheel encoders
		error = target - currentPos; // update the error value (E=R-Y)
		u = Kp*error; // calculate the control effort

		encError = readSensor(RightEnc)-readSensor(LeftEnc);
		deltaU = (int) Kp_straight*encError;

		Pwr = (int) saturate(u, -50, 50); // saturate motor power
		// sending power to motors
		motorPower(LeftMotor, Pwr + deltaU);
		motorPower(RightMotor, Pwr - deltaU);
		delay(100); // repeat at ~10Hz
	}	while((currentPos < (target-20)) || (currentPos > (target + 20)));
			motorPower(LeftMotor,0);
		motorPower(RightMotor, 0);
}



void drivePICont(float target, float Kp, float Ki){
 	// initialise variables
	float currentPos = 0;
	float error = 0;
	float u=0;
	int Pwr = 0;
	int encError = 0;
	int deltaU = 0;
	float Kp_straight = 1;
	int sum = 0;

	// zero encoders
	setSensor(LeftEnc, 0);
	setSensor(RightEnc, 0);

	datalogClear();

	do{
		currentPos = getPosition(); // get current position from wheel encoders
		error = target - currentPos; // update the error value (E=R-Y)

		if ((u >= 50) || (u <= -50)){
	sum = 0;
		}
		else {
			// Error sum
		sum += error;
		}

		u = (Kp*error) + (Ki*sum); // calculate the control effort

		encError = readSensor(RightEnc)-readSensor(LeftEnc);
		deltaU = (int) Kp_straight*encError;

		Pwr = (int) saturate(saturate(u,-150, 150), -30, 30); // saturate motor power

		// sending power to motors
		motorPower(LeftMotor, Pwr + deltaU);
		motorPower(RightMotor, Pwr - deltaU);

delay(50);
		datalogDataGroupStart();
		datalogAddValue(0, target);
		datalogAddValue(1, currentPos);
		datalogAddValue(2, error);
		datalogAddValue(3, u);
		datalogAddValue(4, Pwr);
		datalogDataGroupEnd();

	}	while (exitFunction(error, 1));  //while((currentPos < (target)) || (currentPos > (target)));
		motorPower(LeftMotor, 0);
		motorPower(RightMotor, 0);
}

int exitFunction(int error, int tollerance){
	int exit = 1;

	if (abs(error) > tollerance){
		exit = 1;
	}
	else if ((error <= tollerance) && (error >= -tollerance)){
		passPI = 1 + passPI;
		printf("%f", passPI);
		if (passPI == 30){
			exit = 0;
		}
		else {
			exit = 1;
		}
	}
	return exit;
}



void sonarDriving(float target, float Kp){  // steady state error of 20cm on RVW

	// initialise variables
	float currentPos, error, u=0;
	int Pwr = 0;
	int encError, deltaU;
	float Kp_straight = 1;

	// zero encoders
	setSensor(LeftEnc, 0);
	setSensor(RightEnc, 0);

	// Update initialSonar
	if (initialSonar == 0){
		initialSonar = readSensor(sonarSensor);
	}

	do{
		currentPos = readSensor(sonarSensor); // get current position from sonar sensor
		error = currentPos - target; // update the error value (E=R-Y)
		u = Kp*error; // calculate the control effort

		encError = (readSensor(RightEnc)-readSensor(LeftEnc));
		deltaU = (int) Kp_straight*encError;

		Pwr = (int) saturate(u, -30, 30); // saturate motor power

		// sending power to motors
		motorPower(LeftMotor, Pwr + deltaU);
		motorPower(RightMotor, Pwr - deltaU);

	}	while((currentPos < (target-40)) || (currentPos > (target + 40)));
		motorPower(LeftMotor, 0);
		motorPower(RightMotor, 0);
}


void turnRobot(float angle){
	//declaration of variables
	float currentAng, error, u=0;
	int Pwr=0;
	int encError, deltaU;
	int target ,arclength;
	//kp value we are using
	float Kp=1.5;

//target angle

		target=angle;

	//set encoders to 0
	setSensor(LeftEnc,0);
	setSensor(RightEnc,0);
	do{
		//runs code if angle is positive

			//checks current encoder count
			currentAng=((count_to_mm(readSensor(RightEnc))*360)/(PI*(robotWidth-wheelWidth));

			printf("%f", currentAng);
			//compares current encoder count to the target
			error=target-currentAng;
			//uses above compariason to find motorpower
			u=Kp*error;
			//compares how much each encoder shaft has turned
			//encError=readSensor(LeftEnc)+readSensor(RightEnc);
			////uses above comparison to add power values to motors, to make them more equal
			//deltaU = (int)(Kp*encError);
			//Saturates power level
			Pwr = saturate(u,-50,50);
			//gives motor required power
			motorPower(LeftMotor,-Pwr);
			motorPower(RightMotor,Pwr);


	}while((currentAng>(target+8))||(currentAng<(target-8)));
		motorPower(LeftMotor,0);
		motorPower(RightMotor, 0);
}




void followLine(){

int left, mid, right;

	// line colour values:
	// Left = 2125 (max)
	// Mid = 2140 (max)
	// Right = 2190 (max)

// reading values from light sensors.
left = readSensor(LeftLight);
mid = readSensor(MidLight);
right = readSensor(RightLight);
//upper light values
int upperLeft = 2300;
int lowerLeft = 1400;
int upperMid = 2300;
int lowerMid = 1400;
int upperRight = 2300;
int lowerRight = 1400;
int pwr = 25;

//moves forward while no black
while(left<lowerRight&&mid<lowerMid&&right<lowerRight){
	left = readSensor(LeftLight);
	mid = readSensor(MidLight);
	right = readSensor(RightLight);
	motorPower(RightMotor,pwr);
	motorPower(LeftMotor,pwr);
}
	motorPower(RightMotor,0);
	motorPower(LeftMotor,0);
//stops
delay(1000);
//moves onto brown line
drivePCont(50, 1);
	//runs while sensor senses no black
	while(left<upperLeft&&mid<upperMid&&right<upperRight){
		left = readSensor(LeftLight);
		mid = readSensor(MidLight);
		right = readSensor(RightLight);

		//runs while middle sensor is brown
		if (mid>lowerMid&&mid<upperMid){
			if (left>lowerLeft&&left<upperLeft){
				//when all light sensors read brown turns robot left
				if (right>lowerRight&&right<upperRight){
				motorPower(RightMotor,pwr);
				motorPower(LeftMotor,-pwr);
				}
				//when the left and middle light sensors read brown robot drives straight
				else {
					motorPower(RightMotor,pwr);
					motorPower(LeftMotor,pwr);
				}
			}
			//when the right and middle light sensors read brown robot drives straight
			else if (right>lowerRight&&right<upperRight){
				motorPower(RightMotor,pwr);
				motorPower(LeftMotor,pwr);
			}
			//when the middle light sensor reads brown robot drives straight
			else{
				motorPower(RightMotor,pwr);
				motorPower(LeftMotor,pwr);
			}
		}else if (right>lowerRight&&right<upperRight){
		//when the right and left light sensors read brown robot turns left
			if (left>lowerLeft&&left<upperLeft){
				motorPower(RightMotor,-pwr);
				motorPower(LeftMotor,pwr);
			}
			//when the right light sensor reads brown robot turns right
			motorPower(RightMotor,-pwr);
			motorPower(LeftMotor,pwr);
			//when the left light sensor reads brown robot turns left
		}else if(left>lowerLeft&&left<upperLeft){
		motorPower(RightMotor,pwr);
		motorPower(LeftMotor,-pwr);
		delay(10);
		}
		//when the robot reads no brown, the robot turns left
		else{
			motorPower(LeftMotor,-20);
			motorPower(RightMotor,20);
		}
		delay(10);
	}

}
