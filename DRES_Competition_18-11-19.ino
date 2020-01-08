/* DRES Competition | Darcy MacNeill | B00688504
 *  
 *  This program monitors a hall sensor as well as a 
 *  motor input, and outputs these values to the 
 *  serial port in the form (float,float,float,float,float,float).
 */
 
 #define sample 3000              //Number of seconds between output 

 const int button_pin = 2;        //Button variables 
 int button_counter = 0;
 int button_state = 0; 
 int last_button_state = 0; 
 
 #define hall_upper_thresh 3.0    //Hall effect sensor variables
 #define hall_lower_thresh 1.0 
 #define hall_pin 7 
 const int LED = 5;
 float hall_voltage; 
 float rpm_instant = 0; 
 float rpm_avg = 0; 
 float rpm_sum = 0;
 float rpm_max = 0;
 float rpm_max_avg = 0;
 int rpm_counter = 0; 

 #define motor_pin 6              //Motor variables
 float motor_voltage;
 float power_instant = 0; 
 float power_avg = 0; 
 float power_sum = 0; 
 float power_max = 0;
 float power_max_avg = 0;
 int power_counter = 0; 

 long prev_millis = 0;            //Other variables
 int count = 0; 
 int flag; 

 int reset_var = 0;               //State tracker: Reset var = 1 when transmitting, 0 otherwise

 void setup(){
  Serial.begin(9600);
  pinMode(button_pin,INPUT);
  pinMode(LED,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(3),reset,RISING);
  wait();                         //Wait until button is pressed to begin
 }

 void loop(){

  reset_var = 1;                                    //State var is 1, meaning we are transmitting information.
  
  unsigned long current_millis = millis();
  long timer = current_millis-prev_millis;

  int hall_in = analogRead(hall_pin);
  hall_voltage = hall_in*(5.0/1024.0);

  if(hall_voltage >= hall_upper_thresh || hall_voltage <= hall_lower_thresh){
    if(!flag){
      count++;
      digitalWrite(LED,HIGH);
      flag = 1; 
      delay(15);
      digitalWrite(LED,LOW);
    }
  }else{
    flag = 0;
  }

  if(timer >= sample){
    prev_millis = current_millis;
    int motor_in = analogRead(motor_pin);
    motor_voltage = motor_in*(5.0/1024.0);
    calcs(); 
    //debug();
    transmit();
    count = 0; 
  }
 }

 void debug(void){
  Serial.print("H_Voltage: ");      //Print all the RPM stuff
  Serial.print(hall_voltage);
  Serial.print("  count: ");
  Serial.print(count);
  Serial.print("  rpm_inst: ");
  Serial.print(rpm_instant); 
  Serial.print("  rpm_avg: "); 
  Serial.print(rpm_avg);
  Serial.print("  rpm_max: ");
  Serial.println(rpm_max);
  Serial.print("M_Voltage: ");      //Print all the power stuff on another line
  Serial.print(motor_voltage);
  Serial.print("  Power_Inst: ");
  Serial.print(power_instant); 
  Serial.print("  Power_Avg: "); 
  Serial.print(power_avg); 
  Serial.print("  Power_max: ");
  Serial.println(power_max);
  Serial.println("");               //New line so it's easy to see
 }

 void calcs(void){
  rpm_counter++;
  rpm_instant = 60*(count / (sample/1000));  
  rpm_sum = rpm_sum + rpm_instant; 
  rpm_avg = rpm_sum / rpm_counter; 
  if(rpm_instant > rpm_max){                  //Is this the max rpm? 
    rpm_max = rpm_instant;
  }
  if(rpm_avg > rpm_max_avg){                  //Is this the highest avg RPM so far? 
    rpm_max_avg = rpm_avg;
  }
  
  power_counter++; 
  power_instant = motor_voltage*(motor_voltage/1003.7); //Resistance of motor = 3.7 ohms
  power_sum = power_sum + power_instant; 
  power_avg = power_sum / power_counter; 
  if(power_instant > power_max){              //Is this the max power? 
    power_max = power_instant;      
  }
  if(power_avg > power_max_avg){              //Is this the highest avg power so far?
    power_max_avg = power_avg; 
  }
 }

 void transmit(void){
  Serial.print(rpm_avg);          //Instant avg rpm
  Serial.print(",");
  Serial.print(power_avg);        //Instant avg power 
  Serial.print(",");
  Serial.print(rpm_max);          //Instant max rpm
  Serial.print(",");
  Serial.print(rpm_max_avg);      //Max avg rpm
  Serial.print(",");
  Serial.print(power_max);        //Instant max power 
  Serial.print(",");
  Serial.print(power_max_avg);  //Max avg power
  Serial.print(",");
  Serial.println(reset_var);    //Are we resetting for another trial (For use in the Processing code)
 }

 void reset(void){
  button_counter = 0;             //Reset all variables that change during the measuring process
  button_state = 0; 
  last_button_state = 0; 
  hall_voltage = 0; 
  rpm_instant = 0; 
  rpm_avg = 0; 
  rpm_max = 0; 
  rpm_max_avg = 0;
  power_instant = 0; 
  power_max = 0; 
  power_max_avg = 0; 
  power_avg = 0;
  rpm_counter = 0; 
  rpm_sum = 0; 
  power_counter = 0; 
  power_sum = 0;
  prev_millis = 0; 
  count = 0; 

  reset_var = 0;                //Change the state to 0, meaning we no longer transmit.

  wait();
 }

 void wait(void){
  reset_var = 0;                //State begins at 0, so that we do not transmit.
  while(1){
    button_state = digitalRead(button_pin);
    if(button_state != last_button_state){
      if(button_state == HIGH){
        button_counter++;
        if(button_counter >= 1){
          break;
        }
      }
    }
  }
 }
