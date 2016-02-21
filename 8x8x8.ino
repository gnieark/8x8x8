/*
* Low level code for cube led 8x8x8
* By Gnieark https://blog-du-grouik.tinad.fr February 2016
* GNU GPL V2
*/

#define pinOE 11
boolean cube[8][8][8];

const int layersPins[]={A0,A1,A2,A3,A4,A5,12,13};
const int busPins[]={0,1,2,3,4,5,6,7};

// IC Pins are "hard coded". here just for info,
const int ICPins[]={8,9,10}; 
//  If you change IC pin configuration, 
//  You'll have to modify const ICtable[9]

//see SN74HC138 specs 
//http://www.ti.com/lit/ds/symlink/sn54hc138.pdf page 2
const byte ICtable[9]={
 B00000000,
 B00000001,
 B00000010,
 B00000011,
 B00000100,
 B00000101,
 B00000110,
 B00000111,
 B00000000 //une façon de boucler sur 0
};

void writeCube(int tdelay){
    static int currentLayer=0;
    unsigned long startedAt = millis();
    while(millis() - startedAt < tdelay){ 

        delay(1);// wait a bit on previous layer
        digitalWrite(layersPins[currentLayer],LOW);//turn off current Layer

        //change currentLayer
        if(currentLayer == 8)
            currentLayer=0;
        else
            currentLayer++;  

        digitalWrite(pinOE, HIGH);//Output Enabled off
        //lines
        PORTB=ICtable[0];//set IC
        for(int ligne=0;ligne < 8; ligne++){
            //write BUS for this leds'line
            for (int led=0;led <8;led++){
                digitalWrite(busPins[led], cube[currentLayer][ligne][led]); 
            }
            PORTB=ICtable[ligne +1]; //74HC574 is writen when clock pin goes LOW to HIGH. SO go to the next value for IC.
        }
        //OE ON
        digitalWrite(pinOE, LOW);
        //layer on
        digitalWrite(layersPins[currentLayer],HIGH);
    }
}
void fillCube(){
 for (int i=0;i<8;i++){
  for(int j=0;j<8;j++){
   for(int k=0;k<8;k++){
       cube[i][j][k]=true;
   }
  }
 }
}
void lowCube(){
 for (int i=0;i<8;i++){
  for(int j=0;j<8;j++){
   for(int k=0;k<8;k++){
       cube[i][j][k]=false;
   }
  }
 }
}

void setup(){
    for(int pin=0; pin<8; pin++){
        pinMode(layersPins[pin], OUTPUT);
        pinMode(busPins[pin], OUTPUT);
    }
    for(int pin=0;pin<3;pin++){
     pinMode(ICPins[pin],OUTPUT);   
    }
    pinMode(pinOE, OUTPUT);
    digitalWrite(pinOE, LOW);
    lowCube();  
}

void loop(){
    //remplir tout le layer haut
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
        cube[7][i][j]=true;   
        }
    }
    writeCube(1000);
    
    //descendre les leds 1 par une
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            for(int layer=7; layer >0; layer--){
                    cube[layer][i][j]=false;
                    cube[layer -1][i][j]=true;
                    writeCube(15);
            }
            
        }
    }
    //monter
        for(int i=7;i>=0;i--){
        for(int j=7;j>=0;j--){
            for(int layer=0; layer <7; layer++){
                    cube[layer][i][j]=false;
                    cube[layer +1][i][j]=true;
                    writeCube(15);
            }
            
        }
    }
}
