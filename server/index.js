import express from "express";
import bodyParser from "body-parser";
import cors from "cors";
import fileUpload from "express-fileupload";
import mongoose from "mongoose";
import fs from "fs"; 
import path from "path";
import dotenv from "dotenv";
import Sera from "./sera_model.js";
import mqtt from 'mqtt';
import processImageClassification from "./classification.js";
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

const client = mqtt.connect('mqtt://broker.hivemq.com:1883', {
  
});

client.on('connect', function () {
    console.log('Connected to MQTT broker');
    client.subscribe('testTopic', function (err) {
        if (!err) {
            // The message to send to the ESP32
            client.publish('UPT/SmartSera/esp32/command', 'Turn on light');
        }
    });
});

client.on('message', function (topic, message) {
    // message is a Buffer
    console.log(topic.toString());
    console.log(message.toString());
});

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

app.post("/upload", async (req, res) => {
  console.log("Received POST request to /upload");

  if (!req.files || !req.files.image) {
    console.log("No files uploaded");
    return res.sendStatus(400);
  }

  const image = req.files.image;

  if (!/^image/.test(image.mimetype)) {
    console.log("Invalid image mimetype:", image.mimetype);
    return res.sendStatus(400);
  }

  const currentDir = path.dirname(new URL(import.meta.url).pathname);
  const uploadDir = path.join(currentDir, 'images');
  const uploadPath = path.join(uploadDir, image.name);

  // Ensure the uploads directory exists
  fs.mkdirSync(uploadDir, { recursive: true });

  image.mv(uploadPath, async (err) => {
    if (err) {
      console.error("Error saving file:", err);
      return res.sendStatus(500);
    }

    try {
      const predictedClassPlants = await processImageClassification(image.name, "plants_model/plants.py");
      const predictedClassPests = await processImageClassification(image.name, "plants_model/pests.py");

      // Remove the temporary file after processing
      fs.unlinkSync(uploadPath);

      res.status(200).json({ predictedClassPlants, predictedClassPests });
    } catch (err) {
      console.error("Error processing image classification:", err);
      res.sendStatus(500);
    }
  });
});

app.listen(port, () => {
  console.log(`Server listening on port ${port}`);
});
