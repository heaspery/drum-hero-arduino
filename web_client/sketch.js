// Songs data
const marioBros = {
  name: "marioBros",
  notes: [
    64, 64, 64,
    60, 64, 67, 55,

    60, 55, 52,
    57, 59, 58, 57,
    55, 64, 67, 69,
    65, 67, 64, 60, 62, 59,

    60, 55, 52,
    57, 59, 57,

    67, 66, 65, 62, 64,
    55, 57, 60,
    57, 60, 62,
    63, 62, 60,

    60, 55, 52,
    57, 59, 57
  ],
  duration: [
    150, 150, 150,
    150, 150, 200, 200,

    150, 200, 200,
    150, 150, 150, 150,
    150, 150, 150, 150,
    150, 150, 150, 150, 150, 150,

    150, 200, 200,
    150, 150, 150,

    150, 150, 150, 150, 150,
    150, 150, 150,
    150, 150, 150,
    150, 150, 150,

    150, 200, 200,
    150, 150, 150
  ]
};

const tetris = {
  name: "tetris",
  notes: [
  76, 71, 72, 74, 72, 71, 69,
  69, 72, 76, 74, 72,
  71, 72, 74, 76,
  72, 69, 69,
  74, 77, 81,
  79, 77, 76,
  72, 76, 74, 72,
  71, 71, 72, 74,
  76, 72, 69, 69
],
  duration: [
  200, 100, 100, 200, 100, 100, 200,
  100, 100, 200, 100, 100,
  200, 100, 100, 200,
  200, 200, 200,
  200, 100, 200,
  100, 100, 200,
  100, 200, 100, 100,
  200, 100, 100, 200,
  200, 200, 200, 200
]
};

const pokemonBicycle = {
  name: "pokemonBicycle",
  notes: [
  64, 65, 67, 72, 71,
  69, 71, 69,
  65, 67, 69, 74, 72, 71, 69, 71, 72,
  69, 67, 70,
  65, 72, 71, 72, 69,
  65, 64, 65, 67, 72, 71, 72,
  67, 64, 62, 64, 65, 72,
  76, 74, 72, 71, 72,
  74, 74, 72, 71
],
  duration: [
  150, 150, 150, 150, 200,
  150, 150, 200,
  150, 150, 150, 150, 150, 150, 150, 150, 200,
  150, 150, 200,
  150, 150, 150, 150, 200,
  150, 150, 150, 150, 150, 150, 200,
  150, 150, 150, 150, 150, 200,
  150, 150, 150, 150, 200,
  150, 150, 150, 200
]
};

// one randomized song 
let randomizeSong = {
  name: "random",
  notes: [],
  duration: []
};

// function that generate the randomized song
function randomSong() {
  randomizeSong.notes = [];
  randomizeSong.duration = [];
  for (let i = 0; i < 50; i++) {
    randomizeSong.notes.push(Math.floor(Math.random() * 25) + 60);
    randomizeSong.duration.push(Math.floor(Math.random() * 150) + 100); 
  }
}

randomSong();

// array of songs to choose from
const songs = [tetris, marioBros, pokemonBicycle, randomizeSong];


let serial; // variable to hold an instance of the serialport library
let portName = "/dev/tty.usbmodem3401"; // fill in your serial port name here

let width = window.innerWidth;
let height = window.innerHeight;

let combo = 0;
let score = 0;
let bestScore = 0;
let bestCombo = 0;

let index = 0;
let notes = [];
let noteDuration = [];
let currentSong = null;

let isMiss = false;
let isSuccess = false;
let gameState = 0;

let successHistory = [];
let osc;

function setup() {
  console.log("Setup sketch");
  createCanvas(width, height);

  // Load random song
  loadRandomSong();

  document.addEventListener('mousedown', resumeAudio);
  document.addEventListener('keydown', resumeAudio);
  document.addEventListener('touchstart', resumeAudio);

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

  // instantiate the oscillator for the sound effects
  osc = new p5.SawOsc();
  osc.start();
  osc.amp(0);

}

// get the list of ports:
function printList(portList) {
  // portList is an array of serial port names
  for (let i = 0; i < portList.length; i++) {
    console.log(i + ": " + portList[i]);
  }
}

// Function to resume audio context on user interaction
function resumeAudio() {
  getAudioContext().resume().catch(err => {
    console.log("Audio context resume failed:", err);
  });
  document.removeEventListener('mousedown', resumeAudio);
  document.removeEventListener('keydown', resumeAudio);
  document.removeEventListener('touchstart', resumeAudio);
}

function serverConnected() {
  console.log("connected to server.");
}

function portOpen() {
  console.log("the serial port is opened.");
}

// This function is called when data is received from the serial port
function serialEvent() {
  let rawData = serial.readLine();
  if (!rawData) return;

  // Check for MISS or SUCCESS first
  if (rawData === "MISS") {
    isMiss = true;
    isSuccess = false;
    successHistory.push(false);
  } else if (rawData === "SUCCESS") {
    isSuccess = true;
    isMiss = false;
    successHistory.push(true);
    playNote(notes[index], noteDuration[index]); // if success, play the note
    index++;
    if (index >= notes.length) {
      index = 0;
    }
  } else {
    isMiss = false;
    isSuccess = false;

    rawData = rawData.trim();
    if (!rawData) return;

    // if it's not MISS or SUCCESS, try to parse it as JSON
    try {
      let data = JSON.parse(rawData);
      console.log(data);

      if (data.combo !== undefined) {
        combo = data.combo;
      }

      if (data.score !== undefined) {
        score = data.score;
      }

      if (data.state !== undefined) {
        // Load new random song when transitioning from state 0 to 1
        if (gameState === 0 && data.state === 1) {
          loadRandomSong();
        }

        gameState = data.state;
        if (data.state === 0) {
          successHistory = [];
          index = 0; // Reset note index
        }
      }
    } catch (e) {
      console.warn("Invalid JSON from serial:", rawData);
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

// function to get a random song from the array
function getRandomSong() {
  const i = Math.floor(Math.random() * songs.length);
  return songs[i];
}

// Function to load a random song from the songs array
function loadRandomSong() {
  currentSong = getRandomSong();
  notes = currentSong.notes;
  noteDuration = currentSong.duration || Array(currentSong.notes.length).fill(200);
  index = 0;
  console.log("Loaded song:", currentSong.name);
}

// activate the note for a certain duration
function playNote(noteNumber, duration) {
  if (!osc) return;

  let frequency = 440 * Math.pow(2, (noteNumber - 69) / 12);

  let noteDur = duration || 200;

  // Play the note
  osc.freq(frequency);
  osc.amp(0.2, 0.05);
  setTimeout(() => {
    osc.amp(0, 0.05);
  }, noteDur * 0.8);
}

function preload() {
  font = loadFont('/assets/Evil_Empire.otf'); // load the local font
}

// draw text and game state
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

  // Draw success history
  for (let i = 0; i < successHistory.length; i++) {
    if (successHistory[i]) {
      fill(0, 255, 0);
      stroke(255, 255, 255);

    } else {
      fill(255, 0, 0);
      stroke(255, 255, 255);

    }

    rect(width / 2 - 500 + i * 20, height - 100, 15, 15);
    fill(0);
  }

  // Display game state messages
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

