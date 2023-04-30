const itt = require('../lib');

const fs = require('fs');

const tesseractdata = process.env.TESSDATA_PREFIX;
console.log(tesseractdata);

let filePath = './testfiles/go.png';

fs.readFile(filePath, null, async (err, data) => {
    if (err) {
        console.log(err);
    } else {
        try {
            const res = await itt.scanText(tesseractdata, data);
            console.log(res)
        } catch(err){
            console.log('error...');
            console.log(err);
        }
    }
});

// fs.readFile(filePath, null, (err, data) => {
//     if (err) {
//         console.log(err);
//     } else {

//         itt.preprocess('.png', data, (err, res) => {
//             if (err) {
//                 console.log(err);
//             } else {
//                 fs.writeFile('./testfiles/out.png', res, 'binary', (err) => {
//                     if (err) {
//                         console.log(err);
//                     }
//                 });
//             }
            
//         });
        
//     }
// });

// fs.readFile(filePath, null, (err, data) => {
//     if (err) {
//         console.log(err);
//     } else {

//         itt.setContrast('.png', data, 1.0, 50, (err, res) => {
//             if (err) {
//                 console.log('error: ', err);
//             } else {
//                 fs.writeFile('./testfiles/out.png', res, 'binary', (err) => {
//                     if (err) {
//                         console.log(err);
//                     }
//                 });
//             }
            
//         });
        
//     }
// });