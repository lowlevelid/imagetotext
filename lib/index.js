let itt;

try {
    itt = require('../build/Release/imagetotext.node');
} catch(e) {
    if (e.code == 'MODULE_NOT_FOUND') throw e;
    itt = require('../build/Debug/imagetotext.node');
}

module.exports = itt;

// Basic Usage

// const fs = require('fs');

// const tesseractdata = process.env.TESSDATA_PREFIX;
// console.log(tesseractdata);

// let filePath = './testfiles/go.png';

// fs.readFile(filePath, null, (err, data) => {
//     if (err) {
//         console.log(err);
//     } else {
//         itt.scanText(tesseractdata, data, (err, res) => {
//             if (err) {
//                 console.log('error...');
//                 console.log(err);
//             } else {
//                 console.log(res);
//             }
//         });
//     }
// });