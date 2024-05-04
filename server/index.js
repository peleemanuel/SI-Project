import express from "express";
import bodyParser from "body-parser";
import cors from "cors";
import fileUpload from "express-fileupload";
import mongoose from "mongoose";
import fs from "fs"; 
import dotenv from "dotenv";

dotenv.config();
const app = express();
const port = 5000;

app.use(
  fileUpload({
    limits: {
      fileSize: 10000000,
    },
    abortOnLimit: true,
  })
);

// MongoDB connection
mongoose.connect(process.env.MONGO_URI);

app.use(cors());
app.use(bodyParser.json());

app.post('/data', async (req, res) => {
    console.log(req.body); // Log the received data
    const { id, humidity, temperature, light, soil_moisture_plant_1, soil_moisture_plant_2, soil_moisture_plant_3, soil_moisture_plant_4 } = req.body;

    // Check if the id already exists
    const existingData = await Sera.findOne({ id });
    
    // If the id already exists, update the data
    if (existingData) {
        await Sera.updateOne({ id }, { humidity, temperature, light, soil_moisture_plant_1, soil_moisture_plant_2, soil_moisture_plant_3, soil_moisture_plant_4 });

        console.log('Data updated');
    } else {
        // If the id does not exist, create a new entry
        const newSera = new Sera({ id, humidity, temperature, light, soil_moisture_plant_1, soil_moisture_plant_2, soil_moisture_plant_3, soil_moisture_plant_4 });

        await newSera.save();

        console.log('New entry saved');
    }
    
    res.status(200).send('Data received');
});

app.get("/", (req, res) => {
  console.log("Received GET request to /");
  res.send("Hello World!");
});

app.post("/upload", (req, res) => {
  console.log("Received POST request to /upload");

  // If no files were uploaded, exit
  if (!req.files) {
    console.log("No files uploaded");
    return res.sendStatus(400);
  }

  // The name of the input field (i.e. "image") is used to retrieve the uploaded file
  let image = req.files.image;

  // If no image submitted, exit
  if (!image) {
    console.log("No image submitted");
    return res.sendStatus(400);
  }

  // If does not have image mime type prevent from uploading
  if (!/^image/.test(image.mimetype)) {
    console.log("Invalid image mimetype:", image.mimetype);
    return res.sendStatus(400);
  }

  // Add the timestamp to the image name
  image.name = Date.now() + "_" + image.name;

  // Check if the folder exists, if not create it
  if (!fs.existsSync("./images")) {
    fs.mkdirSync("./images");
  }

  // Move the uploaded image to our upload folder
  image.mv("./images/" + image.name);

  console.log("Image uploaded successfully:", image.name);

  // All good
  res.sendStatus(200);
});

app.listen(port, () => {
  console.log(`Server listening on port ${port}`);
});
