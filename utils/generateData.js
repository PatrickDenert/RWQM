const fs = require('fs')

let currTime  = 1667770372826;
let temp = 50;
let ph = 7;
let flow = 30;
let cdt = 20;
let tb = 10;
let currtds = 40;

const arr = [];

for(let i = 0; i<1000; i++) {
    date = new Date(currTime)
    let currtemp = temp + 3*Math.cos((2*Math.PI/400)*i) + 0.003*i + (Math.random()*2 -1)*0.1;
    let currph = ph + Math.cos((2*Math.PI/100)*i) - 0.001*i + (Math.random()*2 -1)*0.2;
    let currflow = flow + 5*Math.cos((2*Math.PI/200)*i) + 0.01*i + (Math.random()*2 -1)*0.07;
    let currcdt = cdt + 2*Math.cos((2*Math.PI/50)*i) + 0.005*i + (Math.random()*2 -1)*0.5;
    let currtb = tb + 2*Math.cos((2*Math.PI/100)*i) - 0.008 + (Math.random()*2 -1)*0.8;
    currtds += (Math.random()*2 -1)*0.03;
    arr.push({ time: date.toISOString(), temp: currtemp, ph: currph, flow: currflow, cdt: currcdt, tb: currtb, tds: currtds });
    currTime += 900000;
}

let json =  JSON.stringify(arr);
fs.writeFile('data/testdata.json', json, 'utf8', () => {}); // write it back
