#include <Arduino.h>
#include "actuator.h"
#include <Wire.h>
//This is the Cpp file for the actuator class


//Constructer function for the class
Actuator::Actuator(byte encAddr, int oPin){
        pinMode(oPin, OUTPUT);                                  //Defines output pin number

        slaveadress = encAddr;                                      //Updates the pin value
        outPin     = oPin;
}


//PID algorithm function
void Actuator::computePID(){     
        currentTime = millis();                                    //get current time
        elapsedTime = (double)(currentTime - previousTime);        //compute time elapsed from previous computation
        
        error = _setPoint - angle;                                 // determine error
        cumError += error * elapsedTime;                           // compute integral
        rateError = (error - lastError)/elapsedTime;               // compute derivative
 
        double out = _kp*error + _ki*cumError + _kd*rateError;     //PID output               
        
        if (out > maxAngle)
        {
                out = maxAngle;
        }

        if (out < minAngle)
        {
                out = minAngle;
        }
        
        
        lastError = error;                                        //remember current error
        previousTime = currentTime;                               //remember current time

        output = out;
 
}


//Sets a new setpoint
void Actuator::setSetpoint(int r){
        _setPoint = r;
}

//Sets a output, generated by pid
void Actuator::setOutput(){
        output= (output*255.0)/360.0;
        analogWrite(outPin, output);
}

//Sets new PID parameters
void Actuator::setParameters(double kp, double ti, double td ){  //Oppdaterer regulator parameterene
        _kp=kp;
        _ki=ti;
        _kd=td;
}

float Actuator::getAngle(){
        return angle;
}       



//Gets the current actuator position
void Actuator::readAngle(){           //Gjøres ikke så enkelt som dette, må se hvilken enkoder vi ender opp med
    Wire.beginTransmission(slaveadress);   //starts transmition with slaver nr. i
    int available = Wire.requestFrom(slaveadress, (uint8_t)2);  //requests bytes from slave
  
    if(available == 2)                //Checks if 2 bytes are avavible
    {
       encData = Wire.read() << 8 | Wire.read();       //Reads upper and lower byte and converts to int (0-1023)
    }
    else                                                  //Error in transmition
    {
      Serial.print("Unexpected number of bytes received: ");
      Serial.println(available);
    }

    int result = Wire.endTransmission();                //end transmition and chack if sucessfulll
    if(result)
    {
      Serial.print("Unexpected endTransmission result: ");
      Serial.println(result);
    }
     
     angle = (360.0*(float)encData)/1023.0;     //Converts to degrees (0-360)
}
