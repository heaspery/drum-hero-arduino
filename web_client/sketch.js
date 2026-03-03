console.log("sketch");

let serial; // variable to hold an instance of the serialport library
let portName = "/dev/tty.usbmodem3301"; // fill in your serial port name here

let width = window.innerWidth;
let height = window.innerHeight;

let combo = 0;
let score = 0;
let bestScore = 0;
let bestCombo = 0;

let isMiss = false;
let isSuccess = false;
let gameState = 0;

let successHistory = [];

function setup() {
  console.log("Setup sketch");
  createCanvas(width, height);

  // serial
  serial = new p5.SerialPort();
  console.log(serial)// make a new instance of the serialport library
  serial.on("list", printList); // set a callback function for the serialport list event
  serial.on("connected", serverConnected); // callback for connecting to the server
  serial.on("open", portOpen); // callback for the port opening
  serial.on("data", serialEvent); // callback for when new data arrives
  serial.on("error", serialError); // callback for errors
  serial.on("close", portClose); // callback for the port closing


  serial.list(); // list the serial ports
  serial.open(portName); // open a serial port

}

// get the list of ports:
function printList(portList) {
  // portList is an array of serial port names
  for (let i = 0; i < portList.length; i++) {
    console.log(i + ": " + portList[i]);
  }
}

function serverConnected() {
  console.log("connected to server.");
}

function portOpen() {
  console.log("the serial port is opened.");
}

function serialEvent() {
  let rawData = serial.readLine();
  if (!rawData) return;

  if (rawData === "MISS") {
    isMiss = true;
    isSuccess = false;
    successHistory.push(false);
  } else if (rawData === "SUCCESS") {
    isSuccess = true;
    isMiss = false;
    successHistory.push(true);
  } else {
    isMiss = false;
    isSuccess = false;
    let data = JSON.parse(rawData);
    console.log(data);

    if (data.combo !== undefined) {
      combo = data.combo;
    }

    if (data.score !== undefined) {
      score = data.score;
    }

    if (data.state !== undefined) {
      gameState = data.state;
      if (data.state === 0) {
        successHistory = [];
      }
    }

  }

}

function serialError(err) {
  console.log("Something went wrong with the serial port. " + err);
}

function portClose() {
  console.log("The serial port is closed.");
}

function sendValues() {

}

function preload() {
  font = loadFont('/assets/Evil_Empire.otf');
}

function draw() {
  background(300);
  fill(0);
  textAlign(CENTER);
  textFont(font);
  textSize(80);


  text('Drum Hero', width / 2, 100);

  textSize(70);
  textFont(font);
  text('Score: ' + score, width / 2, 350);

  textSize(50);
  text('Combo: ' + combo, width / 2, 450);

  textSize(30);
  text('Best score: ' + bestScore, width / 2, 550);
  text('Best combo: ' + bestCombo, width / 2, 600);

  if (score > bestScore) {
    bestScore = score;
  }
  if (combo > bestCombo) {
    bestCombo = combo;
  }

  for (let i = 0; i < successHistory.length; i++) {
    if (successHistory[i]) {
      fill(0, 255, 0);
      stroke(255, 255, 255);

    } else {
      fill(255, 0, 0);
      stroke(255, 255, 255);

    }

    rect(width / 2 - 500 + i * 20, height - 200, 15, 15);
    fill(0);
  }

  if (gameState == 0) {
    textFont('Arial');
    textSize(30);
    text('Press the left button to start the game', width / 2, 200);
  }

  if (gameState == 1) {
    textFont('Arial');
    textSize(30);
    text('Game in progress...', width / 2, 200);
  }
}


