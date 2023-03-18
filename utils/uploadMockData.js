const fs= require('fs');
const fetch = require('node-fetch');
let arr = fs.readFileSync('./data/testdata.json', 'utf8', (err, data) => {
  if (err) {
    console.error(err);
    return;
  }
});
arr = JSON.parse(arr)
sendData(arr);

async function postSensorData(payload) {
    try{
        const response = await fetch('http://localhost:3000/reading', {
            method: 'POST',
            body: JSON.stringify(payload),
            headers: {'Content-Type': 'application/json'}
        });
        const data = await response.json();
        return data
    } catch (err) {
        console.log(err)
    }
}

function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}

async function sendData(arr){
    for (var i = 0; i < arr.length; i++) {
        let response = postSensorData(arr[i]);
        await sleep(200);
        console.log(response);
    }
}
