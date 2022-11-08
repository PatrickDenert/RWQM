fs = require('fs')

let currTime  = 1667770372826;
let currtemp = 50;
let currph = 7;
let currflow = 30;
let currcdt = 20;
let currtb = 10;

const arr = [];

for(let i = 0; i<1000; i++) {
    date = new Date(currTime)
    arr.push({ time: date.toISOString(), temp: currtemp, ph: currph, flow: currflow, cdt: currcdt, tb: currtb });
    currTime -= 900000;
    currtemp += (Math.random()*2 -1)*0.1;
    currph+= (Math.random()*2 -1)*0.01;
    currflow += (Math.random()*2 -1)*0.01;
    currcdt += (Math.random()*2 -1)*0.01;
    currtb += (Math.random()*2 -1)*0.01;
}

let json =  JSON.stringify(arr);
fs.writeFile('data/testdata.json', json, 'utf8', () => {}); // write it back 

