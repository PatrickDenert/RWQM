var md5 = require('md5');
var aesjs = require('aes-js')

// initialize keys and AES module
const key = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
const iv = [21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,35, 36];

// global variables
const MAC_SIZE = 16;
const failedStatusCode = 401;

module.exports = function() {
    return function secured(req, res, next) {

        // reset AES state and
        var aes = new aesjs.ModeOfOperation.cbc(key, iv);
        let payload = Array.from(aes.decrypt(req.body.payload))
        let length = payload.length

        let MAC = payload.splice(length - MAC_SIZE, MAC_SIZE)
        let message = payload;

        let newMAC = hashStringToArray(md5(message));
        var aes = new aesjs.ModeOfOperation.cbc(key, iv);
        newMAC = aes.encrypt(newMAC);

        if (MACs_MATCH(MAC, newMAC)) {
            console.log("Success, The MACs Match");
            message = removePadding(message)
            message = aesjs.utils.utf8.fromBytes(message);
            message = message.split(',');
            var result = '';
            for (let i = 0; i < message.length; i++) {
              result += String.fromCharCode(parseInt(message[i]));
            }
            console.log(result, typeof(result))
            req.body = message
            return next();
        } else {
            console.log('Failure, the MACs do not match');
            return res.status(500).send(JSON.stringify({ response: 'Credentials Not Valid' }));
        }
    };
};

function hashStringToArray(string) {
    const arr = new Uint8Array(16)
    for (let i = 0; i < 32; i += 2){
        arr[i/2] = parseInt(string.substr(i, 2), 16);
    }
    return arr;
}

function MACs_MATCH(MAC, newMAC){
    for(let i = 0; i < MAC.length; i++){
        if(MAC[i] !== newMAC[i]){ return false }
    }
    return true;
}

function removePadding(message) {
    while(message[message.length-1] === 0){
        message.pop()
    }
    return message
}
