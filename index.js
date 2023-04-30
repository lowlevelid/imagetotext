const itt = require('./lib');

const express = require('express');
const cookieParser = require('cookie-parser');
const bodyParser = require('body-parser');
const morgan = require('morgan');
const multer = require('multer');
const filesig = require('filesig');
const winston = require('winston');
const path = require('path');

const app = express();
const server = require('http').createServer(app);

// set default PORT
const PORT = process.env.HTTP_PORT;
const tesseractdata = process.env.TESSDATA_PREFIX;

const logger = winston.createLogger({
    format: winston.format.combine(
        winston.format.timestamp({
        format: 'YYYY-MM-DD HH:mm:ss'
        }),
        winston.format.errors({ stack: true }),
        winston.format.splat(),
        winston.format.json(),
        winston.format.colorize({ all: true })
    ),
    transports: [new winston.transports.Console()]
});


// initialize multer for handling multipart/form-data
const storage = multer.memoryStorage();
const upload = multer({ storage: storage })

// view engine
// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'html');
app.engine('.html', require('ejs').__express);

//middleware
app.use(morgan('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended: false}));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

// routes
app.get('/', (req, res, next) => {
    return res.status(200).json({succees: true, message: 'server up and running'});
});

app.post('/scan', (req, res, next) => {
    const uploadFn = upload.single('file');
    try {
        uploadFn(req, res, async (err) => {
            if (err) {
                logger.error(err);
                return res.status(400).json({succees: false, message: 'error upload image file'});
            }

            if (!req.file) {
                logger.error(err);
                return res.status(400).json({succees: false, message: 'error upload image file'});
            }
    
            const file = req.file;
            let validFile = filesig.oneOf(file.buffer, 
                filesig.isJpeg, 
                filesig.isPng, 
                filesig.isWebp);
                
            if (!validFile) {
                logger.error('invalid file');
                return res.status(400).json({succees: false, message: 'file is not valid'});
            }

            try {
                let output = await itt.scanText(tesseractdata, file.buffer);
                output = output.replace(/\n\n*/g, '|');
                output = output.replace(/[\t\s]+/g, ' ');
                return res.status(200).json({
                    succees: true, 
                    message: 'scan text from image succeed, the | show if its a newline', 
                    data: output
                });
            } catch(err){
                logger.error(err);
                return res.status(400).json({succees: false, message: 'error scan text from image'});
            }
        });
    } catch(e) {
        logger.error(e);
        return res.status(400).json({succees: false, message: 'error upload image file'});
    }
});

app.use((req, res, next) => {
    return res.status(404).json({succees: false, message: 'path not found'});
});

// listen app on PORT
server.listen(PORT, err => {
    if (err) {
        logger.error(`error on start up: ${err}`);
    } else {
        logger.info(`app listen on port ${PORT}`);
    }
});