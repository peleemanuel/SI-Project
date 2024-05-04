import mongoose from "mongoose";

const seraSchema = new mongoose.Schema({
    id: {   
        type: String,
        required: true,
    },
    humidity: {
        type: Number,
        required: true,
    },
    temperature: {
        type: Number,
        required: true,
    },
    light: {
        type: Number,
        required: true,
    },
    soil_moisture_plant_1: {
        type: Number,
        required: true,
    },
    soil_moisture_plant_2: {
        type: Number,
        required: true,
    },
    soil_moisture_plant_3: {
        type: Number,
        required: true,
    },
    soil_moisture_plant_4: {
        type: Number,
        required: true,
    }
});

const Sera = mongoose.model("Sera", seraSchema);

module.exports = Sera;