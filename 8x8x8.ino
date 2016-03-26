/*
* Low level code for cube led 8x8x8
* By Gnieark https://blog-du-grouik.tinad.fr February 2016
* GNU GPL V2
*/


//************** Low level functions *********************
#define pinOE 11
boolean cube[8][8][8];

const int layersPins[]={A0,A1,A2,A3,A4,A5,12,13};
const int busPins[]={0,1,2,3,4,5,6,7};

// IC Pins are "hard coded". here just for info,
const int ICPins[]={8,9,10};
//  If you change IC pin configuration, 
//  You'll have to modify write cube void

//see SN74HC138 specs 
//http://www.ti.com/lit/ds/symlink/sn54hc138.pdf page 2

void writeCube(int tdelay){
  static int currentLayer = 0;
  unsigned long startedAt = millis();
  while(millis() - startedAt < tdelay) {
    // Wait for a bit on previous layer
    delay(1);

    // Turn off all layers
    for(int pin = 0; pin < 8; pin++){
      digitalWrite(layersPins[pin],LOW);
    }

    // Output Enabled off
    digitalWrite(pinOE, HIGH);

    // Lines
    // Set IC
    PORTB = 0;
    for(int ligne = 0; ligne < 8; ligne++) {
      // Write BUS for this leds'line
      for (int led=0;led <8;led++) {
        digitalWrite(busPins[led], cube[currentLayer][ligne][led]); 
      }

      // 74HC574 is writed when clock pin goes LOW to HIGH. SO go to the next value for IC.
      PORTB = (ligne + 1) & B00000111;
    }

    // Output Enabled on
    digitalWrite(pinOE, LOW);

    // Layer on
    digitalWrite(layersPins[currentLayer],HIGH);
        
    // Change currentLayer
    if(currentLayer == 8) {
      currentLayer = 0;
    } else {
      currentLayer++;        
    }
  }
}

void fillCube() {
  for(int i=0; i < 512; i++) ((byte *)cube)[i] = true;
}

void lowCube() {
  for(int i=0; i < 512; i++) ((byte *)cube)[i] = false;
}

void setup() {
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

//***************** Some animate samples **************************

void drawline(int x1, int y1, int z1, const int x2, const int y2, const int z2, byte value){
  // Bresenham3D from https://gist.github.com/yamamushi/5823518
  int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
  int point[3];
  
  point[0] = x1;
  point[1] = y1;
  point[2] = z1;
  dx = x2 - x1;
  dy = y2 - y1;
  dz = z2 - z1;
  x_inc = (dx < 0) ? -1 : 1;
  l = abs(dx);
  y_inc = (dy < 0) ? -1 : 1;
  m = abs(dy);
  z_inc = (dz < 0) ? -1 : 1;
  n = abs(dz);
  dx2 = l << 1;
  dy2 = m << 1;
  dz2 = n << 1;
    
  if ((l >= m) && (l >= n)) {
    err_1 = dy2 - l;
    err_2 = dz2 - l;
    for (i = 0; i < l; i++) {
      cube[point[0]][point[1]][point[2]] ^= value;
      if (err_1 > 0) {
        point[1] += y_inc;
        err_1 -= dx2;
      }
      if (err_2 > 0) {
        point[2] += z_inc;
        err_2 -= dx2;
      }
      err_1 += dy2;
      err_2 += dz2;
      point[0] += x_inc;
    }
  } else if ((m >= l) && (m >= n)) {
    err_1 = dx2 - m;
    err_2 = dz2 - m;
    for (i = 0; i < m; i++) {
      cube[point[0]][point[1]][point[2]] ^= value;
      if (err_1 > 0) {
	point[0] += x_inc;
	err_1 -= dy2;
      }
      if (err_2 > 0) {
	point[2] += z_inc;
	err_2 -= dy2;
      }
      err_1 += dx2;
      err_2 += dz2;
      point[1] += y_inc;
    }
  } else {
    err_1 = dy2 - n;
    err_2 = dx2 - n;
    for (i = 0; i < n; i++) {
      cube[point[0]][point[1]][point[2]] ^= value;
      if (err_1 > 0) {
	point[1] += y_inc;
	err_1 -= dz2;
      }
      if (err_2 > 0) {
	point[0] += x_inc;
	err_2 -= dz2;
      }
      err_1 += dy2;
      err_2 += dx2;
      point[2] += z_inc;
    }
  }
  cube[point[0]][point[1]][point[2]] ^= value;
}

void dolines() {
  for(int i = 0; i < 8; i++) {
    drawline(0, 0, i, 7, i, 7, true);
    drawline(0, i, 0, i, 7, 7, true);
    drawline(i, 0, 0, 7, 7, i, true);
    writeCube(100);
  }
  for(int i = 0; i < 8; i++) {
    drawline(i, 0, i, 7, i, 7, true);
    drawline(0, i, 0, i, 7, 7, true);
    drawline(i, 0, i, 7, 7, i, true);
    writeCube(100);
  }
  for(int i = 0; i < 8; i++) {
    drawline(i, 0, 0, 7, i, 7, true);
    drawline(0, i, 0, i, 7, 7, true);
    drawline(0, 0, i, 7, 7, i, true);
    writeCube(100);
  }
}

void fillface() {
  int i,j;

  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,i,j,0, true);
      writeCube(60);
      drawline(4,4,4,i,j,0, true);
      cube[i][j][0] = true;
    }
  }
    
  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,0,i,j, true);
      writeCube(60);
      drawline(4,4,4,0,i,j, true);
      cube[0][i][j] = true;
    }
  }
    
  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,i,0,j, true);
      writeCube(60);
      drawline(4,4,4,i,0,j, true);
      cube[i][0][j] = true;
    }
  }

  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,i,j,7, true);
      writeCube(60);
      drawline(4,4,4,i,j,7, true);
      cube[i][j][7] = true;
    }
  }
    
  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,7,i,j, true);
      writeCube(60);
      drawline(4,4,4,7,i,j, true);
      cube[7][i][j] = true;
    }
  }
    
  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,i,7,j, true);
      writeCube(60);
      drawline(4,4,4,i,7,j, true);
      cube[i][7][j] = true;
    }
  }

  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,i,j,0, true);
      writeCube(60);
      drawline(4,4,4,i,j,0, true);
      cube[i][j][0] = false;
    }
  }
    
  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,0,i,j, true);
      writeCube(60);
      drawline(4,4,4,0,i,j, true);
      cube[0][i][j] = false;
    }
  }
    
  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,i,0,j, true);
      writeCube(60);
      drawline(4,4,4,i,0,j, true);
      cube[i][0][j] = false;
    }
  }

  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,i,j,7, true);
      writeCube(60);
      drawline(4,4,4,i,j,7, true);
      cube[i][j][7] = false;
    }
  }
    
  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,7,i,j, false);
      writeCube(60);
      drawline(4,4,4,7,i,j, false);
      cube[7][i][j] = false;
    }
  }
    
  for(i = 0; i < 8; i++) {
    for(j= 0; j < 8; j++) {
      drawline(4,4,4,i,7,j, false);
      writeCube(60);
      drawline(4,4,4,i,7,j, false);
      cube[i][7][j] = false;
    }
  }
  lowCube();
}

void laserToOppositeFace(){
 for(int i = 0; i < 8; i++) {
    for(int j= 0; j < 8; j++) {
      drawline(0,0,0,7,i,j, true);
      writeCube(50);
      
      
      //lowCube();
      
      for(int k=0;k<7;k++){
	for(int l=0;l<7;l++){
	    for(int m=0;m<7;m++){
		cube[k][l][m]=false;
	    }
	}
      }
      
      
      
    }
    for(int j= 0; j < 8; j++) {
      drawline(0,0,0,7-j,i,7, true);
      writeCube(50);
      
      //lowCube();
      
       for(int k=0;k<7;k++){
	for(int l=0;l<7;l++){
	    for(int m=0;m<7;m++){
		cube[k][l][m]=false;
	    }
	}
      }
      
      
    }
  }     

}

void loop() {
  laserToOppositeFace();
  lowCube();
  fillface();

}